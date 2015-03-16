/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * This file is part of libxls -- A multiplatform, C/C++ library
 * for parsing Excel(TM) files.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY David Hoerl ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL David Hoerl OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright 2004 Komarov Valery
 * Copyright 2006 Christophe Leitienne
 * Copyright 2013 Bob Colbert
 * Copyright 2008-2013 David Hoerl
 *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <memory.h>
#include <math.h>
#include <sys/types.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>

#include "libxls/endian.h"
#include "libxls/xls.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#define DEBUG_DRAWINGS
int xls_debug = 0;

static double NumFromRk(DWORD_UA drk);
static xls_formula_handler formula_handler;

extern void xls_addSST(xlsWorkBook* pWB,SST* sst,DWORD size);
extern void xls_appendSST(xlsWorkBook* pWB,BYTE* buf,DWORD size);
extern void xls_addFormat(xlsWorkBook* pWB,FORMAT* format);
extern BYTE* xls_addSheet(xlsWorkBook* pWB,BOUNDSHEET* bs);
extern void xls_addRow(xlsWorkSheet* pWS,ROW* row);
extern void xls_makeTable(xlsWorkSheet* pWS);
extern struct st_cell_data *xls_addCell(xlsWorkSheet* pWS,BOF* bof,BYTE* buf);
extern BYTE *xls_addFont(xlsWorkBook* pWB,FONT* font);
extern void xls_addXF8(xlsWorkBook* pWB,XF8* xf);
extern void xls_addXF5(xlsWorkBook* pWB,XF5* xf);
extern void xls_addColinfo(xlsWorkSheet* pWS,COLINFO* colinfo);
extern void xls_mergedCells(xlsWorkSheet* pWS,BOF* bof,BYTE* buf);
extern void xls_parseWorkBook(xlsWorkBook* pWB);
extern void xls_preparseWorkSheet(xlsWorkSheet* pWS);
extern void xls_formatColumn(xlsWorkSheet* pWS);
extern void xls_parseWorkSheet(xlsWorkSheet* pWS);
extern void xls_dumpSummary(char *buf,int isSummary,xlsSummaryInfo	*pSI);

#ifdef AIX
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

typedef struct {
	uint32_t		format[4];
	uint32_t		offset;
} sectionList;

typedef struct {
	uint16_t		sig;
	uint16_t		_empty;
	uint32_t		os;
	uint32_t		format[4];
	uint32_t		count;
	sectionList		secList[0];
} header;

typedef struct {
	uint32_t		propertyID;
	uint32_t		sectionOffset;
} propertyList;

typedef struct {
	uint32_t		length;
	uint32_t		numProperties;
	propertyList	properties[0];
} sectionHeader;

typedef struct {
	uint32_t		propertyID;
	uint32_t		data[0];
} property;

#ifdef DEBUG_DRAWINGS
struct drawHeader {
	unsigned int rec : 4;
	unsigned int instance : 12;
	unsigned int type : 16;
	unsigned int len : 32;
};

static char *formData;
static char *formFunc;
static struct drawHeader drawProc(uint8_t *buf, uint32_t maxLen, uint32_t *off, int level);
static void dumpRec(char *comment, struct drawHeader *h, int len, uint8_t *buf);
static int finder(uint8_t *buf, uint32_t len, uint16_t pattern);
static uint32_t sheetOffset;
#endif

#pragma pack(pop)

int xls(int debug)
{
	xls_debug = debug;
    return 1;
}

void xls_addSST(xlsWorkBook* pWB,SST* sst,DWORD size)
{
    verbose("xls_addSST");

    pWB->sst.continued=0;
    pWB->sst.lastln=0;
    pWB->sst.lastid=0;
    pWB->sst.lastrt=0;
    pWB->sst.lastsz=0;

    pWB->sst.count = sst->num;
    pWB->sst.string =(struct str_sst_string *)calloc(pWB->sst.count, sizeof(struct str_sst_string));
    xls_appendSST(pWB,&sst->strings,size-8);
}

void xls_appendSST(xlsWorkBook* pWB,BYTE* buf,DWORD size)
{
    DWORD ln;	// String character count
    DWORD ofs;	// Current offset in SST buffer
    DWORD rt;	// Count of rich text formatting runs
    DWORD sz;	// Size of asian phonetic settings block
    BYTE flag;	// String flags
    BYTE* ret;

    if (xls_debug) {
	    printf("xls_appendSST %u\n", size);
    }

	sz = rt = ln = 0;	// kch
    ofs=0;

	while(ofs<size)
    {
        int ln_toread;

        // Restore state when we're in a continue record
        // or read string length
        if (pWB->sst.continued)
        {
            ln=pWB->sst.lastln;
            rt=pWB->sst.lastrt;
            sz=pWB->sst.lastsz;
        }
        else
        {
            ln=xlsShortVal(*(WORD_UA *)(buf+ofs));
            rt = 0;
            sz = 0;

            ofs+=2;
        }

		if (xls_debug) {
        	printf("ln=%u\n", ln);
		}

        // Read flags
        if ( (!pWB->sst.continued) || ( (pWB->sst.continued) && (ln != 0) ) )
        {
            flag=*(BYTE *)(buf+ofs);
            ofs++;

            // Count of rich text formatting runs
            if (flag & 0x8)
            {
                rt=xlsShortVal(*(WORD_UA *)(buf+ofs));
                ofs+=2;
            }

            // Size of asian phonetic settings block
            if (flag & 0x4)
            {
                sz=xlsIntVal(*(DWORD_UA *)(buf+ofs));
                ofs+=4;

				if (xls_debug) {
					printf("sz=%u\n", sz);
				}
            }
        }
        else
        {
            flag = 0;
        }

		// Read characters (compressed or not)
        ln_toread = 0;
        if (ln > 0)
        {
            if (flag & 0x1)
            {
                size_t new_len = 0;
                ln_toread = min((size-ofs)/2, ln);
                ret=unicode_decode(buf+ofs,ln_toread*2,&new_len,pWB->charset);

                if (ret == NULL)
                {
                    ret = (BYTE *)strdup("*failed to decode utf16*");
                    new_len = strlen((char *)ret);
                }

                ret = (BYTE *)realloc(ret,new_len+1);
                *(BYTE*)(ret+new_len)=0;

                ln -= ln_toread;
                ofs+=ln_toread*2;

                if (xls_debug) {
	                printf("String16SST: %s(%zd)\n",ret,new_len);
                }
            }
            else
            {
                ln_toread = min((size-ofs), ln);

				ret = utf8_decode((buf+ofs), ln_toread, pWB->charset);

                ln  -= ln_toread;
                ofs +=ln_toread;

                if (xls_debug) {
                	printf("String8SST: %s(%u) \n",ret,ln);
                }
            }
        }
        else
        {
         ret = (BYTE *)strdup("");
        }

        if (  (ln_toread > 0)
            ||(!pWB->sst.continued) )
        {
            // Concat string if it's a continue, or add string in table
            if (!pWB->sst.continued)
            {
                pWB->sst.lastid++;
                pWB->sst.string[pWB->sst.lastid-1].str=ret;
            }
            else
            {
                BYTE *tmp;
                tmp=pWB->sst.string[pWB->sst.lastid-1].str;
                tmp=(BYTE *)realloc(tmp,strlen((char *)tmp)+strlen((char *)ret)+1);
                pWB->sst.string[pWB->sst.lastid-1].str=tmp;
                memcpy(tmp+strlen((char *)tmp),ret,strlen((char *)ret)+1);
				free(ret);
            }

			if (xls_debug) {
	            printf("String %4u: %s<end>\n", pWB->sst.lastid-1, pWB->sst.string[pWB->sst.lastid-1].str);
			}
        }

		// Jump list of rich text formatting runs
        if (  (ofs < size)
            &&(rt > 0) )
          {
           int rt_toread = min((size-ofs)/4, rt);
           rt -= rt_toread;
           ofs += rt_toread*4;
          }

		// Jump asian phonetic settings block
        if (  (ofs < size)
            &&(sz > 0) )
          {
           int sz_toread = min((size-ofs), sz);
           sz -= sz_toread;
           ofs += sz_toread;
          }

        pWB->sst.continued=0;
    }

    // Save current character count and count of rich text formatting runs and size of asian phonetic settings block
	if (ln > 0 || rt > 0 || sz > 0) {
		pWB->sst.continued = 1;
		pWB->sst.lastln = ln;
		pWB->sst.lastrt = rt;
		pWB->sst.lastsz = sz;

		if (xls_debug) {
			printf("continued: ln=%u, rt=%u, sz=%u\n", ln, rt, sz);
		}
	}
}

static double NumFromRk(DWORD_UA drk)
{
	double ret;

	// What kind of value is this ?
    if (drk & 0x02) {
    	// Integer value
		int tmp = (int)drk >> 2;	// cast to keep it negative in < 0
        ret = (double)tmp;
    } else {
    	// Floating point value;
		unsigned64_t tmp = drk & 0xfffffffc;
		tmp <<= 32;
		memcpy(&ret, &tmp, sizeof(unsigned64_t));
    }
    // Is value multiplied by 100 ?
    if (drk & 0x01) {
        ret /= 100.0;
    }
    return ret;
}

