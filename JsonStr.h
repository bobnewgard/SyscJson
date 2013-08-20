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

/** \file  JsonStr.h
 *  \brief Declares the JsonStr class.
 */
#ifndef _JSON_STR_H_
    #define _JSON_STR_H_

    #include <string>
    #include <SyscMsg.h>
    #include <JsonToken.h>

    namespace SyscJson
    {
        using std::string;
        using std::unique_ptr;

        /** \class JsonStr
         *  \brief Methods to operate on a JSON representation.
         *
         *  The JsonStr methods are used to operate on an internal representation
         *  of a JSON string.
         */
        class JsonStr
        {
            private:
            unique_ptr<string> str;

            bool need_comma ( void );

            public:
            JsonStr(void);
            ~JsonStr(void);

            void add_obj_bgn ( void             );
            void add_obj_end ( void             );
            void add_arr_bgn ( void             );
            void add_arr_end ( void             );
            void add_key     ( string&          );
            void add_key     ( const char*      );
            void add_str     ( string&          );
            void add_str     ( const char*      );
            void add_num     ( string&          );
            void add_num     ( const char*      );
            void add_nul     ( void             );
            void add_tru     ( void             );
            void add_fal     ( void             );
            void add_val     ( Tokens&          );
            void add_val     ( string&          );
            void rem_all     ( void             );

            string & get_str(void) const;
        };
    }
#endif
