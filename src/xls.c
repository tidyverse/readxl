/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * This file is part of libxls -- A multiplatform, C library
 * for parsing Excel(TM) files.
 *
 * libxls is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libxls is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libxls.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Copyright 2004 Komarov Valery
 * Copyright 2006 Christophe Leitienne
 * Copyright 2008 David Hoerl
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <memory.h>
#include <math.h>
#include <sys/types.h>
#include <string.h>
#include <wchar.h>

#include <libxls/xls.h>

#define min(a,b) ((a) < (b) ? (a) : (b))

int xls_debug=0;	// now global, so users can turn it on

static double NumFromRk(BYTE* rk);
extern char* xls_addSheet(xlsWorkBook* pWB,BOUNDSHEET* bs);
extern void xls_addRow(xlsWorkSheet* pWS,ROW* row);
extern void xls_makeTable(xlsWorkSheet* pWS);
extern void xls_addCell(xlsWorkSheet* pWS,BOF* bof,BYTE* buf);
extern char *xls_addFont(xlsWorkBook* pWB,FONT* font);
extern void xls_addXF8(xlsWorkBook* pWB,XF8* xf);
extern void xls_addXF5(xlsWorkBook* pWB,XF5* xf);
extern void xls_addColinfo(xlsWorkSheet* pWS,COLINFO* colinfo);
extern void xls_mergedCells(xlsWorkSheet* pWS,BOF* bof,BYTE* buf);
extern void xls_parseWorkBook(xlsWorkBook* pWB);
extern void xls_preparseWorkSheet(xlsWorkSheet* pWS);
extern void xls_formatColumn(xlsWorkSheet* pWS);
extern void xls_parseWorkSheet(xlsWorkSheet* pWS);

int xls(void)
{
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
    DWORD ln; // String character count
    DWORD ofs; // Current offset in SST buffer
    DWORD rt; // Count or rich text formatting runs
    DWORD sz; // Size of asian phonetic settings block
    BYTE flag; // String flags
    char* ret;

    if (xls_debug) {
	    printf("xls_appendSST %ld\n", size);
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
            ln=*(WORD*)(buf+ofs);
            rt = 0;
            sz = 0;

            ofs+=2;
        }

		if (xls_debug) {
        	printf("ln=%ld\n", ln);
		}

        // Read flags
        if (  (!pWB->sst.continued)
            ||(  (pWB->sst.continued)
               &&(ln != 0) ) )
        {
            flag=*(BYTE*)(buf+ofs);
            ofs++;

            // Count of rich text formatting runs
            if (flag & 0x8)
            {
                rt=*(WORD*)(buf+ofs);
                ofs+=2;
            }

            // Size of asian phonetic settings block
            if (flag & 0x4)
            {
                sz=*(DWORD*)(buf+ofs);
                ofs+=4;

				if (xls_debug) {
					printf("sz=%ld\n", sz);
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
                int new_len = 0;
                ln_toread = min((size-ofs)/2, ln);

                ret=unicode_decode(buf+ofs,ln_toread*2, &new_len,pWB->charset);
   
                if (ret == NULL)
                {
                    char *str = "*failed to decode utf16*";
                    ret = strdup(str);
                    new_len = strlen(str);
                }

                ret = (char *)realloc(ret,new_len+1);
                *(char*)(ret+new_len)=0;

                ln -= ln_toread;
                ofs+=ln_toread*2;
                
                if (xls_debug) {
	                printf("String16: %s(%i)\n",ret,new_len);
                }
            }
            else
            {
                ln_toread = min((size-ofs), ln);

                ret = (char *)malloc(ln_toread + 1);
                memcpy (ret, (buf+ofs),ln_toread);
                *(char*)(ret+ln_toread)=0;

                ln -= ln_toread;
                ofs+=ln_toread;
                
                if (xls_debug) {
                	printf("String8: %s(%li) \n",ret,ln);
                }
            }
        }
        else
        {
         ret = strdup("");
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
                char *tmp;
                tmp=pWB->sst.string[pWB->sst.lastid-1].str;
                tmp=(char *)realloc(tmp,strlen(tmp)+strlen(ret)+1);
                pWB->sst.string[pWB->sst.lastid-1].str=tmp;
                memcpy(tmp+strlen(tmp),ret,strlen(ret)+1);
            }

			if (xls_debug) {
	            printf("String % 4ld: %s<end>\n", pWB->sst.lastid-1, pWB->sst.string[pWB->sst.lastid-1].str);
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
			printf("continued: ln=%ld, rt=%ld, sz=%ld\n", ln, rt, sz);
		}
	}
}

