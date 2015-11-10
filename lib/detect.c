/*
    lib/detect.c
 
	detect charset type related functions
 
    NATSPEC library
    This code contains some pieces of code and ideas from
             BibleTime project (file bibletime/backend/btstringmgr.cpp)
	See also:
	   http://csourcesearch.net/package/nvu/0.90/mozilla/xpcom/string/src/nsReadableUtils.cpp
							
    Copyright (c) 2005 Etersoft
    Copyright (c) 2005 Vitaly Lipatov <lav@etersoft.ru>
 
    $Id: detect.c,v 1.1 2006/03/11 22:05:31 vitlav Exp $
 
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

#define F 0   /* character never appears in text */
#define T 1   /* character appears in plain ASCII text */
#define I 2   /* character appears in ISO-8859 text */
#define X 3   /* character appears in non-ISO extended ASCII (Mac, IBM PC) */

static const unsigned char text_chars[256] =
    {
        /*                  BEL BS HT LF    FF CR    */
        F, F, F, F, F, F, F, T, T, T, T, F, T, T, F, F,  /* 0x0X */
        /*                              ESC          */
        F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  /* 0x1X */
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x2X */
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x3X */
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x4X */
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x5X */
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x6X */
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  /* 0x7X */
        /*            NEL                            */
        X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  /* 0x8X */
        X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  /* 0x9X */
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xaX */
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xbX */
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xcX */
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xdX */
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xeX */
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   /* 0xfX */
    };

int natspec_isutf8(const char *buf)
{
    int i, n;
    register unsigned char c;
    int gotone = 0;


    for (i = 0; (c = buf[i]); i++)
    {
        if ((c & 0x80) == 0) /* 0xxxxxxx is plain ASCII */
        {        
            /*
             * Even if the whole file is valid UTF-8 sequences,
             * still reject it if it uses weird control characters.
             */

            if (text_chars[c] != T)
                return 0;

        }
        else if ((c & 0x40) == 0)   /* 10xxxxxx never 1st byte */
        { 
            return 0;
        }
        else
        {                           /* 11xxxxxx begins UTF-8 */
            int following;

            if ((c & 0x20) == 0)      /* 110xxxxx */
                following = 1;
            else if ((c & 0x10) == 0) /* 1110xxxx */
                following = 2;
            else if ((c & 0x08) == 0) /* 11110xxx */
                following = 3;
            else if ((c & 0x04) == 0) /* 111110xx */
                following = 4;
            else if ((c & 0x02) == 0) /* 1111110x */
                following = 5;
            else
                return 0;

            for (n = 0; n < following; n++)
            {
                i++;
                if (!(c = buf[i]))
                    return gotone;

                if ((c & 0x80) == 0 || (c & 0x40))
                    return 0;
            }
            gotone = 1;
        }
    }
    return gotone;   /* don't claim it's UTF-8 if it's all 7-bit */
}

#undef F
#undef T
#undef I
#undef X

