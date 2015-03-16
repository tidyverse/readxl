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
 * Copyright 2006 Christophe Leitienne
 * Copyright 2008-2012 David Hoerl
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "libxls/xls.h"

static char  stringSeparator = '\"';
static char *lineSeparator = "\n";
static char *fieldSeparator = ";";
static char *encoding = "UTF-8";


static void OutputString(const char *string);
static void OutputNumber(const double number);
static void Usage(char *progName);

static void Usage(char *progName)
{
    fprintf(stderr, "usage: %s <Excel xls file> [-l] [-e encoding] [-t sheet] [-q quote char] [-f field separator]\n", progName);
    fprintf(stderr, "  Output Excel file cells as delimited values (default is comma separated)\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -l            : list the sheets contained in the file but do not output their contents.\n");
    fprintf(stderr, "    -t sheet_name : only process the named sheet\n");
    fprintf(stderr, "    -e encoding   : the iconv encoding (default \"%s\")\n", encoding);
    fprintf(stderr, "    -q character  : used to quote strings (default '%c')\n", stringSeparator);
    fprintf(stderr, "    -f string     : used to separate fields (default \"%s\")\n", fieldSeparator);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

extern int getopt(int nargc, char * const *nargv, const char *ostr);

int main(int argc, char *argv[]) {
	xlsWorkBook* pWB;
	xlsWorkSheet* pWS;
	unsigned int i;
    int justList = 0;
    char *sheetName = "";

    if(argc < 2) {
        Usage(argv[0]);
    }

	//fprintf(stderr, "DIR: %s\n\n", getcwd(NULL, 1024));

    optind = 2; // skip file arg

    int ch;
    while ((ch = getopt(argc, argv, "lt:e:q:f:")) != -1) {
        switch (ch) {
        case 'l':
            justList = 1;
            break;
        case 'e':
            encoding = strdup(optarg);
            break;
        case 't':
            sheetName = strdup(optarg);
            break;
        case 'q':
            stringSeparator = optarg[0];
            break;
        case 'f':
            fieldSeparator = strdup(optarg);
            break;
        default:
            Usage(argv[0]);
            break;
        }
     }

	struct st_row_data* row;
	WORD cellRow, cellCol;

	// open workbook, choose standard conversion
	pWB = xls_open(argv[1], encoding);
	if (!pWB) {
		fprintf(stderr, "File not found");
		fprintf(stderr, "\n");
		return EXIT_FAILURE;
	}

	// check if the requested sheet (if any) exists
	if (sheetName[0]) {
		for (i = 0; i < pWB->sheets.count; i++) {
			if (strcmp(sheetName, (char *)pWB->sheets.sheet[i].name) == 0) {
				break;
			}
		}

		if (i == pWB->sheets.count) {
			fprintf(stderr, "Sheet \"%s\" not found", sheetName);
			fprintf(stderr, "\n");
			return EXIT_FAILURE;
		}
	}

	// process all sheets
	for (i = 0; i < pWB->sheets.count; i++) {
		int isFirstLine = 1;

        // just looking for sheet names
        if (justList) {
            printf("%s\n", pWB->sheets.sheet[i].name);
            continue;
        }

		// check if this the sheet we want
		if (sheetName[0]) {
			if (strcmp(sheetName, (char *)pWB->sheets.sheet[i].name) != 0) {
				continue;
			}
		}

		// open and parse the sheet
		pWS = xls_getWorkSheet(pWB, i);
		xls_parseWorkSheet(pWS);

		// process all rows of the sheet
		for (cellRow = 0; cellRow <= pWS->rows.lastrow; cellRow++) {
			int isFirstCol = 1;
			row = xls_row(pWS, cellRow);

			// process cells
			if (!isFirstLine) {
				printf("%s", lineSeparator);
			} else {
				isFirstLine = 0;
			}

			for (cellCol = 0; cellCol <= pWS->rows.lastcol; cellCol++) {
                //printf("Processing row=%d col=%d\n", cellRow+1, cellCol+1);

				xlsCell *cell = xls_cell(pWS, cellRow, cellCol);

				if ((!cell) || (cell->isHidden)) {
					continue;
				}

				if (!isFirstCol) {
					printf("%s", fieldSeparator);
				} else {
					isFirstCol = 0;
				}

				// display the colspan as only one cell, but reject rowspans (they can't be converted to CSV)
				if (cell->rowspan > 1) {
					fprintf(stderr, "Warning: %d rows spanned at col=%d row=%d: output will not match the Excel file.\n", cell->rowspan, cellCol+1, cellRow+1);
				}

				// display the value of the cell (either numeric or string)
				if (cell->id == 0x27e || cell->id == 0x0BD || cell->id == 0x203) {
					OutputNumber(cell->d);
				} else if (cell->id == 0x06) {
                    // formula
					if (cell->l == 0) // its a number
					{
						OutputNumber(cell->d);
					} else {
						if (!strcmp((char *)cell->str, "bool")) // its boolean, and test cell->d
						{
							OutputString((int) cell->d ? "true" : "false");
						} else if (!strcmp((char *)cell->str, "error")) // formula is in error
						{
							OutputString("*error*");
						} else // ... cell->str is valid as the result of a string formula.
						{
							OutputString((char *)cell->str);
						}
					}
				} else if (cell->str != NULL) {
					OutputString((char *)cell->str);
				} else {
					OutputString("");
				}
			}
		}
		xls_close_WS(pWS);
	}

	xls_close(pWB);
	return EXIT_SUCCESS;
}

// Output a CSV String (between double quotes)
// Escapes (doubles)" and \ characters
static void OutputString(const char *string) {
	const char *str;

	printf("%c", stringSeparator);
	for (str = string; *str; str++) {
		if (*str == stringSeparator) {
			printf("%c%c", stringSeparator, stringSeparator);
		} else if (*str == '\\') {
			printf("\\\\");
		} else {
			printf("%c", *str);
		}
	}
	printf("%c", stringSeparator);
}

// Output a CSV Number
static void OutputNumber(const double number) {
	printf("%.15g", number);
}
