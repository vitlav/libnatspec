/*
 * Public domain
 * Copyright (c) 2005 Etersoft
 * Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>
 *
 * $Id: test_get_charset.c,v 1.12 2005/02/26 15:23:24 lav Exp $
 *
 */

#include <stdlib.h>
#include <locale.h>
#include <iconv.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "natspec_internal.h"
#include "data/charset_names.h"
#include "data/get_charset_data.h"

void test_for_enrich()
{
	char *ret, *ret1;
	ret1 = natspec_get_enriched_fs_options(NULL, NULL);
	printf("enrich NULL: %s\n",ret1);
	ret = NULL;
	ret1 = natspec_get_enriched_fs_options("udf", ret);
	printf("enrich udf: ret='%s', ret1='%s'\n",ret, ret1);

	ret = ",";
	ret1 = natspec_get_enriched_fs_options("udf", ret);
	printf("enrich udf: ret='%s', ret1='%s'\n",ret, ret1);
	assert (strcmp(ret,ret1));

	ret = "defaults";
	ret1 = natspec_get_enriched_fs_options("vfat", ret);
	printf("enrich VFAT: %s %p:%p\n",ret,ret,ret1);
	assert (strcmp(ret,ret1));

	ret = "iocharset=koi8-r";
	ret1 = natspec_get_enriched_fs_options("vfat", ret);
	printf("enrich VFAT with io: %s %p:%p\n",ret,ret,ret1);
	assert (!strcmp(ret,ret1));

	ret = "";
	ret1 = natspec_get_enriched_fs_options("smb", ret);
	printf("enrich SMB: %s\n",ret1);
	assert (strcmp(ret,ret1));

	ret = strdup("defaults");
	ret1 = natspec_get_enriched_fs_options("reiserfs", ret);
	printf("enrich reiserfs: %s\n",ret1);
	assert (!strcmp(ret,ret1));
}

void test_for_iconv()
{
  int i;
  for (i = 0; i < sizeof (charset_names) / sizeof (charset_names[0]); i++)
    {
	  const char *cs = charset_names[i].charset_name;
	  iconv_t it;
	  it = iconv_open(cs,"UTF-8");
	  if ( it == (iconv_t)(-1))
	  {
	  	printf("TEST_ICONV: does not know '%s'\n",cs);
		exit (1);
		}
	else
		iconv_close(it);
    }
	printf("TEST_ICONV: comformance test is completed: %d charsets\n",i);
}

void test_for_convert()
{
	char *t, *ti; 
	ti = "Test ��������";
	t = natspec_convert_with_translit(ti, "ASCII", NULL);
	printf("1 %s -> '%s'\n",ti,t);
	t = natspec_convert_with_translit(ti, "UTF8", "");
	printf("2 %s -> '%s'\n",ti,t);
	ti = natspec_convert_with_translit(t, "", "UTF8");
	printf("3 %s -> '%s'\n",t,ti);
	ti = "��������";
	t = natspec_convert_with_translit(ti, "", "CP1251");
	printf("%s -> '%s'\n",ti,t);
	ti = "��������";
	t = natspec_convert(ti, "", "CP1251");
	printf("%s -> '%s'\n",ti,t);
	ti = "�������� \xb9";
	t = natspec_convert(ti, "", "CP1251");
	printf("%s -> '%s'\n",ti,t);
}

void test_nls()
{
	int i;
	for (i = 0; i< sizeof(charset_relation)/sizeof(charset_relation[0]); i++)
	{
		const char *t;
		iconv_t it;
		assert (t=natspec_get_filename_encoding(charset_relation[i].locale));
		it = iconv_open(t,"UTF-8");
		if ( it == (iconv_t)(-1))
	  	{
		  	printf("TEST_ICONV for nls: does not know '%s'\n",t);
			exit (1);
		}
		else
			iconv_close(it);
		
	}
	printf("TEST_NLS: comformance test is completed: %d locales\n",i);
}

int main(void)
{
	int i;
	char *locale[7];
	locale[0] = getenv("LANG");
	locale[1] = "POSIX";
	locale[2] = "C";
	locale[3] = NULL;
	locale[4] = "ru";
	locale[5] = "ru_RU";
	locale[6] = "";
	printf("current locale:%s\n",locale[0]);
	printf("user locale:%s\n",natspec_get_user_locale ());
	printf("system locale:%s\n",natspec_get_system_locale ());
	printf("current charset (nl):%s\n",	natspec_get_charset ());
	printf("current charset:%s\n",	natspec_get_charset_by_charset (NATSPEC_UNIXCS,NATSPEC_UNIXCS,NULL));
	for (i=0; i<sizeof(locale)/sizeof(char*); i++)
	{
		int j;
		printf("locale:'%s'\n",locale[i]);
		for (j = NATSPEC_UNIXCS; j<=NATSPEC_MACCS; j++)
		{
			const char *e = natspec_get_charset_by_locale(j,locale[i]);
			printf ("\tfor %d CS: '%s' (%s)\n", j, e, natspec_get_nls_by_charset(e));
		}
	}
	printf("fileenc:%s\n",natspec_get_filename_encoding (""));
	test_for_iconv();
	test_nls();
	test_for_enrich();
	test_for_convert();
	return 0;
}