BYTE* xls_addSheet(xlsWorkBook* pWB, BOUNDSHEET *bs)
{
	BYTE* name;
	DWORD filepos;
	BYTE visible, type;

	filepos = bs->filepos;
	visible = bs->visible;
	type = bs->type;

	// printf("charset=%s uni=%d\n", pWB->charset, unicode);
	// printf("bs name %.*s\n", bs->name[0], bs->name+1);
	name=get_string(bs->name, 0, pWB->is5ver, pWB->charset);
	// printf("name=%s\n", name);

	if(xls_debug) {
		printf ("xls_addSheet[0x%x]\n", type);
		switch (type & 0x0f)
		{
		case 0x00:
			/* worksheet or dialog sheet */
			printf ("85: Worksheet or dialog sheet\n");
			break;
		case 0x01:
			/* Microsoft Excel 4.0 macro sheet */
			printf ("85: Microsoft Excel 4.0 macro sheet\n");
			break;
		case 0x02:
			/* Chart */
			printf ("85: Chart sheet\n");
			break;
		case 0x06:
			/* Visual Basic module */
			printf ("85: Visual Basic sheet\n");
			break;
		default:
			printf ("???\n");
			break;
		}
		printf("visible: %x\n", visible);
		printf("    Pos: %Xh\n",filepos);
		printf("   type: %.4Xh\n",type);
		printf("   name: %s\n", name);
	}

    if (pWB->sheets.count==0)
    {
        pWB->sheets.sheet=(struct st_sheet_data *) malloc(sizeof (struct st_sheet_data));
    }
    else
    {
        pWB->sheets.sheet=(struct st_sheet_data *) realloc(pWB->sheets.sheet,(pWB->sheets.count+1)*sizeof (struct st_sheet_data));
    }
    pWB->sheets.sheet[pWB->sheets.count].name=name;
    pWB->sheets.sheet[pWB->sheets.count].filepos=filepos;
    pWB->sheets.sheet[pWB->sheets.count].visibility=visible;
    pWB->sheets.sheet[pWB->sheets.count].type=type;
    pWB->sheets.count++;

	return name;
}


void xls_addRow(xlsWorkSheet* pWS,ROW* row)
{
    struct st_row_data* tmp;

    //verbose ("xls_addRow");

    tmp=&pWS->rows.row[row->index];
    tmp->height=row->height;
    tmp->fcell=row->fcell;
    tmp->lcell=row->lcell;
    tmp->flags=row->flags;
    tmp->xf=row->xf&0xfff;
    tmp->xfflags=(row->xf >> 8)&0xf0;
    if(xls_debug) xls_showROW(tmp);
}

void xls_makeTable(xlsWorkSheet* pWS)
{
    DWORD i,t;
    struct st_row_data* tmp;
    verbose ("xls_makeTable");

    pWS->rows.row=(struct st_row_data *)calloc((pWS->rows.lastrow+1),sizeof(struct st_row_data));

	// printf("ALLOC: rows=%d cols=%d\n", pWS->rows.lastrow, pWS->rows.lastcol);
    for (t=0;t<=pWS->rows.lastrow;t++)
    {
        tmp=&pWS->rows.row[t];
        tmp->index=t;
        tmp->fcell=0;
        tmp->lcell=pWS->rows.lastcol;

		tmp->cells.count = pWS->rows.lastcol+1;
        tmp->cells.cell=(struct st_cell_data *)calloc(tmp->cells.count,sizeof(struct st_cell_data));

        for (i=0;i<=pWS->rows.lastcol;i++)
        {
            tmp->cells.cell[i].col=i;
            tmp->cells.cell[i].row=t;
            tmp->cells.cell[i].width=pWS->defcolwidth;
            tmp->cells.cell[i].xf=0;
            tmp->cells.cell[i].str=NULL;
            tmp->cells.cell[i].d=0;
            tmp->cells.cell[i].l=0;
            tmp->cells.cell[i].isHidden=0;
            tmp->cells.cell[i].colspan=0;
            tmp->cells.cell[i].rowspan=0;
            tmp->cells.cell[i].id=XLS_RECORD_BLANK;
            tmp->cells.cell[i].str=NULL;
        }
    }
}

struct st_cell_data *xls_addCell(xlsWorkSheet* pWS,BOF* bof,BYTE* buf)
{
    struct st_cell_data*	cell;
    struct st_row_data*		row;
    int						i;

	verbose ("xls_addCell");

	// printf("ROW: %u COL: %u\n", xlsShortVal(((COL*)buf)->row), xlsShortVal(((COL*)buf)->col));
    row=&pWS->rows.row[xlsShortVal(((COL*)buf)->row)];
    //cell=&row->cells.cell[((COL*)buf)->col - row->fcell]; DFH - inconsistent
    cell=&row->cells.cell[xlsShortVal(((COL*)buf)->col)];
    cell->id=bof->id;
    cell->xf=xlsShortVal(((COL*)buf)->xf);

    switch (bof->id)
    {
    case XLS_RECORD_FORMULA:
    case XLS_RECORD_FORMULA_ALT:
		// test for formula, if
		xlsConvertFormula((FORMULA *)buf);
        cell->id=XLS_RECORD_FORMULA;
        if (((FORMULA*)buf)->res!=0xffff) {
			// if a double, then set double and clear l
			cell->l=0;
			memcpy(&cell->d, &((FORMULA*)buf)->resid, sizeof(double));	// Required for ARM
			cell->str=xls_getfcell(pWS->workbook,cell, NULL);
		} else {
			cell->l = 0xFFFF;
			double d = ((FORMULA*)buf)->resdata[1];
			switch(((FORMULA*)buf)->resid) {
			case 0:		// String
				break;	// cell is half complete, get the STRING next record
			case 1:		// Boolean
				memcpy(&cell->d, &d, sizeof(double)); // Required for ARM
				sprintf((char *)(cell->str = malloc(sizeof("bool"))), "bool");
				break;
			case 2:		// error
				memcpy(&cell->d, &d, sizeof(double)); // Required for ARM
				sprintf((char *)(cell->str = malloc(sizeof("error"))), "error");
				break;
			case 3:		// empty string
				cell->str = calloc(1,1);
				break;
			}
		}
		if(formula_handler) formula_handler(bof->id, bof->size, buf);
        break;
    case XLS_RECORD_MULRK:
printf("MULRK: %d\n", bof->size);
        for (i = 0; i < (bof->size - 6)/6; i++)	// 6 == 2 row + 2 col + 2 trailing index
        {
            cell=&row->cells.cell[xlsShortVal(((MULRK*)buf)->col + i)];
			// printf("i=%d col=%d\n", i, xlsShortVal(((MULRK*)buf)->col + i) );
            cell->id=XLS_RECORD_RK;
            cell->xf=xlsShortVal(((MULRK*)buf)->rk[i].xf);
            cell->d=NumFromRk(xlsIntVal(((MULRK*)buf)->rk[i].value));
            cell->str=xls_getfcell(pWS->workbook,cell, NULL);
        }
        break;
    case XLS_RECORD_MULBLANK:
        for (i = 0; i < (bof->size - 6)/2; i++)	// 6 == 2 row + 2 col + 2 trailing index
        {
            cell=&row->cells.cell[xlsShortVal(((MULBLANK*)buf)->col) + i];
            cell->id=XLS_RECORD_BLANK;
            cell->xf=xlsShortVal(((MULBLANK*)buf)->xf[i]);
            cell->str=xls_getfcell(pWS->workbook,cell, NULL);
        }
        break;
    case XLS_RECORD_LABELSST:
    case XLS_RECORD_LABEL:
		cell->str=xls_getfcell(pWS->workbook,cell,(WORD_UA *)&((LABEL*)buf)->value);
		sscanf((char *)cell->str, "%d", &cell->l);
		sscanf((char *)cell->str, "%lf", &cell->d);
		break;
    case XLS_RECORD_RK:
        cell->d=NumFromRk(xlsIntVal(((RK*)buf)->value));
        cell->str=xls_getfcell(pWS->workbook,cell, NULL);
        break;
    case XLS_RECORD_BLANK:
        break;
    case XLS_RECORD_NUMBER:
        xlsConvertDouble((BYTE *)&((BR_NUMBER*)buf)->value);
		memcpy(&cell->d, &((BR_NUMBER*)buf)->value, sizeof(double)); // Required for ARM
        cell->str=xls_getfcell(pWS->workbook,cell, NULL);
        break;
    case XLS_RECORD_BOOLERR:
        cell->d = ((BOOLERR *)buf)->value;
        if (((BOOLERR *)buf)->iserror) {
            sprintf((char *)(cell->str = malloc(sizeof("error"))), "error");
        } else {
            sprintf((char *)(cell->str = malloc(sizeof("bool"))), "bool");
        }
        break;
    default:
        cell->str=xls_getfcell(pWS->workbook,cell, NULL);
        break;
    }
    if (xls_debug) xls_showCell(cell);

	return cell;
}

BYTE *xls_addFont(xlsWorkBook* pWB, FONT* font)
{
    struct st_font_data* tmp;

    verbose("xls_addFont");
    if (pWB->fonts.count==0)
    {
        pWB->fonts.font=(struct st_font_data *) malloc(sizeof(struct st_font_data));
    } else {
        pWB->fonts.font=(struct st_font_data *) realloc(pWB->fonts.font,(pWB->fonts.count+1)*sizeof(struct st_font_data));
    }

    tmp=&pWB->fonts.font[pWB->fonts.count];

    tmp->name=get_string((BYTE*)&font->name, 0, pWB->is5ver, pWB->charset);

    tmp->height=font->height;
    tmp->flag=font->flag;
    tmp->color=font->color;
    tmp->bold=font->bold;
    tmp->escapement=font->escapement;
    tmp->underline=font->underline;
    tmp->family=font->family;
    tmp->charset=font->charset;

    //	xls_showFont(tmp);
    pWB->fonts.count++;

	return tmp->name;
}

