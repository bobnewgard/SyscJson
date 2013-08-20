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

/** \file  JsonFind.cxx
 *  \brief Defines the JsonFind and JsonFindErr classes.
 */

#include <cctype>
#include <JsonVec.h>
#include <JsonStr.h>
#include <JsonFind.h>

namespace SyscJson
{
    using namespace std;
    using namespace SyscMsg;
    using namespace SyscMsg::Chars;
    using namespace JsonParse;

    // =============================================================================
    // Class JsonFindErr
    // =============================================================================
    /** \brief Constructor for JsonFindErr exception class
     *
     *  Argument string may be used to describe the exception.
     *
     */
    JsonFindErr::JsonFindErr(string s)
    {
        this->err_msg = s;
    }

    /** \brief Destructor for JsonFindErr exception class
     *
     *  No-op.
     *
     */
    JsonFindErr::~JsonFindErr(void)
    {
    }

    /** \brief Accessor method for JsonFindErr exception message
     *
     *  Returns the message string.
     *
     */
    string
    JsonFindErr::get_msg(void)
    {
        return "JsonFindErr reports" + SP + this->err_msg;
    }

    // =============================================================================
    // Class JsonFind
    // =============================================================================
    /** \brief Constructor for JsonFind debug instance
     *
     *  The string parameter specifies that debug messages are desired,
     *  and is used to prefix all debug messages.
     *
     *  The search context, search path and context token are undefined at
     *  construction.  They are set using set_search_context(),
     *  set_search_path() or find().
     */
    JsonFind::JsonFind(const string & arg_msgid)
    {
        this->msg            = unique_ptr<Msg>(new Msg(arg_msgid.c_str()));
        this->search_context = unique_ptr<Tokens>(nullptr);
        this->search_path    = unique_ptr<Tokens>(nullptr);
        this->context_token  = unique_ptr<Token>(nullptr);
    }

    /** \brief Constructor for JsonFind non-debug instance
     *
     *  The search context, search path and context token are undefined at
     *  construction.  They are set using set_search_context(),
     *  set_search_path() or find().
     */
    JsonFind::JsonFind(void)
    {
        this->msg            = unique_ptr<Msg>(nullptr);
        this->search_context = unique_ptr<Tokens>(nullptr);
        this->search_path    = unique_ptr<Tokens>(nullptr);
        this->context_token  = unique_ptr<Token>(nullptr);
    }

    /** \brief Destructor for JsonFind
     *
     *   No-op.
     */
    JsonFind::~JsonFind(void) { }

    /** \brief Parse a JSON string
     *
     *  The token vector is loaded with tokens
     *  parsed from a JSON string argument.
     */
    void
    JsonFind::parse(Tokens & arg_tok, string & arg_str)
    {
        unique_ptr<JsonVec> jv;

        try
        {
            if (this->msg == nullptr)
            {
                jv = unique_ptr<JsonVec>(new JsonVec(arg_str));
            }
            else
            {
                jv = unique_ptr<JsonVec>(new JsonVec(arg_str, this->msg->get_str_r_msgid() + "JsonVec parse:"));
            }
        }
        catch (JsonVecErr & err)
        {
            if (this->msg != nullptr) { this->msg->cerr_err("catch() while parsing"); }
            if (this->msg != nullptr) { this->msg->cerr_err(err.get_msg()); }

            throw JsonFindErr("failure in JsonFind::parse():" + SP + err.get_msg());
        }

        arg_tok.swap(jv->get_tokens());
    }

