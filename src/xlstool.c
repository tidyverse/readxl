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
#include <math.h>
#include <sys/types.h>
#include <wchar.h>
#include <stdio.h>

#ifdef HAVE_ICONV
#include <iconv.h>
#else
#include <locale.h>
#endif

#include <stdlib.h>
#include <errno.h>
#include <memory.h>
#include <string.h>

#include <libxls/xls.h>
#include <libxls/brdb.h>

static void xls_showBOUNDSHEET(void* bsheet);

void dumpbuf(char* fname,long size,BYTE* buf)
{
    FILE *f = fopen (fname, "wb");
    fwrite (buf, 1, size, f);
    fclose(f);

}

// Display string if in debug mode
void verbose(char* str)
{
    if (xls_debug)
        printf("libxls : %s\n",str);
}

// Convert unicode string to to_enc encoding
char* unicode_decode(const BYTE *s, int len, int *newlen, const char* to_enc)
{
#if HAVE_ICONV
	// Do iconv conversion
    const char *from_enc = "UTF-16LE";
    char* outbuf = 0;
    
    if(s && len && from_enc && to_enc)
    {
        size_t outlenleft = len;
        int outlen = len;
        size_t inlenleft = len;
        iconv_t ic = iconv_open(to_enc, from_enc);
        char* src_ptr = (char*) s;
        char* out_ptr = 0;

        if(ic != (iconv_t)-1)
        {
            size_t st; 
            outbuf = (char*)malloc(outlen + 1);

			if(outbuf)
            {
                out_ptr = (char*)outbuf;
                while(inlenleft)
                {
                    st = iconv(ic, &src_ptr, &inlenleft, &out_ptr,(size_t *) &outlenleft);
                    if(st == (size_t)(-1))
                    {
                        if(errno == E2BIG)
                        {
                            int diff = out_ptr - outbuf;
                            outlen += inlenleft;
                            outlenleft += inlenleft;
                            outbuf = (char*)realloc(outbuf, outlen + 1);
                            if(!outbuf)
                            {
                                break;
                            }
                            out_ptr = outbuf + diff;
                        }
                        else
                        {
                            free(outbuf), outbuf = NULL;
                            break;
                        }
                    }
                }
            }
            iconv_close(ic);
        }
        outlen -= outlenleft;

        if (newlen)
        {
            *newlen = outbuf ? outlen : 0;
        }
        if(outbuf)
        {
            outbuf[outlen] = 0;
        }
    }
    return outbuf;
#else
	// Do wcstombs conversion
	char *converted = NULL;
	int count, count2;

	if (setlocale(LC_CTYPE, "") == NULL) {
		printf("setlocale failed: %d\n", errno);
		return "*null*";
	}

	count = wcstombs(NULL, (wchar_t*)s, 0);
	if (count <= 0) {
		if (newlen) *newlen = 0;
		return NULL;
	}

	converted = calloc(count+1, sizeof(char));
	count2 = wcstombs(converted, (wchar_t*)s, count+1);
	if (count2 <= 0) {
		printf("wcstombs failed (%d)\n", len);
		if (newlen) *newlen = 0;
		return converted;
	} else {
		if (newlen) *newlen = count2;
		return converted;
	}
#endif
}

// Read and decode string
char* get_string(BYTE *s, BYTE is2, BYTE fmt, char *charset)
{
    WORD ln;
    DWORD ofs;
    DWORD sz;
    WORD rt;
    BYTE flag;
    BYTE* str;
    char* ret;
    int new_len;
	
	new_len = 0;
	flag = 0;
    str=s;


    ofs=0;

    if (is2)
    {
        ln=*(WORD*)str;
        ofs+=2;
    }
    else
    {
        ln=*(BYTE*)str;
        ofs++;
    }

	if(fmt) {
		flag=*(BYTE*)(str+ofs);
		ofs++;
	}
    if (flag&0x8)
    {
        rt=*(WORD*)(str+ofs);
        ofs+=2;
    }
    if (flag&0x4)
    {
        sz=*(DWORD*)(str+ofs);
        ofs+=4;
    }

    if (flag&0x1) {
        ret=unicode_decode(str+ofs,ln*2, &new_len,charset);
        ofs+=ln*2;
    } else {
        ret=(char *)malloc(ln+1);
        memcpy (ret,(str+ofs),ln);
        *(char*)(ret+ln)=0;
        ofs+=ln;
    }

    if (flag&0x8)
        ofs+=4*rt;
    if (flag&0x4)
        ofs+=sz;

    return ret;
}

DWORD xls_getColor(const WORD color,WORD def)
{
    int cor=8;
    int size = 64 - cor;
    int max = size;
    WORD idx=color;
    if( idx >= cor)
        idx -= cor;
    if( idx < max )
    {
        return colors[idx];
    }
    else
        return colors[def];
}


