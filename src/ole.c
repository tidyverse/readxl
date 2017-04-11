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

#include "config.h" 

#include <memory.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include "libxls/ole.h"
#include "libxls/xlstool.h"
#include "libxls/endian.h"

extern int xls_debug;

//#define OLE_DEBUG

//static const DWORD MSATSECT		= 0xFFFFFFFC;	// -4
//static const DWORD FATSECT		= 0xFFFFFFFD;	// -3
static const DWORD ENDOFCHAIN	= 0xFFFFFFFE;	// -2
static const DWORD FREESECT		= 0xFFFFFFFF;	// -1

static size_t sector_pos(OLE2* ole2, size_t sid);
static int sector_read(OLE2* ole2, BYTE *buffer, size_t sid);
static size_t read_MSAT(OLE2* ole2, OLE2Header *oleh);

// Read next sector of stream
void ole2_bufread(OLE2Stream* olest) 
{
	BYTE *ptr;

	assert(olest);
	assert(olest->ole);

    if ((DWORD)olest->fatpos!=ENDOFCHAIN)
    {
		if(olest->sfat) {
			assert(olest->ole->SSAT);
			assert(olest->buf);
			assert(olest->ole->SSecID);

			ptr = olest->ole->SSAT + olest->fatpos*olest->ole->lssector;
			memcpy(olest->buf, ptr, olest->bufsize); 

			olest->fatpos=xlsIntVal(olest->ole->SSecID[olest->fatpos]);
			olest->pos=0;
			olest->cfat++;
		} else {

			assert(olest->fatpos >= 0);

			//printf("fatpos: %d max=%u\n",olest->fatpos, (olest->ole->cfat*olest->ole->lsector)/4);
			if(olest->fatpos > (olest->ole->cfat*olest->ole->lsector)/4) exit(-1);

#if 0 // TODO: remove
			fseek(olest->ole->file,olest->fatpos*olest->ole->lsector+512,0);
			ret = fread(olest->buf,1,olest->bufsize,olest->ole->file);
			assert(ret == olest->bufsize);
#endif
			assert((int)olest->fatpos >= 0);
			sector_read(olest->ole, olest->buf, olest->fatpos);
			//printf("Fat val: %d[0x%X]\n",olest->fatpos,olest->ole->SecID[olest->fatpos], olest->ole->SecID[olest->fatpos]);
			olest->fatpos=xlsIntVal(olest->ole->SecID[olest->fatpos]);
			olest->pos=0;
			olest->cfat++;
		}
    }
	// else printf("ENDOFCHAIN!!!\n");
}

