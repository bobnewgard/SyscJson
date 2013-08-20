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

/** \file  JsonToken.h
 *  \brief Declares the JsonToken class.
 *
 *  This file declares the JsonToken class along with it's helper types and
 *  constants.
 *
 *  It also contains doxygen text that may not fit anywhere else.
 *
 */

#ifndef _JSON_TOK_H_
    #define _JSON_TOK_H_

    #include <array>
    #include <vector>

    /** \brief Public namespace for the SyscJson library, libsyscjson
     *
     *  The SyscJson library is separated into two namespaces, SyscJson
     *  and JsonParser.  The JsonParser namespace is intended to be
     *  visible only inside the SyscJson library, and not visible to
     *  users of the library.  This separation may be enforced by 
     *  installing only JsonToken.h, JsonFind.h and JsonStr.h into
     *  /usr/include/syscjson
     */
    namespace SyscJson
    {
        using std::string;
        using std::array;
        using std::vector;

        /** \brief JSON token structural types
         *
         *  The JSON structural types are enumerated as JsonStrucTypes,
         *  which indicate
         *  + the start and end of JSON objects and arrays
         *  + JSON values that represent object keys
         *  + non-object, non-array JSON values (AKA elements)
         */
        enum JsonStructTypes
        {
            json_styp_obj_bgn,    /**< object begin                */
            json_styp_obj_end,    /**< object end                  */
            json_styp_arr_bgn,    /**< array begin                 */
            json_styp_arr_end,    /**< array end                   */
            json_styp_key,        /**< object key                  */
            json_styp_elem,       /**< non-array, non-object value */
            json_styp_LAST        /**< end of enumeration          */
        };

        // TBD, dox >= 1.8.2
        const array <string, json_styp_LAST> str_json_struct
        {
            {
                "obj_bgn",
                "obj_end",
                "arr_bgn",
                "arr_end",
                "key",
                "elem",
            }
        };

        /** \brief JSON token element types
         *
         *  The JSON element types (JSON non-array and non-object values)
         *  are enumerated as JsonElementTypes, which indicate the JSON
         *  number, string, true, false or null.
         */
        enum JsonElementTypes
        {
            json_etyp_str,        /**< string value             */
            json_etyp_num,        /**< number value             */
            json_etyp_nul,        /**< null value               */
            json_etyp_tru,        /**< true value               */
            json_etyp_fal,        /**< false value              */
            json_etyp_LAST,       /**< end of enumeration       */
        };

        // TBD, dox >= 1.8.2
        const array <string, json_etyp_LAST> str_json_element
        {
            {
                "str",
                "num",
                "nul",
                "tru",
                "fal",
            }
        };

        /** \class JsonToken
         *  \brief Container for Token
         *
         *  This class is used by the JSON parser that underlies SyscJson
         *  to hold a parsed token.
         *  The JSON parser creates a representation of a JSON string by
         *  tokenizing the string and serializing the tokens into a
         *  vector<\ref JsonToken>, a.k.a. ::Tokens
         *
         *  This class may be used to discover and use the contents of a
         *  JsonToken.
         *
         */
        /** \var   JsonToken::struct_type
         *  \brief JSON structural type
         *
         */
        /** \var   JsonToken::element_type
         *  \brief JSON element type
         *
         */
        /** \var   JsonToken::element_str
         *  \brief String data for JSON string and number types
         *
         */
        class JsonToken
        {
            public:
            JsonStructTypes  struct_type;
            JsonElementTypes element_type;
            string           element_str;

            JsonToken(JsonStructTypes, JsonElementTypes, const string&);
            JsonToken(JsonStructTypes, JsonElementTypes);
            JsonToken(JsonStructTypes);
            JsonToken(void);
            ~JsonToken(void);

            bool has_struc     ( void    ) const;
            bool has_elem      ( void    ) const;
            bool has_elem_str  ( void    ) const;
            void dump_token    ( string& );
            bool get_token_str ( string& );
        };

        /** \typedef Tokens
         *  \brief   Internal representation of JSON 
         *
         *  The parser loads a Tokens instance with parsed JSON tokens.
         *  Tokens instances may be passed to JsonStr methods.
         */
        /** \typedef TokenI
         *  \brief   Iterator for the internal representation
         */
        /** \typedef TokenCI
         *  \brief   Constified iterator for the internal representation
         */
        /** \typedef Token
         *  \brief   Hopefully more useful alias for JsonToken
         */
        typedef vector    <JsonToken>                 Tokens;
        typedef vector    <JsonToken>::iterator       TokenI;
        typedef vector    <JsonToken>::const_iterator TokenCI;
        typedef JsonToken                             Token;
    }
#endif
