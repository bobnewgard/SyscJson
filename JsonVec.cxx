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

#include "JsonVec.h"

namespace JsonParse
{
    using namespace std;
    using namespace SyscMsg;
    using namespace SyscMsg::Chars;
    using namespace SyscJson;

    // =============================================================================
    // Class JsonVecErr
    // =============================================================================
    JsonVecErr::JsonVecErr(string s)
    {
        this->err_msg = s;
    }

    JsonVecErr::~JsonVecErr(void)
    {
    }

    string
    JsonVecErr::get_msg(void)
    {
        return "JsonVecErr reports" + SP + this->err_msg;
    }

    // =============================================================================
    // Class JsonVec
    // =============================================================================
    JsonVec::JsonVec(const string & arg_src, const string & arg_msgid)
    {
        this->msg = unique_ptr<Msg>(new Msg(arg_msgid.c_str()));
        this->vec = unique_ptr<Tokens>(new Tokens);
        this->str = arg_src;

        try
        {
            int ret = json_parse(reinterpret_cast<void*>(this), const_cast<char*>(this->str.c_str()));

            if (ret)
            {
                this->msg->cerr_err("json_parse() returns" + SP + to_string(ret));
                throw JsonVecErr("json_parse() returns" + SP + to_string(ret));
            }
        }
        catch (JsonVecErr & err)
        {
            this->msg->cerr_err("catch() in constructor");
            this->msg->cerr_err(err.get_msg());
            throw JsonVecErr("failure in JsonVec constructor:" + SP + err.get_msg());
        }
    }

    JsonVec::JsonVec(const string & arg_src)
    {
        this->msg = unique_ptr<Msg>(nullptr);
        this->vec = unique_ptr<Tokens>(new Tokens);
        this->str = arg_src;

        try
        {
            int ret = json_parse(reinterpret_cast<void*>(this), const_cast<char*>(this->str.c_str()));

            if (ret)
            {
                throw JsonVecErr("json_parse() returns" + SP + to_string(ret));
            }
        }
        catch (JsonVecErr & err)
        {
            throw JsonVecErr("failure in JsonVec constructor:" + SP + err.get_msg());
        }
    }

    JsonVec::~JsonVec(void) { }

    void
    JsonVec::dump_vec(void)
    {
        for (TokenI it = this->vec->begin() ; it != this->vec->end() ; it++)
        {
            switch (it->struct_type)
            {
                case json_styp_obj_bgn :
                {
                    cout << "{";
                    break;
                }
                case json_styp_obj_end :
                {
                    cout << "}";
                    break;
                }
                case json_styp_arr_bgn :
                {
                    cout << "[";
                    break;
                }
                case json_styp_arr_end :
                {
                    cout << "]";
                    break;
                }
                case json_styp_key :
                {
                    cout << DQ << it->element_str << DQ << CN;
                    break;
                }
                case json_styp_elem :
                {
                    switch (it->element_type)
                    {
                        case json_etyp_str :
                        {
                            cout << DQ << it->element_str << DQ;
                            break;
                        }
                        case json_etyp_num :
                        {
                            cout << it->element_str;
                            break;
                        }
                        case json_etyp_nul :
                        {
                            cout << "null";
                            break;
                        }
                        case json_etyp_tru :
                        {
                            cout << "true";
                            break;
                        }
                        case json_etyp_fal :
                        {
                            cout << "false";
                            break;
                        }
                        default : break;
                    }
                    break;
                }
                default : break;
            }
        }

        cout << NL;
    }

    void
    JsonVec::set_obj_bgn(void)
    {
        
        JsonToken tmp(json_styp_obj_bgn);

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf("begin object");
        }
    }

    void
    JsonVec::set_obj_end(void)
    {
        JsonToken tmp(json_styp_obj_end);

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf("end object");
        }
    }

    void
    JsonVec::set_arr_bgn(void)
    {
        JsonToken tmp(json_styp_arr_bgn);

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf("begin array");
        }
    }

    void
    JsonVec::set_arr_end(void)
    {
        JsonToken tmp(json_styp_arr_end);

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf("end array");
        }
    }

    void
    JsonVec::set_obj_key(char * arg_str)
    {
        JsonToken tmp(json_styp_key, json_etyp_str, string(arg_str));

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf((DQ + tmp.element_str + DQ + CN).c_str());
        }
    }

    void
    JsonVec::set_elem_nul(void)
    {
        JsonToken tmp(json_styp_elem, json_etyp_nul);

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf("null");
        }
    }

    void
    JsonVec::set_elem_tru(void)
    {
        JsonToken tmp(json_styp_elem, json_etyp_tru);

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf("true");
        }
    }

    void
    JsonVec::set_elem_fal(void)
    {
        JsonToken tmp(json_styp_elem, json_etyp_fal);

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf("false");
        }
    }

    void
    JsonVec::set_elem_str(char * arg_str)
    {
        JsonToken tmp(json_styp_elem, json_etyp_str, string(arg_str));

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf((DQ + tmp.element_str + DQ).c_str());
        }
    }
    void
    JsonVec::set_elem_num(char * arg_str)
    {
        JsonToken tmp(json_styp_elem, json_etyp_num, string(arg_str));

        this->vec->push_back(tmp);

        if (this->msg)
        {
            this->msg->cerr_inf(tmp.element_str.c_str());
        }
    }

    Tokens &
    JsonVec::get_tokens(void)
    {
        return *(this->vec);
    }

    // =============================================================================
    // "C" Bindings for creating JsonVec instances
    // =============================================================================
    extern "C"
    {
        void c_set_obj_bgn(void * cjv)              { (reinterpret_cast<JsonVec*>(cjv))->set_obj_bgn();                }
        void c_set_obj_end(void * cjv)              { (reinterpret_cast<JsonVec*>(cjv))->set_obj_end();                }
        void c_set_arr_bgn(void * cjv)              { (reinterpret_cast<JsonVec*>(cjv))->set_arr_bgn();                }
        void c_set_arr_end(void * cjv)              { (reinterpret_cast<JsonVec*>(cjv))->set_arr_end();                }
        void c_set_obj_key(void * cjv, char * str)  { (reinterpret_cast<JsonVec*>(cjv))->set_obj_key(str); free(str);  }
        void c_set_elem_nul(void * cjv)             { (reinterpret_cast<JsonVec*>(cjv))->set_elem_nul();               }
        void c_set_elem_tru(void * cjv)             { (reinterpret_cast<JsonVec*>(cjv))->set_elem_tru();               }
        void c_set_elem_fal(void * cjv)             { (reinterpret_cast<JsonVec*>(cjv))->set_elem_fal();               }
        void c_set_elem_str(void * cjv, char * str) { (reinterpret_cast<JsonVec*>(cjv))->set_elem_str(str); free(str); }
        void c_set_elem_num(void * cjv, char * str) { (reinterpret_cast<JsonVec*>(cjv))->set_elem_num(str);            }
    }
}