// Read part of stream
size_t ole2_read(void* buf,size_t size,size_t count,OLE2Stream* olest)
{
    size_t didReadCount=0;
    size_t totalReadCount;
	size_t needToReadCount;

	totalReadCount=size*count;

	// olest->size inited to -1
	// printf("===== ole2_read(%ld bytes)\n", totalReadCount);

    if ((long)olest->size>=0 && !olest->sfat)	// directory is -1
    {
		size_t rem;
		rem = olest->size - (olest->cfat*olest->ole->lsector+olest->pos);		
        totalReadCount = rem<totalReadCount?rem:totalReadCount;
        if (rem<=0) olest->eof=1;

		// printf("  rem=%ld olest->size=%d - subfunc=%d\n", rem, olest->size, (olest->cfat*olest->ole->lsector+olest->pos) );
		//printf("  totalReadCount=%d (rem=%d size*count=%ld)\n", totalReadCount, rem, size*count);
	}

	while ((!olest->eof) && (didReadCount!=totalReadCount))
	{
		unsigned long remainingBytes;

		needToReadCount	= totalReadCount - didReadCount;
		remainingBytes	= olest->bufsize - olest->pos;
		//printf("  test: (totalReadCount-didReadCount)=%d (olest->bufsize-olest->pos)=%d\n", (totalReadCount-didReadCount), (olest->bufsize-olest->pos) );

		if (needToReadCount < remainingBytes)	// does the current sector contain all the data I need?
		{
			// printf("  had %d bytes of memory, copy=%d\n", (olest->bufsize-olest->pos), needToReadCount);
			memcpy((BYTE*)buf + didReadCount, olest->buf + olest->pos, needToReadCount);
			olest->pos		+= needToReadCount;
			didReadCount	+= needToReadCount;
		} else {
			// printf("  had %d bytes of memory, copy=%d\n", remainingBytes, remainingBytes);
			memcpy((BYTE*)buf + didReadCount, olest->buf + olest->pos, remainingBytes);
			olest->pos		+= remainingBytes;
			didReadCount	+= remainingBytes;
			ole2_bufread(olest);
		}
		assert(didReadCount <= totalReadCount);
		//printf("  if(fatpos=0x%X==EOC=0x%X) && (pos=%d >= bufsize=%d)\n", olest->fatpos, ENDOFCHAIN, olest->pos, olest->bufsize);
		if (((DWORD)olest->fatpos == ENDOFCHAIN) && (olest->pos >= olest->bufsize))
		{
			olest->eof=1;
		}

		//printf("  eof=%d (didReadCount=%ld != totalReadCount=%ld)\n", olest->eof, didReadCount, totalReadCount);
	}
	// printf("  didReadCount=%ld EOF=%d\n", didReadCount, olest->eof);
	// printf("=====\n");

#ifdef OLE_DEBUG
    printf("----------------------------------------------\n");
    printf("ole2_read (end)\n");
    printf("start:		%li \n",olest->start);
    printf("pos:		%li \n",olest->pos);
    printf("cfat:		%d \n",olest->cfat);
    printf("size:		%d \n",olest->size);
    printf("fatpos:		%li \n",olest->fatpos);
    printf("bufsize:		%li \n",olest->bufsize);
    printf("eof:		%d \n",olest->eof);
#endif

    return(didReadCount);
}

// Open stream in logical ole file
OLE2Stream* ole2_sopen(OLE2* ole,DWORD start, size_t size)
{
    OLE2Stream* olest=NULL;

#ifdef OLE_DEBUG
    printf("----------------------------------------------\n");
    printf("ole2_sopen start=%lXh\n", start);
#endif

	olest=(OLE2Stream*)calloc(1, sizeof(OLE2Stream));
	olest->ole=ole;
	olest->size=size;
	olest->fatpos=start;
	olest->start=start;
	olest->pos=0;
	olest->eof=0;
	olest->cfat=-1;
	if((long)size > 0 && size < (size_t)ole->sectorcutoff) {
		olest->bufsize=ole->lssector;
		olest->sfat = 1;
	} else {
		olest->bufsize=ole->lsector;
	}
	olest->buf=malloc(olest->bufsize);
	ole2_bufread(olest);

	// if(xls_debug) printf("sopen: sector=%d next=%d\n", start, olest->fatpos);
    return olest;
}

// Move in stream
void ole2_seek(OLE2Stream* olest,DWORD ofs)
{
	if(olest->sfat) {
		ldiv_t div_rez=ldiv(ofs,olest->ole->lssector);
		int i;
		olest->fatpos=olest->start;

		if (div_rez.quot!=0)
		{
			for (i=0;i<div_rez.quot;i++)
				olest->fatpos=xlsIntVal(olest->ole->SSecID[olest->fatpos]);
		}

		ole2_bufread(olest);
		olest->pos=div_rez.rem;
		olest->eof=0;
		olest->cfat=div_rez.quot;
		//printf("%i=%i %i\n",ofs,div_rez.quot,div_rez.rem);
	} else {
		ldiv_t div_rez=ldiv(ofs,olest->ole->lsector);
		int i;
		olest->fatpos=olest->start;

		if (div_rez.quot!=0)
		{
			for (i=0;i<div_rez.quot;i++)
				olest->fatpos=xlsIntVal(olest->ole->SecID[olest->fatpos]);
		}

		ole2_bufread(olest);
		olest->pos=div_rez.rem;
		olest->eof=0;
		olest->cfat=div_rez.quot;
		//printf("%i=%i %i\n",ofs,div_rez.quot,div_rez.rem);
	}
}

