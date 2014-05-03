#
# Copyright 2013 Robert Newgard
#
# This file is part of SyscJson.
#
# SyscJson is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SyscJson is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SyscJson.  If not, see <http://www.gnu.org/licenses/>.
#

#%# Makefile for lib
#
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:
#
# includes
# ------------------------------------------------------------------------------
include dep.mk
#
# components
# ------------------------------------------------------------------------------
define print-hint-header
	@col=$$(tput cols) ; while [ "$$col" -gt 0 ] ; do echo -n '-' ; col=$$(($$col - 1)) ; done
	@echo
endef
#
define print-hints
	@echo "lib      build $(LIBNAM).so"
	@echo "test-2   build test 2"
	@echo "run2     run test-2"
	@echo "run2u    run test-2, with LANG=\"en_US.utf8\""
	@echo "run2v    run test-2, under valgrind"
	@echo "dox      doxygen generate"
	@echo "clean    remove build intermediates"
	@echo "spotless remove build intermediates and library"
	@echo
	@echo "OBJC   is $(OBJC)"
	@echo "OBJCXX is $(OBJCXX)"
endef
#
define phonys
    lib clean run run2 run2u run2v
endef
#
define srcy
    json_lex.l
    json_parse.y
endef
#
#
define srcc
    parser.c
endef
#
define srccxx
    JsonFind.cxx
    JsonToken.cxx
    JsonStr.cxx
    JsonVec.cxx
endef
#
# build
# ------------------------------------------------------------------------------
# bison --graph=parser.dot json_parse.y
# dot -Tsvg parser.dot -o parser.svg
# flex -b -d json_lex.l
define bldy
	bison json_parse.y
	flex json_lex.l
endef
#
# try to eliminate loop
# * try setting infile to -
define cpp-for-depends
	@echo "[INF] building $@"
	rm -f $@
        for file in $^ ; do cpp $(CXX_VER) -MM $(IPATH) -MF tmp $$file        ; cat tmp >> $@ ; done
        for file in $^ ; do cpp $(CXX_VER) -MM -MT $@ $(IPATH) -MF tmp $$file ; cat tmp >> $@ ; done
	rm -f tmp
endef
#
define compile-c-for-obj
	g++ $(C_VER) $(DFLAGS) -c $(IPATH) $(CFLAGS) -o $@ $<
endef
#
define compile-cxx-for-obj
	g++ $(CXX_VER) -c $(IPATH) $(CFLAGS) -o $@ $<
endef
#
define ar-for-archive
        ar cr $@ $%
endef
#
define link-for-shared-obj
	g++ $(CFLAGS) -shared -Wl,--whole-archive $^ -Wl,--no-whole-archive -o $@
endef
#
define dox
	rm -f Readme.md
	echo "\\mainpage %SyscJson Project" >> Readme.md
	cat README.md >> Readme.md
	doxygen dox.conf
endef
#
# variables
# ------------------------------------------------------------------------------
SHELL     := /bin/bash
SYSCMSG   := ../SyscMsg
SYSC_DIR  := /opt/systemc/2.3.0
C_VER     := -x c -std=gnu11
CXX_VER   := -x c++ -std=c++11
DFLAGS    := -DYY_NO_INPUT
CFLAGS    := -Wall -m64 -g -pthread -fPIC
LFLAGS    := -L . -L $(SYSCMSG) -L $(SYSC_DIR)/lib-linux64
IPATH     := -I . -I $(SYSCMSG) -isystem $(SYSC_DIR)/include
LIBNAM    := libsyscjson
SRCY      := $(strip $(srcy))
SRCC      := $(strip $(srcc))
SRCCXX    := $(strip $(srccxx))
SRCA      := $(SRCC) $(SRCCXX)
OBJC      := $(patsubst %.c,%.o,$(SRCC))
OBJCXX    := $(patsubst %.cxx,%.o,$(SRCCXX))
OBJA      := $(OBJC) $(OBJCXX)
LIBA      := $(LIBNAM).a($(OBJA))
#
# intermdiate rules
# ------------------------------------------------------------------------------
.INTERMEDIATE : $(OBJA)
#
# library rules
# ------------------------------------------------------------------------------
parser.c       : $(SRCY)       ; $(bldy)
(%)            : %             ; $(ar-for-archive)
%.o            : %.c           ; $(compile-c-for-obj)
%.o            : %.cxx         ; $(compile-cxx-for-obj)
$(LIBNAM).a    : $(LIBA)       ;
$(LIBNAM).so   : $(LIBNAM).a   ; $(link-for-shared-obj)
dep.mk         : $(SRCA)       ; $(cpp-for-depends)
lib            : $(LIBNAM).so  ;
#
# test rules
# ------------------------------------------------------------------------------
TPREQ := $(LIBNAM).so
LPATH := $(SYSC_DIR)/lib-linux64:$(SYSCMSG):.
#
test2.o  : test2.cxx            ; $(compile-cxx-for-obj)
test-2   : test2.o $(TPREQ)     ; g++ $(CFLAGS) test2.o -o test-2 $(LFLAGS) -lsystemc -lsyscjson -lsyscmsg
run2     : test-2               ; export LD_LIBRARY_PATH=$(LPATH) ; ./test-2
run2u    : test-2               ; export LANG="en_US.utf8" ; export LD_LIBRARY_PATH=$(LPATH) ; ./test-2
run2v    : test-2               ; export LD_LIBRARY_PATH=$(LPATH) ; valgrind --leak-check=yes ./test-2
#
run      : run2
#
# Doxygen rules
# ------------------------------------------------------------------------------
dox : ; $(dox)
#
# cleanup rules
# ------------------------------------------------------------------------------
clean    :       ; rm -rf lexer* parser* *.o *.a *.so *.mk test-* Readme.md
spotless : clean ; rm -rf doxygen
#
# other rules
# ------------------------------------------------------------------------------
hints       : hint-header ; $(print-hints)
hint-header :             ; $(print-hint-header)
.PHONY      : $(phonys)   ; 
#
.DEFAULT_GOAL := hints