    /** \brief Return the number of tokens in the value or [key:value] pair
     *
     *  Return
     *  * 0 for non-array, non-object value
     *  * 1 for key associated with non-array, non-object value
     *  * N for array, object or key associated with array or object value
     *
     *  Procedure
     *  1. Determine what is to be done
     *  ** Object: set up a search for end token
     *  ** Array: set up a search for end token
     *  ** Key
     *  *** For key with non-array, non-object value, return 1
     *  *** For key with an array or object, set up a search for end token
     *  ** Non-array, non-object value, return 0
     *  2. Search for end token
     *  3. Return distance from context token to end token
     */
    ptrdiff_t
    JsonFind::get_dist(void)
    {
        TokenI          dit = this->search_context_iter;
        int             sstk;
        JsonStructTypes styp_bgn;
        JsonStructTypes styp_end;

        // initial
        switch (dit->struct_type)
        {
            case json_styp_obj_bgn :
            {
                styp_bgn = json_styp_obj_bgn;
                styp_end = json_styp_obj_end;
                sstk     = 0;

                break;
            }
            case json_styp_arr_bgn :
            {
                styp_bgn = json_styp_arr_bgn;
                styp_end = json_styp_arr_end;
                sstk     = 0;

                break;
            }
            case json_styp_key :
            {
                dit++;

                switch (dit->struct_type)
                {
                    case json_styp_obj_bgn :
                    {
                        styp_bgn = json_styp_obj_bgn;
                        styp_end = json_styp_obj_end;
                        sstk     = 0;

                        break;
                    }
                    case json_styp_arr_bgn :
                    {
                        styp_bgn = json_styp_arr_bgn;
                        styp_end = json_styp_arr_end;
                        sstk     = 0;

                        break;
                    }
                    case json_styp_elem :
                    {
                        return 1;
                    }
                    default :
                    {
                        // TODO: exception
                        return 0;
                    }
                }

                break;
            }
            default :
            {
                return 0;
            }
        }

        // continue for arr and obj
        for (dit++ ; dit != search_context->end() ; dit++)
        {
            if (dit->struct_type == styp_bgn)
            {
                sstk++;
            }
            else if (dit->struct_type == styp_end)
            {
                if (sstk == 0)
                {
                    return distance(this->search_context_iter, dit);
                }
                else
                {
                    sstk--;
                }
            }
        }

        return 0;
    }

    /** \brief Set the context
     *
     *  The search context iterator is set to the search target token.
     *
     *  The search path iterator is reset to the beginning of the path.
     *
     *  The search target token is copied to the context token.
     */
    void
    JsonFind::set_context(TokenI arg)
    {
        this->context_token       = unique_ptr<Token>(new Token(*arg));
        this->search_context_iter = arg;
        this->search_path_iter    = this->search_path->begin();
    }

    /** \brief Clear the context
     *
     *  The search context iterator is set to the beginning.
     *
     *  The search path iterator is reset to the beginning.
     *
     *  The context token is cleared.
     */
    void
    JsonFind::clr_context(void)
    {
        this->context_token       = unique_ptr<Token>(new Token());
        this->search_context_iter = this->search_context->begin();
        this->search_path_iter    = this->search_path->begin();
    }

    /** \brief Initialize the search context
     *
     *  The search context is parsed from the string argument and
     *  if valid, saved.
     *
     *  The context token is cleared.
     */
    void
    JsonFind::set_search_context(string & arg_str)
    {
        this->context_token       = unique_ptr<Token>(new Token());
        this->search_context      = unique_ptr<Tokens>(new Tokens());

        this->parse(*(this->search_context), arg_str);

        this->search_context_iter = this->search_context->begin();
    }


    /** \brief Initialize the search path
     *
     *  The search path is parsed from the string argument and
     *  if valid, saved.
     *
     *  The context token is cleared.
     */
    void
    JsonFind::set_search_path(string & arg_str)
    {
        this->context_token    = unique_ptr<Token>(new Token());
        this->search_path      = unique_ptr<Tokens>(new Tokens());

        this->parse(*(this->search_path), arg_str);

        this->search_path_iter = this->search_path->begin();
    }