void xls_addFormat(xlsWorkBook* pWB, FORMAT* format)
{
    struct st_format_data* tmp;

    verbose("xls_addFormat");
    if (pWB->formats.count==0)
    {
        pWB->formats.format=(struct st_format_data *) malloc(sizeof(struct st_format_data));
    } else {
        pWB->formats.format=(struct st_format_data *) realloc(pWB->formats.format,(pWB->formats.count+1)*sizeof(struct st_format_data));
    }

    tmp=&pWB->formats.format[pWB->formats.count];
    tmp->index=format->index;
    tmp->value=get_string(format->value, (BYTE)!pWB->is5ver, (BYTE)pWB->is5ver, pWB->charset);
    if(xls_debug) xls_showFormat(tmp);
    pWB->formats.count++;
}

void xls_addXF8(xlsWorkBook* pWB,XF8* xf)
{
    struct st_xf_data* tmp;

    verbose("xls_addXF");
    if (pWB->xfs.count==0)
    {
        pWB->xfs.xf=(struct st_xf_data *) malloc(sizeof(struct st_xf_data));
    }
    else
    {
        pWB->xfs.xf=(struct st_xf_data *) realloc(pWB->xfs.xf,(pWB->xfs.count+1)*sizeof(struct st_xf_data));
    }

    tmp=&pWB->xfs.xf[pWB->xfs.count];

    tmp->font=xf->font;
    tmp->format=xf->format;
    tmp->type=xf->type;
    tmp->align=xf->align;
    tmp->rotation=xf->rotation;
    tmp->ident=xf->ident;
    tmp->usedattr=xf->usedattr;
    tmp->linestyle=xf->linestyle;
    tmp->linecolor=xf->linecolor;
    tmp->groundcolor=xf->groundcolor;

    //	xls_showXF(tmp);
    pWB->xfs.count++;
}
void xls_addXF5(xlsWorkBook* pWB,XF5* xf)
{
    struct st_xf_data* tmp;

    verbose("xls_addXF");
    if (pWB->xfs.count==0)
    {
        pWB->xfs.xf=(struct st_xf_data *) malloc(sizeof(struct st_xf_data));
    }
    else
    {
        pWB->xfs.xf=(struct st_xf_data *) realloc(pWB->xfs.xf,(pWB->xfs.count+1)*sizeof(struct st_xf_data));
    }

    tmp=&pWB->xfs.xf[pWB->xfs.count];

    tmp->font=xf->font;
    tmp->format=xf->format;
    tmp->type=xf->type;
    tmp->align=(BYTE)xf->align;
/*
    tmp->rotation=xf->rotation;
    tmp->ident=xf->ident;
    tmp->usedattr=xf->usedattr;
    tmp->linestyle=xf->linestyle;
    tmp->linecolor=xf->linecolor;
    tmp->groundcolor=xf->groundcolor;
*/

    //	xls_showXF(tmp);
    pWB->xfs.count++;
}

void xls_addColinfo(xlsWorkSheet* pWS,COLINFO* colinfo)
{
    struct st_colinfo_data* tmp;

    verbose("xls_addColinfo");
    if (pWS->colinfo.count==0)
    {
        pWS->colinfo.col=(struct st_colinfo_data *) malloc(sizeof(struct st_colinfo_data));
    }
    else
    {
        pWS->colinfo.col=(struct st_colinfo_data *) realloc(pWS->colinfo.col,(pWS->colinfo.count+1)*sizeof(struct st_colinfo_data));
    }

    tmp=&pWS->colinfo.col[pWS->colinfo.count];
    tmp->first=colinfo->first;
    tmp->last=colinfo->last;
    tmp->width=colinfo->width;
    tmp->xf=colinfo->xf;
    tmp->flags=colinfo->flags;

    if(xls_debug) xls_showColinfo(tmp);
    pWS->colinfo.count++;
}

void xls_mergedCells(xlsWorkSheet* pWS,BOF* bof,BYTE* buf)
{
    int count=xlsShortVal(*((WORD_UA *)buf));
    int i,c,r;
    struct MERGEDCELLS* span;
    verbose("Merged Cells");
    for (i=0;i<count;i++)
    {
        span=(struct MERGEDCELLS*)(buf+(2+i*sizeof(struct MERGEDCELLS)));
        xlsConvertMergedcells(span);
        //		printf("Merged Cells: [%i,%i] [%i,%i] \n",span->colf,span->rowf,span->coll,span->rowl);
        for (r=span->rowf;r<=span->rowl;r++)
            for (c=span->colf;c<=span->coll;c++)
                pWS->rows.row[r].cells.cell[c].isHidden=1;
        pWS->rows.row[span->rowf].cells.cell[span->colf].colspan=(span->coll-span->colf+1);
        pWS->rows.row[span->rowf].cells.cell[span->colf].rowspan=(span->rowl-span->rowf+1);
        pWS->rows.row[span->rowf].cells.cell[span->colf].isHidden=0;
    }
}

void xls_parseWorkBook(xlsWorkBook* pWB)
{
    BOF bof1;
    BOF bof2;
    BYTE* buf;
	BYTE once;

	// this to prevent compiler warnings
	once=0;
	bof2.size = 0;
	bof2.id = 0;
    verbose ("xls_parseWorkBook");
    do
    {
		if(xls_debug > 10) {
			printf("READ WORKBOOK filePos=%ld\n",  (long)pWB->filepos);
			printf("  OLE: start=%d pos=%zd size=%zd fatPos=%zu\n", pWB->olestr->start, pWB->olestr->pos, pWB->olestr->size, pWB->olestr->fatpos); 
		}

        ole2_read(&bof1, 1, 4, pWB->olestr);
        xlsConvertBof(&bof1);
 		if(xls_debug) xls_showBOF(&bof1);

        buf=(BYTE *)malloc(bof1.size);
        ole2_read(buf, 1, bof1.size, pWB->olestr);

        switch (bof1.id) {
        case XLS_RECORD_EOF:
            //verbose("EOF");
            break;
        case XLS_RECORD_BOF:	// BIFF5-8
			{
				BIFF *b = (BIFF*)buf;
                xlsConvertBiff(b);
				if (b->ver==0x600)
					pWB->is5ver=0;
				else
					pWB->is5ver=1;
				pWB->type=b->type;

				if(xls_debug) {
					printf("version: %s\n", pWB->is5ver ? "BIFF5" : "BIFF8" );
					printf("   type: %.2X\n", pWB->type);
				}
			}
            break;

        case XLS_RECORD_CODEPAGE:
            pWB->codepage=xlsShortVal(*(WORD_UA *)buf);
			if(xls_debug) printf("codepage=%x\n", pWB->codepage);
            break;

        case XLS_RECORD_CONTINUE:
			if(once) {
				if (bof2.id==XLS_RECORD_SST)
					xls_appendSST(pWB,buf,bof1.size);
				bof1=bof2;
			}
            break;

		case XLS_RECORD_WINDOW1:
			{
				WIND1 *w = (WIND1*)buf;
                xlsConvertWindow(w);
				pWB->activeSheetIdx = w->itabCur;
				if(xls_debug) {
					printf("WINDOW1: ");
					printf("xWn    : %d\n", w->xWn/20);
					printf("yWn    : %d\n", w->yWn/20);
					printf("dxWn   : %d\n", w->dxWn/20);
					printf("dyWn   : %d\n", w->dyWn/20);
					printf("grbit  : %d\n", w->grbit);
					printf("itabCur: %d\n", w->itabCur);
					printf("itabFi : %d\n", w->itabFirst);
					printf("ctabSel: %d\n", w->ctabSel);
					printf("wTabRat: %d\n", w->wTabRatio);
				}
			}
			break;

        case XLS_RECORD_SST:
			//printf("ADD SST\n");
			//if(xls_debug) dumpbuf((BYTE *)"/tmp/SST",bof1.size,buf);
            xlsConvertSst((SST *)buf);
            xls_addSST(pWB,(SST*)buf,bof1.size);
            break;

        case XLS_RECORD_EXTSST:
            //if(xls_debug > 1000) dumpbuf((BYTE *)"/tmp/EXTSST",bof1.size,buf);
            break;

        case XLS_RECORD_BOUNDSHEET:
			{
				//printf("ADD SHEET\n");
				BOUNDSHEET *bs = (BOUNDSHEET *)buf;
                xlsConvertBoundsheet(bs);
				//char *s;
				// different for BIFF5 and BIFF8
				/*s = */ xls_addSheet(pWB,bs);
			}
            break;

        case XLS_RECORD_XF:
			if(pWB->is5ver) {
				XF5 *xf;
				xf = (XF5 *)buf;
                xlsConvertXf5(xf);

				xls_addXF5(pWB,xf);
				if(xls_debug) {
					printf("   font: %d\n", xf->font);
					printf(" format: %d\n", xf->format);
					printf("   type: %.4x\n", xf->type);
					printf("  align: %.4x\n", xf->align);
					printf("rotatio: %.4x\n", xf->color);
					printf("  ident: %.4x\n", xf->fill);
					printf("usedatt: %.4x\n", xf->border);
					printf("linesty: %.4x\n", xf->linestyle);
				}
			} else {
				XF8 *xf;
				xf = (XF8 *)buf;
                xlsConvertXf8(xf);

				xls_addXF8(pWB,xf);
				if(xls_debug) {
					xls_showXF(xf);
				}
			}
            break;

        case XLS_RECORD_FONT:
        case XLS_RECORD_FONT_ALT:
			{
				BYTE *s;
				FONT *f = (FONT*)buf;
                xlsConvertFont(f);
				s = xls_addFont(pWB,f);
				if(xls_debug) {
					printf(" height: %d\n", f->height);
					printf("   flag: 0x%x\n", f->flag);
					printf("  color: 0x%x\n", f->color);
					printf(" weight: %d\n", f->bold);
					printf("escapem: 0x%x\n", f->escapement);
					printf("underln: 0x%x\n", f->underline);
					printf(" family: 0x%x\n", f->family);
					printf("charset: 0x%x\n", f->charset);
					if(s) printf("   name: %s\n", s);
				}
			}
			break;

        case XLS_RECORD_FORMAT:
            xlsConvertFormat((FORMAT *)buf);
            xls_addFormat(pWB,(FORMAT*)buf);
            break;

		case XLS_RECORD_STYLE:
			if(xls_debug) {
				struct { unsigned short idx; unsigned char ident; unsigned char lvl; } *styl;
				styl = (void *)buf;

				printf("    idx: 0x%x\n", styl->idx & 0x07FF);
				if(styl->idx & 0x8000) {
					printf("  ident: 0x%x\n", styl->ident);
					printf("  level: 0x%x\n", styl->lvl);
				} else {
					BYTE *s = get_string(&buf[2], 1, pWB->is5ver, pWB->charset);
					printf("  name=%s\n", s);
				}
			}
			break;

        case XLS_RECORD_PALETTE:
			if(xls_debug > 10) {
				unsigned char *p = buf + 2;
				int idx, len;

				len = *(WORD_UA *)buf;
				for(idx=0; idx<len; ++idx) {
					printf("   Index=0x%2.2x %2.2x%2.2x%2.2x\n", idx+8, p[0], p[1], p[2] );
					p += 4;
				}
			}
			break;

		case XLS_RECORD_1904:
			pWB->is1904 = *(BYTE *)buf;	// the field is a short, but with little endian the first byte is 0 or 1
			if(xls_debug) {
				printf("   mode: 0x%x\n", pWB->is1904);
			}
			break;
		
		case XLS_RECORD_DEFINEDNAME:
			printf("DEFINEDNAME: ");
			for(int i=0; i<bof1.size; ++i) printf("%2.2x ", buf[i]);
			printf("\n");
			break;
			
#ifdef DEBUG_DRAWINGS
		case XLS_RECORD_MSODRAWINGGROUP:
		{
			printf("DRAWING GROUP size=%d\n", bof1.size);
			unsigned int total = bof1.size;
			unsigned int off = 0;

			while(off < total) {
				struct drawHeader fooper = drawProc(buf, total, &off, 0);
				(void)fooper;
			}
			printf("Total=%d off=%d\n", total, off);
			
			if(formData) printf("%s\n", formData);
			if(formFunc) printf("%s\n", formFunc);
			free(formData), formData = NULL;
			free(formFunc), formFunc = NULL;

		}	break;
#endif
        default:
			if(xls_debug)
			{
				//xls_showBOF(&bof1);
				printf("    Not Processed in parseWoorkBook():  BOF=0x%4.4X size=%d\n", bof1.id, bof1.size);
			}
            break;
        }
		free(buf);

        bof2=bof1;
		once=1;
    }
    while ((!pWB->olestr->eof)&&(bof1.id!=XLS_RECORD_EOF));
}


