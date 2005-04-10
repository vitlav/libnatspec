/*
    lib/filesystem.c

	filename charset related functions

    NATSPEC library
    This code contains some pieces of code and ideas from
                            WINE project (file dlls/kernel/locale.c)
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>

    $Id: filesystem.c,v 1.6 2005/04/10 18:24:42 pv Exp $

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
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

#include "data/charset_names.h"

#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif


static int charset_cmp( const void *name, const void *entry )
{
    const struct charset_entry *charset = (const struct charset_entry *)entry;
    return strcmp( (const char *)name, charset->charset_name );
}


/*! Returns codepage from charset */
static char __cfc[10]; /* FIXME this ugly thing */
const char *natspec_get_codepage_by_charset(const char *charset)
{
	const struct charset_entry *entry;
	char *cs = natspec_humble_charset(charset);
	/* We are sure now cs is normalized */
	entry = bsearch( cs, charset_names,
                     sizeof(charset_names)/sizeof(charset_names[0]),
                     sizeof(charset_names[0]), charset_cmp );
	free(cs);
    if (entry)
	{
		assert( entry->codepage < 1000000);
		sprintf(__cfc,"%d",entry->codepage);
		return __cfc;
	}
	return "437"; /* codepage for ascii? */
}

/*! Returns nls name (in Linux kernel notation) from charset */
const char *natspec_get_nls_by_charset(const char *charset)
{
	const struct charset_entry *entry = NULL;
	if (charset != NULL)
	{
		char *cs = natspec_humble_charset(charset);
		/* We are sure now cs is normalized */
		entry = bsearch( cs, charset_names,
                     sizeof(charset_names)/sizeof(charset_names[0]),
                     sizeof(charset_names[0]), charset_cmp );
		free(cs);
	    if (entry)
			return entry->nls;
	}
	/* returns value should be always correctly */
	return "iso8859-1"; /* ascii or ANSIX341968 ?? */
}


/*! Return filename encoding for locale */
const char *natspec_get_filename_encoding(const char *locale)
{
	/* Try get from environment variable */
	const char *buf = getenv("G_FILENAME_ENCODING");
    /*! \todo FIXME: test for comma-separated list in G_FILENAME_ENCODING */
	if (buf != NULL && locale != NULL && locale[0] == '\0' /*&& natspec_check_charset(NATSPEC_UNIXCS, buf)*/)
	{
		/* If get from env non empty string,
		   search in static table the same name: normalized */
		buf = natspec_get_charset_by_charset(NATSPEC_UNIXCS, NATSPEC_UNIXCS, buf);
	}
	/* If above try is failed */
	if (buf == NULL || buf[0] == '\0')
	{
		/* Get charset from locale */
		buf = natspec_get_charset_by_locale(NATSPEC_UNIXCS, locale);
	}
	return natspec_get_nls_by_charset(buf);
}

