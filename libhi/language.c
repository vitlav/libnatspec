/*
    lib/get_locale.c

	locale related functions

    NATSPEC library

    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>

    $Id: language.c,v 1.4 2005/07/21 21:51:14 vitlav Exp $

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

#include "natspec_internal.h"

/* /usr/share/locale/../entry.desktop для получения названия языка */
const char *natspec_get_language_name(const int type, const char *locale)
{
	return "english";
}

const char *natspec_get_country_name(const char *locale)
{
	return "Russia";
}

int natspec_get_country_code(const char *locale)
{
	return 7;
}

int natspec_get_country_id(const char *locale)
{
	return 7;
}