void xls_showBookInfo(xlsWorkBook* pWB)
{
    verbose("BookInfo");
    printf("  is5ver: %i\n",pWB->is5ver);
    printf("codepage: %i\n",pWB->codepage);
    printf("    type: %.4X ",pWB->type);
    switch (pWB->type)
    {
    case 0x5:
        printf("Workbook globals\n");
        break;
    case 0x6:
        printf("Visual Basic module\n");
        break;
    case 0x10:
        printf("Worksheet\n");
        break;
    case 0x20:
        printf("Chart\n");
        break;
    case 0x40:
        printf("BIFF4 Macro sheet\n");
        break;
    case 0x100:
        printf("BIFF4W Workbook globals\n");
        break;
    }
    printf("------------------- END BOOK INFO---------------------------\n");
}


void xls_showBOF(BOF* bof)
{
    printf("----------------------------------------------\n");
    verbose("BOF");
    printf("   ID: %.4Xh %s (%s)\n",bof->id,brdb[get_brbdnum(bof->id)].name,brdb[get_brbdnum(bof->id)].desc);
    printf("   Size: %i\n",bof->size);
}

#if 0
static void xls_showBOUNDSHEET(BOUNDSHEET* bsheet)
{
    switch (bsheet->type & 0x000f)
    {
    case 0x0000:
        /* worksheet or dialog sheet */
        verbose ("85: Worksheet or dialog sheet");
        break;
    case 0x0001:
        /* Microsoft Excel 4.0 macro sheet */
        verbose ("85: Microsoft Excel 4.0 macro sheet");
        break;
    case 0x0002:
        /* Chart */
        verbose ("85: Chart sheet");
        break;
    case 0x0006:
        /* Visual Basic module */
        verbose ("85: Visual Basic sheet");
        break;
    default:
        break;
    }
    printf("    Pos: %Xh\n",bsheet->filepos);
    printf("  flags: %.4Xh\n",bsheet->type);
    //	printf("   Name: [%i] %s\n",bsheet->len,bsheet->name);
}
#endif

void xls_showROW(struct st_row_data* row)
{
    verbose("ROW");
    printf("    Index: %i \n",row->index);
    printf("First col: %i \n",row->fcell);
    printf(" Last col: %i \n",row->lcell);
    printf("   Height: %i (1/20 px)\n",row->height);
    printf("    Flags: %.4X \n",row->flags);
    printf("       xf: %i \n",row->xf);
    printf("----------------------------------------------\n");
}

void xls_showColinfo(struct st_colinfo_data* col)
{
    verbose("COLINFO");
    printf("First col: %i \n",col->first);
    printf(" Last col: %i \n",col->last);
    printf("    Width: %i (1/256 px)\n",col->width);
    printf("       XF: %i \n",col->xf);
    printf("    Flags: %i (",col->flags);
    if (col->flags & 0x1)
        printf("hiddeh ");
    if (col->flags & 0x700)
        printf("outline ");
    if (col->flags & 0x1000)
        printf("collapsed ");
    printf(")\n");
    printf("----------------------------------------------\n");
}

void xls_showCell(struct st_cell_data* cell)
{
	if(cell->id == 0x0201) return;

    printf("  -----------\n");
    printf("     ID: %.4Xh %s (%s)\n",cell->id, brdb[get_brbdnum(cell->id)].name, brdb[get_brbdnum(cell->id)].desc);
    printf("   Cell: %c%i\n",cell->col+65,cell->row+1);
    printf("     xf: %i\n",cell->xf);
    printf(" double: %f\n",cell->d);
    printf("   long: %i\n",cell->l);
    if (cell->str!=NULL)
        printf("    str: %s\n",cell->str);
}


void xls_showFont(struct st_font_data* font)
{

    printf("      name: %s\n",font->name);
    printf("    height: %i\n",font->height);
    printf("      flag: %.4X\n",font->flag);
    printf("     color: %.6X\n",font->color);
    printf("      bold: %i\n",font->bold);
    printf("escapement: %i\n",font->escapement);
    printf(" underline: %i\n",font->underline);
    printf("    family: %i\n",font->family);
    printf("   charset: %i\n",font->charset);

}

void xls_showXF(struct st_xf_data* xf)
{
    printf("       Font: %i\n",xf->font);
    printf("     Format: %i\n",xf->format);
    printf("       Type: %i\n",xf->type);
    printf("      Align: %i\n",xf->align);
    printf("   Rotation: %i\n",xf->rotation);
    printf("      Ident: %i\n",xf->ident);
    printf("   UsedAttr: %i\n",xf->usedattr);
    printf("  LineStyle: %i\n",xf->linestyle);
    printf("  Linecolor: %i\n",xf->linecolor);
    printf("GroundColor: %i\n",xf->groundcolor);
}