static double NumFromRk(BYTE* rk)
{
    volatile double num; // volatile necessary at least for Cygwin GCC 3.4.4
    DWORD drk;
    drk=*(DWORD*)rk;

	// What kind of value is this ?
    if (drk & 0x02) {
    	// Floating point value;
        num = (double)(drk >> 2);
    } else {
    	// Integer value
        *(1+(DWORD *)&num) = drk & 0xfffffffc;
        *((DWORD *)&num) = 0;
    }
    
    // Is value multiplied by 100 ?
    if (drk & 0x01) {
        num = num / 100.0;
    }
    
    return num;
}


char* xls_addSheet(xlsWorkBook* pWB,BOUNDSHEET *bs)
{
	char* name;
	DWORD filepos;
	BYTE visible, type, unicode;
	
	filepos = bs->filepos;
	visible = bs->visible;
	type = bs->type;
		
	if(pWB->is5ver) {
		unicode=0;
	} else {
		unicode=1;
	}
	// printf("charset=%s uni=%d\n", pWB->charset, unicode);
	// printf("bs name %.*s\n", bs->name[0], bs->name+1);
	name=get_string(bs->name,unicode,0,pWB->charset);	// TODO: unicode relevant here?
	// printf("name=%s\n", name);

	if(xls_debug) {
		printf ("xls_addSheet\n");
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
        pWB->sheets.sheet=(struct st_sheet_data *) malloc((pWB->sheets.count+1)*sizeof (struct st_sheet_data));
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

    verbose ("xls_addRow");

    tmp=&pWS->rows.row[row->index];
    tmp->height=row->height;
    tmp->fcell=row->fcell;
    tmp->lcell=row->lcell;
    tmp->flags=row->flags;
    tmp->xf=row->xf&0xfff;
    tmp->xfflags=row->xf&0xf000;
    //	xls_showROW(tmp);
}

void xls_makeTable(xlsWorkSheet* pWS)
{
    WORD i,t;
    struct st_row_data* tmp;
    verbose ("xls_makeTable");

    pWS->rows.row=(struct st_row_data *)calloc((pWS->rows.lastrow+1),sizeof(struct st_row_data));

    for (t=0;t<=pWS->rows.lastrow;t++)
    {
        tmp=&pWS->rows.row[t];
        tmp->index=t;
        tmp->fcell=0;
        tmp->lcell=pWS->rows.lastcol;

        tmp->cells.cell=(struct st_cell_data *)calloc((pWS->rows.lastcol+1),sizeof(struct st_cell_data));

        for (i=0;i<=pWS->rows.lastcol;i++)
        {
            tmp->cells.cell[i].col=i;
            tmp->cells.cell[i].row=t;
            tmp->cells.cell[i].width=pWS->defcolwidth;
            tmp->cells.cell[i].xf=0;
            tmp->cells.cell[i].str=NULL;
            tmp->cells.cell[i].d=0;
            tmp->cells.cell[i].l=0;
            tmp->cells.cell[i].ishiden=0;
            tmp->cells.cell[i].colspan=0;
            tmp->cells.cell[i].rowspan=0;
            tmp->cells.cell[i].id=0x201;
            tmp->cells.cell[i].str=NULL;
        }
    }
}

void xls_addCell(xlsWorkSheet* pWS,BOF* bof,BYTE* buf)
{
    struct st_cell_data*	cell;
    struct st_row_data*	row;
    int i;

    verbose ("xls_addCell");

    row=&pWS->rows.row[((COL*)buf)->row];
    cell=&row->cells.cell[((COL*)buf)->col-row->fcell];

    cell->id=bof->id;
    cell->xf=((COL*)buf)->xf;

    switch (bof->id)
    {
    case 0x06:	//FORMULA
        if (((FORMULA*)buf)->res!=0xffff)
            cell->d=*(double *)&((FORMULA*)buf)->resid;
        cell->str=xls_getfcell(pWS->workbook,cell);
        break;
    case 0x0BD:	//MULRK
        for (i=0;i<=*(WORD *)(buf+(bof->size-2))-((COL*)buf)->col;i++)
        {
            cell=&row->cells.cell[((COL*)buf)->col-row->fcell+i];
            //				col=row->cols[i];
            cell->id=bof->id;
            cell->xf=*((WORD *)(buf+(4+i*6)));
            cell->d=NumFromRk((BYTE *)(buf+(4+i*6+2)));
            cell->str=xls_getfcell(pWS->workbook,cell);
        }
        break;
    case 0x0BE:	//MULBLANK
        for (i=0;i<=*(WORD *)(buf+(bof->size-2))-((COL*)buf)->col;i++)
        {
            cell=&row->cells.cell[((COL*)buf)->col-row->fcell+i];
            //				col=row->cols[i];
            cell->id=bof->id;
            cell->xf=*((WORD *)(buf+(4+i*2)));
            cell->str=xls_getfcell(pWS->workbook,cell);
        }
        break;
    case 0xFD:	//LABELSST
        cell->l=*(WORD *)&((LABELSST*)buf)->value;
        cell->str=xls_getfcell(pWS->workbook,cell);
        break;
    case 0x27E:	//RK
        cell->d=NumFromRk(((RK*)buf)->value);
        cell->str=xls_getfcell(pWS->workbook,cell);
        break;
    case 0x203:	//NUMBER
        cell->d=*(double *)&((BR_NUMBER*)buf)->value;
        cell->str=xls_getfcell(pWS->workbook,cell);
        break;
    default:
        cell->str=xls_getfcell(pWS->workbook,cell);
        break;
    }
    
    if (xls_debug) {
    	xls_showCell(cell);
    }
}

char *xls_addFont(xlsWorkBook* pWB,FONT* font)
{
    struct st_font_data* tmp;

    verbose("xls_addFont");
    if (pWB->fonts.count==0)
    {
        pWB->fonts.font=(struct st_font_data *) malloc(sizeof(struct st_font_data));
    }
    else
    {
        pWB->fonts.font=(struct st_font_data *) realloc(pWB->fonts.font,(pWB->fonts.count+1)*sizeof(struct st_font_data));
    }

    tmp=&pWB->fonts.font[pWB->fonts.count];

    tmp->name=get_string((BYTE*)&font->name,0,0,pWB->charset);	// TODO: unicode relevant here?
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

void xls_addFormat(xlsWorkBook* pWB,FORMAT* format)
{
    struct st_format_data* tmp;

    verbose("xls_addFormat");
    if (pWB->formats.count==0)
    {
        pWB->formats.format=(struct st_format_data *) malloc(sizeof(struct st_format_data));
    }
    else
    {
        pWB->formats.format=(struct st_format_data *) realloc(pWB->formats.format,(pWB->formats.count+1)*sizeof(struct st_format_data));
    }

    tmp=&pWB->formats.format[pWB->formats.count];

    tmp->index=format->index;
    tmp->value=get_string((BYTE*)format + 2,0,1,pWB->charset);	// TODO: unicode relevant here?

    //	xls_showFormat(tmp);
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
    tmp->align=xf->align;
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

    //	xls_showColinfo(tmp);
    pWS->colinfo.count++;
}

void xls_mergedCells(xlsWorkSheet* pWS,BOF* bof,BYTE* buf)
{
    int count=*((WORD*)buf);
    int i,c,r;
    struct MERGEDCELLS* span;
    verbose("Merged Cells");
    for (i=0;i<count;i++)
    {
        span=(struct MERGEDCELLS*)(buf+(2+i*sizeof(struct MERGEDCELLS)));
        //		printf("Merged Cells: [%i,%i] [%i,%i] \n",span->colf,span->rowf,span->coll,span->rowl);
        for (r=span->rowf;r<=span->rowl;r++)
            for (c=span->colf;c<=span->coll;c++)
                pWS->rows.row[r].cells.cell[c].ishiden=1;
        pWS->rows.row[span->rowf].cells.cell[span->colf].colspan=(span->coll-span->colf+1);
        pWS->rows.row[span->rowf].cells.cell[span->colf].rowspan=(span->rowl-span->rowf+1);
        pWS->rows.row[span->rowf].cells.cell[span->colf].ishiden=0;
    }
}

void xls_parseWorkBook(xlsWorkBook* pWB)
{
    BOF bof1;
    BOF bof2;
    BYTE* buf;
	BYTE once;
    //DWORD size;
	
	// this to prevent compiler warnings
	once=0;
	bof2.size = 0;
	bof2.id = 0;
    verbose ("xls_parseWorkBook");
    do
    {
        ole2_read(&bof1, 1, 4, pWB->olestr);	
 		if(xls_debug) xls_showBOF(&bof1);

        buf=(BYTE *)malloc(bof1.size);
        ole2_read(buf, 1, bof1.size, pWB->olestr);
        switch (bof1.id) {
        case 0x0A:		// EOF
            //verbose("EOF");
            break;
        case 0x809:		// BIFF5-8
			{
				BIFF *b = (BIFF*)buf;
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
        case 0x042:		// CODEPAGE
            pWB->codepage=*(WORD*)buf;
            break;
        case 0x3c:		// CONTINUE
			if(once) {
				if (bof2.id==0xfc)
					xls_appendSST(pWB,buf,bof1.size);
				bof1=bof2;
			}
            break;
		case 0x003D:	// WINDOW1
			{
				WIND1 *w = (WIND1*)buf;
				if(xls_debug) {
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
        case 0xfc:		// SST
            xls_addSST(pWB,(SST*)buf,bof1.size);
            break;
        case 0xff:		// EXTSST
            if(xls_debug) dumpbuf("EXTSST",bof1.size,buf);
            break;
        case 0x85:		// BOUNDSHEET
			{
				BOUNDSHEET *bs = (BOUNDSHEET *)buf;
				char *s;
				// different for BIFF5 and BIFF8
				s = xls_addSheet(pWB,bs);
			}
            break;
        case 0x0e0:  	// XF
			if(pWB->is5ver) {
				XF5 *xf;
				xf = (XF5 *)buf;
				
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
				
				xls_addXF8(pWB,xf);
				if(xls_debug) {
					printf("   font: %d\n", xf->font);
					printf(" format: %d\n", xf->format);
					printf("   type: %d\n", xf->type);
					printf("  align: %d\n", xf->align);
					printf("rotatio: %d\n", xf->rotation);
					printf("  ident: %d\n", xf->ident);
					printf("usedatt: %d\n", xf->usedattr);
					printf("linesty: %d\n", xf->linestyle);
					printf("linecol: %d\n", xf->linecolor);
					printf("groundc: %d\n", xf->groundcolor);
				}
			}
            break;
        case 0x031:		// FONT
			{
				char *s;
				FONT *f = (FONT*)buf;
				s = xls_addFont(pWB,f);
				if(xls_debug) {
					printf("height : %d\n", f->height);
					printf("flag   : %d\n", f->flag);
					printf("color  : %d\n", f->color);
					printf("escapem: %d\n", f->escapement);
					printf("underli: %d\n", f->underline);
					printf("family : %d\n", f->family);
					printf("charset: %d\n", f->charset);
					if(s) printf("name   : %s\n", s);
				}
			}
			break;
        case 0x41E:	//FORMAT
            xls_addFormat(pWB,(FORMAT*)buf);
            break;
		case 0x0293:	// STYLE
			break;
        default:
			if(xls_debug) printf("UNKNOWN BOF=%Xh\n", bof1.id);
            break;
        }
		free(buf);

        bof2=bof1;
		once=1;

    }
    while ((!pWB->olestr->eof)&&(bof1.id!=0x0A));
}


void xls_preparseWorkSheet(xlsWorkSheet* pWS)
{
    BOF tmp;
    BYTE* buf;

    verbose ("xls_preparseWorkSheet");

    ole2_seek(pWS->workbook->olestr,pWS->filepos);
    do
    {
        ole2_read(&tmp, 1,4,pWS->workbook->olestr);
        buf=(BYTE *)malloc(tmp.size);
        ole2_read(buf, 1,tmp.size,pWS->workbook->olestr);
        //	xls_showBOF(&tmp);
        switch (tmp.id)
        {
        case 0x55:     //DEFCOLWIDTH
            pWS->defcolwidth=*(WORD*)buf*256;
            break;
        case 0x7D:     //COLINFO
            xls_addColinfo(pWS,(COLINFO*)buf);
            break;
        case 0x208:		//ROW
            if (pWS->rows.lastcol<((ROW*)buf)->lcell)
                pWS->rows.lastcol=((ROW*)buf)->lcell;
            if (pWS->rows.lastrow<((ROW*)buf)->index)
                pWS->rows.lastrow=((ROW*)buf)->index;
            break;
        }
        free(buf);
    }
    while ((!pWS->workbook->olestr->eof)&&(tmp.id!=0x0A));
}

void xls_formatColumn(xlsWorkSheet* pWS)
{
    int i,t,ii;
    int fcol,lcol;

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

        for (t=fcol;t<=lcol;t++)
            for (ii=0;ii<=pWS->rows.lastrow;ii++)
            {
                if (pWS->colinfo.col[i].flags&1)
                    pWS->rows.row[ii].cells.cell[t].ishiden=1;
                pWS->rows.row[ii].cells.cell[t].width=pWS->colinfo.col[i].width;
            }
    }
}

void xls_parseWorkSheet(xlsWorkSheet* pWS)
{
    BOF tmp;
    BYTE* buf;
    verbose ("xls_parseWorkSheet");

    xls_preparseWorkSheet(pWS);
	// printf("size=%d fatpos=%d)\n", pWS->workbook->olestr->size, pWS->workbook->olestr->fatpos);

    xls_makeTable(pWS);
    xls_formatColumn(pWS);

    ole2_seek(pWS->workbook->olestr,pWS->filepos);
    do
    {
        ole2_read(&tmp, 1,4,pWS->workbook->olestr);
        buf=(BYTE *)malloc(tmp.size);
        ole2_read(buf, 1,tmp.size,pWS->workbook->olestr);

        //	xls_showBOF(&tmp);
        switch (tmp.id)
        {
        case 0x0A:		//EOF
            break;
        case 0x0E5: 	//MERGEDCELLS
            xls_mergedCells(pWS,&tmp,buf);
            break;
        case 0x208:		//ROW
            xls_addRow(pWS,(ROW*)buf);
            break;
        case 0x20B:		//INDEX
            break;
        case 0x0BD:		//MULRK
        case 0x0BE:		//MULBLANK
        case 0x203:		//NUMBER
        case 0x06:		//FORMULA
        case 0x27e:		//RK
        case 0xFD:		//LABELSST
        case 0x201:		//BLANK
            xls_addCell(pWS,&tmp,buf);
            break;
        default:
            break;
        }

        free(buf);
    }
    while ((!pWS->workbook->olestr->eof)&&(tmp.id!=0x0A));


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

xlsWorkBook* xls_open(char *file,char* charset)
{
    xlsWorkBook* pWB;
    OLE2*		ole;

    pWB=(xlsWorkBook*)calloc(1, sizeof(xlsWorkBook));
    verbose ("xls_open");

    // open excel file
    if (!(ole=ole2_open(file, charset)))
    {
        if(xls_debug) printf("File \"%s\" not found\n",file);
        return(NULL);
    }

    // open Workbook
    if (!(pWB->olestr=ole2_fopen(ole,"Workbook")) && !(pWB->olestr=ole2_fopen(ole,"Book")))
    {
        if(xls_debug) printf("Workbook not found 123\n");
        return(NULL);
    }

    pWB->sheets.count=0;
    pWB->xfs.count=0;
    pWB->fonts.count=0;
    pWB->charset = (char *)malloc(strlen(charset) * sizeof(char));
    strcpy(pWB->charset, charset);
    xls_parseWorkBook(pWB);
    return(pWB);
}

void xls_close(xlsWorkBook* pWB)
{
	OLE2*		ole;
	verbose ("xls_close");
	ole=pWB->olestr->ole;
	ole2_fclose(pWB->olestr);
	ole2_close(ole);
	free(pWB);
}

extern const char* xls_getVersion(void)
{
    return PACKAGE_VERSION;
}
