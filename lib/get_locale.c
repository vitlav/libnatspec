/*
    lib/get_locale.c

	locale related functions

    NATSPEC library
    This code contains some pieces of code and ideas from
                            WINE project (file dlls/kernel/locale.c)
                            GETTEXT project (file localcharset.c)
                            GNOME project (file gnome-libs/libgnome/gnome-i18n.c)

    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>

    $Id: get_locale.c,v 1.22 2008/02/14 22:37:08 vitlav Exp $

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
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include "natspec_internal.h"

/*! Try LC_ALL:LC_CTYPE:LANG from environment
   Returns NULL if locale is missed, empty or POSIX/C
   \todo FIXME: is it equivalent of setlocale(LC_ALL,"")?
   \todo FIXME: LANGUAGE can has multiple value (and ':')
*/
/* From gettext(3):
> If the LANGUAGE environment variable is set to a  nonempty  value,  and
> the  locale  is not the "C" locale, the value of LANGUAGE is assumed to
> contain a colon separated list of  locale  names.  The  functions  will
> attempt  to  look  up  a translation of msgid in each of the locales in
> turn.  This is a GNU extension.
*/

char *natspec_internal_get_locale_from_env()
{
	char *tmp, *next;
	/* The highest priority value is the `LANGUAGE' environment
	variable.  This is a GNU extension.  */
/*	tmp = getenv ("LANGUAGE");
	if (_n_isempty(tmp)) */
	tmp = getenv("LC_ALL");
	if (_n_isempty(tmp))
		tmp = getenv("LC_CTYPE");
	if (_n_isempty(tmp))
		tmp = getenv("LANG");
	if (_n_isempty(tmp) ||
		!strcmp(tmp, "POSIX") || !strcmp(tmp, "C") )
			return NULL;
	tmp = strdup(tmp);
	/* Use only first locale int string */
	next = strchr(tmp,':'); if (next) *next++ = '\0';
	return tmp;
}

/*! Returns user locale string (malloc allocated)
 * \todo FIXME: can we do it with glibc not my get_from_env?
 */
char *natspec_get_current_locale()
{
	char *locale = natspec_internal_get_locale_from_env();
	if (locale == NULL)
		return natspec_get_system_locale();
	return locale;
}

char *natspec_get_user_locale()
{
	return natspec_get_current_locale();
}

/*
const char *get_i18n_filename()
{
	return "/etc/sysconfig/i18n";
}
*/

/* File with locale information:
 *   SuSE         - /etc/rc.local       RC_LANG=
 *   RedHat-based - /etc/sysconfig/i18n LANG=
 *   Debian       - /etc/enviroment     LANG=
 *   Slackware    - /etc/profile        export LANG=
 *   FreeBSD      - /etc/profile        LANG=ru_RU.KOI8-R; export LANG
*/

/*! Read system wide locale, return str or NULL if it does not exist 
 *  Since POSIX:1996 locale info we get from LANG variable 
 * \todo fix ugly buf, fgets
*/
static char *get_from_system_i18n(const char *str)
{
	int i;
	char *locale = NULL, *t;
	FILE *fd;
	fd = fopen(NATSPEC_I18N_FILE,"r");
	for (;fd;)
	{
		char buf[100], buf1[100];
		char *r;
		/* Read next line */
		r = fgets(buf, 99, fd);
		if (!r) break;
		/* Remove unneeded symbols FIXME: some glibc func?*/
		for (i = 0; *r; r++)
		{
			/* Break line at ';#' symbols */
			if (*r == ';' || *r == '#')
				break;
			switch (*r)
			{
				case ' ':
				case '\n':
				case '\t':
				case '"':
				case '\r':
					break;
				default:
					buf1[i++] = *r;
			}
		}
		buf1[i] = 0;
		DEBUG (printf("GSL: after space removing '%s'",buf1));
		/* Skip empty strings and comments */
		if (!i)
			continue;
		i = strlen(str);
		/* Find str in any position */
		t = strstr(buf1, str);
		if (t == NULL)
			continue;
		/* CHECKME: if i < strlen(buf1)? */
		if (!strncmp(t, str, i) && t[i] == '=')
		{
			locale = strdup ( t+i+1 );
			break;
		}
	}
	if (fd)
		fclose(fd);
	return locale;
}

/*! Returns system locale string (malloc allocated) */
char *natspec_get_system_locale()
{
	char *locale = get_from_system_i18n("LANG");
	if (locale == NULL)
		locale = natspec_internal_get_locale_from_env();
	/*! \todo FIXME: we get SegFault in library (when empty sysconfig/i18n and POSIX locale) if return NULL */
	if (locale == NULL)
		locale = strdup("C");
	return locale;
}


/*! Returns charset get from _locale_ */
char *natspec_extract_charset_from_locale(const char *locale)
{
	char *lang, *next, *dialect, *charset, *ret;
	if (_n_isempty(locale))
		return NULL;
	lang = strdup( locale );
	next = strchr(lang,':'); if (next) *next++ = '\0';
	dialect = strchr(lang,'@'); if (dialect) *dialect++ = '\0';
	charset = strchr(lang,'.'); if (charset) *charset++ = '\0';
	ret = natspec_humble_charset(charset);
	free (lang);
	return ret;
}


/*! Internal: repack locale string (compress charset, fix register) */
char *_natspec_repack_locale(const char *locale)
{
	int i;
	char *buf, *lang, *next, *dialect, *charset, *country;
	if (_n_isempty(locale))
		return NULL;
	DEBUG (printf("repack_locale\n"));
	lang = strdup( locale );
	buf = malloc( strlen(locale) + 1 );
	next = strchr(lang,':'); if (next) *next++ = '\0';
	dialect = strchr(lang,'@'); if (dialect) *dialect++ = '\0';
	charset = strchr(lang,'.'); if (charset) *charset++ = '\0';
	country = strchr(lang,'_'); if (country) *country++ = '\0';

	for (i=0; i<strlen(lang); i++)
		lang[i] = _n_tolower(lang[i]);
	strcpy(buf, lang);
	if (country)
	{
		for (i=0; i<strlen(country); i++)
			country[i] = _n_toupper(country[i]);
		strcat(buf, "_");
		strcat(buf, country);
	}
	charset = natspec_extract_charset_from_locale(locale);
	if (charset)
	{
		strcat(buf, ".");
		strcat(buf, charset);
		free (charset);
	}
	if (dialect)
	{
		for(i=0; i<strlen(dialect); i++)
			dialect[i] = _n_tolower(dialect[i]);
		strcat(buf, "@");
		strcat(buf, dialect);
	}
	free (lang);
	return buf;
}