    /** \brief Search for a JSON value or [key, value] pair
     *
     *  If the search is successful, the context token is copied from
     *  the search target token.
     *
     *  If the search is unsuccessful, the context token is cleared.
     */
    void
    JsonFind::find(void)
    {
        /*
         *  * coded with the assumption that both search_context and search_path Token vectors represent
         *    valid JSON, as provided by JsonFind::parse()
         *  * auxiliary state variables (auxiliary to the primary state variables pit and dit)
         *    are used
         *      * sstk, search stack
         *          * when encountering an immaterial object or array during
         *            a search, sstk is incremented upon entry and decremented upon exit
         *      * sidx, search index
         *          * set at the beginning of an array to the number specified in the path;
         *            it is decremented for each item in the array; selected item is found when sidx is zero
         */
        int     sstk = 0;
        int     sidx = -1;
        TokenI  pit  = this->search_path->begin();

        this->clr_context();

        if (this->search_context->size() == 0)
        {
            return;
        }

        if (this->search_path->size() == 0)
        {
            return;
        }

        if (this->msg != nullptr)
        {
            this->msg->cerr_inf(
                "find:" +TB+ "dstyp" +TB+ "detyp" +TB+ "delem" +TB+ "pstyp" +TB+ "petyp" +TB+ "pelem" +TB+ "sstk" +TB+ "sidx"
            );
            this->msg->cerr_inf("find:" +TB+ "------------------------------------------------------------");
        }

        for (TokenI dit = this->search_context->begin() ; dit != this->search_context->end() ; dit++)
        {
            if (this->msg != nullptr)
            {
                string tmp_str = "find:" + TB;

                // dit
                tmp_str = tmp_str + str_json_struct[dit->struct_type] + TB;

                if (dit->has_elem())
                {
                    tmp_str = tmp_str + str_json_element[dit->element_type];
                }

                tmp_str = tmp_str + TB;

                if (dit->has_elem_str())
                {
                    tmp_str = tmp_str + dit->element_str;
                }

                tmp_str = tmp_str + TB;

                // pit
                tmp_str = tmp_str + str_json_struct[pit->struct_type] + TB;

                if (pit->has_elem())
                {
                    tmp_str = tmp_str + str_json_element[pit->element_type];
                }

                tmp_str = tmp_str + TB;

                if (pit->has_elem_str())
                {
                    tmp_str = tmp_str + pit->element_str;
                }

                tmp_str = tmp_str + TB;
                tmp_str = tmp_str + to_string(sstk) + TB;
                tmp_str = tmp_str + to_string(sidx);

                this->msg->cerr_inf(tmp_str);
            }

            switch (pit->struct_type)
            {
                case json_styp_obj_bgn :
                {
                    switch (dit->struct_type)
                    {
                        case json_styp_obj_bgn :
                        {
                            // pit:OS, dit:OS
                            if (sstk == 0)
                            {
                                TokenI pit_next = pit + 1;

                                if (pit_next->struct_type == json_styp_obj_end)
                                {
                                    this->set_context(dit);
                                    return;
                                }

                                pit++;
                            }

                            break;
                        }
                        case json_styp_arr_bgn :
                        {
                            // pit:OS, dit:AS
                            sstk++;
                            break;
                        }
                        case json_styp_arr_end :
                        {
                            // pit:OS, dit:AE
                            sstk--;
                            break;
                        }
                        default : break;
                    }

                    break;
                }
                case json_styp_arr_bgn :
                {
                    switch (dit->struct_type)
                    {
                        case json_styp_arr_bgn :
                        {
                            // pit:AS, dit:AS
                            if (sstk == 0)
                            {
                                TokenI pit_next = pit + 1;

                                if (pit_next->struct_type == json_styp_arr_end)
                                {
                                    this->set_context(dit);
                                    return;
                                }

                                if (pit_next->struct_type != json_styp_elem)
                                {
                                    pit = pit_next;

                                    break;
                                }

                                if (pit_next->element_type != json_etyp_num)
                                {
                                    this->clr_context();
                                    return;
                                }

                                sidx = stoul(pit_next->element_str, nullptr, 10);
                                pit  = pit_next;

                                break;
                            }

                            break;
                        }
                        case json_styp_obj_bgn :
                        {
                            // pit:AS, dit:OS
                            sstk++;
                            break;
                        }
                        case json_styp_obj_end :
                        {
                            // pit:AS, dit:OE
                            sstk--;
                            break;
                        }
                        default : break;
                    }

                    break;
                }
                case json_styp_key :
                {
                    switch (dit->struct_type)
                    {
                        case json_styp_key :
                        {
                            // pit:key, dit:key
                            if ((sstk == 0) && (dit->element_str == pit->element_str))
                            {
                                TokenI pit_next = pit + 1;
                                TokenI dit_next = dit + 1;

                                if (pit_next->struct_type == json_styp_elem)
                                {
                                    if (pit_next->element_type == json_etyp_nul)
                                    {
                                        this->set_context(dit);
                                        return;
                                    }
                                    else if (pit_next->element_type == json_etyp_tru)
                                    {
                                        this->set_context(dit_next);
                                        return;
                                    }
                                    else
                                    {
                                        this->clr_context();
                                        return;
                                    }
                                }

                                if (pit_next->struct_type != dit_next->struct_type)
                                {
                                    this->clr_context();
                                    return;
                                }

                                pit  = pit_next;
                            }

                            break;
                        }
                        case json_styp_obj_bgn :
                        {
                            // pit:key, dit:OS
                            sstk++;
                            break;
                        }
                        case json_styp_obj_end :
                        {
                            // pit:key, dit:OE
                            sstk--;
                            break;
                        }
                        case json_styp_arr_bgn :
                        {
                            // pit:key, dit:AS
                            sstk++;
                            break;
                        }
                        case json_styp_arr_end :
                        {
                            // pit:key, dit:AE
                            sstk--;
                            break;
                        }
                        default : break;
                    }

                    break;
                }
                case json_styp_elem :
                {
                    switch (dit->struct_type)
                    {
                        case json_styp_elem :
                        {
                            // pit:elem, dit:elem
                            if ((sstk == 0) && (sidx == 0))
                            {
                                // * dit is elem in array
                                // * choose action based on next path structure type
                                //   * elem, true: found, return true
                                //   * other: undefined
                                TokenI pit_1 = pit + 1;

                                if ((pit_1->struct_type == json_styp_elem) && (pit_1->element_type == json_etyp_tru))
                                {
                                    this->set_context(dit);
                                    return;
                                }
                                else
                                {
                                    this->clr_context();
                                    return;
                                }
                            }
                            else if ((sstk == 0) && (sidx > 0))
                            {
                                sidx--;
                            }
                            else if (sstk == 0)
                            {
                                this->clr_context();
                                return;
                            }

                            break;
                        }
                        case json_styp_obj_bgn :
                        {
                            // pit:elem, dit:OS
                            if ((sstk == 0) && (sidx == 0))
                            {
                                // * dit is value in array
                                // * choose action based on next path structure types
                                TokenI pit_1 = pit + 1;
                                TokenI pit_2 = pit + 2;

                                if ((pit_1->struct_type == json_styp_obj_bgn) && (pit_2->struct_type == json_styp_obj_end))
                                {
                                    this->set_context(dit);
                                    return;
                                }
                                else if (pit_1->struct_type == json_styp_obj_bgn)
                                {
                                    pit = pit_2;

                                    continue;
                                }
                                else
                                {
                                    this->clr_context();
                                    return;
                                }
                            }
                            else if ((sstk == 0) && (sidx > 0))
                            {
                                if ((pit + 1)->struct_type == json_styp_elem)
                                {
                                    sidx--;
                                }
                                else
                                {
                                    sidx--;
                                    sstk++;
                                }
                            }
                            else
                            {
                                sstk++;
                            }

                            break;
                        }
                        case json_styp_obj_end :
                        {
                            // pit:elem, dit:OE
                            sstk--;
                            break;
                        }
                        case json_styp_arr_bgn :
                        {
                            // pit:elem, dit:AS
                            if ((sstk == 0) && (sidx == 0))
                            {
                                // * dit is value in array
                                // * choose action based on next path structure types
                                TokenI pit_1 = pit + 1;
                                TokenI pit_2 = pit + 2;

                                if ((pit_1->struct_type == json_styp_arr_bgn) && (pit_2->struct_type == json_styp_arr_end))
                                {
                                    this->set_context(dit);
                                    return;
                                }
                                if (pit_1->struct_type == json_styp_arr_bgn)
                                {
                                    if (pit_2->struct_type != json_styp_elem)
                                    {
                                        pit = pit_2;

                                        break;
                                    }

                                    if (pit_2->element_type != json_etyp_num)
                                    {
                                        this->clr_context();
                                        return;
                                    }

                                    sidx = stoul(pit_2->element_str, nullptr, 10);
                                    pit  = pit_2;

                                    break;
                                }
                                else
                                {
                                    this->clr_context();
                                    return;
                                }
                            }
                            else if ((sstk == 0) && (sidx > 0))
                            {
                                if ((pit + 1)->struct_type == json_styp_elem)
                                {
                                    sidx--;
                                }
                                else
                                {
                                    sidx--;
                                    sstk++;
                                }
                            }
                            else
                            {
                                sstk++;
                            }

                            break;
                        }
                        case json_styp_arr_end :
                        {
                            // pit:elem, dit:AE
                            if ((sstk == 0) && (sidx > -1))
                            {
                                this->clr_context();
                                return;
                            }

                            sstk--;
                            break;
                        }
                        default : break;
                    }

                    break;
                }
                default : break;
            }
        }

        this->clr_context();
        return;
    }

