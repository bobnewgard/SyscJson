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

#ifndef _JSON_VEC_H_
    #define _JSON_VEC_H_

    #include <string>
    #include <iomanip>
    #include <sstream>
    #include <vector>
    #include <array>
    #include <cstdio>

    #include "SyscMsg.h"
    #include "JsonToken.h"

    namespace JsonParse
    {
        using std::string;
        using std::unique_ptr;
        using SyscMsg::Msg;
        using SyscJson::Tokens;

        class JsonVecErr
        {
            public:
            string err_msg;

            JsonVecErr(string);
            ~JsonVecErr(void);

            string get_msg(void);
        };

        class JsonVec
        {
            private:
            unique_ptr<Msg>    msg;
            unique_ptr<Tokens> vec;
            string             str;

            public:
            JsonVec(const string&, const string&);
            JsonVec(const string&);
            ~JsonVec(void);

            void dump_vec(void);
            void set_obj_bgn(void);
            void set_obj_end(void);
            void set_arr_bgn(void);
            void set_arr_end(void);
            void set_obj_key(char*);
            void set_elem_nul(void);
            void set_elem_tru(void);
            void set_elem_fal(void);
            void set_elem_str(char*);
            void set_elem_num(char*);

            Tokens    & get_tokens(void);
        };

        extern "C"
        {
            #include "JsonVec_y.h"
        }
    }
#endif
