/*
    lib/natspec.h
	
	main header file for LIBNATSPEC

    NATSPEC library
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>
	http://etersoft.ru/natspec

    $Id: natspechi_internal.h,v 1.3 2005/07/21 21:51:14 vitlav Exp $

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

#ifdef HAVE_CONFIG_H
#	include "../config.h"
#else
#	error Something is broken
#endif

#include "natspechi.h"

#define NATSPEC_TEX     1
#define NATSPEC_BABEL   2
#define NATSPEC_GUI     3
#define NATSPEC_ISO639  4

/* Country codes:
 * http://www.cia.gov/cia/publications/factbook/appendix/appendix-d.html
 * ALL http://www.unc.edu/~rowlett/units/codes/country.htm
 * ISO3166-1 http://www.iso.org/iso/en/prods-services/iso3166ma/02iso-3166-code-lists/list-en1.html
 * MARC http://www.loc.gov/marc/countries/cou_home.html
 */

struct language_entry
{
	const char *locale;
	const char *tex_name;
	const char *babel_name;
	const char *gui_name;
	char RTL;
	const char *isocode;
	const short country_id; /* international numbering */
}

#ifdef __cplusplus
}
#endif

#endif
