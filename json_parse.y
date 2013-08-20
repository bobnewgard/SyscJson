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
#define _GNU_SOURCE
#include <features.h>
#include "lexer.c"
#include "JsonVec_y.h"
%}

%require "2.5"
%defines "parser.h"
%output  "parser.c"
%debug
%error-verbose

%{
    void            * cjv;
    YY_BUFFER_STATE   cjb;
%}

%union {
    int    ival;
    char * sval;
}

%token          END             0   "end of file"
%token          begin_object
%token          end_object
%token          begin_array
%token          end_array
%token          comma
%token          colon
%token <sval>   string
%token <sval>   number
%token          null
%token          yes
%token          no
%start          json
%%
json:           object
                | array
                ;
object:         begin_object   { c_set_obj_bgn(cjv); }         end_object { c_set_obj_end(cjv); }
                | begin_object { c_set_obj_bgn(cjv); } members end_object { c_set_obj_end(cjv); }
                ;
array:          begin_array   { c_set_arr_bgn(cjv); }        end_array { c_set_arr_end(cjv); }
                | begin_array { c_set_arr_bgn(cjv); } values end_array { c_set_arr_end(cjv); }
                ;
members:        member
                | members comma member
                ;
values:         value
                | values comma value
                ;
member:         string colon   { c_set_obj_key(cjv, $1); } object
                | string colon { c_set_obj_key(cjv, $1); } array
                | string colon number { c_set_obj_key(cjv, $1); c_set_elem_num(cjv, $3); }
                | string colon string { c_set_obj_key(cjv, $1); c_set_elem_str(cjv, $3); }
                | string colon null   { c_set_obj_key(cjv, $1); c_set_elem_nul(cjv);     }
                | string colon yes    { c_set_obj_key(cjv, $1); c_set_elem_tru(cjv);     }
                | string colon no     { c_set_obj_key(cjv, $1); c_set_elem_fal(cjv);     }
                ;
value:          object
                | array
                | number { c_set_elem_num(cjv, $1); }
                | string { c_set_elem_str(cjv, $1); }
                | null   { c_set_elem_nul(cjv);     }
                | yes    { c_set_elem_tru(cjv);     }
                | no     { c_set_elem_fal(cjv);     }
                ;
%%
int json_parse(void * vec, char * str)
{
    int ret;

    cjv        = vec;
    lex_start  = 1;
    lex_ascii  = 1;
    lex_idx    = 0;
    lex_buffer = str;

    locale_codeset();
    cjb = yy_scan_string(str);
    ret = yyparse();
    yy_delete_buffer(cjb);
    return ret;
}
