/*
 * Copyright 2013 Robert Newgard
 *
 * This file is part of SyscJson.
 *
 * SyscJson is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SyscJson is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SyscJson.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <langinfo.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define JL_DBG 0

int    lex_start = 1;
int    lex_ascii = 1;
int    lex_idx   = 0;
char * str_accum;
char * lex_buffer;

void
locale_codeset ()
{
    char * codeset;

    setlocale(LC_CTYPE,"");
    codeset = nl_langinfo(CODESET);
 
    if (strcmp(codeset, "UTF-8") == 0)
    {
        lex_ascii = 0;
    }
    else if (strcmp(codeset, "utf8") == 0)
    {
        lex_ascii = 0;
    }

    if (JL_DBG)
    {
        if (lex_ascii == 0)
        {
            fprintf(stderr, "codeset is \"%s\"; using UTF-8 character processing\n", codeset);
        }
        else
        {
            fprintf(stderr, "codeset is \"%s\"; using lex_ascii character processing\n", codeset);
        }
    }
}

void
lexer_init ()
{
    if (JL_DBG)
    {
        fprintf (stderr, "lexer_init()\n");
    }

    lex_start = 0;
}

int
concat_str (char ** str, const char * cat)
{
    char       * newp;
    char       * wp;
    size_t       str_len;
    size_t       cat_len = strlen (cat);

    if (JL_DBG)
    {
        fprintf (stderr, "++ in concat_str(), cat is \"%s\", cat_len is %zd\n", cat, cat_len);
    }

    if (*str == NULL)
    {
        if (JL_DBG)
        {
            fprintf (stderr, "++ in concat_str(), *str is NULL\n");
        }
        str_len = 0;
        newp    = (char *) malloc (cat_len + 1);

        if (newp == NULL)
        {
            if (JL_DBG)
            {
                fprintf (stderr, "++ in concat_str(), newp is NULL\n");
            }
            return -1;
        }
    }
    else
    {
        if (JL_DBG)
        {
            fprintf (stderr, "++ in concat_str(), *str is \"%s\"\n", *str);
        }
        str_len = strlen (*str);
        newp    = (char *) realloc (*str, str_len + cat_len + 1);

        if (newp == NULL)
        {
            if (JL_DBG)
            {
                fprintf (stderr, "++ in concat_str(), newp is NULL\n");
            }
            free (*str);
            *str = NULL;
            return -1;
        }
    }

    if (JL_DBG)
    {
        fprintf (stderr, "++ in concat_str(), newp is %p\n", newp);
    }

    wp = newp + str_len;
    wp = mempcpy (wp, cat, cat_len);
    wp = memset (wp, '\0', 1);

    for (char * p = newp ; p < newp + 5 ; p++)
    {
        if (JL_DBG)
        {
            fprintf (stderr, "++ in concat_str(), newp[%p] is 0x%02X or \'%c\'\n", p, *p, *p);
        }
    }

    *str = newp;
    if (JL_DBG)
    {
        fprintf (stderr, "++ in concat_str(), *str is %p\n", *str);
    }

    return 0;
}

int
concat_char (char ** str, const char cat)
{
    char cat_str[2];

    cat_str[0] = cat;
    cat_str[1] = '\0';

    return concat_str(str, cat_str);
}

int
concat_uni (char ** str, const char * json_utf8)
{
    if (lex_ascii)
    {
        return concat_str(str, json_utf8);
    }
    else
    {
        unsigned code;
        int      ret;

        ret = sscanf((json_utf8 + 2), "%4x", &code);

        if (ret == 1)
        {
            if (JL_DBG)
            {
                fprintf(stderr, "[INF] extracted %04X from unicode hex characters \"%s\"\n", code, json_utf8);
            }
        }
        else
        {
            fprintf(stderr, "[ERR] failed to extract unicode hex characters from \"%s\"\n", json_utf8);
            return -1;
        }

        if (code < 0x80ul)
        {
            char utf = 0;
            ret = 0;
            utf = (0x007Ful & code);
            ret = ret | concat_char(str, utf);
            return ret;
        }
        else if (code < 0x800ul)
        {
            char utf = 0;
            ret = 0;
            utf = (0x0C0ul) | ((0x07C0ul & code) >> 6);
            ret = ret | concat_char(str, utf);
            utf = 0x80ul | (0x003Ful & code);
            ret = ret | concat_char(str, utf);
            return ret;
        }
        else if (code < 0x10000ul)
        {
            char utf = 0;
            ret = 0;
            utf = 0xE0ul | ((0xF000ul & code) >> 12);
            ret = ret | concat_char(str, utf);
            utf = 0x80ul | ((0x0FC0ul & code) >> 6);
            ret = ret | concat_char(str, utf);
            utf = 0x80ul | (0x003Ful & code);
            ret = ret | concat_char(str, utf);
            return ret;
        }
        else
        {
            return concat_str(str, json_utf8);
        }
    }
}

void
posn_accum(const char * s)
{
    lex_idx = lex_idx + strlen(s);
}

int yyerror(const char * s)
{
    fprintf(stderr, "parser error: \"%s\" at character %d\n", s, lex_idx);
    fprintf(stderr, "parser error: \'%s\'\n", lex_buffer);
    fprintf(stderr, "parser error:  ");

    for (int i = 0 ; i < (lex_idx - 1) ; i++)
    {
        fprintf(stderr, ".");
    }

    fprintf(stderr, "^\n");
    return 0;
}
