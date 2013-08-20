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

/** \file  JsonToken.cxx
 *  \brief Defines the JsonToken class.
 */
#include "SyscMsg.h"
#include "JsonToken.h"

namespace SyscJson
{
    using namespace std;
    using namespace SyscMsg;
    using namespace SyscMsg::Chars;

    // =============================================================================
    // Class JsonToken
    // =============================================================================
    /** \brief Constructor for JSON tokens describing key, number or string
     *
     *  Use when a token is required, for instance, to add to an existing token
     *  vector:
     *
     *          token_vector.emplace_back(styp, vtyp, str);
     */
    JsonToken::JsonToken(JsonStructTypes arg_styp, JsonElementTypes arg_vtyp, const string & arg_cstr)
    {
        struct_type  = arg_styp;
        element_type = arg_vtyp;
        element_str  = arg_cstr;
    }

    /** \brief Constructor for JSON tokens describing true, false or null
     *
     */
    JsonToken::JsonToken(JsonStructTypes arg_styp, JsonElementTypes arg_vtyp)
    {
        struct_type  = arg_styp;
        element_type = arg_vtyp;
        element_str  = "";
    }

    /** \brief Constructor for JSON tokens describing begin or end of array or object
     *
     */
    JsonToken::JsonToken(JsonStructTypes arg_styp)
    {
        struct_type  = arg_styp;
        element_type = json_etyp_LAST;
        element_str  = "";
    }

    /** \brief Constructor for an empty token
     *
     */
    JsonToken::JsonToken(void)
    {
        struct_type  = json_styp_LAST;
        element_type = json_etyp_LAST;
        element_str  = "";
    }

    /** \brief No-op
     *
     */
    JsonToken::~JsonToken(void) { }

    /** \brief True when token #struct_type is defined
     *
     */
    bool
    JsonToken::has_struc(void) const
    {
        if (this->struct_type != json_styp_LAST)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /** \brief True when token #element_type is defined
     *
     */
    bool
    JsonToken::has_elem(void) const
    {
        if (this->element_type != json_etyp_LAST)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /** \brief True when token #element_str is defined
     *
     */
    bool
    JsonToken::has_elem_str(void) const
    {
        if ((this->has_elem()) && (this->element_type == json_etyp_str))
        {
            return true;
        }
        else if ((this->has_elem()) && (this->element_type == json_etyp_num))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /** \brief Appends decoded token information to the argument string
     *
     */
    void
    JsonToken::dump_token(string & str)
    {
        // struct
        if (this->has_struc())
        {
            str.append(NL + TB + str_json_struct[this->struct_type]);
        }
        else
        {
            str.append(NL + TB + "empty");
            return;
        }

        // element
        if (this->has_elem())
        {
            str.append(TB + str_json_element[this->element_type]);
        }
        else
        {
            return;
        }

        // string
        if (this->has_elem_str())
        {
            if (this->element_type == json_etyp_str)
            {
                str.append(TB + DQ + this->element_str + DQ);
            }
            else
            {
                str.append(TB + this->element_str);
            }
        }

        return;
    }

    /** \brief Copies json_etyp_str to the argument string
     *
     *  Returns true when Token contains a json_etyp_str to copy.
     */
    bool
    JsonToken::get_token_str(string & str)
    {
        // struct
        if (!this->has_struc())
        {
            return false;
        }

        // element
        if (! this->has_elem())
        {
            return false;
        }

        // string
        if (!this->has_elem_str())
        {
            return false;
        }

        // json_etyp_str
        if (this->element_type != json_etyp_str)
        {
            return false;
        }

        str.clear();
        str.insert(0, this->element_str);
        return true;
    }
}
