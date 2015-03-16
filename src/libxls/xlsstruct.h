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

#ifndef XLS_STRUCT_INC
#define XLS_STRUCT_INC

#include "libxls/ole.h"

#ifdef AIX
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

typedef struct BOF
{
    WORD id;
    WORD size;
}
BOF;

typedef struct BIFF
{
    WORD ver;
    WORD type;
    WORD id_make;
    WORD year;
    DWORD flags;
    DWORD min_ver;
    BYTE buf[100];
}
BIFF;

typedef struct WIND1
{
    WORD xWn;
    WORD yWn;
    WORD dxWn;
    WORD dyWn;
    WORD grbit;
    WORD itabCur;
    WORD itabFirst;
    WORD ctabSel;
    WORD wTabRatio;
}
WIND1;

typedef struct BOUNDSHEET
{
    DWORD	filepos;
    BYTE	type;
    BYTE	visible;
    BYTE	name[];
}
BOUNDSHEET;

typedef struct ROW
{
    WORD	index;
    WORD	fcell;
    WORD	lcell;
    WORD	height;
    WORD	notused;
    WORD	notused2; //used only for BIFF3-4
    WORD	flags;
    WORD	xf;
}
ROW;

typedef struct COL
{
    WORD	row;
    WORD	col;
    WORD	xf;
}
COL;


typedef struct FORMULA // BIFF8
{
    WORD	row;
    WORD	col;
    WORD	xf;
	// next 8 bytes either a IEEE double, or encoded on a byte basis
    BYTE	resid;
    BYTE	resdata[5];
    WORD	res;
    WORD	flags;
    BYTE	chn[4]; // BIFF8
    WORD	len;
    BYTE	value[1]; //var
}
FORMULA;

typedef struct FARRAY // BIFF8
{
    WORD	row1;
    WORD	row2;
    BYTE	col1;
    BYTE	col2;
    WORD	flags;
    BYTE	chn[4]; // BIFF8
    WORD	len;
    BYTE	value[1]; //var
}
FARRAY;

typedef struct RK
{
    WORD	row;
    WORD	col;
    WORD	xf;
    DWORD_UA value;
}
RK;

typedef struct MULRK
{
    WORD	row;
    WORD	col;
	struct {
		WORD	xf;
		DWORD_UA value;
	}		rk[];
	//WORD	last_col;
}
MULRK;

typedef struct MULBLANK
{
    WORD	row;
    WORD	col;
    WORD	xf[];
	//WORD	last_col;
}
MULBLANK;

typedef struct BLANK
{
    WORD	row;
    WORD	col;
    WORD	xf;
}
BLANK;

typedef struct LABEL
{
    WORD	row;
    WORD	col;
    WORD	xf;
    BYTE	value[1]; // var
}
LABEL;
typedef LABEL LABELSST;

typedef struct SST
{
    DWORD	num;
    DWORD	numofstr;
    BYTE	strings;
}
SST;

typedef struct XF5
{
    WORD	font;
    WORD	format;
    WORD	type;
    WORD	align;
    WORD	color;
    WORD	fill;
    WORD	border;
    WORD	linestyle;
}
XF5;

typedef struct XF8
{
    WORD	font;
    WORD	format;
    WORD	type;
    BYTE	align;
    BYTE	rotation;
    BYTE	ident;
    BYTE	usedattr;
    DWORD	linestyle;
    DWORD	linecolor;
    WORD	groundcolor;
}
XF8;

typedef struct BR_NUMBER
{
    WORD	row;
    WORD	col;
    WORD	xf;
    double value;
}
BR_NUMBER;

typedef struct COLINFO
{
    WORD	first;
    WORD	last;
    WORD	width;
    WORD	xf;
    WORD	flags;
    WORD	notused;
}
COLINFO;

typedef struct MERGEDCELLS
{
    WORD	rowf;
    WORD	rowl;
    WORD	colf;
    WORD	coll;
}
MERGEDCELLS;

typedef struct FONT
{
    WORD	height;
    WORD	flag;
    WORD	color;
    WORD	bold;
    WORD	escapement;
    BYTE	underline;
    BYTE	family;
    BYTE	charset;
    BYTE	notused;
    BYTE	name;
}
FONT;