    /** \brief Check for a cleared context
     *
     *  Returns true if the context token is cleared.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_none(void)
    {
        if (this->context_token->struct_type != json_styp_LAST) return false;
        return false;
    }

    /** \brief Check for JSON object begin at the context token
     *
     *  Returns true if the context token is a JSON object beginning token.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_obj_bgn(void)
    {
        if (this->context_token->struct_type != json_styp_obj_bgn) return false;
        return true;
    }

    /** \brief Check for JSON object end at the context token
     *
     *  Returns true if the context token is a JSON object ending token.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_obj_end(void)
    {
        if (this->context_token->struct_type != json_styp_obj_end) return false;
        return true;
    }

    /** \brief Check for JSON array begin at the context token
     *
     *  Returns true if the context token is a JSON array beginning token.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_arr_bgn(void)
    {
        if (this->context_token->struct_type != json_styp_arr_bgn) return false;
        return true;
    }

    /** \brief Check for JSON array end at the context token
     *
     *  Returns true if the context token is a JSON array ending token.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_arr_end(void)
    {
        if (this->context_token->struct_type != json_styp_arr_end) return false;
        return true;
    }

    /** \brief Check for key at the context token
     *
     *  Returns true if the context token is the key portion of a JSON [key, value] pair.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_key(void)
    {
        if (this->context_token->struct_type != json_styp_key) return false;
        if (this->context_token->element_type != json_etyp_str) return false;
        if (this->context_token->element_str == "") return false;
        return true;
    }

    /** \brief Check for JSON string at the context token
     *
     *  Returns true if the context token is a JSON string.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_str(void)
    {
        if (this->context_token->struct_type != json_styp_elem) return false;
        if (this->context_token->element_type != json_etyp_str) return false;
        return true;
    }

    /** \brief Check for JSON number at the context token
     *
     *  Returns true if the context token is a JSON number.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_num(void)
    {
        if (this->context_token->struct_type != json_styp_elem) return false;
        if (this->context_token->element_type != json_etyp_num) return false;
        if (this->context_token->element_str == "") return false;
        return true;
    }

    /** \brief Check for JSON null at the context token
     *
     *  Returns true if the context token is a JSON null.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_nul(void)
    {
        if (this->context_token->struct_type != json_styp_elem) return false;
        if (this->context_token->element_type != json_etyp_nul) return false;
        return true;
    }

    /** \brief Check for JSON true at the context token
     *
     *  Returns true if the context token is a JSON true.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_tru(void)
    {
        if (this->context_token->struct_type != json_styp_elem) return false;
        if (this->context_token->element_type != json_etyp_tru) return false;
        return true;
    }

    /** \brief Check for JSON false at the context token
     *
     *  Returns true if the context token is a JSON false.
     *
     *  The context token position is unchanged.
     */
    bool
    JsonFind::context_is_fal(void)
    {
        if (this->context_token->struct_type != json_styp_elem) return false;
        if (this->context_token->element_type != json_etyp_fal) return false;
        return true;
    }

