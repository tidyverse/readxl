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

#include <libxls/xlsstruct.h>

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

void dumpbuf(char* fname,long size,BYTE* buf);
void verbose(char* str);
char* unicode_decode(const BYTE *s, int len, int *newlen, const char* encoding);
char* get_string(BYTE *s,BYTE is2, BYTE fmt, char *charset);
DWORD xls_getColor(const WORD color,WORD def);

extern void xls_showBookInfo(xlsWorkBook* pWB);
extern void xls_showROW(struct st_row_data* row);
extern void xls_showColinfo(struct st_colinfo_data* col);
extern void xls_showCell(struct st_cell_data* cell);
extern void xls_showFont(struct st_font_data* font);
extern void xls_showXF(struct st_xf_data* xf);
extern void xls_showFormat(struct st_format_data* format);
extern char* xls_getfcell(xlsWorkBook* pWB,struct st_cell_data* cell);
extern char* xls_getCSS(xlsWorkBook* pWB);
extern void xls_showBOF(BOF* bof);