void xls_preparseWorkSheet(xlsWorkSheet* pWS)
{
    BOF tmp;
    BYTE* buf;

    verbose ("xls_preparseWorkSheet");

    ole2_seek(pWS->workbook->olestr,pWS->filepos);
    do
    {
		size_t read;
        read = ole2_read(&tmp, 1,4,pWS->workbook->olestr);
		assert(read == 4);
        xlsConvertBof(&tmp);
        buf=(BYTE *)malloc(tmp.size);
        read = ole2_read(buf, 1,tmp.size,pWS->workbook->olestr);
		assert(read == tmp.size);
        switch (tmp.id)
        {
        case XLS_RECORD_DEFCOLWIDTH:
            pWS->defcolwidth=xlsShortVal(*(WORD_UA *)buf)*256;
            break;
        case XLS_RECORD_COLINFO:
            xlsConvertColinfo((COLINFO*)buf);
            xls_addColinfo(pWS,(COLINFO*)buf);
            break;
        case XLS_RECORD_ROW:
            xlsConvertRow((ROW*)buf);
            if (pWS->rows.lastcol<((ROW*)buf)->lcell)
                pWS->rows.lastcol=((ROW*)buf)->lcell;
            if (pWS->rows.lastrow<((ROW*)buf)->index)
                pWS->rows.lastrow=((ROW*)buf)->index;
            break;
        /* If the ROW record is incorrect or missing, infer the information from
         * cell data. */
        case XLS_RECORD_MULRK:
            if (pWS->rows.lastcol<xlsShortVal(((MULRK*)buf)->col) + (tmp.size - 6)/6 - 1)
                pWS->rows.lastcol=xlsShortVal(((MULRK*)buf)->col) + (tmp.size - 6)/6 - 1;
            if (pWS->rows.lastrow<xlsShortVal(((MULRK*)buf)->row))
                pWS->rows.lastrow=xlsShortVal(((MULRK*)buf)->row);
            break;
        case XLS_RECORD_MULBLANK:
            if (pWS->rows.lastcol<xlsShortVal(((MULBLANK*)buf)->col) + (tmp.size - 6)/2 - 1)
                pWS->rows.lastcol=xlsShortVal(((MULBLANK*)buf)->col) + (tmp.size - 6)/2 - 1;
            if (pWS->rows.lastrow<xlsShortVal(((MULBLANK*)buf)->row))
                pWS->rows.lastrow=xlsShortVal(((MULBLANK*)buf)->row);
            break;
        case XLS_RECORD_NUMBER:
        case XLS_RECORD_RK:
        case XLS_RECORD_LABELSST:
        case XLS_RECORD_BLANK:
        case XLS_RECORD_LABEL:
        case XLS_RECORD_FORMULA:
        case XLS_RECORD_FORMULA_ALT:
        case XLS_RECORD_BOOLERR:
            if (pWS->rows.lastcol<xlsShortVal(((COL*)buf)->col))
                pWS->rows.lastcol=xlsShortVal(((COL*)buf)->col);
            if (pWS->rows.lastrow<xlsShortVal(((COL*)buf)->row))
                pWS->rows.lastrow=xlsShortVal(((COL*)buf)->row);
            break;
        }
        free(buf);
    }
    while ((!pWS->workbook->olestr->eof)&&(tmp.id!=XLS_RECORD_EOF));
}

void xls_formatColumn(xlsWorkSheet* pWS)
{
    DWORD i,t,ii;
    DWORD fcol,lcol;

    for (i=0;i<pWS->colinfo.count;i++)
    {
        if (pWS->colinfo.col[i].first<=pWS->rows.lastcol)
            fcol=pWS->colinfo.col[i].first;
        else
            fcol=pWS->rows.lastcol;

        if (pWS->colinfo.col[i].last<=pWS->rows.lastcol)
            lcol=pWS->colinfo.col[i].last;
        else
            lcol=pWS->rows.lastcol;

        for (t=fcol;t<=lcol;t++) {
            for (ii=0;ii<=pWS->rows.lastrow;ii++)
            {
                if (pWS->colinfo.col[i].flags&1)
                    pWS->rows.row[ii].cells.cell[t].isHidden=1;
                pWS->rows.row[ii].cells.cell[t].width=pWS->colinfo.col[i].width;
            }
        }
    }
}

