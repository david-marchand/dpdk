# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2024 Red Hat, Inc.

ifeq ($(V),)
Q ?= @
else
Q =
endif

O ?= build

PKGCONF ?= pkg-config

ifneq ($(shell $(PKGCONF) --exists libdpdk && echo 0),0)
$(error "no installation of DPDK found")
endif

ifeq ($(I),)

.PHONY: headers_list
.ONESHELL:
headers_list:
	$(Q)for dir in $$($(PKGCONF) --cflags-only-I libdpdk); do
		dir=$${dir##-I}
		[ -e $$dir/rte_build_config.h ] || continue
		$(MAKE) I="$$dir" O="$(O)"
		break
	done
else

HEADERS := $(shell find $(I) -name "*.h" | grep -vE $(I)'/(generic|internal)/')
SRCS := $(patsubst $(I)/%.h, $(O)/%.c, $(HEADERS))
.PRECIOUS: $(SRCS)

PC_FILE := $(shell $(PKGCONF) --path libdpdk 2>/dev/null)
CFLAGS = $(shell $(PKGCONF) --cflags libdpdk)
LDFLAGS = $(shell $(PKGCONF) --libs libdpdk)

all: $(O)/chkincs $(O)/chkincs-exp $(O)/chkincs-all

OBJS := $(O)/main.o $(patsubst %.c, %.o, $(SRCS))
OBJS_EXP := $(O)/main.o $(patsubst %.c, %_exp.o, $(SRCS))
OBJS_ALL := $(O)/main.o $(patsubst %.c, %_all.o, $(SRCS))

$(O):
	$(Q)mkdir -p $(O)

$(O)/%.c: $(I)/%.h $(O) gen_c_file_for_header.py Makefile
	$(Q)python3 gen_c_file_for_header.py $< $@

$(O)/chkincs: $(OBJS)
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

$(O)/chkincs-exp: $(OBJS_EXP)
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

$(O)/chkincs-all: $(OBJS_ALL)
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

$(O)/main.o: main.c $(O) Makefile $(PC_FILE)
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

$(O)/%.o: $(O)/%.c Makefile $(PC_FILE)
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

$(O)/%_exp.o: $(O)/%.c Makefile $(PC_FILE)
	$(Q)$(CC) $(CFLAGS) -DALLOW_EXPERIMENTAL_API -o $@ -c $<

$(O)/%_all.o: $(O)/%.c Makefile $(PC_FILE)
	$(Q)$(CC) $(CFLAGS) -DALLOW_EXPERIMENTAL_API -DALLOW_INTERNAL_API -o $@ -c $<

CXXFLAGS = $(shell $(PKGCONF) --cflags libdpdk)

all: $(O)/chkincs-cpp $(O)/chkincs-cpp-exp $(O)/chkincs-cpp-all

OBJS_CPP := $(O)/main_cpp.o $(patsubst %.c, %.cpp.o, $(SRCS))
OBJS_CPP_EXP := $(O)/main_cpp.o $(patsubst %.c, %_exp.cpp.o, $(SRCS))
OBJS_CPP_ALL := $(O)/main_cpp.o $(patsubst %.c, %_all.cpp.o, $(SRCS))

$(O)/chkincs-cpp: $(OBJS_CPP)
	$(Q)$(CXX) $(LDFLAGS) -o $@ $^

$(O)/chkincs-cpp-exp: $(OBJS_CPP_EXP)
	$(Q)$(CXX) $(LDFLAGS) -o $@ $^

$(O)/chkincs-cpp-all: $(OBJS_CPP_ALL)
	$(Q)$(CXX) $(LDFLAGS) -o $@ $^

$(O)/main_cpp.o: main.c $(O) Makefile $(PC_FILE)
	$(Q)$(CXX) $(CXXFLAGS) -o $@ -c $<

$(O)/%.cpp.o: $(O)/%.c Makefile $(PC_FILE)
	$(Q)$(CXX) $(CXXFLAGS) -o $@ -c $<

$(O)/%_exp.cpp.o: $(O)/%.c Makefile $(PC_FILE)
	$(Q)$(CXX) $(CXXFLAGS) -DALLOW_EXPERIMENTAL_API -o $@ -c $<

$(O)/%_all.cpp.o: $(O)/%.c Makefile $(PC_FILE)
	$(Q)$(CXX) $(CXXFLAGS) -DALLOW_EXPERIMENTAL_API -DALLOW_INTERNAL_API -o $@ -c $<

endif
