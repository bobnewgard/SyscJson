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

%{
#include "parser.h"
#include "json_lex.h"
%}

%option noyywrap
%option nounput
%option batch
%option outfile="lexer.c"
%option header-file="lexer.h"

%x ss

wspc    [ \t\n]
bgnl    \x5b
endl    \x5d
bgno    \x7b
endo    \x7d
rsol    \x5c
fsol    \x2f
dqot    \x22
coln    :
coma    ,
numb    -?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?(0|[1-9][0-9]*))?
char_u  \\u[0-9A-Fa-f]{4}
char_x  [^\\\"]
null    null
%%
%{
    if (lex_start == 1) lexer_init();
%}
{wspc}+             { posn_accum(yytext); }
{coln}              { posn_accum(yytext); return colon;        }
{coma}              { posn_accum(yytext); return comma;        }
{bgno}              { posn_accum(yytext); return begin_object; }
{endo}              { posn_accum(yytext); return end_object;   }
{bgnl}              { posn_accum(yytext); return begin_array;  }
{endl}              { posn_accum(yytext); return end_array;    }
true                { posn_accum(yytext); return yes;          }
false               { posn_accum(yytext); return no;           }
{null}              { posn_accum(yytext); return null;         }
{numb}              {
                        posn_accum(yytext);
                        yylval.sval = yytext;
                        return number;
                    }
{dqot}              {
                        BEGIN(ss);
                        posn_accum(yytext);
                        str_accum = NULL;
                    }
<ss>{dqot}          {
                        BEGIN(INITIAL);
                        posn_accum(yytext);
                        yylval.sval = str_accum;
                        return string;
                    }
<ss>{char_x}+       { posn_accum(yytext); concat_str (&str_accum, yytext); } 
<ss>{rsol}{dqot}    { posn_accum(yytext); concat_str (&str_accum, "\""  ); } 
<ss>{rsol}{fsol}    { posn_accum(yytext); concat_str (&str_accum, "/"   ); } 
<ss>{rsol}{rsol}    { posn_accum(yytext); concat_str (&str_accum, "\\"  ); } 
<ss>{rsol}b         { posn_accum(yytext); concat_str (&str_accum, "\b"  ); } 
<ss>{rsol}t         { posn_accum(yytext); concat_str (&str_accum, "\t"  ); } 
<ss>{rsol}f         { posn_accum(yytext); concat_str (&str_accum, "\f"  ); } 
<ss>{rsol}n         { posn_accum(yytext); concat_str (&str_accum, "\n"  ); } 
<ss>{rsol}r         { posn_accum(yytext); concat_str (&str_accum, "\r"  ); } 
<ss>{char_u}        { posn_accum(yytext); concat_uni (&str_accum, yytext); }
.|\n                {
                        fprintf (stderr, "[ERR] unrecognized character \"%s\"\n", yytext);
                        return 1;
                    }
%%
