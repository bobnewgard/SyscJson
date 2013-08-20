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

int  json_parse(void*, char*);
void c_set_obj_bgn(void*);
void c_set_obj_end(void*);
void c_set_arr_bgn(void*);
void c_set_arr_end(void*);
void c_set_obj_key(void*, char*);
void c_set_elem_nul(void*);
void c_set_elem_tru(void*);
void c_set_elem_fal(void*);
void c_set_elem_str(void*, char*);
void c_set_elem_num(void*, char*);