typedef struct FORMAT
{
    WORD	index;
    BYTE	value[0];
}
FORMAT;

#pragma pack(pop)

//---------------------------------------------------------

typedef	struct st_sheet
{
    DWORD count;        // Count of sheets
    struct st_sheet_data
    {
        DWORD filepos;
        BYTE visibility;
        BYTE type;
        BYTE* name;
    }
    * sheet;
}
st_sheet;

typedef	struct st_font
{
    DWORD count;		// Count of FONT's
    struct st_font_data
    {
        WORD	height;
        WORD	flag;
        WORD	color;
        WORD	bold;
        WORD	escapement;
        BYTE	underline;
        BYTE	family;
        BYTE	charset;
        BYTE*	name;
    }
    * font;
}
st_font;

typedef struct st_format
{
    DWORD count;		// Count of FORMAT's
    struct st_format_data
    {
         WORD index;
         BYTE *value;
    }
    * format;
}
st_format;

typedef	struct st_xf
{
    DWORD count;	// Count of XF
    //	XF** xf;
    struct st_xf_data
    {
        WORD	font;
        WORD	format;
        WORD	type;
        BYTE	align;
        BYTE	rotation;
        BYTE	ident;
        BYTE	usedattr;
        DWORD	linestyle;
        DWORD	linecolor;
        WORD	groundcolor;
    }
    * xf;
}
st_xf;


typedef	struct st_sst
{
    DWORD count;
    DWORD lastid;
    DWORD continued;
    DWORD lastln;
    DWORD lastrt;
    DWORD lastsz;
    struct str_sst_string
    {
        BYTE* str;
    }
    * string;
}
st_sst;


typedef	struct st_cell
{
    DWORD count;
    struct st_cell_data
    {
        WORD	id;
        WORD	row;
        WORD	col;
        WORD	xf;
        BYTE*	str;		// String value;
        double	d;
        int32_t	l;
        WORD	width;		// Width of col
        WORD	colspan;
        WORD	rowspan;
        BYTE	isHidden;	// Is cell hidden
    }
    * cell;
}
st_cell;


typedef	struct st_row
{
    //	DWORD count;
    WORD lastcol;	// numCols - 1
    WORD lastrow;	// numRows - 1
    struct st_row_data
    {
        WORD index;
        WORD fcell;
        WORD lcell;
        WORD height;
        WORD flags;
        WORD xf;
        BYTE xfflags;
        st_cell cells;
    }
    * row;
}
st_row;


typedef	struct st_colinfo
{
    DWORD count;				//Count of COLINFO
    struct st_colinfo_data
    {
        WORD	first;
        WORD	last;
        WORD	width;
        WORD	xf;
        WORD	flags;
    }
    * col;
}
st_colinfo;

typedef struct xlsWorkBook
{
    //FILE*		file;
    OLE2Stream*	olestr;
    int32_t		filepos;		// position in file

    //From Header (BIFF)
    BYTE		is5ver;
    BYTE		is1904;
    WORD		type;

    //Other data
    WORD		codepage;		//Charset codepage
    char*		charset;
    st_sheet	sheets;
    st_sst		sst;			//SST table
    st_xf		xfs;			//XF table
    st_font		fonts;
    st_format	formats;		//FORMAT table

	char		*summary;		// ole file
	char		*docSummary;	// ole file
}
xlsWorkBook;

typedef struct xlsWorkSheet
{
    DWORD		filepos;
    WORD		defcolwidth;
    st_row		rows;
    xlsWorkBook *workbook;
    st_colinfo	colinfo;
}
xlsWorkSheet;

#ifdef __cplusplus
typedef struct st_cell::st_cell_data xlsCell;
typedef	struct st_row::st_row_data xlsRow;
#else
typedef struct st_cell_data xlsCell;
typedef	struct st_row_data xlsRow;
#endif

typedef struct xls_summaryInfo
{
	BYTE		*title;
	BYTE		*subject;
	BYTE		*author;
	BYTE		*keywords;
	BYTE		*comment;
	BYTE		*lastAuthor;
	BYTE		*appName;
	BYTE		*category;
	BYTE		*manager;
	BYTE		*company;
}
xlsSummaryInfo;

typedef void (*xls_formula_handler)(WORD bof, WORD len, BYTE *formula);

#endif
