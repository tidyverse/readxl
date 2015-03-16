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
 * Copyright 2008-2012 David Hoerl
 *
 */

#ifndef XLS_INCLUDE
#define XLS_INCLUDE
 
#ifdef __cplusplus
namespace xls {
extern "C" {
#endif

#include "libxls/xlstypes.h"
#include "libxls/xlsstruct.h"
#include "libxls/xlstool.h"


#define BLANK_CELL		0x0201


extern const char* xls_getVersion(void);

extern int xls(int debug);	// Set debug. Force library to load?
extern void xls_set_formula_hander(xls_formula_handler handler);

extern void xls_parseWorkBook(xlsWorkBook* pWB);
extern void xls_parseWorkSheet(xlsWorkSheet* pWS);

extern xlsWorkBook* xls_open(const char *file,const char *charset);	// convert 16bit strings within the spread sheet to this 8-bit encoding (UTF-8 default)
#define xls_close xls_close_WB                  // historical
extern void xls_close_WB(xlsWorkBook* pWB);     // preferred name

extern xlsWorkSheet * xls_getWorkSheet(xlsWorkBook* pWB,int num);
extern void xls_close_WS(xlsWorkSheet* pWS);

extern xlsSummaryInfo *xls_summaryInfo(xlsWorkBook* pWB);
extern void xls_close_summaryInfo(xlsSummaryInfo *pSI);

// utility function
xlsRow *xls_row(xlsWorkSheet* pWS, WORD cellRow);
xlsCell	*xls_cell(xlsWorkSheet* pWS, WORD cellRow, WORD cellCol);

#ifdef __cplusplus
} // extern c block
} // namespace
#endif

#endif