// Open logical file contained in physical OLE file
OLE2Stream*  ole2_fopen(OLE2* ole,BYTE* file)
{
    OLE2Stream* olest;
    int i;

#ifdef OLE_DEBUG
    printf("----------------------------------------------\n");
    printf("ole2_fopen %s\n", file);
#endif

    for (i=0;i<ole->files.count;i++) {
		BYTE *str = ole->files.file[i].name;
#ifdef OLE_DEBUG
		printf("----------------------------------------------\n");
		printf("ole2_fopen found %s\n", str);
#endif
        if (str && strcmp((char *)str,(char *)file)==0)	// newer versions of Excel don't write the "Root Entry" string for the first set of data
        {
            olest=ole2_sopen(ole,ole->files.file[i].start,ole->files.file[i].size);
            return(olest);
        }
	}
    return(NULL);
}

// Open physical file
OLE2* ole2_open(const BYTE *file)
{
    //BYTE buf[1024];
    OLE2Header* oleh;
    OLE2* ole;
    OLE2Stream* olest;
    PSS*	pss;
    BYTE* name = NULL;

#ifdef OLE_DEBUG
    printf("----------------------------------------------\n");
    printf("ole2_open %s\n", file);
#endif

	if(xls_debug) printf("ole2_open: %s\n", file);
    ole=(OLE2*)calloc(1, sizeof(OLE2));
    if (!(ole->file=fopen((char *)file,"rb")))
    {
        if(xls_debug) printf("File not found\n");
        free(ole);
        return(NULL);
    }
    // read header and check magic numbers
    oleh=(OLE2Header*)malloc(512);
    fread(oleh,1,512,ole->file);
    xlsConvertHeader(oleh);

	// make sure the file looks good. Note: this code only works on Little Endian machines
	if(oleh->id[0] != 0xE011CFD0 || oleh->id[1] != 0xE11AB1A1 || oleh->byteorder != 0xFFFE) {
		fclose(ole->file);
        printf("Not an excel file\n");
		free(ole);
		return NULL;
	}

    //ole->lsector=(WORD)pow(2,oleh->lsector);
    //ole->lssector=(WORD)pow(2,oleh->lssector);
	ole->lsector=512;
    ole->lssector=64;
	assert(oleh->lsectorB==9);	// 2**9 == 512
	assert(oleh->lssectorB==6);	// 2**6 == 64
	
    ole->cfat=oleh->cfat;
    ole->dirstart=oleh->dirstart;
    ole->sectorcutoff=oleh->sectorcutoff;
    ole->sfatstart=oleh->sfatstart;
    ole->csfat=oleh->csfat;
    ole->difstart=oleh->difstart;
    ole->cdif=oleh->cdif;
    ole->files.count=0;

#ifdef OLE_DEBUG
		printf("==== OLE HEADER ====\n");
		//printf ("Header Size:   %i \n", sizeof(OLE2Header));
		//printf ("id[0]-id[1]:   %X-%X \n", oleh->id[0], oleh->id[1]);
		printf ("verminor:      %X \n",oleh->verminor);
		printf ("verdll:        %X \n",oleh->verdll);
		//printf ("Byte order:    %X \n",oleh->byteorder);
		printf ("sect len:      %X (%i)\n",ole->lsector,ole->lsector);		// ole
		printf ("mini len:      %X (%i)\n",ole->lssector,ole->lssector);	// ole
		printf ("Fat sect.:     %i \n",oleh->cfat);
		printf ("Dir Start:     %i \n",oleh->dirstart);
		
		printf ("Mini Cutoff:   %i \n",oleh->sectorcutoff);
		printf ("MiniFat Start: %X \n",oleh->sfatstart);
		printf ("Count MFat:    %i \n",oleh->csfat);
		printf ("Dif start:     %X \n",oleh->difstart);
		printf ("Count Dif:     %i \n",oleh->cdif);
		printf ("Fat Size:      %u (0x%X) \n",oleh->cfat*ole->lsector,oleh->cfat*ole->lsector);
#endif
    // read directory entries
    read_MSAT(ole, oleh);

	// reuse this buffer
    pss = (PSS*)oleh;
	// oleh = (void *)NULL; // Not needed as oleh not used from here on
	
    olest=ole2_sopen(ole,ole->dirstart, -1);
    do
    {
        ole2_read(pss,1,sizeof(PSS),olest);
        xlsConvertPss(pss);
        name=unicode_decode(pss->name, pss->bsize, 0, "UTF-8");
#ifdef OLE_DEBUG	
		printf("OLE NAME: %s count=%d\n", name, ole->files.count);
#endif
        if (pss->type == PS_USER_ROOT || pss->type == PS_USER_STREAM) // (name!=NULL) // 
        {

#ifdef OLE_DEBUG		
			printf("OLE TYPE: %s file=%d \n", pss->type == PS_USER_ROOT ? "root" : "user", ole->files.count);
#endif		
            if (ole->files.count==0)
            {
                ole->files.file=malloc(sizeof(struct st_olefiles_data));
            } else {
                ole->files.file=realloc(ole->files.file,(ole->files.count+1)*sizeof(struct st_olefiles_data));
            }
            ole->files.file[ole->files.count].name=name;
            ole->files.file[ole->files.count].start=pss->sstart;
            ole->files.file[ole->files.count].size=pss->size;
            ole->files.count++;
			
			if(pss->sstart == ENDOFCHAIN) {
				if (xls_debug) verbose("END OF CHAIN\n");
			} else
			if(pss->type == PS_USER_STREAM) {
#ifdef OLE_DEBUG
					printf("----------------------------------------------\n");
					printf("name: %s (size=%d [c=%c])\n", name, pss->bsize, name ? name[0]:' ');
					printf("bsize %i\n",pss->bsize);
					printf("type %i\n",pss->type);
					printf("flag %i\n",pss->flag);
					printf("left %X\n",pss->left);
					printf("right %X\n",pss->right);
					printf("child %X\n",pss->child);
					printf("guid %.4X-%.4X-%.4X-%.4X %.4X-%.4X-%.4X-%.4X\n",pss->guid[0],pss->guid[1],pss->guid[2],pss->guid[3],
						pss->guid[4],pss->guid[5],pss->guid[6],pss->guid[7]);
					printf("user flag %.4X\n",pss->userflags);
					printf("sstart %.4d\n",pss->sstart);
					printf("size %.4d\n",pss->size);
#endif
			} else
			if(pss->type == PS_USER_ROOT) {
				DWORD sector, k, blocks;
				BYTE *wptr;
				
				blocks = (pss->size + (ole->lsector - 1)) / ole->lsector;	// count partial
				ole->SSAT = (BYTE *)malloc(blocks*ole->lsector);
				// printf("blocks %d\n", blocks);

				assert(ole->SSecID);
				
				sector = pss->sstart;
				wptr=(BYTE*)ole->SSAT;
				for(k=0; k<blocks; ++k) {
					// printf("block %d sector %d\n", k, sector);
					assert(sector != ENDOFCHAIN);
					fseek(ole->file,sector*ole->lsector+512,0);
					fread(wptr,1,ole->lsector,ole->file);
					wptr += ole->lsector;
					sector = xlsIntVal(ole->SecID[sector]);
				}
			}	
		} else {
			free(name);
		}
    }
    while (!olest->eof);

	ole2_fclose(olest);
    free(pss);

    return ole;
}

