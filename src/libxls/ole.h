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

#ifndef OLE_INCLUDE
#define OLE_INCLUDE

#pragma pack(1)

#include <stdio.h>			// FILE *
#include <libxls/xlstypes.h>

typedef struct TIME_T
{
    DWORD	LowDate;
    DWORD	HighDate;
}
TIME_T;

typedef struct OLE2Header
{
    DWORD		id[2];		//D0CF11E0 A1B11AE1
    DWORD		clid[4];
    WORD		verminor;	//0x3e
    WORD		verdll;		//0x03
    WORD		byteorder;
    WORD		lsectorB;
    WORD		lssectorB;

    WORD		reserved1;
    DWORD		reserved2;
    DWORD		reserved3;

    DWORD		cfat;			// count full sectors
    DWORD		dirstart;

    DWORD		reserved4;

    DWORD		sectorcutoff;	// min size of a standard stream ; if less than this then it uses short-streams
    DWORD		sfatstart;		// first short-sector or EOC
    DWORD		csfat;			// count short sectors
    DWORD		difstart;		// first sector master sector table or EOC
    DWORD		cdif;			// total count
    DWORD		MSAT[109];		// First 109 MSAT
}
OLE2Header;


//-----------------------------------------------------------------------------------
typedef	struct st_olefiles
{
    long count;
    struct st_olefiles_data
    {
        char*	name;
        DWORD	start;
        DWORD	size;
   }
    * file;
}
st_olefiles;

typedef struct OLE2
{
    FILE*		file;
    WORD		lsector;
    WORD		lssector;
    DWORD		cfat;
    DWORD		dirstart;

    DWORD		sectorcutoff;
    DWORD		sfatstart;
    DWORD		csfat;
    DWORD		difstart;
    DWORD		cdif;
    DWORD*		SecID;	// regular sector data
	DWORD*		SSecID;	// short sector data
	BYTE*		SSAT;	// directory of short sectors
    st_olefiles	files;
}
OLE2;

typedef struct OLE2Stream
{
    OLE2*	ole;
    DWORD	start;
    DWORD	pos;
    int		cfat;
    int		size;
    DWORD	fatpos;
    BYTE*	buf;
    DWORD	bufsize;
    BYTE	eof;
	BYTE	sfat;	// short
}
OLE2Stream;

typedef struct PSS
{
    BYTE	name[64];
    WORD	bsize;
    BYTE	type;		//STGTY
#define PS_EMPTY		00
#define PS_USER_STORAGE	01
#define PS_USER_STREAM	02
#define PS_USER_ROOT	05
    BYTE	flag;		//COLOR
#define BLACK	1
    DWORD	left;
    DWORD	right;
    DWORD	child;
    WORD	guid[8];
    DWORD	userflags;
    TIME_T	time[2];
    DWORD	sstart;
    DWORD	size;
    DWORD	proptype;
}
PSS;

extern int ole2_read(void* buf,long size,long count,OLE2Stream* olest);
extern OLE2Stream* ole2_sopen(OLE2* ole,DWORD start, int size);
extern void ole2_seek(OLE2Stream* olest,DWORD ofs);
extern OLE2Stream*  ole2_fopen(OLE2* ole,char* file);
extern void ole2_fclose(OLE2Stream* ole2st);
extern OLE2* ole2_open(char *file, char *charset);
extern void ole2_close(OLE2* ole2);
extern void ole2_bufread(OLE2Stream* olest);

#endif
