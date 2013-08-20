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

/** \file  JsonStr.cxx
 *  \brief Defines the JsonStr class.
 */
#include <cctype>
#include <JsonVec.h>
#include <JsonToken.h>
#include <JsonStr.h>

namespace SyscJson
{
    using namespace std;
    using namespace SyscMsg;
    using namespace SyscMsg::Chars;

    // =============================================================================
    // Class JsonStr
    // =============================================================================
    /** \brief Constructor for JsonStr.
     *
     *  Instantiates a JsonStr instance.  JsonStr contains a
     *  string object class attribute which is initialized to
     *  the empty state.
     */
    JsonStr::JsonStr(void)
    {
        this->str = unique_ptr<string>(new string());
    }

    /** \brief Destructor for JsonStr.
     *
     *  Deletes the string object.
     */
    JsonStr::~JsonStr(void) {}

    /* returns a boolean indicating that a comma
     * needs to be appended to the current value
     * of this->str.
     * 
     * Algorithm
     * 1. return false if string is empty
     * 2. find the last JSON character
     * 3. return true or false based on the last JSON character
     */
    bool
    JsonStr::need_comma(void)
    {
        string::const_iterator sit;

        if (this->str->length() == 0)
        {
            return false;
        }

        for (sit = this->str->cend() ; sit != this->str->begin() ; sit--)
        {
            if ((isprint(*sit)) && (!(isspace(*sit))))
            {
                break;
            }
        }

        switch (*sit)
        {
            case ',' : return false;
            case ':' : return false;
            case '{' : return false;
            case '[' : return false;
            default  : return true;
        }
    }

    /** \brief Aquire access to string class attribute
     *
     *  Returns a const reference to the string class attribute
     */
    string &
    JsonStr::get_str(void) const
    {
        return *(this->str);
    }

    /** \brief Append a JSON object start character
     *
     *  Appends the '{' character (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_obj_bgn(void)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append("{");

        return;
    }

    /** \brief Append a JSON object end character
     *
     *  Appends the '}' character to the string class attribute
     */
    void
    JsonStr::add_obj_end(void)
    {
        this->str->append("}");

        return;
    }

    /** \brief Append a JSON array start character
     *
     *  Appends the '[' character (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_arr_bgn(void)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append("[");

        return;
    }

    /** \brief Append a JSON array end character
     *
     *  Appends the ']' character to the string class attribute
     */
    void
    JsonStr::add_arr_end(void)
    {
        this->str->append("]");

        return;
    }

    /** \brief Append a JSON object key string
     *
     *  Appends the string
     *
     *          '"' + arg_key + '"' + ':'
     *
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_key(string & arg_key)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append(DQ + arg_key + DQ + CN);

        return;
    }

    /** \brief Append a JSON object key string
     *
     *  Appends the string
     *
     *          '"' + string(arg_key) + '"' + ':'
     *
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_key(const char * arg_key)
    {
        string tmp = string(arg_key);
        this->add_key(tmp);
        return;
    }

    /** \brief Append a JSON string value
     *
     *  Appends the string
     *
     *          '"' + arg_val + '"'
     *
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_str(string & arg_val)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append(DQ + arg_val + DQ);

        return;
    }

    /** \brief Append a JSON string value
     *
     *  Appends the string
     *
     *          '"' + string(arg_val) + '"'
     *
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_str(const char * arg_val)
    {
        string tmp = string(arg_val);
        this->add_str(tmp);
        return;
    }

    /** \brief Append a JSON number value
     *
     *  Appends the string
     *
     *          arg_val
     *
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_num(string & arg_val)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append(arg_val);

        return;
    }

    /** \brief Append a JSON number value
     *
     *  Appends the string
     *
     *          string(arg_val)
     *
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_num(const char * arg_val)
    {
        string tmp = string(arg_val);
        this->add_num(tmp);
        return;
    }

    /** \brief Append a JSON null value
     *
     *  Appends the string "null"
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_nul(void)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append("null");

        return;
    }

    /** \brief Append a JSON true value
     *
     *  Appends the string "true"
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_tru(void)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append("true");

        return;
    }

    /** \brief Append a JSON false value
     *
     *  Appends the string "false"
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_fal(void)
    {
        if (this->need_comma())
        {
            this->str->append(CM);
        }

        this->str->append("false");

        return;
    }

    /** \brief Append a JSON snippet from a token vector
     *
     *  Appends a string based on the arg_val argument
     *  (with a leading comma if required) to the string class attribute
     */
    void
    JsonStr::add_val(Tokens & arg_val)
    {
        for (TokenI dit = arg_val.begin() ; dit != arg_val.end() ; dit++)
        {
            switch (dit->struct_type)
            {
                case json_styp_obj_bgn :
                {
                    this->add_obj_bgn();

                    break;
                }
                case json_styp_obj_end :
                {
                    this->add_obj_end();

                    break;
                }
                case json_styp_arr_bgn :
                {
                    this->add_arr_bgn();

                    break;
                }
                case json_styp_arr_end :
                {
                    this->add_arr_end();

                    break;
                }
                case json_styp_key :
                {
                    this->add_key(dit->element_str);

                    break;
                }
                case json_styp_elem :
                {
                    switch (dit->element_type)
                    {
                        case json_etyp_str :
                        {
                            this->add_str(dit->element_str);

                            break;
                        }
                        case json_etyp_num :
                        {
                            this->add_num(dit->element_str);

                            break;
                        }
                        case json_etyp_nul :
                        {
                            this->add_nul();

                            break;
                        }
                        case json_etyp_tru :
                        {
                            this->add_tru();

                            break;
                        }
                        case json_etyp_fal :
                        {
                            this->add_fal();

                            break;
                        }
                        default : break;
                    }

                    break;
                }
                default : break;
            }
        }

        return;
    }

    /** \brief Append a JSON snippet from a string
     *
     *  Appends a string based on the arg_val argument
     *  (with a leading comma if required) to the string class attribute
     *
     *  TBD: check for leading comma in arg_val
     */
    void
    JsonStr::add_val(string & arg_val)
    {
        this->add_num(arg_val);
    }

    /** \brief Remove all JSON values
     *
     *  Resets the JsonStr to empty.
     */
    void
    JsonStr::rem_all(void)
    {
        this->str->clear();
    }
}