void ole2_close(OLE2* ole2)
{
    int i;
	fclose(ole2->file);

	for(i=0; i<ole2->files.count; ++i) {
		free(ole2->files.file[i].name);
	}
	free(ole2->files.file);
	free(ole2->SecID);
	free(ole2->SSecID);
	free(ole2->SSAT);
	free(ole2);
}

void ole2_fclose(OLE2Stream* ole2st)
{
	free(ole2st->buf);
	free(ole2st);
}

// Return offset in bytes of a sector from its sid
static size_t sector_pos(OLE2* ole2, size_t sid)
{
    return 512 + sid * ole2->lsector;
}
// Read one sector from its sid
static int sector_read(OLE2* ole2, BYTE *buffer, size_t sid)
{
	size_t num;
	size_t seeked;

	//printf("sector_read: sid=%zu (0x%zx) lsector=%u sector_pos=%zu\n", sid, sid, ole2->lsector, sector_pos(ole2, sid) );
    seeked = fseek(ole2->file, sector_pos(ole2, sid), SEEK_SET);
	if(seeked != 0) {
		printf("seek: wanted to seek to sector %zu (0x%zx) loc=%zu\n", sid, sid, sector_pos(ole2, sid));
	}
	assert(seeked == 0);
	
	num = fread(buffer, ole2->lsector, 1, ole2->file);
	if(num != 1) {
		fprintf(stderr, "fread: wanted 1 got %zu loc=%zu\n", num, sector_pos(ole2, sid));
	}
	assert(num == 1);

    return 0;
}

