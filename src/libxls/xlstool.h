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

#include "libxls/xlsstruct.h"

extern void dumpbuf(BYTE* fname,long size,BYTE* buf);
extern void verbose(char* str);

extern BYTE *utf8_decode(BYTE *str, DWORD len, char *encoding);
extern BYTE* unicode_decode(const BYTE *s, int len, size_t *newlen, const char* encoding);
extern BYTE* get_string(BYTE *s,BYTE is2, BYTE isUnicode, char *charset);
extern DWORD xls_getColor(const WORD color,WORD def);

extern void xls_showBookInfo(xlsWorkBook* pWB);
extern void xls_showROW(struct st_row_data* row);
extern void xls_showColinfo(struct st_colinfo_data* col);
extern void xls_showCell(struct st_cell_data* cell);
extern void xls_showFont(struct st_font_data* font);
extern void xls_showXF(XF8* xf);
extern void xls_showFormat(struct st_format_data* format);
extern BYTE* xls_getfcell(xlsWorkBook* pWB,struct st_cell_data* cell,WORD *label);
extern char* xls_getCSS(xlsWorkBook* pWB);
extern void xls_showBOF(BOF* bof);
