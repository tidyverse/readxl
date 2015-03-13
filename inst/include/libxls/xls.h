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

#include <libxls/xlstool.h>

extern const char* xls_getVersion(void);

extern int xls_debug;

extern int xls(void);

extern void xls_addSST(xlsWorkBook* pWB,SST* sst,DWORD size);
extern void xls_appendSST(xlsWorkBook* pWB,BYTE* buf,DWORD size);

extern void xls_addFormat(xlsWorkBook* pWB,FORMAT* format);
extern void xls_parseWorkBook(xlsWorkBook* pWB);
extern void xls_parseWorkSheet(xlsWorkSheet* pWS);

extern xlsWorkBook* xls_open(char *file,char* charset);
extern void xls_close(xlsWorkBook* pWB);
extern xlsWorkSheet * xls_getWorkSheet(xlsWorkBook* pWB,int num);
