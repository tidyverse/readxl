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

struct str_brdb
{
    WORD opcode;
    char * name;			/* printable name */
    char * desc;			/* printable description */
};
typedef struct str_brdb record_brdb;

record_brdb brdb[] =
    {
#include <libxls/brdb.c.h>
    };

static int get_brbdnum(int id)
{

    int i;
    i=0;
    do
    {
        if (brdb[i].opcode==id)
            return i;
        i++;
    }
    while (brdb[i].opcode!=0xFFF);
    return 0;
}
