/*
    lib/filesystem.c

	filename charset related functions

    NATSPEC library
    This code contains some pieces of code and ideas from
                            WINE project (file dlls/kernel/locale.c)
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>

    $Id: filesystem.c,v 1.12 2008/02/14 22:37:08 vitlav Exp $

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

#include "data/charset_names.h"

#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif

/* FIXME: due typedef int (*__compar_fn_t) (__const void *, __const void *); */
/*! Internal: charset comparing for bsearch */
static int charset_cmp( __const void *name, __const void *entry )
{
    const struct charset_entry *charset = (const struct charset_entry *)entry;
    return strcmp( (const char *)name, charset->charset_name );
}


/*! Returns codepage from charset */
int natspec_get_codepage_by_charset(const char *charset)
{
	const struct charset_entry *entry;
	char *cs = natspec_humble_charset(charset);
	/* We are sure now cs is normalized */
	entry = _n_bsearch( cs, charset_names, charset_cmp );
	free(cs);
	if (!entry)
		return 437; /* codepage for ascii? */
	return entry->codepage;
}

/*! Returns nls name (in Linux kernel notation) from charset */
const char *natspec_get_nls_by_charset(const char *charset)
{
	const struct charset_entry *entry = NULL;
	if (charset != NULL)
	{
		char *cs = natspec_humble_charset(charset);
		/* We are sure now CS is normalized */
		entry = _n_bsearch( cs, charset_names, charset_cmp );
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
	/* Try get from environment variable (it can be comma-separated,
	 * but we using first part only */
	const char *buf = getenv("G_FILENAME_ENCODING");
	/* The special token "@locale" specifies the character set for the current locale. */
	/* Note: We already do not use G_FILENAME_ENCODING if it contains not locale name */
	/* TODO: will we use strstr here? */
	if (buf != NULL && !strcmp(buf, "@locale"))
		buf = NULL;

	if (!_n_isempty(buf) && locale != NULL && locale[0] == '\0')
	{
		/* If get from env G_FILENAME_ENCODING non empty string, and use current locale (""),
		   search in static table the same name: normalized */
		buf = natspec_get_charset_by_charset(NATSPEC_UNIXCS, NATSPEC_UNIXCS, buf);
	}
	/* If above try is failed */
	if (_n_isempty(buf))
	{
		/* Get charset from locale */
		buf = natspec_get_charset_by_locale(NATSPEC_UNIXCS, locale);
	}
	return natspec_get_nls_by_charset(buf);
}

