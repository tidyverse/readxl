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
 * Copyright 2013 Bob Colbert
 *
 */

#include <libxls/xlsstruct.h>

int xls_is_bigendian();
int xlsIntVal (int i);
unsigned short xlsShortVal (short s);

void xlsConvertHeader(OLE2Header *h);
void xlsConvertPss(PSS* pss);

void xlsConvertDouble(BYTE *d);
void xlsConvertBof(BOF *b);
void xlsConvertBiff(BIFF *b);
void xlsConvertWindow(WIND1 *w);
void xlsConvertSst(SST *s);
void xlsConvertXf5(XF5 *x);
void xlsConvertXf8(XF8 *x);
void xlsConvertFont(FONT *f);
void xlsConvertFormat(FORMAT *f);
void xlsConvertBoundsheet(BOUNDSHEET *b);
void xlsConvertColinfo(COLINFO *c);
void xlsConvertRow(ROW *r);
void xlsConvertMergedcells(MERGEDCELLS *m);
void xlsConvertCol(COL *c);
void xlsConvertFormula(FORMULA *f);
void xlsConvertFormulaArray(FARRAY *f);
void xlsConvertHeader(OLE2Header *h);
void xlsConvertPss(PSS* pss);
#if 0 // unused
void xlsConvertUnicode(wchar_t *w, char *s, int len);
#endif

#define W_ENDIAN(a) a=xlsShortVal(a)
#define D_ENDIAN(a) a=xlsIntVal(a)