void xls_parseWorkSheet(xlsWorkSheet* pWS)
{
    BOF tmp;
    BYTE* buf;
	long offset = pWS->filepos;
	int continueRec = 0;

	struct st_cell_data *cell;
	xlsWorkBook *pWB = pWS->workbook;

    verbose ("xls_parseWorkSheet");

    xls_preparseWorkSheet(pWS);
	// printf("size=%d fatpos=%d)\n", pWS->workbook->olestr->size, pWS->workbook->olestr->fatpos);

    xls_makeTable(pWS);
    xls_formatColumn(pWS);

	cell = (void *)0;
    ole2_seek(pWS->workbook->olestr,pWS->filepos);
    do
    {
		long lastPos = offset;

		if(xls_debug > 10) {
			printf("LASTPOS=%ld pos=%zd filePos=%d filePos=%d\n", lastPos, pWB->olestr->pos, pWS->filepos, pWB->filepos);
		}
        ole2_read(&tmp, 1,4,pWS->workbook->olestr);
        xlsConvertBof((BOF *)&tmp);
        buf=(BYTE *)malloc(tmp.size);
        ole2_read(buf, 1,tmp.size,pWS->workbook->olestr);
		offset += 4 + tmp.size;

		if(xls_debug)
			xls_showBOF(&tmp);

        switch (tmp.id)
        {
        case XLS_RECORD_EOF:
            break;
        case XLS_RECORD_MERGEDCELLS:
            xls_mergedCells(pWS,&tmp,buf);
            break;
        case XLS_RECORD_ROW:
			if(xls_debug > 10) printf("ROW: %x at pos=%ld\n", tmp.id, lastPos);
            xlsConvertRow((ROW *)buf);
            xls_addRow(pWS,(ROW*)buf);
            break;
		case XLS_RECORD_DEFCOLWIDTH:
			if(xls_debug > 10) printf("DEFAULT COL WIDTH: %d\n", *(WORD_UA *)buf);
			break;
		case XLS_RECORD_DEFAULTROWHEIGHT:
			if(xls_debug > 10) printf("DEFAULT ROW Height: 0x%x %d\n", ((WORD_UA *)buf)[0], ((WORD_UA *)buf)[1]);
			break;
		case XLS_RECORD_DBCELL:
			if(xls_debug > 10) {
				DWORD *foo = (DWORD_UA *)buf;
                WORD *goo;
				int i;
                printf("DBCELL: size %d\n", tmp.size);
				printf("DBCELL OFFSET=%4.4u -> ROW %ld\n", foo[0], lastPos-foo[0]);
				++foo;
				goo = (WORD *)foo;
				for(i=0; i<5; ++i) printf("goo[%d]=%4.4x %u\n", i, goo[i], goo[i]);
			}
			break;
        case XLS_RECORD_INDEX:
			if(xls_debug > 10) {
				DWORD *foo = (DWORD_UA *)buf;
                int i;
				printf("INDEX: size %d\n", tmp.size);
				for(i=0; i<5; ++i) printf("FOO[%d]=%4.4x %u\n", i, foo[i], foo[i]);
			}
#if 0
			0	4 4	4 8	4
			12	4 16	4∙nm
			Not used Index to first used row (rf, 0-based) Index to first row of unused tail of sheet (rl, last used row + 1, 0-based)
			Absolute stream position of the DEFCOLWIDTH record (➜5.32) of the current sheet. If this record does not exist, the offset points to the record at the position where the DEFCOLWIDTH record would occur.
			Array of nm absolute stream positions to the DBCELL record (➜5.29) of each Row Block
#endif
            break;
        case XLS_RECORD_MULRK:
        case XLS_RECORD_MULBLANK:
        case XLS_RECORD_NUMBER:
        case XLS_RECORD_BOOLERR:
        case XLS_RECORD_RK:
        case XLS_RECORD_LABELSST:
        case XLS_RECORD_BLANK:
        case XLS_RECORD_LABEL:
        case XLS_RECORD_FORMULA:
        case XLS_RECORD_FORMULA_ALT:
            cell = xls_addCell(pWS,&tmp,buf);
            break;
		case XLS_RECORD_ARRAY:
			if(formula_handler) formula_handler(tmp.id, tmp.size, buf);
			break;

		case XLS_RECORD_STRING:
			if(cell && (cell->id == XLS_RECORD_FORMULA || cell->id == XLS_RECORD_FORMULA_ALT)) {
				cell->str = get_string(buf, (BYTE)!pWB->is5ver, pWB->is5ver, pWB->charset);
				if (xls_debug) xls_showCell(cell);
			}
			break;
#if 0 // debugging
		case XLS_RECORD_HYPERREF:
			if(xls_debug) {
				printf("HYPERREF: ");
				unsigned char xx, *foo = (void *)buf;

				for(xx=0; xx<tmp.size; ++xx, ++foo) {
					printf("%2.2x ", *foo);
				}
				printf("\n");
			}
			break;
		case XLS_RECORD_WINDOW2:
			if(xls_debug) {
				printf("WINDOW2: ");
				unsigned short xx, *foo = (void *)buf;

				for(xx=0; xx<7; ++xx, ++foo) {
					printf("0x%4.4x ", *foo);
				}
				printf("\n");
			}
			break;
#endif

#ifdef DEBUG_DRAWINGS
#ifdef AIX
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif
		case XLS_RECORD_MSODRAWING:	// MSDRAWING
		{
			printf("DRAWING size=%d\n", tmp.size);
			sheetOffset = 100;
			unsigned int total = tmp.size;
			unsigned int off = 0;
			
			while(off < total) {
				struct drawHeader fooper  = drawProc(buf, total, &off, 0);
				(void)fooper;
				printf("---------------Total=%d off=%d\n", total, off);
			}

			if(formData) printf("%s\n", formData);
			if(formFunc) printf("%s\n", formFunc);
			free(formData), formData = NULL;
			free(formFunc), formFunc = NULL;
			
		}	break;
		
		case XLS_RECORD_TXO:
		{
			struct {
				uint16_t	grbit;
				uint16_t	rot;
				char		reserved1[6];
				uint16_t	cchText;
				uint16_t	cbRuns;
				uint16_t	ifntEmpty;
				uint16_t	reserved2;
			} foo;
			memcpy(&foo, buf, 18);
			printf("TXO: grbit=0x%4.4X rot=0x%4.4X chText=0x%4.4X cbRuns=0x%4.4X ifntEmpty=0x%X reserved2=0x%X\n", foo.grbit, foo.rot, foo.cchText, foo.cbRuns, foo.ifntEmpty, foo.reserved2);
			
			printf("Res1: ");
			for(int i=0; i<6; ++i) printf("%2.2x ", foo.reserved1[i]);
			printf("\n");
			
			continueRec = 1;
			goto printBOF;
		}	break;
		
		case XLS_RECORD_CONTINUE:
		{
			if(continueRec == 1) {
				continueRec = 2;
				
				printf("TEXT: ");
				for(int i=0; i<tmp.size; ++i) printf("%2.2x ", buf[i]);
				printf("\n");
				printf("\"%.*s\"\n", tmp.size-1, buf+1);
			} else
			if(continueRec == 2) {
				continueRec = 0;
				int off = 0;

				struct {
					uint16_t	ichFirst;
					uint16_t	ifnt;
					char		reserved[4];
				} foo;
				
				for(int i=0; i<tmp.size/8; ++i) {
					memcpy(&foo, buf+off, 8);
					printf("TXORUN: %d 0x%x\n", foo.ichFirst, foo.ifnt);
					off += 8;
				}
			}
			goto printBOF;
		} break;
		
		case XLS_RECORD_OBJ:
			xls_showBOF(&tmp);
		{
			struct  {
				uint16_t	ft;
				uint16_t	cb;
				uint16_t	ot;
				uint16_t	idx;
				uint16_t	flags;
				uint16_t	unused[6];
			} foo;
			memcpy(&foo, buf, sizeof(foo));
			
			int len = (int)(tmp.size - sizeof(foo));
			int off = sizeof(foo);
			
			printf("OBJ ft=0x%X cb=0x%X ot=0x%X idx=0x%X flags=0x%X len=%d ", foo.ft, foo.cb, foo.ot, foo.idx, foo.flags, (int)(tmp.size - sizeof(foo)) );
			//for(int i=0; i<6; ++i) printf(" 0x%02.2x", foo.unused[i]);
			printf("\n");
			
			if(foo.ot == 0x08) {
				struct {
					uint16_t ft;
					uint16_t cb;
					uint16_t flags;
				} ftcf;
				memcpy(&ftcf, buf+off, sizeof(ftcf));
				printf(" ft=%x cb=%x flags=%4.4x\n", ftcf.ft, ftcf.cb, ftcf.flags);
				off += sizeof(ftcf);

				struct {
					uint16_t ft;
					uint16_t cb;
					uint16_t flags;
				} FtPioGrbit;
				memcpy(&FtPioGrbit, buf+off, sizeof(FtPioGrbit));
				printf(" ft=%x cb=%x flags=%4.4x\n", FtPioGrbit.ft, FtPioGrbit.cb, FtPioGrbit.flags);
				off += sizeof(FtPioGrbit);
			} else {
				printf("Extra: ");
				for(int i=0; i<len; ++i) printf("%2.2x ", buf[i+off]);
				printf("\n");
			}

#if 0
			struct {
				uint16_t	ft;
				uint16_t	cb;
				uint8_t		guid[16];
				uint16_t	fSharedNote;
				uint32_t	unused;
			} FtNts;
			memcpy(&FtNts, buf+off, sizeof(FtNts));
			off += sizeof(FtNts);
			printf("  ft=%X cb=%X fSharedNote=0x%X guid: ", FtNts.ft, FtNts.cb, FtNts.fSharedNote);
			for(int i=0; i<16; ++i) printf("%2.2x ", FtNts.guid[i]);
			printf("\n");
			
			uint32_t last;
			memcpy(&last, buf+off, 4);
			printf("  LAST 0x%8.8X off=%d s1=%ld s2=%ld\n", last, off+4, sizeof(foo), sizeof(FtNts) );
#endif
			goto printBOF;
		}	break;
		
		case XLS_RECORD_NOTE:
		{
			struct {
				uint16_t	row;
				uint16_t	col;
				uint16_t	flags;
				uint16_t	idx;
				uint16_t	strLen;
				uint8_t		strType;
			} note;
			memcpy(&note, buf, sizeof(note));
			printf("NOTE: row=%d col=%d flags=0x%x idx=%d strLen=%d strType=%d :  ", note.row, note.col, note.flags, note.idx, note.strLen, note.strType);
			for(int i=0; i<note.strLen; ++i) printf("%2.2x ", buf[i+sizeof(note)]);
			printf("\n  %.*s now at %ld len=%d\n", note.strLen, buf + sizeof(note), sizeof(note)+note.strLen, tmp.size);

			goto printBOF;
		}	break;
#pragma pack(pop)
#endif

        default:
		  printBOF:
			if(xls_debug)
			{
				//xls_showBOF(&tmp);
				printf("   [%d:%d]: 0x%X at pos=%lu size=%u\n", xlsShortVal(((COL*)buf)->row), xlsShortVal(((COL*)buf)->col), tmp.id, lastPos, tmp.size);
			}
            break;
        }
        free(buf);
    }
    while ((!pWS->workbook->olestr->eof)&&(tmp.id!=XLS_RECORD_EOF));
}

xlsWorkSheet * xls_getWorkSheet(xlsWorkBook* pWB,int num)
{
    xlsWorkSheet * pWS;
    verbose ("xls_getWorkSheet");
    pWS=(xlsWorkSheet *)calloc(1, sizeof(xlsWorkSheet));
    pWS->filepos=pWB->sheets.sheet[num].filepos;
    pWS->workbook=pWB;
    pWS->rows.lastcol=0;
    pWS->rows.lastrow=0;
    pWS->colinfo.count=0;
    return(pWS);
}

