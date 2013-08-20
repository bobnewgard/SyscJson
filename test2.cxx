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

// Unit test for SyscJson

#include <systemc.h>
#include <SyscJson.h>

using namespace std;
using namespace SyscMsg;
using namespace SyscMsg::Chars;
using namespace SyscJson;

bool debug_msg      = false;
bool enable_test_01 = true;
bool enable_test_02 = true;
bool enable_test_03 = true;
bool enable_test_04 = true;
bool enable_test_05 = true;
bool enable_test_06 = true;
bool enable_test_07 = true;
bool enable_test_08 = true;
bool enable_test_09 = true;
bool enable_test_10 = true;
bool enable_test_11 = true;
bool enable_test_12 = true;
bool enable_test_13 = true;
bool enable_test_14 = true;
bool enable_test_15 = true;
bool enable_test_16 = true;
bool enable_test_17 = true;

string path_parse_err_str = "catch while parsing JSON path";

bool test_a_path(const string & arg_m, JsonFind & arg_c, string & arg_p, Token & arg_et, string & arg_es)
{
    bool  ret  = true;

    Msg    msg(arg_m);
    Token  obsv;
    string tmp_str;

    try
    {
        arg_c.set_search_path(arg_p);
    }
    catch (JsonFindErr & err)
    {
        throw JsonFindErr(path_parse_err_str);
    }

    arg_c.find();

    if (arg_c.context_is_obj_bgn())
    {
        obsv.struct_type = json_styp_obj_bgn;
    }
    else if (arg_c.context_is_obj_end())
    {
        obsv.struct_type = json_styp_obj_end;
    }
    else if (arg_c.context_is_arr_bgn())
    {
        obsv.struct_type = json_styp_arr_bgn;
    }
    else if (arg_c.context_is_arr_end())
    {
        obsv.struct_type = json_styp_arr_end;
    }
    else if (arg_c.context_is_key())
    {
        obsv.struct_type = json_styp_key;
        obsv.element_type = json_etyp_str;
        arg_c.get_context_string(obsv.element_str);
    }
    else if (arg_c.context_is_str())
    {
        obsv.struct_type = json_styp_elem;
        obsv.element_type = json_etyp_str;
        arg_c.get_context_string(obsv.element_str);
    }
    else if (arg_c.context_is_num())
    {
        obsv.struct_type = json_styp_elem;
        obsv.element_type = json_etyp_num;
        arg_c.get_context_string(obsv.element_str);
    }
    else if (arg_c.context_is_nul())
    {
        obsv.struct_type = json_styp_elem;
        obsv.element_type = json_etyp_nul;
    }
    else if (arg_c.context_is_tru())
    {
        obsv.struct_type = json_styp_elem;
        obsv.element_type = json_etyp_tru;
    }
    else if (arg_c.context_is_fal())
    {
        obsv.struct_type = json_styp_elem;
        obsv.element_type = json_etyp_fal;
    }
    else
    {
        ret = false;
    }

    if (ret)
    {
        if (obsv.struct_type != arg_et.struct_type)
        {
            msg.cerr_err(
                "fail, find() returned unexpected struct_type:" + SP
                + "exp" + SP + str_json_struct[arg_et.struct_type] + CM + SP
                + "obs" + SP + str_json_struct[obsv.struct_type]
            );
            ret = false;
        }
        else if (obsv.struct_type == json_styp_obj_bgn)
        {
            msg.cerr_inf("pass, find() returned expected object");
        }
        else if (obsv.struct_type == json_styp_arr_bgn)
        {
            msg.cerr_inf("pass, find() returned expected array");
        }
        else if (obsv.struct_type == json_styp_key)
        {
            if (obsv.element_type != json_etyp_str)
            {
                msg.cerr_err(
                    "fail, find() returned key with unexpected element_type:" + SP
                    + "exp" + SP + str_json_element[json_etyp_str] + CM + SP
                    + "obs" + SP + str_json_element[obsv.element_type]
                );
                ret = false;
            }

            if (obsv.element_str != arg_et.element_str)
            {
                msg.cerr_err(
                    "fail, find() returned key with unexpected element_str:" + SP
                    + "exp" + SP + arg_et.element_str + CM + SP
                    + "obs" + SP + obsv.element_str
                );
                ret = false;
            }
            else
            {
                if (debug_msg)
                {
                    msg.cerr_inf("pass, find() returned key with expected element_str" + SP + DQ + obsv.element_str + DQ);
                }
                else
                {
                    msg.cerr_inf("pass, find() returned expected element");
                }
            }
        }
        else if (obsv.element_type != arg_et.element_type)
        {
            msg.cerr_err(
                "fail, find() returned unexpected element_type:" + SP
                + "exp" + SP + str_json_element[arg_et.element_type] + CM + SP
                + "obs" + SP + str_json_element[obsv.element_type]
            );
            ret = false;
        }
        else if ((arg_et.has_elem_str()) && (obsv.element_str != arg_et.element_str))
        {
            msg.cerr_err(
                "fail, find() returned unexpected element_str:" + SP
                + "exp" + SP + arg_et.element_str + CM + SP
                + "obs" + SP + obsv.element_str
            );
            ret = false;
        }
        else if (obsv.element_type == json_etyp_nul)
        {
            if (debug_msg)
            {
                msg.cerr_inf("pass, find() returned expected element null");
            }
            else
            {
                msg.cerr_inf("pass, find() returned expected element");
            }
        }
        else if (obsv.element_type == json_etyp_tru)
        {
            if (debug_msg)
            {
                msg.cerr_inf("pass, find() returned expected element true");
            }
            else
            {
                msg.cerr_inf("pass, find() returned expected element");
            }
        }
        else if (obsv.element_type == json_etyp_fal)
        {
            if (debug_msg)
            {
                msg.cerr_inf("pass, find() returned expected element false");
            }
            else
            {
                msg.cerr_inf("pass, find() returned expected element");
            }
        }
        else if (obsv.element_type == json_etyp_num)
        {
            if (debug_msg)
            {
                msg.cerr_inf("pass, find() returned number with expected element_str" + SP + obsv.element_str);
            }
            else
            {
                msg.cerr_inf("pass, find() returned expected element");
            }
        }
        else
        {
            if (debug_msg)
            {
                msg.cerr_inf("pass, find() returned number with expected element_str" + SP + DQ + obsv.element_str + DQ);
            }
            else
            {
                msg.cerr_inf("pass, find() returned expected element");
            }
        }
    }
    else
    {
        if (arg_et.struct_type == json_styp_LAST)
        {
            msg.cerr_inf("pass, find() returned expected empty");
            ret = true;
        }
        else
        {
            msg.cerr_err("fail, find() returned unexpected empty");
            ret = false;
        }
    }

    arg_c.get_context_string(tmp_str);

    if (arg_es == "")
    {
        msg.cerr_inf("pass, get_context_str() not checked");
    }
    else if (tmp_str.compare(arg_es) == 0)
    {
        if (debug_msg)
        {
            msg.cerr_inf("pass, get_context_str() returned expected string:" + SP + DQ + tmp_str + DQ);
        }
        else
        {
            msg.cerr_inf("pass, get_context_str() returned expected string");
        }
    }
    else
    {
        msg.cerr_err("fail, get_context_str() returned unexpected string:" + SP + DQ + tmp_str + DQ);
        ret = false;
    }

    return ret;
}

