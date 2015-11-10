/*
    lib/natspec.h
	
	main header file for LIBNATSPEC

    NATSPEC library
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>
	http://etersoft.ru/natspec

    $Id: natspechi.h,v 1.4 2005/07/21 21:51:14 vitlav Exp $

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

/* Types of language nama representation */
#define NATSPEC_TEX     1
#define NATSPEC_BABEL   2
#define NATSPEC_GUI     3
#define NATSPEC_ISO639  4

/* Returns name of language by locale */
const char *natspec_get_language_name(const int type, const char *locale);

/* Returns name of country by locale */
const char *natspec_get_country_name(const char *locale);

/* Returns code of the country */
int natspec_get_country_code(const char *locale);

/* Returns country ID (for DOS) */
int natspec_get_country_id(const char *locale);

#ifdef __cplusplus
}
#endif

#endif
