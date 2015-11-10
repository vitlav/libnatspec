/*
    lib/get_charset.c

	charset and locale related functions

    NATSPEC library
    This code contains some pieces of code and ideas from
                            WINE project (file dlls/kernel/locale.c)
                            GETTEXT project (file localcharset.c)
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>

    $Id: get_charset.c,v 1.24 2007/07/30 09:01:49 vitlav Exp $

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


#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "natspec_internal.h"

#include "data/get_charset_data.h"

#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif


/* FIXME: use __const due typedef int (*__compar_fn_t) (__const void *, __const void *); */
/*! Internal: Helper for bsearch */
static int charset_locale_cmp( __const void *name, __const void *entry )
{
    const struct charsetrel_entry *rel = (const struct charsetrel_entry *)entry;
	DEBUG (printf ("Compare %s with %s\n", (const char*)name, rel->locale));
    return strcmp( (const char *)name, rel->locale );
}

/*! Removes punctuation characters from charset name */
char *natspec_humble_charset( const char *charset)
{
	int i, j;
	char *buf;
	if (_n_isempty(charset))
		return NULL;
	/* Do not remove punctuation for ANSI encoding... */
	if( strstr(charset,"ANSI_X3") )
		return strdup(charset);
	buf = alloca( strlen(charset) + 1 );
   	for (i = 0, j = 0; charset[i]; i++)
	{
		if (charset[i] == ':')
			break;
		if (_n_isalnum(charset[i]))
			buf[j++] = _n_toupper(charset[i]);
	}
   	buf[j] = 0;
	DEBUG (printf ("Clean charset '%s', after: '%s'",charset, buf));
	return strdup(buf);
}

/*! Returns current charset */
const char *natspec_get_charset()
{
	const char *charset, *c;
#if defined(HAVE_LANGINFO_CODESET)
	charset = nl_langinfo(CODESET);
	c = natspec_get_charset_by_charset(NATSPEC_UNIXCS, NATSPEC_UNIXCS, charset);
	return c ? c : charset;
#else
	#error "I have not an idea how do it"
	/* recursive :) FIXME: I
	return natspec_get_charset_by_locale(NATSPEC_UNIXCS, "");*/
#endif
}


/*!
\todo Приводит к виду, использующемуся в iconv
static char *normalize_charset(const char *charset)
{
	char *buf = malloc( strlen(charset) + 1 );
	strcpy(buf, charset);
	return buf;
}
*/

/*! Returns charset by type of OS. It never returns NULL,
 * but current charset if other fails */
static const char *get_cs_by_type(const int type,
	const struct charsetrel_entry* entry)
{
	/* FIXME? DEBUG (assert (entry != NULL)); */
	if (entry)
		switch(type)
		{
			case NATSPEC_WINCS:
				return entry->win_cs ? entry->win_cs : "CP1252";
			case NATSPEC_DOSCS:
				return entry->dos_cs ? entry->dos_cs : "CP437";
			case NATSPEC_MACCS:
				return entry->mac_cs ? entry->mac_cs: "MAC";
			case NATSPEC_UNIXCS:
				return entry->unix_cs ? entry->unix_cs : "iso8859-1"; /* Really? */
		}
	/* fallback to locale charset */
	return natspec_get_charset();
}


/*! Internal: try search in charset_relation by encoding
    (Note: troubles with the same enc)
 */
static const struct charsetrel_entry* get_entry_by_charset(const int bytype,
	const char *charset)
{
	const struct charsetrel_entry *entry = NULL;
	char *must_free = NULL;
	DEBUG (printf("get_entry_by_charset charset=%s\n",charset));
	/* If charset is NULL, use current charset */
	if (_n_isempty(charset) && bytype == NATSPEC_UNIXCS)
	{
		/* Get charset for current locale */
		charset = must_free = natspec_humble_charset(natspec_get_charset());
		DEBUG (printf("2 cs=%s\n", charset));
	}
	if ( !_n_isempty(charset))
	{
		int i;
		DEBUG (printf("Find with charset '%s'\n", charset));
		/* Fixme: can we strict suppose enemy encoding by unix charset? */
		for (i = 0; i< sizeof(charset_relation)/sizeof(charset_relation[0]); i++)
			if (!strcmp (charset, get_cs_by_type(bytype,&charset_relation[i])))
			{
				entry = &charset_relation[i];
				break;
			}
	}
	free (must_free);
	return entry;
}

/*! Internal: Search _locale_ in list and returns entry pointer or NULL */
static const struct charsetrel_entry* get_entry_by_locale(const char *locale)
{
	const struct charsetrel_entry *entry;
	char *replocale = _natspec_repack_locale(locale);

	/* Search the same locale string in table. We are sure locale is repacked (normalized) */
	if ( _n_isempty(replocale)) {
		free (replocale);
		return NULL;
	}
	entry = _n_bsearch( replocale, charset_relation, charset_locale_cmp );
	free (replocale);

	DEBUG (if (!entry) printf ("Can't find the locale '%s'\n", locale));
	return entry;
}


/*! Returns charset by locale for type */
const char * natspec_get_charset_by_locale(const int type, const char *locale)
{
	char *must_free = NULL;
	const struct charsetrel_entry* entry;
	/* use system locale for NULL or user locale if empty */
	if (locale == NULL)
		locale = must_free = natspec_get_system_locale();
	else if (locale[0] == '\0')
		locale = must_free = natspec_get_current_locale();
	entry = get_entry_by_locale(locale);
	free (must_free);
	return get_cs_by_type(type, entry);
}

/* Returns 0 if locale not in utf8 encoding */
int natspec_locale_is_utf8(const char * locale)
{
	const char *charset = natspec_get_charset_by_locale(NATSPEC_UNIXCS, locale);
	return !strcmp(charset, "UTF8");
}


/*! Returns static string by charset by bytype for type */
const char * natspec_get_charset_by_charset(const int type,
	const int bytype, const char *charset)
{
	const struct charsetrel_entry* entry;
	char *buf = natspec_humble_charset(charset);
	entry = get_entry_by_charset(bytype, buf);
	free (buf);
	if (entry == NULL)
		return NULL;
	return get_cs_by_type(type, entry);
}

/*
int natspec_check_charset(const int type, const char *charset)
{
	const struct charsetrel_entry* entry;
	char *buf = natspec_humble_charset(charset);
	entry = get_entry_by_charset(bytype, buf);
	free (buf);
	return (entry != NULL ? 1 : 0);
}
*/