// Read MSAT
static size_t read_MSAT(OLE2* ole2, OLE2Header* oleh)
{
    int sectorNum;

    // reconstitution of the MSAT
    ole2->SecID=malloc(ole2->cfat*ole2->lsector);

    // read first 109 sectors of MSAT from header
    {
        int count;
        count = (ole2->cfat < 109) ? ole2->cfat : 109;
        for (sectorNum = 0; sectorNum < count; sectorNum++)
        {
			assert(sectorNum >= 0);
            sector_read(ole2, (BYTE*)(ole2->SecID)+sectorNum*ole2->lsector, oleh->MSAT[sectorNum]);
        }
    }

    // Add additionnal sectors of the MSAT
    {
        DWORD sid = ole2->difstart;

		BYTE *sector = malloc(ole2->lsector);
		//printf("sid=%u (0x%x) sector=%u\n", sid, sid, ole2->lsector);
        while (sid != ENDOFCHAIN && sid != FREESECT) // FREESECT only here due to an actual file that requires it (old Apple Numbers bug)
		{
           int posInSector;
           // read MSAT sector
           sector_read(ole2, sector, sid);

           // read content
           for (posInSector = 0; posInSector < (ole2->lsector-4)/4; posInSector++)
		   {
              DWORD s = *(DWORD_UA *)(sector + posInSector*4);
              //printf("   s[%d]=%d (0x%x)\n", posInSector, s, s);

              if (s != FREESECT)
                {
                 sector_read(ole2, (BYTE*)(ole2->SecID)+sectorNum*ole2->lsector, s);
                 sectorNum++;
                }
			}
			sid = *(DWORD_UA *)(sector + posInSector*4);
			//printf("   s[%d]=%d (0x%x)\n", posInSector, sid, sid);
		}
		free(sector);
    }
#ifdef OLE_DEBUG
	if(xls_debug) {
		//printf("==== READ IN SECTORS FOR MSAT TABLE====\n");
		int i;
		for(i=0; i<512/4; ++i) {	// just the first block
			if(ole2->SecID[i] != FREESECT) printf("SecID[%d]=%d\n", i, ole2->SecID[i]);
		}
	}
	//exit(0);
#endif

	// read in short table
	if(ole2->sfatstart != ENDOFCHAIN) {
		DWORD sector, k;
		BYTE *wptr;
		
		ole2->SSecID = (DWORD *)malloc(ole2->csfat*ole2->lsector);
		sector = ole2->sfatstart;
		wptr=(BYTE*)ole2->SSecID;
		for(k=0; k<ole2->csfat; ++k) {
			assert(sector != ENDOFCHAIN);
			fseek(ole2->file,sector*ole2->lsector+512,0);
			fread(wptr,1,ole2->lsector,ole2->file);
			wptr += ole2->lsector;
			sector = ole2->SecID[sector];
		}
#ifdef OLE_DEBUG
		if(xls_debug) {
			int i;
			for(i=0; i<512/4; ++i) {
				if(ole2->SSecID[i] != FREESECT) printf("SSecID[%d]=%d\n", i, ole2->SSecID[i]);
			}
		}
#endif
	}

    return 0;
}


