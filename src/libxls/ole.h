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

#ifndef OLE_INCLUDE
#define OLE_INCLUDE

#include <stdio.h>			// FILE *

#include "libxls/xlstypes.h"

#ifdef AIX
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

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

#pragma pack(pop)

//-----------------------------------------------------------------------------------
typedef	struct st_olefiles
{
    long count;
    struct st_olefiles_data
    {
        BYTE*	name;
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
    size_t	pos;
    size_t	cfat;
    size_t	size;
    size_t	fatpos;
    BYTE*	buf;
    DWORD	bufsize;
    BYTE	eof;
	BYTE	sfat;	// short
}
OLE2Stream;

#ifdef AIX
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

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

#pragma pack(pop)

extern size_t ole2_read(void* buf,size_t size,size_t count,OLE2Stream* olest);
extern OLE2Stream* ole2_sopen(OLE2* ole,DWORD start, size_t size);
extern void ole2_seek(OLE2Stream* olest,DWORD ofs);
extern OLE2Stream*  ole2_fopen(OLE2* ole,BYTE* file);
extern void ole2_fclose(OLE2Stream* ole2st);
extern OLE2* ole2_open(const BYTE *file);
extern void ole2_close(OLE2* ole2);
extern void ole2_bufread(OLE2Stream* olest);


#endif
