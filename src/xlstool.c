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

//#include "xls.h"
#include "libxls/xlstypes.h"
#include "libxls/xlsstruct.h"
#include "libxls/xlstool.h"
#include "libxls/brdb.h"
#include "libxls/endian.h"

extern int xls_debug;

static void xls_showBOUNDSHEET(void* bsheet);

static const DWORD colors[] =
    {
        0x000000,
        0xFFFFFF,
        0xFF0000,
        0x00FF00,
        0x0000FF,
        0xFFFF00,
        0xFF00FF,
        0x00FFFF,
        0x800000,
        0x008000,
        0x000080,
        0x808000,
        0x800080,
        0x008080,
        0xC0C0C0,
        0x808080,
        0x9999FF,
        0x993366,
        0xFFFFCC,
        0xCCFFFF,
        0x660066,
        0xFF8080,
        0x0066CC,
        0xCCCCFF,
        0x000080,
        0xFF00FF,
        0xFFFF00,
        0x00FFFF,
        0x800080,
        0x800000,
        0x008080,
        0x0000FF,
        0x00CCFF,
        0xCCFFFF,
        0xCCFFCC,
        0xFFFF99,
        0x99CCFF,
        0xFF99CC,
        0xCC99FF,
        0xFFCC99,
        0x3366FF,
        0x33CCCC,
        0x99CC00,
        0xFFCC00,
        0xFF9900,
        0xFF6600,
        0x666699,
        0x969696,
        0x003366,
        0x339966,
        0x003300,
        0x333300,
        0x993300,
        0x993366,
        0x333399,
        0x333333
    };

#if HAVE_ASPRINTF != 1

#include <stdarg.h>

#ifdef MSDN
static int asprintf(char **ret, const char *format, ...)
{
	int i, size=100;
    char *p, *np;

	va_list ap;

	if ((p = (char *)malloc(size)) == NULL)
        return -1;

    while (1) {
	    va_start(ap, format); 

	    i = _vsnprintf(p, size, format, ap);

	    va_end(ap);

        if (i > -1 && i < size)
        {
            i++;
            break;
        }

        if (i > -1)     /* glibc 2.1 */
            size = i+1; /* precisely what is needed */
        else            /* glibc 2.0 */
            size *= 2;  /* twice the old size */

        if ((np = realloc (p, size)) == NULL) {
            free(p);
            return -1;
        } else {
            p = np;
        }
    }

	*ret = p;
	return i > 255 ? 255 : i;
}

#else

static int asprintf(char **ret, const char *format, ...)
{
	int i;
    char *str;

	va_list ap;

	va_start(ap, format); 

	i = vsnprintf(NULL, 0, format, ap) + 1;
	str = (char *)malloc(i);
	i = vsnprintf(str, i, format, ap);

	va_end(ap);

	*ret = str;
	return i > 255 ? 255 : i;
}
#endif

#endif


void dumpbuf(BYTE* fname,long size,BYTE* buf)
{
    FILE *f = fopen((char *)fname, "wb");
    fwrite (buf, 1, size, f);
    fclose(f);

}

// Display string if in debug mode
void verbose(char* str)
{
    if (xls_debug)
        printf("libxls : %s\n",str);
}

BYTE *utf8_decode(BYTE *str, DWORD len, char *encoding)
{
	int utf8_chars = 0;
	BYTE *ret;
    DWORD i;
	
	for(i=0; i<len; ++i) {
		if(str[i] & (BYTE)0x80) {
			++utf8_chars;
		}
	}
	
	if(utf8_chars == 0 || strcmp(encoding, "UTF-8")) {
		ret = (BYTE *)malloc(len+1);
		memcpy(ret, str, len);
		ret[len] = 0;
	} else {
        DWORD i;
        BYTE *out;
		// UTF-8 encoding inline
		ret = (BYTE *)malloc(len+utf8_chars+1);
		out = ret;
		for(i=0; i<len; ++i) {
			BYTE c = str[i];
			if(c & (BYTE)0x80) {
				*out++ = (BYTE)0xC0 | (c >> 6);
				*out++ = (BYTE)0x80 | (c & 0x3F);
			} else {
				*out++ = c;
			}
		}
		*out = 0;
	}

	return ret;
}