xlsWorkBook* xls_open(const char *file,const char* charset)
{
    xlsWorkBook* pWB;
    OLE2*		ole;

    pWB=(xlsWorkBook*)calloc(1, sizeof(xlsWorkBook));
    verbose ("xls_open");

    // open excel file
    if (!(ole=ole2_open((BYTE *)file)))
    {
        if(xls_debug) printf("File \"%s\" not found\n",file);
		free(pWB);
        return(NULL);
    }

    if ((pWB->olestr=ole2_fopen(ole, (BYTE *)"\005SummaryInformation")))
    {
        pWB->summary = calloc(1,4096);
		ole2_read(pWB->summary, 4096, 1, pWB->olestr);
		ole2_fclose(pWB->olestr);
	}

    if ((pWB->olestr=ole2_fopen(ole, (BYTE *)"\005DocumentSummaryInformation")))
    {
        pWB->docSummary = calloc(1,4096);
		ole2_read(pWB->docSummary, 4096, 1, pWB->olestr);
		ole2_fclose(pWB->olestr);
	}

#if 0
	if(xls_debug) {
		printf("summary=%d docsummary=%d\n", pWB->summary ? 1 : 0, pWB->docSummary ? 1 : 0);
		xlsSummaryInfo *si = xls_summaryInfo(pWB);
		printf("title=%s\n", si->title);
		printf("subject=%s\n", si->subject);
		printf("author=%s\n", si->author);
		printf("keywords=%s\n", si->keywords);
		printf("comment=%s\n", si->comment);
		printf("lastAuthor=%s\n", si->lastAuthor);
		printf("appName=%s\n", si->appName);
		printf("category=%s\n", si->category);
		printf("manager=%s\n", si->manager);
		printf("company=%s\n", si->company);
	}
#endif

    // open Workbook
    if (!(pWB->olestr=ole2_fopen(ole,(BYTE *)"Workbook")) && !(pWB->olestr=ole2_fopen(ole,(BYTE *)"Book")))
    {
        if(xls_debug) printf("Workbook not found\n");
        ole2_close(ole);
		free(pWB);
        return(NULL);
    }


    pWB->sheets.count=0;
    pWB->xfs.count=0;
    pWB->fonts.count=0;
    pWB->charset = (char *)malloc(strlen(charset) * sizeof(char)+1);
    strcpy(pWB->charset, charset);
    xls_parseWorkBook(pWB);

    return(pWB);
}

xlsRow *xls_row(xlsWorkSheet* pWS, WORD cellRow)
{
    struct st_row_data *row;

    if(cellRow > pWS->rows.lastrow) return NULL;
    row = &pWS->rows.row[cellRow];

    return row;
}

xlsCell	*xls_cell(xlsWorkSheet* pWS, WORD cellRow, WORD cellCol)
{
    struct st_row_data	*row;

    if(cellRow > pWS->rows.lastrow) return NULL;
    row = &pWS->rows.row[cellRow];
    if(cellCol >= row->lcell) return NULL;

    return &row->cells.cell[cellCol];
}

void xls_close_WB(xlsWorkBook* pWB)
{
	OLE2*		ole;

	verbose ("xls_close");

	if(!pWB) return;

    // OLE first
	ole=pWB->olestr->ole;
	
	ole2_fclose(pWB->olestr);

	ole2_close(ole);

    // WorkBook
    free(pWB->charset);

    // Sheets
    {
        DWORD i;
        for(i=0; i<pWB->sheets.count; ++i) {
            free(pWB->sheets.sheet[i].name);
        }
        free(pWB->sheets.sheet);
    }

    // SST
    {
        DWORD i;
        for(i=0; i<pWB->sst.count; ++i) {
            free(pWB->sst.string[i].str);
        }
        free(pWB->sst.string);
    }

    // xfs
    {
        free(pWB->xfs.xf);
    }

    // fonts
    {
        DWORD i;
        for(i=0; i<pWB->fonts.count; ++i) {
            free(pWB->fonts.font[i].name);
        }
        free(pWB->fonts.font);
    }

    // formats
    {
        DWORD i;
        for(i=0; i<pWB->formats.count; ++i) {
            free(pWB->formats.format[i].value);
        }
        free(pWB->formats.format);
    }

    // buffers
	if(pWB->summary)  free(pWB->summary);
	if(pWB->docSummary) free(pWB->docSummary);

	// TODO - free other dynamically allocated objects like string table??
	free(pWB);
}

void xls_close_WS(xlsWorkSheet* pWS)
{
	if(!pWS) return;

    // ROWS
    {
        DWORD i, j;
        for(j=0; j<=pWS->rows.lastrow; ++j) {
            struct st_row_data *row = &pWS->rows.row[j];
            for(i=0; i<row->cells.count; ++i) {
                free(row->cells.cell[i].str);
            }
            free(row->cells.cell);
        }
        free(pWS->rows.row);

    }

    // COLINFO
    {
        free(pWS->colinfo.col);
    }
    free(pWS);
}

const char* xls_getVersion(void)
{
    return PACKAGE_VERSION;
}

//
// http://poi.apache.org/hpsf/internals.html
// or google "DocumentSummaryInformation and UserDefined Property Sets" and look for MSDN hits
//

xlsSummaryInfo *xls_summaryInfo(xlsWorkBook* pWB)
{
	xlsSummaryInfo	*pSI;

	pSI = (xlsSummaryInfo *)calloc(1, sizeof(xlsSummaryInfo));
	xls_dumpSummary(pWB->summary, 1, pSI);
	xls_dumpSummary(pWB->docSummary, 0, pSI);

	return pSI;
}

void xls_close_summaryInfo(xlsSummaryInfo *pSI)
{
	if(!pSI) return;

	if(pSI->title)		free(pSI->title);
	if(pSI->subject)	free(pSI->subject);
	if(pSI->author)		free(pSI->author);
	if(pSI->keywords)	free(pSI->keywords);
	if(pSI->comment)	free(pSI->comment);
	if(pSI->lastAuthor)	free(pSI->lastAuthor);
	if(pSI->appName)	free(pSI->appName);
	if(pSI->category)	free(pSI->category);
	if(pSI->manager)	free(pSI->manager);
	if(pSI->company)	free(pSI->company);

	free(pSI);
}

void xls_dumpSummary(char *buf,int isSummary,xlsSummaryInfo *pSI) {
	header			*head;
	sectionList		*secList;
	propertyList	*plist;
	sectionHeader	*secHead;
	property		*prop;
	uint32_t i, j;

	if(!buf) return;	// perhaps the document was missing??

	head = (header *)buf;
	//printf("header: \n");
	//printf("  sig=%x\n", head->sig);
	//printf("  os=%x\n", head->os >> 16);
	//printf("  class=%8.8x%8.8x%8.8x%8.8x\n", head->format[0], head->format[1], head->format[2], head->format[3]);
	//printf("  count=%x\n", head->count);

	for(i=0; i<head->count; ++i) {
		secList = &head->secList[i];
		//printf("Section %d:\n", i);
		//printf("  class=%8.8x%8.8x%8.8x%8.8x\n", secList->format[0], secList->format[1], secList->format[2], secList->format[3]);
		//printf("  offset=%d (now at %ld\n", secList->offset, (char *)secList - (char *)buf + sizeof(sectionList));


		secHead = (sectionHeader *)((char *)head + secList->offset);
		//printf("  len=%d\n", secHead->length);
		//printf("  properties=%d\n", secHead->numProperties);
		for(j=0; j<secHead->numProperties; ++j) {
			BYTE **s;

			plist = &secHead->properties[j];
			//printf("      ---------\n");
			//printf("      propID=%d offset=%d\n", plist->propertyID, plist->sectionOffset);
			prop = (property *)((char *)secHead + plist->sectionOffset);
			//printf("      propType=%d\n", prop->propertyID);

			switch(prop->propertyID) {
			case 2:
				//printf("      xlsShortVal=%x\n", *(uint16_t *)prop->data);
				break;
			case 3:
				//printf("      wordVal=%x\n", *(uint32_t *)prop->data);
				break;
			case 30:
				//printf("      longVal=%llx\n", *(uint64_t *)prop->data);
				//printf("      s[%u]=%s\n", *(uint32_t  *)prop->data, (char *)prop->data + 4);
				if(isSummary) {
					switch(plist->propertyID) {
					case 2:		s = &pSI->title;		break;
					case 3:		s = &pSI->subject;		break;
					case 4:		s = &pSI->author;		break;
					case 5:		s = &pSI->keywords;		break;
					case 6:		s = &pSI->comment;		break;
					case 8:		s = &pSI->lastAuthor;	break;
					case 18:	s = &pSI->appName;		break;
					default:	s = NULL;				break;
					}
				} else {
					switch(plist->propertyID) {
					case 2:		s = &pSI->category;		break;
					case 14:	s = &pSI->manager;		break;
					case 15:	s = &pSI->company;		break;
					default:	s = NULL;				break;
					}
				}
				if(s) *s = (BYTE *)strdup((char *)prop->data + 4);
				break;
			case 64:
				//printf("      longVal=%llx\n", *(uint64_t *)prop->data);
				break;
			case 65:
#if 0
				{
				uint32_t k;
				for(k=0; k<*(uint32_t  *)prop->data; ++k) {
				unsigned char *t = (unsigned char *)prop->data + 4 + k;
				printf(" %2.2x(%c)", *t, *t);
				}
				printf("\n");
				}
#endif
				break;
			default:
				//printf("      UNKNOWN!\n");
				break;
			}
		}
	}
}