int sc_main(int argc, char * argv[])
{

    const string  OS    = "{";
    const string  OE    = "}";
    const string  AS    = "[";
    const string  AE    = "]";

    bool                 pass;
    Msg                  msg("test2:");
    JsonStr              cstr;
    unique_ptr<JsonFind> json;

    cstr.add_obj_bgn();
        cstr.add_key("str1");
        cstr.add_str("dog");
        cstr.add_key("dat1");
        cstr.add_arr_bgn();
            cstr.add_num("0");
            cstr.add_num("1");
        cstr.add_arr_end();
        cstr.add_key("dat2");
        cstr.add_obj_bgn();
            cstr.add_key("key1");
            cstr.add_str("2");
            cstr.add_key("key2");
            cstr.add_obj_bgn();
            cstr.add_obj_end();
            cstr.add_key("key3");
            cstr.add_arr_bgn();
            cstr.add_arr_end();
        cstr.add_obj_end();
        cstr.add_key("num1");
        cstr.add_num("67");
        cstr.add_key("num2");
        cstr.add_num("-67");
        cstr.add_key("num3");
        cstr.add_num("-0.1");
        cstr.add_key("num4");
        cstr.add_num("0.5e7");
        cstr.add_key("num5");
        cstr.add_num("3.5e+7");
        cstr.add_key("num6");
        cstr.add_num("33.5e-7");
        cstr.add_key("num7");
        cstr.add_num("0.5e77");
        cstr.add_key("num8");
        cstr.add_num("0.57");
        cstr.add_key("others");
        cstr.add_arr_bgn();
            cstr.add_tru();
            cstr.add_fal();
            cstr.add_nul();
            cstr.add_num("5");
            cstr.add_str("str");
            cstr.add_obj_bgn();
                cstr.add_key("key1");
                cstr.add_str("val1");
                cstr.add_key("key2");
                cstr.add_str("val2");
            cstr.add_obj_end();
            cstr.add_arr_bgn();
                cstr.add_str("val1");
                cstr.add_str("val2");
                cstr.add_str("val3");
                cstr.add_arr_bgn();
                    cstr.add_str("val4");
                    cstr.add_str("val5");
                    cstr.add_str("val6");
                cstr.add_arr_end();
            cstr.add_arr_end();
        cstr.add_arr_end();
    cstr.add_obj_end();

    pass = true;

    if (debug_msg)
    {
        json = unique_ptr<JsonFind>(new JsonFind(msg.get_str_r_msgid() + "JsonFind:"));
    }
    else
    {
        json = unique_ptr<JsonFind>(new JsonFind());
    }

    try
    {
        json->set_search_context(cstr.get_str());
    }
    catch (JsonFindErr & err)
    {
        msg.cerr_err("catch() while parsing JSON data");
        msg.cerr_err(err.get_msg());
        return 1;
    }

    if (enable_test_01)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "1" + "]:");
        Token   etok(json_styp_elem, json_etyp_str, "dog");
        string  estr("dog");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("str1");
            pstr.add_tru();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_02)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "2" + "]:");
        Token   etok(json_styp_LAST);
        string  estr("");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("fakekey");
            pstr.add_tru();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_03)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "3" + "]:");
        Token   etok(json_styp_LAST);
        string  estr("");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("str1");
            pstr.add_obj_bgn();
                pstr.add_key("key1");
                pstr.add_tru();
            pstr.add_obj_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_04)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "4" + "]:");
        Token   etok(json_styp_elem, json_etyp_str, "2");
        string  estr("2");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("dat2");
            pstr.add_obj_bgn();
                pstr.add_key("key1");
                pstr.add_tru();
            pstr.add_obj_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_05)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "5" + "]:");
        Token   etok(json_styp_elem, json_etyp_num, "1");
        string  estr("1");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("dat1");
            pstr.add_arr_bgn();
                pstr.add_num("1");
                pstr.add_tru();
            pstr.add_arr_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_06)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "6" + "]:");
        Token   etok(json_styp_LAST);
        string  estr("");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("dat1");
            pstr.add_arr_bgn();
                pstr.add_num("2");
                pstr.add_tru();
            pstr.add_arr_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_07)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "7" + "]:");
        Token   etok(json_styp_elem, json_etyp_nul);
        string  estr("null");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_arr_bgn();
                pstr.add_num("2");
                pstr.add_tru();
            pstr.add_arr_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_08)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "8" + "]:");
        Token   etok(json_styp_elem, json_etyp_num, "-67");
        string  estr("-67");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("num2");
            pstr.add_tru();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_09)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "9" + "]:");
        Token   etok(json_styp_elem, json_etyp_str, "val2");
        string  estr("val2");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_arr_bgn();
                pstr.add_num("5");
                pstr.add_obj_bgn();
                    pstr.add_key("key2");
                    pstr.add_tru();
                pstr.add_obj_end();
            pstr.add_arr_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_10)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "10" + "]:");
        Token   etok(json_styp_elem, json_etyp_str, "val3");
        string  estr("val3");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_arr_bgn();
                pstr.add_num("6");
                pstr.add_arr_bgn();
                    pstr.add_num("2");
                    pstr.add_tru();
                pstr.add_arr_end();
            pstr.add_arr_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_11)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "11" + "]:");
        Token   etok(json_styp_obj_bgn);
        JsonStr estr;
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_arr_bgn();
                pstr.add_num("5");
                pstr.add_obj_bgn();
                pstr.add_obj_end();
            pstr.add_arr_end();
        pstr.add_obj_end();

        estr.add_obj_bgn();
            estr.add_key("key1");
            estr.add_str("val1");
            estr.add_key("key2");
            estr.add_str("val2");
        estr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr.get_str());
    }

    if (enable_test_12)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "12" + "]:");
        Token   etok(json_styp_arr_bgn);
        JsonStr estr;
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_arr_bgn();
                pstr.add_num("6");
                pstr.add_arr_bgn();
                pstr.add_arr_end();
            pstr.add_arr_end();
        pstr.add_obj_end();

        estr.add_arr_bgn();
            estr.add_str("val1");
            estr.add_str("val2");
            estr.add_str("val3");
            estr.add_arr_bgn();
                estr.add_str("val4");
                estr.add_str("val5");
                estr.add_str("val6");
            estr.add_arr_end();
        estr.add_arr_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr.get_str());
    }

    if (enable_test_13)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "13" + "]:");
        Token   etok(json_styp_arr_bgn);
        JsonStr estr;
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_arr_bgn();
            pstr.add_arr_end();
        pstr.add_obj_end();

        estr.add_arr_bgn();
            estr.add_tru();
            estr.add_fal();
            estr.add_nul();
            estr.add_num("5");
            estr.add_str("str");
            estr.add_obj_bgn();
                estr.add_key("key1");
                estr.add_str("val1");
                estr.add_key("key2");
                estr.add_str("val2");
            estr.add_obj_end();
            estr.add_arr_bgn();
                estr.add_str("val1");
                estr.add_str("val2");
                estr.add_str("val3");
                estr.add_arr_bgn();
                    estr.add_str("val4");
                    estr.add_str("val5");
                    estr.add_str("val6");
                estr.add_arr_end();
            estr.add_arr_end();
        estr.add_arr_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr.get_str());
    }

    if (enable_test_14)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "14" + "]:");
        Token   etok(json_styp_elem, json_etyp_str, "val5");
        string  estr("val5");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_arr_bgn();
                pstr.add_num("6");
                pstr.add_arr_bgn();
                    pstr.add_num("3");
                    pstr.add_arr_bgn();
                        pstr.add_num("1");
                        pstr.add_tru();
                    pstr.add_arr_end();
                pstr.add_arr_end();
            pstr.add_arr_end();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_15)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "15" + "]:");
        Token   etok(json_styp_key, json_etyp_str, "str1");
        string  estr("str1");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("str1");
            pstr.add_nul();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_16)
    {
        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "16" + "]:");
        Token   etok(json_styp_key, json_etyp_str, "others");
        string  estr("others");
        JsonStr pstr;

        pstr.add_obj_bgn();
            pstr.add_key("others");
            pstr.add_nul();
        pstr.add_obj_end();

        pass = pass & test_a_path(msgid, *json, pstr.get_str(), etok, estr);
    }

    if (enable_test_17)
    {
        bool ret;

        string  msgid(msg.get_str_r_msgid() + "test_a_path[" + "17" + "]:");
        Token   etok(json_styp_LAST);
        string  estr("");

        string pstr = "{\"frame_len\":true}{\"frame\":[0,true]}";

        try
        {
            ret = test_a_path(msgid, *json, pstr, etok, estr);
            msg.cerr_err("fail, no catch() in test_a_path[17]");
            ret = false;
        }
        catch (JsonFindErr & err)
        {
            string cmp_str = "JsonFindErr reports" + SP + path_parse_err_str;

            if (cmp_str.compare(err.get_msg()) == 0)
            {
                ret = true;

                msg.cerr_inf("pass, expected catch() in test_a_path[17]");
            }
            else
            {
                ret = false;

                msg.cerr_inf("fail, unexpected catch() in test_a_path[17]");
            }
        }

        pass = pass & ret;
    }

    if (pass)
    {
        msg.cerr_inf("pass");
    }
    else
    {
        msg.cerr_err("fail");
    }

    return 0;
}
