/*
    lib/natspec.h
	
	main header file for LIBNATSPEC

    NATSPEC library
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>
	http://etersoft.ru/natspec

    $Id: natspec.h,v 1.24 2008/02/14 22:37:09 vitlav Exp $

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

#ifndef __NATSPEC_H
#define __NATSPEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <iconv.h>

/******************* Locale/charset *************************/

/* Types of character set tables for various OS */

#define NATSPEC_UNIXCS 0
#define NATSPEC_WINCS 1
#define NATSPEC_DOSCS 2
#define NATSPEC_MACCS 3

/*! 
 * Returns malloc allocated string with current locale.
 * Gets from LC_ALL:LC_CTYPE:LANG evironment variable, if fails,
 * gets value from get_system_locale()
 */
char *natspec_get_current_locale();

/*! Obsolete (user locale is locale from ~/.i18n, but we do not use it now): */
char *natspec_get_user_locale();


/*! 
 * Returns malloc allocated string with system locale
 * retrieved from LANG variable in /etc/sysconfig/i18n file
 */
char *natspec_get_system_locale();

/*!
 * Returns 0 if locale not in utf8 encoding
 * If locale is empty, use user locale
 * If locale is NULL, use system locale
 */
int natspec_locale_is_utf8(const char * locale);

/*!
 * Returns static string with current charset
 * (according to locale, not filename encoding)
 * \note you have to use setlocale(LC_ALL,"") before call this function
 */
const char *natspec_get_charset();

/*!
 * Returns static string in nls form (like in kernel) with filename charset
 * for locale.
 * If G_FILENAME_ENCODING exists and correct, returns its value
 * If locale is empty, use user locale
 * If locale is NULL, use system locale
 */
const char * natspec_get_filename_encoding(const char * locale);


/*! Returns static string with charset of _type_ operating system
 * for locale in form language[_territory][.charset][@modifier}
 * where language is an ISO 639 language code, territory
 * is an ISO 3166 country code, and codeset is a character set or encoding
 * identifier like ISO-8859-1 or UTF-8
 
 * If locale is empty string, use current locale
 * If locale is NULL, use system locale
 */
const char * natspec_get_charset_by_locale(const int type,
	const char *locale);


/************* Useful functions ********************/

/* Converts a string from one character set to another.
 * If tocode or fromcode is NULL, it assume as local charset
 * If the conversion was successful,
 * Returns: a newly allocated nul-terminated string,
 * 	which must be freed with free(). Otherwise NULL.
 * Note: do not forget call setlocale(LC_ALL, "");
 */
 
/*! Transliterate inconvenient symbols */
/* \return Returns NULL is conversion is failed */
char *natspec_convert(const char *in_str,
	const char *tocode, const char *fromcode, int transliterate);

/*! Open iconv table. You can use emtpy charset string for locale encoding */
iconv_t natspec_iconv_open(const char *tocode, const char *fromcode);

/*! Do conversion like iconv, with translit if needed */
size_t natspec_iconv(iconv_t cd, char **inbuf, size_t *inbytesleft,
	char **outbuf, size_t *outbytesleft, int transliterate);

int natspec_iconv_close(iconv_t cd);

/************* Helper functions *******************/

/*!
 * Specially for mount-like command.
 * Adds need i18n params for fs type to mount options
 * and return new string
 * Params:
 *  fs      - filesystem type string
 *  options - already exist options string
 * \return Returns new allocated string with enriched options or NULL if failed
 * \note options enriched according to user locale in LIBNATSPEC term
 */
char * natspec_get_enriched_fs_options(const char* fs, const char *options);

/*!
 * Returns codepage (DOS system) for charset
 * Examples:
 *   for IBM866 returns 866
 *   for cp1251 returns 1251
 */
int natspec_get_codepage_by_charset(const char *cs);

/*!
 * Returns nls form for charset
 * Examples:
 *   for KOI8R returns koi8-r
 */
const char *natspec_get_nls_by_charset(const char *cs);


/******************** Not recommended for use *******************/

/*! 
 * Returns malloc allocated string with charset get from _locale_
 */
char *natspec_extract_charset_from_locale(const char *locale);

/*! Removes punctuation characters from charset name */
/* \todo this function must to fix any charset name to iconv convenient */
char *natspec_humble_charset( const char *charset);


/*!
 * Return static string with charset of _type_ op. system
 * by _charset_ for _bytype_ op. system
 * is charset is unknown, returns NULL
 */
const char * natspec_get_charset_by_charset(const int type,
	const int bytype, const char *charset);

/* Obsoletes: */
char *natspec_convert_with_translit(const char *in_str,
	const char *tocode, const char *fromcode);


/* Unneeded
int natspec_check_charset(const int type, const char *charset);
*/

#ifdef __cplusplus
}
#endif

#endif