// Convert unicode string to to_enc encoding
BYTE* unicode_decode(const BYTE *s, int len, size_t *newlen, const char* to_enc)
{
#ifdef HAVE_ICONV
	// Do iconv conversion
#ifdef AIX
    const char *from_enc = "UTF-16le";
#else
    const char *from_enc = "UTF-16LE";
#endif
    BYTE* outbuf = 0;

    if(s && len && from_enc && to_enc)
    {
        size_t outlenleft = len;
        int outlen = len;
        size_t inlenleft = len;
        iconv_t ic = iconv_open(to_enc, from_enc);
        BYTE* src_ptr = (BYTE*) s;
        BYTE* out_ptr = 0;

        if(ic == (iconv_t)-1)
        {
            // Something went wrong.
            if (errno == EINVAL)
            {
                if (!strcmp(to_enc, "ASCII"))
                {
                    ic = iconv_open("UTF-8", from_enc);
                    if(ic == (iconv_t)-1)
                    {
                        printf("conversion from '%s' to '%s' not available", from_enc, to_enc);
                        return outbuf;
                    }
                }
            }
            else
            {
                printf ("iconv_open: error=%d", errno);
                return outbuf;
            }
        }
        size_t st; 
        outbuf = (BYTE*)malloc(outlen + 1);

		if(outbuf)
        {
            out_ptr = (BYTE*)outbuf;
            while(inlenleft)
            {
                st = iconv(ic, (char **)&src_ptr, &inlenleft, (char **)&out_ptr,(size_t *) &outlenleft);
                if(st == (size_t)(-1))
                {
                    if(errno == E2BIG)
                    {
                        size_t diff = out_ptr - outbuf;
                        outlen += inlenleft;
                        outlenleft += inlenleft;
                        outbuf = (BYTE*)realloc(outbuf, outlen + 1);
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
	int count, count2, i;
	wchar_t *w;
    short *x;
	if (setlocale(LC_CTYPE, "") == NULL) {
		printf("setlocale failed: %d\n", errno);
		return "*null*";
	}

    x=(short *)s;

    w = (wchar_t*)malloc((len+1)*sizeof(wchar_t));

    for(i=0; i<len; i++)
    {
        w[i]=xlsShortVal(x[i]);
    }
    w[len] = '\0';

    count = wcstombs(NULL, w, 0);

	if (count <= 0) {
		if (newlen) *newlen = 0;
		return NULL;
	}

	converted = calloc(count+1, sizeof(char));
	count2 = wcstombs(converted, w, count);
    free(w);
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
BYTE* get_string(BYTE *s, BYTE is2, BYTE is5ver, char *charset)
{
    WORD ln;
    DWORD ofs;
    BYTE flag;
    BYTE* str;
    BYTE* ret;
	
	flag = 0;
    str=s;

    ofs=0;

    if (is2) {
		// length is two bytes
        ln=xlsShortVal(*(WORD_UA *)str);
        ofs+=2;
    } else {
		// single byte length
        ln=*(BYTE*)str;
        ofs++;
    }

	if(!is5ver) {
		// unicode strings have a format byte before the string
		flag=*(BYTE*)(str+ofs);
		ofs++;
	}
    if (flag&0x8)
    {
		// WORD rt;
        // rt=*(WORD*)(str+ofs); // unused
        ofs+=2;
    }
    if (flag&0x4)
    {
		// DWORD sz;
        // sz=*(DWORD*)(str+ofs); // unused
        ofs+=4;
    }
    if(flag & 0x1)
    {
		size_t new_len = 0;
        ret = unicode_decode(str+ofs,ln*2, &new_len,charset);
    } else {
		ret = utf8_decode((str+ofs), ln, charset);
    }

#if 0	// debugging
	if(xls_debug == 100) {
		ofs += (flag & 0x1) ? ln*2 : ln;

		printf("ofs=%d ret[0]=%d\n", ofs, *ret);
		{
			unsigned char *ptr;
			
			ptr = ret;
			
			printf("%x %x %x %x %x %x %x %x\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7] );
			printf("%s\n", ret);
		}
	}
#endif

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
        printf("hidden ");
    if (col->flags & 0x700)
        printf("outline ");
    if (col->flags & 0x1000)
        printf("collapsed ");
    printf(")\n");
    printf("----------------------------------------------\n");
}

void xls_showCell(struct st_cell_data* cell)
{
    printf("  -----------\n");
    printf("     ID: %.4Xh %s (%s)\n",cell->id, brdb[get_brbdnum(cell->id)].name, brdb[get_brbdnum(cell->id)].desc);
    printf("   Cell: %c:%u  [%u:%u]\n",cell->col+'A',cell->row+1,cell->col,cell->row);
//    printf("   Cell: %u:%u\n",cell->col+1,cell->row+1);
    printf("     xf: %i\n",cell->xf);
	if(cell->id == 0x0201) {
		//printf("BLANK_CELL!\n");
		return;
	}
    printf(" double: %f\n",cell->d);
    printf("    int: %d\n",cell->l);
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
#if 0
typedef struct st_format
	{
		long count;		//Count of FORMAT's
		struct st_format_data
		{
			WORD index;
			char *value;
		}
		* format;
	}
	st_format;
#endif

void xls_showFormat(struct st_format_data* frmt)
{
	printf("    index : %u\n", frmt->index);
    printf("     value: %s\n", frmt->value);
}

void xls_showXF(XF8* xf)
{
	static int idx;
	
    printf("      Index: %u\n",idx++);
    printf("       Font: %u\n",xf->font);
    printf("     Format: %u\n",xf->format);
    printf("       Type: 0x%x\n",xf->type);
    printf("      Align: 0x%x\n",xf->align);
    printf("   Rotation: 0x%x\n",xf->rotation);
    printf("      Ident: 0x%x\n",xf->ident);
    printf("   UsedAttr: 0x%x\n",xf->usedattr);
    printf("  LineStyle: 0x%x\n",xf->linestyle);
    printf("  Linecolor: 0x%x\n",xf->linecolor);
    printf("GroundColor: 0x%x\n",xf->groundcolor);
}

BYTE *xls_getfcell(xlsWorkBook* pWB,struct st_cell_data* cell,WORD *label)
{
    struct st_xf_data *xf;
	WORD	len;
    char	*ret = NULL;

    xf=&pWB->xfs.xf[cell->xf];

    //LABELSST
    switch (cell->id)
    {
    case 0x00FD:		//LABELSST
		//printf("WORD: %u short: %u str: %s\n", *label, xlsShortVal(*label), pWB->sst.string[xlsShortVal(*label)].str );
        asprintf(&ret,"%s",pWB->sst.string[xlsShortVal(*label)].str);
        break;
    case 0x0201:		//BLANK
    case 0x00BE:		//MULBLANK
        asprintf(&ret, "");
        break;
    case 0x0204:		//LABEL (xlslib generates these)
		len = xlsShortVal(*label);
        label++;
		if(pWB->is5ver) {
			asprintf(&ret,"%.*s", len, (char *)label);
			//printf("Found BIFF5 string of len=%d \"%s\"\n", len, ret);
		} else
		if ((*(BYTE *)label & 0x01) == 0) {
			ret = (char *)utf8_decode((BYTE *)label + 1, len, pWB->charset);
		} else {
			size_t newlen;
		    ret = (char *)unicode_decode((BYTE *)label + 1, len*2, &newlen, pWB->charset);
		}
        break;
    case 0x027E:		//RK
    case 0x0203:		//NUMBER
        asprintf(&ret,"%lf", cell->d);
		break;
		//		if( RK || MULRK || NUMBER || FORMULA)
		//		if (cell->id==0x27e || cell->id==0x0BD || cell->id==0x203 || 6 (formula))
    default:
        switch (xf->format)
        {
        case 0:
            asprintf(&ret,"%d",(int)cell->d);
            break;
        case 1:
            asprintf(&ret,"%d",(int)cell->d);
            break;
        case 2:
            asprintf(&ret,"%.1f",cell->d);
            break;
        case 9:
            asprintf(&ret,"%d",(int)cell->d);
            break;
        case 10:
            asprintf(&ret,"%.2f",cell->d);
            break;
        case 11:
            asprintf(&ret,"%.1e",cell->d);
            break;
        case 14:
			//ret=ctime(cell->d);
			asprintf(&ret,"%.0f",cell->d);
            break;
        default:
			// asprintf(&ret,"%.4.2f (%i)",cell->d,xf->format);break;
            asprintf(&ret,"%.2f",cell->d);
            break;
        }
        break;
    }

    return (BYTE *)ret;
}

char* xls_getCSS(xlsWorkBook* pWB)
{
    char color[255];
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
    DWORD i;

    char *ret = malloc(65535);
    char *buf = malloc(4096);
	ret[0] = '\0';

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
            sprintf(borderleft,"%s", "");
            break;
        default:
            sprintf(borderleft,"border-left: 1px solid black;");
            break;
        }

        switch (xf->linestyle & 0x0f0)
        {
        case 0:
            sprintf(borderright,"%s", "");
            break;
        default:
            sprintf(borderright,"border-right: 1px solid black;");
            break;
        }

        switch (xf->linestyle & 0x0f00)
        {
        case 0:
            sprintf(bordertop,"%s", "");
            break;
        default:
            sprintf(bordertop,"border-top: 1px solid black;");
            break;
        }

        switch (xf->linestyle & 0x0f000)
        {
        case 0:
            sprintf(borderbottom,"%s", "");
            break;
        default:
            sprintf(borderbottom,"border-bottom: 1px solid Black;");
            break;
        }

        if (xf->font)
            sprintf(color,"color:#%.6X;",xls_getColor(pWB->fonts.font[xf->font-1].color,0));
        else
            sprintf(color,"%s", "");

        if (xf->font && (pWB->fonts.font[xf->font-1].flag & 2))
            sprintf(italic,"font-style: italic;");
        else
            sprintf(italic,"%s", "");

        if (xf->font && (pWB->fonts.font[xf->font-1].bold>400))
            sprintf(bold,"font-weight: bold;");
        else
            sprintf(bold,"%s", "");

        if (xf->font && (pWB->fonts.font[xf->font-1].underline))
            sprintf(underline,"text-decoration: underline;");
        else
            sprintf(underline,"%s", "");

        if (xf->font)
            size=pWB->fonts.font[xf->font-1].height/20;
        else
            size=10;

        if (xf->font)
            sprintf(fontname,"%s",pWB->fonts.font[xf->font-1].name);
        else
            sprintf(fontname,"Arial");

        background=xls_getColor((WORD)(xf->groundcolor & 0x7f),1);
        sprintf(buf,".xf%i{ font-size:%ipt;font-family: \"%s\";background:#%.6X;text-align:%s;vertical-align:%s;%s%s%s%s%s%s%s%s}\n",
                i,size,fontname,background,align,valign,borderleft,borderright,bordertop,borderbottom,color,italic,bold,underline);

		strcat(ret,buf);
    }
	ret = realloc(ret, strlen(ret)+1);
	free(buf);

    return ret;
}