char*  xls_getfcell(xlsWorkBook* pWB,struct st_cell_data* cell)
{
    struct st_xf_data*  xf;
    char ret[10240];
    char* out;

    xf=&pWB->xfs.xf[cell->xf];

    //LABELSST
    switch (cell->id)
    {
    case 0x0FD:
        sprintf(ret,"%s",pWB->sst.string[cell->l].str);
        break;
    case 0x201:
        sprintf(ret,"");
        break;
    case 0x0BE:
        sprintf(ret,"");
        break;
        //		if (cell->id==0x27e || cell->id==0x0BD || cell->id==0x203 )
    default:
        switch (xf->format)
        {
        case 0:
            sprintf(ret,"%i",(int)cell->d);
            break;
        case 1:
            sprintf(ret,"%i",(int)cell->d);
            break;
        case 2:
            sprintf(ret,"%.1f",cell->d);
            break;

        case 9:
            sprintf(ret,"%i%",(int)cell->d);
            break;
        case 10:
            sprintf(ret,"%.2f%",cell->d);
            break;
        case 11:
            sprintf(ret,"%.1e",cell->d);
            break;
        case 14:
            {
                //				ret=ctime(cell->d);
                sprintf(ret,"%.0f",cell->d);
                break;
            }
            break;
        default:
			// sprintf(ret,"%.4.2f (%i)",cell->d,xf->format);break;
            sprintf(ret,"%.2f",cell->d);
            break;
        }
        break;
    }

    out=(char *)malloc(strlen(ret)+1);
    memcpy(out,ret,strlen(ret)+1);
    return out;
}

char* xls_getCSS(xlsWorkBook* pWB)
{
    char ret[65535];
    char buf[65535];
    char color[255];
    char* out;
    char* align;
    char* valign;
    char borderleft[255];
    char borderright[255];
    char bordertop[255];
    char borderbottom[255];
    char italic[255];
    char underline[255];
    char bold[255];
    WORD size;
    char fontname[255];
    struct st_xf_data* xf;
    DWORD background;
    int i;

    for (i=0;i<pWB->xfs.count;i++)
    {
        xf=&pWB->xfs.xf[i];
        switch ((xf->align & 0x70)>>4)
        {
        case 0:
            valign=(char*)"top";
            break;
        case 1:
            valign=(char*)"middle";
            break;
        case 2:
            valign=(char*)"bottom";
            break;
            //			case 3: valign=(char*)"right"; break;
            //			case 4: valign=(char*)"right"; break;
        default:
            valign=(char*)"middle";
            break;
        }

        switch (xf->align & 0x07)
        {
        case 1:
            align=(char*)"left";
            break;
        case 2:
            align=(char*)"center";
            break;
        case 3:
            align=(char*)"right";
            break;
        default:
            align=(char*)"left";
            break;
        }

        switch (xf->linestyle & 0x0f)
        {
        case 0:
            sprintf(borderleft,"\0");
            break;
        default:
            sprintf(borderleft,"border-left: 1px solid black;");
            break;
        }

        switch (xf->linestyle & 0x0f0)
        {
        case 0:
            sprintf(borderright,"\0");
            break;
        default:
            sprintf(borderright,"border-right: 1px solid black;");
            break;
        }

        switch (xf->linestyle & 0x0f00)
        {
        case 0:
            sprintf(bordertop,"\0");
            break;
        default:
            sprintf(bordertop,"border-top: 1px solid black;");
            break;
        }

        switch (xf->linestyle & 0x0f000)
        {
        case 0:
            sprintf(borderbottom,"\0");
            break;
        default:
            sprintf(borderbottom,"border-bottom: 1px solid Black;");
            break;
        }

        if (xf->font)
            sprintf(color,"color:#%.6X;",xls_getColor(pWB->fonts.font[xf->font-1].color,0));
        else
            sprintf(color,"\0");

        if (xf->font && (pWB->fonts.font[xf->font-1].flag & 2))
            sprintf(italic,"font-style: italic;");
        else
            sprintf(italic,"\0");

        if (xf->font && (pWB->fonts.font[xf->font-1].bold>400))
            sprintf(bold,"font-weight: bold;");
        else
            sprintf(bold,"\0");

        if (xf->font && (pWB->fonts.font[xf->font-1].underline))
            sprintf(underline,"text-decoration: underline;");
        else
            sprintf(underline,"\0");

        if (xf->font)
            size=pWB->fonts.font[xf->font-1].height/20;
        else
            size=10;

        if (xf->font)
        {
            sprintf(fontname,pWB->fonts.font[xf->font-1].name);
        }
        else
            sprintf(fontname,"Arial");

        background=xls_getColor((WORD)(xf->groundcolor & 0x7f),1);
        sprintf(buf,".xf%i{ font-size:%ipt;font-family: \"%s\";background:#%.6X;text-align:%s;vertical-align:%s;%s%s%s%s%s%s%s%s}\n",
                i,size,fontname,background,align,valign,borderleft,borderright,bordertop,borderbottom,color,italic,bold,underline);

        if (i==0)
            strcpy(ret,buf);
        else
            strcat(ret,buf);
    }
    out=(char *)malloc(strlen(ret)+1);
    memcpy(out,ret,strlen(ret)+1);
    return out;
}
