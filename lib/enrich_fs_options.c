/*
    lib/enrich_fs_options.c
	
	support for adding locale related options to mount

    NATSPEC library
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>

    $Id: enrich_fs_options.c,v 1.25 2005/07/11 20:49:22 vitlav Exp $

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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <assert.h>

#include "natspec_internal.h"

/*! FAT/CDFS: iocharset & codepage */
static const char *list_fat_fs[] =
{
	"vfat", "fat", "msdos", NULL
};

/*! SMB/CIFS: iocharset & codepage */
static const char *list_smb_fs[] =
{
	"smb", "smbfs", "cifs", NULL
};

/*! iocharset (jfs use Unicode internal, add it too */
static const char *list_io_fs[] =
{
	"iso9660", "udf", "jfs", NULL
};

/*! If any of this options is already defined, skip enriching */
static const char *list_enc_opts[] =
{
	"iocharset", "codepage", "nls", NULL
};

/*! Is there str in list 
	\todo FIXME: test for comma-separated list in str 
*/
static int str_in_list(const char *str, const char **list)
{
	for (;*list != NULL;list++)
	{
		DEBUG (printf("Comparing '%s' and '%s'\n", str, *list));
		if (!strcmp(str, *list))
			return 1;
	}
	return 0;
}

/*! Is there one string from list in str? */
static int strstr_in_list(const char *str, const char **list)
{
	if (str == NULL)
		return 0;
	for (;*list != NULL;list++)
	{
		DEBUG (printf("Comparing '%s' and '%s'\n", str, *list));
		if (strstr(str, *list))
			return 1;
	}
	return 0;
}

/*!
 * Internal: adds new option1,2 to options param
 * (only if option1 is not empty and option2 is exist)
 * \todo FIXME: add bound control
 */
static void add_option(char *options, const char *option1, const char *option2)
{
	int lenopt = strlen(options);
	if (strlen(option1) == 0 || option2 == NULL)
		return;
	if (lenopt && options[lenopt-1] != ',')
		options[lenopt++] = ',';
	strcpy(options+lenopt, option1);
	strcat(options, option2);
	DEBUG(printf("add_option: '%s', result:%s\n",option1, options));
}

/*! Internal: adds options to buf for filesystem fs */
static void add_options(char *buf, const char *fs)
{
	/* charset of our system */
	const char *charset = natspec_get_filename_encoding("");
	/* codepage of DOS system as assumed from current locale */
	int codepagenum = natspec_get_codepage_by_charset(
			natspec_get_charset_by_locale(NATSPEC_DOSCS, ""));
	char codepage[10];
	assert (codepagenum<1000000);
	sprintf(codepage, "%d", codepagenum);
	DEBUG (fprintf (stderr,"fn=%s ENRICH: codepage=%s\n",charset,codepage));

	/* Specially for each file system */
	if ( str_in_list (fs, list_fat_fs) )
	{
		add_option(buf, "iocharset=", charset);
		add_option(buf, "codepage=", codepage);
	}
	
	else if ( str_in_list (fs, list_io_fs) )
	{
		add_option(buf, "iocharset=", charset);
	}
	
	else if ( !strcmp (fs, "ntfs"))
	{
		add_option(buf, "nls=", charset);
	}
	
	else if ( str_in_list (fs, list_smb_fs) )
	{
		/* smb has some specifity with codepage names */
		const char *cp = natspec_get_nls_by_charset(
			natspec_get_charset_by_locale(NATSPEC_DOSCS, ""));
		add_option(buf, "iocharset=", charset);
		add_option(buf, "codepage=", cp);
	}
	
	else
		DEBUG (fprintf(stderr, "NATSPEC: do not know %s fs\n",fs));
}

/*! Adds needed i18n options for fs type */
char* natspec_get_enriched_fs_options(const char *fs, const char *options)
{
	char *ret;
	/* Our additional options does not need more than 100 bytes */
	char buf[100]; buf[0] = '\0';

	/* if encoding issues already exists in options, do nothing */
	if ( strstr_in_list (options, list_enc_opts) )
		return strdup(options);

	if (fs != NULL)
		add_options(buf, fs);

	if (options == NULL)
		return strdup (buf);

	ret = malloc ( strlen(options) + 
			strlen(buf) + 1 + 1 ); /* add 2 byte for '\0' and ',' */
	strcpy (ret, options);
	add_option (ret, buf, "");
	return ret;
}