void xls_set_formula_hander(xls_formula_handler handler)
{
	formula_handler = handler;
}

#ifdef DEBUG_DRAWINGS

#ifdef AIX
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

static char spaces[] = "                                                                                                                ";

static struct drawHeader drawProc(uint8_t *buf, uint32_t maxLen, uint32_t *off_p, int level)
{
	struct drawHeader head = { 0, 0, 0, 0 };
	uint32_t off = off_p ? *off_p : 0;
	memcpy(&head, buf+off, sizeof(head));
#if 0	// rec is the lower 4 bits
	{
		uint16_t foo0, foo1;
		uint32_t foo2;
		memcpy(&foo0, buf+off, 2);
		memcpy(&foo1, buf+off+2, 2);
		memcpy(&foo2, buf+off+4, 4);
		printf("-----------------------------[%4.4x %4.4x %x] rec=%x instance=%x type=%x len=%x\n", foo0, foo1, foo2, head.rec, head.instance,  head.type, head.len);
	}
#endif
	off += sizeof(head);

	printf("%.*s", level*3, spaces);
	printf("type=%x rec=%x instance=%x len=%d    ", head.type, head.rec, head.instance, head.len);
	
	switch(head.type) {
	case 0xF000:	// OfficeArtDggContainer - F000 - overall header
	{
		printf("OfficeArtDggContainer\n");
		dumpRec("OfficeArtDggContainer", &head, 0, NULL);

		int startOff = off;
		while( (off - startOff) < head.len && off < maxLen) {
			struct drawHeader fooper2 = drawProc(buf, maxLen, &off, level+1);
			(void)fooper2;
		}
		
		printf("%.*s", level*3, spaces);
		printf("Total=%d off=%d ObjectSize=%d\n", maxLen, off, off-startOff);
		
	}	break;

#if 0
	DRAWING 0xf002 208
	  rec=0 instance=1 type=f008 len=8
	  csp=4 spidCur=1027
	rec=f instance=0 type=f003 len=462
	Total=208 off=486
	OBJ id=1 ot=0x19 flags=0x4011 check=0x0 len=30
	  ft=D cb=16 fSharedNote=0x0 guid: 8e 2e 69 ed f2 7d e3 11 99 7f 00 16 cb 93 e7 b5 
	  LAST 0x00000000
	type=f00d rec=0 instance=0 len=0    WTF ?!?!?!

#endif
	case 0xF002:
	{
		printf("OfficeArtDgContainer\n");
		dumpRec("OfficeArtDgContainer", &head, 0, NULL);

		int startOff = off;
		while( (off - startOff) < head.len && off < maxLen) {
			struct drawHeader fooper2 = drawProc(buf, maxLen, &off, level+1);
			(void)fooper2;
		}
		
		printf("%.*s", level*3, spaces);
		printf("Total=%d off=%d ObjectSize=%d\n", maxLen, off, off-startOff);

	}	break;

	case 0xF003:
	{
		printf("OfficeArtSpgrContainer\n");
		dumpRec("OfficeArtSpgrContainer", &head, 0, NULL);

		int startOff = off;
		while( (off - startOff) < head.len && off < maxLen) {
			struct drawHeader fooper2 = drawProc(buf, maxLen, &off, level+1);
			(void)fooper2;
		}
		
		printf("%.*s", level*3, spaces);
		printf("Total=%d off=%d ObjectSize=%d  FIXME FIXME FIXME\n", maxLen, off, off-startOff);
	}	break;

	case 0xF001:
	{
		printf("OfficeArtBStoreContainer\n");
		dumpRec("OfficeArtBStoreContainer", &head, 0, NULL);

		int startOff = off;
		while( (off - startOff) < head.len && off < maxLen) {
			struct drawHeader fooper2 = drawProc(buf, maxLen, &off, level+1);
			(void)fooper2;
		}
		
		printf("%.*s", level*3, spaces);
		printf("Total=%d off=%d ObjectSize=%d\n", maxLen, off, off-startOff);
		}	break;
	case 0xF004:
	{
		printf("OfficeArtSpContainer\n");
		dumpRec("OfficeArtSpContainer", &head, 0, NULL);

		int startOff = off;
		while( (off - startOff) < head.len && off < maxLen) {
			struct drawHeader fooper2 = drawProc(buf, maxLen, &off, level+1);
			(void)fooper2;
		}
		
		printf("%.*s", level*3, spaces);
		printf("Total=%d off=%d ObjectSize=%d\n", maxLen, off, off-startOff);
		}	break;
	case 0xF006:
	{
		// A value that MUST be 0x00000010 + ((head.cidcl - 1) * 0x00000008)
		unsigned int count =  (head.len - 0x10) / 0x8;
		printf("OfficeArtFDGGBlock count=%d\n", count);
		dumpRec("OfficeArtFDGGBlock - needs to be set", &head, 0, NULL);

		// OfficeArtFDGG
		struct {
			uint32_t spidMax;
			uint32_t cidcl;
			uint32_t cspSaved;
			uint32_t cdgSaved;
		} fog;
		memcpy(&fog, buf+off, 16); // OfficeArtRecordHeader F001 - specified BLIP - this is the image
		off += 16;
		printf("%.*s", level*3, spaces);
		printf(" spidMax=%d cidcl=%d cspSaved=%d cdgSaved=%d\n", fog.spidMax, fog.cidcl, fog.cspSaved, fog.cdgSaved);
#if 0
		spidMax (4 bytes): An MSOSPID structure, as defined in section 2.1.2, specifying the current maximum shape identifier that is used in any drawing. This value MUST be less than 0x03FFD7FF.
		cidcl (4 bytes): An unsigned integer that specifies the number of OfficeArtIDCL records, as defined in section 2.2.46, + 1. This value MUST be less than 0x0FFFFFFF.
		cspSaved (4 bytes): An unsigned integer specifying the total number of shapes that have been saved in all of the drawings.
		cdgSaved (4 bytes): An unsigned integer specifying the total number of drawings that have been saved in the file.
#endif
		// OfficeArtIDCL - clusters
		for(int i=0; i<count; ++i) {
			struct {
				uint32_t dgid;
				uint32_t cspidCur;
#if 0
				dgid (4 bytes): An MSODGID structure, as defined in section 2.1.1, specifying the drawing identifier that owns this identifier cluster.
				cspidCur (4 bytes): An unsigned integer that, if less than 0x00000400, specifies the largest shape identifier that is currently assigned in this cluster, or that otherwise specifies that no shapes can be added to the drawing.
#endif
			} foo1;
			memcpy(&foo1, buf+off, 8); // OfficeArtIDCL
			off += 8;
			
			printf("%.*s", level*3, spaces);
			printf("  dgid=%d cspid=%d\n", foo1.dgid, foo1.cspidCur);
		}
		//for(int i=0; i<16; ++i) printf(" %2.2x", *(BYTE *)(buf+off+i));
		//printf("\n");
	}	break;

	case 0xF007:
	{
		printf("OfficeArtFBSE\n");
		//dumpRec("OfficeArtFBSE", &head, 0, NULL);
		struct {
			uint8_t		btWin32;
			uint8_t		btMacOS;
			uint8_t		rgbUid[16];
			uint16_t	tag;
			uint32_t	size;
			uint32_t	cRef;
			uint32_t	foDelay;
			uint8_t		unused1;
			uint8_t		cbName;
			uint8_t		unused2;
			uint8_t		unused3;
		} fooper1;
		memcpy(&fooper1, buf+off, sizeof(fooper1));
		off += sizeof(fooper1);
		printf("%.*s", level*3, spaces);
		printf(" rgbUid: ");
		for(int i=0; i<16; ++i) {
			printf(" %2.2x", fooper1.rgbUid[i]);
		}
		printf("\n");
		
		printf("%.*s", level*3, spaces);
		printf(" w=%d mac=%d tag=0x%x size=%d cRef=%d foDelay=%x cbName=%x", fooper1.btWin32, fooper1.btMacOS, fooper1.tag , fooper1.size , fooper1.cRef , fooper1.foDelay, fooper1.cbName);
		if(fooper1.cbName) printf("name:");
		for(int i=0; i<fooper1.cbName; ++i) {
			printf(" %2.2x", *(BYTE *)(buf+off+i));
		}
		printf("\n");
		off += fooper1.cbName;
		
		printf(" dataLen=%ld\n", head.len - sizeof(fooper1) - fooper1.cbName);
		drawProc(buf, maxLen, &off, level+1);

		
	}	break;


#if 0
	rgbUid (16 bytes): An MD4 message digest, as specified in [RFC1320], that specifies the unique identifier of the pixel data in the BLIP.
	tag (2 bytes): An unsigned integer that specifies an application-defined internal resource tag. This value MUST be 0xFF for external files.
	size (4 bytes): An unsigned integer that specifies the size, in bytes, of the BLIP in the stream.
	cRef (4 bytes): An unsigned integer that specifies the number of references to the BLIP. A value of 0x00000000 specifies an empty slot in the OfficeArtBStoreContainer record, as defined in section 2.2.20.
	foDelay (4 bytes): An MSOFO structure, as defined in section 2.1.4, that specifies the file offset into the associated OfficeArtBStoreDelay record, as defined in section 2.2.21, (delay stream). A value of 0xFFFFFFFF specifies that the file is not in the delay stream, and in this case, cRef MUST be 0x00000000.
	unused1 (1 byte): A value that is undefined and MUST be ignored.
	cbName (1 byte): An unsigned integer that specifies the length, in bytes, of the nameData field, including the terminating NULL character. This value MUST be an even number and less than or equal to 0xFE. If the value is 0x00, nameData will not be written.
	unused2 (1 byte): A value that is undefined and MUST be ignored.
	unused3 (1 byte): A value that is undefined and MUST be ignored.
	nameData (variable): A Unicode null-terminated string that specifies the name of the BLIP.
	embeddedBlip (variable): An OfficeArtBlip record, as defined in section 2.2.23, specifying the BLIP file data that is embedded in this record. If this value is not 0, foDelay MUST be ignored.
#endif

	case 0xF008:
	{
		printf("OfficeArtFDG\n");
		dumpRec("OfficeArtFDG - spidCur needs to be set", &head, 0, NULL);
		struct {
			uint32_t	csp;
			uint32_t	spidCur;
		} fooper1;
		memcpy(&fooper1, buf+off, 8);
		off += 8;
		printf("%.*s", level*3, spaces);
		printf(" csp=%d spidCur=%d\n", fooper1.csp, fooper1.spidCur);
		
#if 0
		csp (4 bytes): An unsigned integer that specifies the number of shapes in this drawing.
		spidCur (4 bytes): An MSOSPID structure, as defined in section 2.1.2, that specifies the shape
		identifier of the last shape in this drawing.
#endif
		
	}	break;

	case 0xF009:
	{
		printf("OfficeArtFSPGR\n");
		dumpRec("OfficeArtFSPGR", &head, head.len, buf+off);
		struct {
			int32_t	xLeft;
			int32_t	yTop;
			int32_t	xRight;
			int32_t	yBottom;
		} foo;
		memcpy(&foo, buf+off, 16);
		off += 16;
		printf("%.*s", level*3, spaces);
		printf(" l=%d t=%d r=%d b=%d\n", foo.xLeft, foo.yTop, foo.xRight, foo.yBottom);
	}	break;

	case 0xF00A:	// OfficeArtFSP
	{
		printf("OfficeArtFSP\n");
		dumpRec("OfficeArtFSP", &head, 0, NULL);
		struct {
			uint32_t	spid;
			uint32_t	flags;
		} foo;
		memcpy(&foo, buf+off, 8);
		off += 8;
		printf("%.*s", level*3, spaces);
		printf(" SPID=%d flags=0x%x\n", foo.spid, foo.flags);
	}	break;

	case 0xF00B:
	{
		printf("OfficeArtFOPT\n");
		dumpRec("OfficeArtFOPT", &head, head.len, buf+off);
		struct {
			//uint16_t blip : 1;
			//uint16_t complex : 1;
			uint16_t opid; // : 14
			uint32_t op;
		} foo;
		
		// OfficeArtFOPTE + complex
		for(int i=0; i<head.instance; ++i) {
			memcpy(&foo, buf+off, 6);
			off += 6;
			printf("%.*s", level*3, spaces);
			printf(" opid=0x%4.4X(%.5d) op=%8.8X\n", foo.opid, foo.opid, foo.op); // blip=%d complex=%d , foo.blip, foo.complex
			//printf("drawDataOPID(data, 0x%4.4X, 0x%8.8X);\n", foo.opid, foo.op);
		}
#if 0
          opid=80 op=000018AC(6316) Text ID
          opid=bf op=0000000A(10) fFitTextToShape
          opid=158 op=00000000(0) // Type of connection sites
          opid=181 op=00000800(2048) // fillColor
          opid=183 op=00000800(2048) // fillBackColor
          opid=1bf op=00000010(16) // hit test
          opid=23f op=00000003(3) // fshadowObscured
#endif
		int complex = head.len - head.instance * 6;
		if(complex) {
			printf("%.*s", level*3, spaces);
			printf(" complex:");
		
			for(int i=0; i<complex; ++i) {
				printf(" %2.2x", *(BYTE *)(buf+off+i));
			}
			printf("\n");
		}
		off += complex;
	}	break;

	case 0xF00D:
		printf("msofbtClientTextbox\n");
		dumpRec("msofbtClientTextbox", &head, head.len, buf+off);
		break;

	case 0xF010:
		printf("msofbtClientAnchor: ");
		dumpRec("msofbtClientAnchor", &head, head.len, buf+off);
		// https://code.google.com/p/excellibrary/source/browse/trunk/src/ExcelLibrary/Office/Excel/EscherRecords/MsofbtClientAnchor.cs?spec=svn18&r=18
		struct {
			uint16_t	Flag;
			uint16_t	Col1;
			uint16_t	DX1;
			uint16_t	Row1;
			uint16_t	DY1;
			uint16_t	Col2;
			uint16_t	DX2;
			uint16_t	Row2;
			uint16_t	DY2;
		} foo;
		memcpy(&foo, buf+off, 18);
		printf(" Flag=0x%2.2x Col1=0x%2.2x DX1=0x%2.2x Row1=0x%2.2x DY1=0x%2.2x Col2=0x%2.2x DX2=0x%2.2x Row2=0x%2.2x DY2=0x%2.2x \n",
			foo.Flag, foo.Col1, foo.DX1, foo.Row1, foo.DY1, foo.Col2, foo.DX2, foo.Row2, foo.DY2);
		off += head.len;
		break;
	case 0xF011:
		printf("msofbtClientData\n");
		dumpRec("msofbtClientData", &head, head.len, buf+off);
		off += head.len;
		break;

	case 0xF01E:
	{	printf("OfficeArtBlipPNG\n");
		dumpRec("OfficeArtBlipPNG", &head, head.len, buf+off);
		struct {
			uint8_t		rgbUid1[16];
			uint16_t	tag;
		} foo;
		memcpy(&foo, buf+off, sizeof(foo));
		
		//off += sizeof(foo);
		printf("%.*s", level*3, spaces);
		printf(" rgbUid1: ");
		for(int i=0; i<16; ++i) {
			printf(" %2.2x", foo.rgbUid1[i]);
		}
		printf("\n");
		off += head.len;
	}	break;

	case 0xF11E:
		printf("OfficeArtSplitMenuColorContainer: Array of colors\n");
		dumpRec("OfficeArtSplitMenuColorContainer", &head, head.len, buf+off);
		off += head.len;
		break;

	case 0xF122:
	{	printf("OfficeArtTertiaryFOPT\n");
		struct {
			//uint16_t blip : 1;
			//uint16_t complex : 1;
			uint16_t opid; // : 14
			uint16_t op1;
			uint16_t op2;
		} foo;
		int count = head.len/6;
		printf("OfficeArtFOPT count=%d\n", count);
		dumpRec("OfficeArtTertiaryFOPT", &head, head.len, buf+off);

		// OfficeArtFOPTE + complex
		for(int i=0; i<head.instance; ++i) {
			memcpy(&foo, buf+off, 6);
			off += 6;
			printf("%.*s", level*3, spaces);
			printf(" opid=0x%4.4X(%.5d) op1=%4.4X op1=%4.4X\n", foo.opid, foo.opid, foo.op1, foo.op2); // blip=%d complex=%d , foo.blip, foo.complex
		}
		//off += head.len;
	}	break;

	default:
		printf("WTF ?!?!?!\n");
		//assert(!"Not Possible");
		off += head.len;
		break;
	}

