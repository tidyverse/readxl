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

#include <libxls/ole.h>

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


typedef struct FORMULA
{
    WORD	row;
    WORD	col;
    WORD	xf;
    //	ULLONG  res;
    BYTE	resid;
    BYTE	resdata[5];
    WORD	res;
    //	double	res;
    WORD	flags;
    BYTE	chn[4];
    WORD	len;
    BYTE	value[1]; //var
}
FORMULA;

typedef struct RK
{
    WORD	row;
    WORD	col;
    WORD	xf;
    BYTE	value[4];
}
RK;

typedef struct LABELSST
{
    WORD	row;
    WORD	col;
    WORD	xf;
    BYTE	value[4];
}
LABELSST;


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
    BYTE	value[1];
}
FORMAT;

//---------------------------------------------------------
typedef	struct st_sheet
{		//Sheets
    long count;
    struct st_sheet_data
    {
        DWORD filepos;
        BYTE visibility;
        BYTE type;
        char* name;
    }
    * sheet;
}
st_sheet;

typedef	struct st_font
{
    long count;		//Count of FONT's
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
        char*	name;
    }
    * font;
}
st_font;

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

typedef	struct st_xf
{
    long count;	//Count of XF
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
        //	long len;
        char* str;
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
        double	d;
        long	l;
        char*	str;		//String value;
        BYTE	ishiden;		//Is cell hident
        WORD	width;		//Width of col
        WORD	colspan;
        WORD	rowspan;
    }
    * cell;
}
st_cell;


typedef	struct st_row
{
    //	DWORD count;
    WORD lastcol;
    WORD lastrow;
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
    long count;	//Count of COLINFO
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
    //FILE*		file;		//
    OLE2Stream*	olestr;
    long		filepos;	//position in file

    //From Header (BIFF)
    BYTE		is5ver;
    WORD		type;

    //Other data
    WORD		codepage;	//Charset codepage
    char*		charset;
    st_sheet	sheets;
    st_sst		sst;		//SST table
    st_xf		xfs;		//XF table
    st_font		fonts;
    st_format	formats;	//FORMAT table
}
xlsWorkBook;

typedef struct xlsWorkSheet
{
    DWORD		filepos;
    WORD		defcolwidth;
    st_row		rows;
    xlsWorkBook * 	workbook;
    st_colinfo	colinfo;
    WORD		maxcol;
}
xlsWorkSheet;
