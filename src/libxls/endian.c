/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Copyright 2013 Bob Colbert
 *
 * This file is part of libxls -- A multiplatform, C/C++ library for parsing
 * Excel(TM) files.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS''AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdlib.h>

#include "../include/libxls/xlstypes.h"
#include "../include/libxls/endian.h"
#include "../include/libxls/ole.h"

int xls_is_bigendian(void)
{
#if defined (__BIG_ENDIAN__)
    return 1;
#elif defined (__LITTLE_ENDIAN__)
    return 0;
#else
    static int n = 1;

    return (*(char *)&n == 0);
#endif
}

DWORD xlsIntVal (DWORD i)
{
    unsigned char c1, c2, c3, c4;

    if (xls_is_bigendian()) {
        c1 = i & 255;
        c2 = (i >> 8) & 255;
        c3 = (i >> 16) & 255;
        c4 = (i >> 24) & 255;

        return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
    } else {
        return i;
    }
}

unsigned short xlsShortVal (short s)
{
    unsigned char c1, c2;
    
    if (xls_is_bigendian()) {
        c1 = s & 255;
        c2 = (s >> 8) & 255;
    
        return (c1 << 8) + c2;
    } else {
        return s;
    }
}

void xlsConvertDouble(unsigned char *d)
{
    unsigned char t;
    int i;

    if (xls_is_bigendian()) {
        for (i=0; i<4; i++)
        {
            t = d[7-i];
            d[7-i] = d[i];
            d[i] = t;
        }
    }
}

void xlsConvertBof(BOF *b)
{
    b->id = xlsShortVal(b->id);
    b->size = xlsShortVal(b->size);
}

void xlsConvertBiff(BIFF *b)
{
    b->ver = xlsShortVal(b->ver);
    b->type = xlsShortVal(b->type);
    b->id_make = xlsShortVal(b->id_make);
    b->year = xlsShortVal(b->year);
    b->flags = xlsIntVal(b->flags);
    b->min_ver = xlsIntVal(b->min_ver);
}

void xlsConvertWindow(WIND1 *w)
{
    w->xWn = xlsShortVal(w->xWn);
    w->yWn = xlsShortVal(w->yWn);
    w->dxWn = xlsShortVal(w->dxWn);
    w->dyWn = xlsShortVal(w->dyWn);
    w->grbit = xlsShortVal(w->grbit);
    w->itabCur = xlsShortVal(w->itabCur);
    w->itabFirst = xlsShortVal(w->itabFirst);
    w->ctabSel = xlsShortVal(w->ctabSel);
    w->wTabRatio = xlsShortVal(w->wTabRatio);
}

void xlsConvertSst(SST *s)
{
    s->num = xlsIntVal(s->num);
    s->numofstr = xlsIntVal(s->numofstr);
}

void xlsConvertXf5(XF5 *x)
{
    x->font=xlsShortVal(x->font);
    x->format=xlsShortVal(x->format);
    x->type=xlsShortVal(x->type);
    x->align=xlsShortVal(x->align);
    x->color=xlsShortVal(x->color);
    x->fill=xlsShortVal(x->fill);
    x->border=xlsShortVal(x->border);
    x->linestyle=xlsShortVal(x->linestyle);
}

void xlsConvertXf8(XF8 *x)
{
    W_ENDIAN(x->font);
    W_ENDIAN(x->format);
    W_ENDIAN(x->type);
    D_ENDIAN(x->linestyle);
    D_ENDIAN(x->linecolor);
    W_ENDIAN(x->groundcolor);
}

void xlsConvertFont(FONT *f)
{
    W_ENDIAN(f->height);
    W_ENDIAN(f->flag);
    W_ENDIAN(f->color);
    W_ENDIAN(f->bold);
    W_ENDIAN(f->escapement);
}

void xlsConvertFormat(FORMAT *f)
{
    W_ENDIAN(f->index);
}

void xlsConvertBoundsheet(BOUNDSHEET *b)
{
    D_ENDIAN(b->filepos);
}