    /** \brief Get the string representation of the JSON value 
     *         at the context token
     *
     *  For array or object begin tokens, the string is set to the JSON
     *  representation of the array or object.
     *
     *  For key, string or number tokens, the string is set to the
     *  string value of the token.
     *
     *  For null, true or false, the string is set to "null", "true" or
     *  "false", respectively.
     *
     *  The context token position is unchanged.
     */
    void
    JsonFind::get_context_string(string & arg)
    {
        if (this->context_is_none())
        {
            arg = "";
        }
        else if (this->context_is_nul())
        {
            arg = "null";
        }
        else if (this->context_is_tru())
        {
            arg = "true";
        }
        else if (this->context_is_fal())
        {
            arg = "false";
        }
        else if (this->context_is_num())
        {
            arg = this->context_token->element_str;
        }
        else if (this->context_is_str())
        {
            arg = this->context_token->element_str;
        }
        else if (this->context_is_key())
        {
            arg = this->context_token->element_str;
        }
        else
        {
            ptrdiff_t dist = this->get_dist();
            Tokens    tvec;
            JsonStr   jstr;

            tvec.assign(this->search_context_iter, (this->search_context_iter) + dist + 1);
            jstr.add_val(tvec);
            arg = jstr.get_str();
        }

        return;
    }
}
