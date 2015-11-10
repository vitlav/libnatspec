/*
    lib/convert.c
	
	sure convert between encoding (with transliteration if needed)

    NATSPEC library
    Copyright (c) 2005 Etersoft
    Copyright (c) 2002, 2005 Vitaly Lipatov <lav@etersoft.ru>

    $Id: convert.c,v 1.20 2008/02/14 22:36:22 vitlav Exp $

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation, version 2.1
    of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA.
*/

#include <iconv.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "natspec_internal.h"
#include "unicode/uni_7b.h"

#define UCS2_SIZE 2

/* FIXME: only hack */
#define MAX_UCS2 5
static struct
{
	iconv_t it, ucs2;
} ucs2table[MAX_UCS2];

/*! Opens iconv table or returns -1 */
iconv_t natspec_iconv_open(const char *tocode, const char *fromcode)
{
	int i;
	if ( _n_isempty(tocode) )
		tocode = natspec_get_charset();
	if ( _n_isempty(fromcode) )
		fromcode = natspec_get_charset();
	for (i=0; i < MAX_UCS2; i++)
		if (!ucs2table[i].it) {
			/* use i element if empty */
			ucs2table[i].it = iconv_open(tocode, fromcode);
			if (ucs2table[i].it == (iconv_t)-1)
				break;
			ucs2table[i].ucs2 = iconv_open("UCS2", fromcode);
			if (ucs2table[i].ucs2 != (iconv_t)-1)
				return ucs2table[i].it;
			break;
		}
	return (iconv_t)-1;
}

/*! For future compatibility */
int natspec_iconv_close(iconv_t cd)
{
	int i;
	for (i=0; i < MAX_UCS2; i++)
		if (ucs2table[i].it == cd) {
			iconv_close(ucs2table[i].ucs2);
			ucs2table[i].it = 0;
		}
	return iconv_close(cd);
}

/* use __const due typedef int (*__compar_fn_t) (__const void *, __const void *); */
static int ucs2_cmp( __const void *ucs2, __const void *entry )
{
	const struct u7_struct *s = (const struct u7_struct*)entry;
	const unsigned short *u = (const unsigned short*)ucs2;
	return *u - s->x;
}

/*! Get transliterated UCS2 character or '_' if it is impossible */
static const char *get_7bit (unsigned short ucs2)
{
	struct u7_struct *entry;
	entry = _n_bsearch( &ucs2, unicode_7b, ucs2_cmp );
	if (entry == NULL || entry->s == NULL)
		return "_";
	DEBUG (printf("ucs2:%x, %s, entry:%x\n",ucs2, entry->s, entry));
	return entry->s;
}

/*! convert as iconv but can transliterate (returns -1 if error)
 * Source: from my old patch for XMMS (2002 year) and sim code */
size_t natspec_iconv(iconv_t cd, char **inbuf, size_t *inbytesleft,
	char **outbuf, size_t *outbytesleft, int transliterate)
{
	int i;
	iconv_t ucs2 = 0;
	size_t result;
	for(;;)
	{
		DEBUG (printf("%d (ucs2: %d), %s:%d\n",cd, ucs2, *inbuf, *inbytesleft));
		result = iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);
		if (result != (size_t) -1)
			return result; /* done converting successfully */
		if ( errno != EILSEQ )
			return result;
		if (!transliterate)
			return result;
		/* find corresport UCS2 table */
		for (i=0; i < MAX_UCS2; i++)
			if (ucs2table[i].it == cd) {
				ucs2 = ucs2table[i].ucs2;
				break;
			}
		if (!ucs2)
			return result;
		else
		{
			/* Replace invalid input character. See code of links, sim, iconv, catdoc */
			int tmpresult;
			unsigned short tmp;
			size_t lentmp = UCS2_SIZE;
			char *tmpptr = (char*) &tmp;
			DEBUG (printf("replace: %d, %s:%d\n",ucs2, *inbuf, *inbytesleft));
			tmpresult = iconv(ucs2, inbuf, inbytesleft, &tmpptr, &lentmp);
			if ((tmpresult == (size_t) -1 && errno == E2BIG) || tmpresult != (size_t) -1)
			{
				const char *t = get_7bit(tmp);
				strcpy(*outbuf, t);
				(*outbuf) += strlen(t);
				(*outbytesleft) -= UCS2_SIZE;
				result += tmpresult;
				DEBUG (printf("br\n"));
			}
			else
			{	/* if iconv could not help, skip one byte from input string */
				/* DEBUG (printf("%d '%c'\n",errno, *ansbptr));
				DEBUG (perror("")); */
				**outbuf = '_';
				(*outbuf)++;
				(*inbytesleft)--;
				(*inbuf)++;
				(*outbytesleft)--;
				result += 1;
			}
		}
	}
}

/*! Returns converts input string from encoding to encoding
 * TODO: Locale depends transliterating
 */
char *natspec_convert(const char *in_str,
	const char *tocode, const char *fromcode, int transliterate)
{
	iconv_t frt;
	size_t lena = strlen(in_str)*6; /* FIXME see E2BIG for errno */
	size_t lenb = strlen(in_str);
	char *ansa = (char*)alloca(lena+1);
	char *ansbptr = (char*)in_str;
	char *ansaptr = ansa;

	frt = natspec_iconv_open(tocode, fromcode);
	if (frt == (iconv_t) -1)
	{
		char buf[200];
		snprintf(buf,199,"Broken encoding: '%s' (to) or '%s' (from) or UCS2. May be you forget setlocale in main or gconv-modules is missed?\n",
			tocode, fromcode);
		perror(buf);
		/* return dup of original string if converting is failed */
		ansaptr = strdup(in_str);
		return ansaptr;
	}
	(void)natspec_iconv(frt, &ansbptr, &lenb, &ansaptr, &lena, transliterate);
	natspec_iconv_close(frt);
	*ansaptr = '\0';
	return strdup(ansa);
}


/* Obsoleted */
char *natspec_convert_with_translit(const char *in_str,
	const char *tocode, const char *fromcode)
{
	return natspec_convert(in_str, tocode, fromcode, 1);
}