void xlsConvertColinfo(COLINFO *c)
{
    W_ENDIAN(c->first);
    W_ENDIAN(c->last);
    W_ENDIAN(c->width);
    W_ENDIAN(c->xf);
    W_ENDIAN(c->flags);
}

void xlsConvertRow(ROW *r)
{
    W_ENDIAN(r->index);
    W_ENDIAN(r->fcell);
    W_ENDIAN(r->lcell);
    W_ENDIAN(r->height);
    W_ENDIAN(r->notused);
    W_ENDIAN(r->notused2);
    W_ENDIAN(r->flags);
    W_ENDIAN(r->xf);
}

void xlsConvertMergedcells(MERGEDCELLS *m)
{
    W_ENDIAN(m->rowf);
    W_ENDIAN(m->rowl);
    W_ENDIAN(m->colf);
    W_ENDIAN(m->coll);
}

void xlsConvertCol(COL *c)
{
    W_ENDIAN(c->row);
    W_ENDIAN(c->col);
    W_ENDIAN(c->xf);
}

void xlsConvertFormula(FORMULA *f)
{
    W_ENDIAN(f->row);
    W_ENDIAN(f->col);
    W_ENDIAN(f->xf);
	if(f->res == 0xFFFF) {
		switch(f->resid) {
		case 0: // string
		case 1: // bool
		case 2: // error
		case 3: // empty string
			break;
		default:
			xlsConvertDouble(&f->resid);
			break;
		}
	} else {
		xlsConvertDouble(&f->resid);
	}

    W_ENDIAN(f->flags);
    W_ENDIAN(f->len);
    //fflush(stdout); left over from debugging?
}

void xlsConvertFormulaArray(FARRAY *f)
{
    W_ENDIAN(f->row1);
    W_ENDIAN(f->row2);
    W_ENDIAN(f->col1);
    W_ENDIAN(f->col2);
    W_ENDIAN(f->flags);
    W_ENDIAN(f->len);
}

void xlsConvertHeader(OLE2Header *h)
{
    unsigned long i;
    for (i=0; i<sizeof(h->id)/sizeof(h->id[0]); i++)
        h->id[i] = xlsIntVal(h->id[i]);
    for (i=0; i<sizeof(h->clid)/sizeof(h->clid[0]); i++)
        h->clid[i] = xlsIntVal(h->clid[i]);
    h->verminor  = xlsShortVal(h->verminor);
    h->verdll    = xlsShortVal(h->verdll);
    h->byteorder = xlsShortVal(h->byteorder);
    h->lsectorB  = xlsShortVal(h->lsectorB);
    h->lssectorB = xlsShortVal(h->lssectorB);
    h->reserved1 = xlsShortVal(h->reserved1);
    h->reserved2 = xlsIntVal(h->reserved2);
    h->reserved3 = xlsIntVal(h->reserved3);

    h->cfat      = xlsIntVal(h->cfat);
    h->dirstart  = xlsIntVal(h->dirstart);

    h->reserved4 = xlsIntVal(h->reserved4);

    h->sectorcutoff = xlsIntVal(h->sectorcutoff);
    h->sfatstart = xlsIntVal(h->sfatstart);
    h->csfat = xlsIntVal(h->csfat);
    h->difstart = xlsIntVal(h->difstart);
    h->cdif = xlsIntVal(h->cdif);
    for (i=0; i<sizeof(h->MSAT)/sizeof(h->MSAT[0]); i++)
        h->MSAT[i] = xlsIntVal(h->MSAT[i]);
}

void xlsConvertPss(PSS* pss)
{
    int i;
    pss->bsize = xlsShortVal(pss->bsize);
    pss->left  = xlsIntVal(pss->left);
    pss->right  = xlsIntVal(pss->right);
    pss->child  = xlsIntVal(pss->child);

    for(i=0; i<8; i++)
        pss->guid[i]=xlsShortVal(pss->guid[i]);
    pss->userflags  = xlsIntVal(pss->userflags);
/*    TIME_T	time[2]; */
    pss->sstart  = xlsIntVal(pss->sstart);
    pss->size  = xlsIntVal(pss->size);
    pss->proptype  = xlsIntVal(pss->proptype);
}