	*off_p = off;
	return head;
}

static void dumpData(char *data);
static void dumpFunc(char *func);

static void dumpRec(char *comment, struct drawHeader *h, int len, uint8_t *buf)
{
	int width = 0;
	static int num;
	char *tmp;
	
return;

	if(len) {
		++num;
		//asprintf(&tmp, "// %s real len = %d\n", comment, h->len);
		asprintf(&tmp, "static unsigned char draw%03.3d[%d] = { ", num+sheetOffset, len);
		width = strlen(tmp);
		dumpData(tmp);
		
		for(int i=0; i<len; ++i) {
			asprintf(&tmp, "0x%02.2X, ", buf[i]);
			width += strlen(tmp);
			dumpData(tmp);
			
			if(width >= 79) {
				dumpData(strdup("\n    "));
				width = 4;
			}
		}
		dumpData(strdup("\n  };\n"));
	}
	
	char name[32];
	if(len) {
		sprintf(name, "draw%03.3d", num+sheetOffset);
	} else {
		strcpy(name, "NULL");
	}

	asprintf(&tmp, "dumpDrawData(data,  0x%X, 0x%X, 0x%X, %u,  %d, %s /* len=%d */ ) ;  // %s\n", h->rec, h->instance, h->type, h->len, len, name, len, comment);
	dumpFunc(tmp);
}


static void dumpData(char *data)
{
	if(!formData) {
		formData = calloc(1,1);
	}
	
	char *oldStr = formData;
	asprintf(&formData, "%s%s", oldStr, data);
	free(oldStr);
	free(data);
}
static void dumpFunc(char *func)
{
	if(!formFunc) {
		formFunc = calloc(1,1);
	}
	
	char *oldStr = formFunc;
	asprintf(&formFunc, "%s%s", oldStr, func);
	free(oldStr);
}

#if 0
static int finder(uint8_t *buf, uint32_t len, uint16_t pattern)
{
	int ret = 0;
	uint8_t b1 = pattern & 0xFF;
	uint8_t b2 = pattern >> 8;
	
	for(int i=0; i<(len-1); ++i) {
		if(buf[i] == b1 && buf[i+1] == b2) {
			printf("GOT FINDER HIT OFFSET %d\n", i);
			ret = 1;
		}
	}
	return ret;
}

// MD4 open source: http://openwall.info/wiki/people/solar/software/public-domain-source-code/md4 or
//                  http://www.opensource.apple.com/source/freeradius/freeradius-11/freeradius/src/lib/md4.c
// Size of TWIPS:   http://support.microsoft.com/kb/76388
#endif

#pragma pack(pop)

#endif





