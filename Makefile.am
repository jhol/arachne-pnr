##
## Copyright (C) 2015-2016 Cotton Seed <cotton@alum.mit.edu>
## Copyright (C) 2016 Joel Holdsworth <joel@airwebreathe.org.uk>
##
## This file is part of arachne-pnr.  Arachne-pnr is free software;
## you can redistribute it and/or modify it under the terms of the GNU
## General Public License version 2 as published by the Free Software
## Foundation.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program. If not, see <http://www.gnu.org/licenses/>.
##

AM_CXXFLAGS = -I$(abs_top_srcdir)/src -std=c++11 -MD -Wall -Wshadow \
	-Wsign-compare -Werror

bin_PROGRAMS = bin/arachne-pnr
noinst_PROGRAMS = \
	tools/gen-ice40-chipdb/gen-ice40-chipdb \
	tests/test_bv \
	tests/test_us

bin_arachne_pnr_SOURCES = \
	src/configuration/configuration.cc \
	src/configuration/bit.cc \
	src/configuration/bitval.cc \
	src/chipdb/chipdb.cc \
	src/chipdb/location.cc \
	src/chipdb/switch.cc \
	src/chipdb/tiletype.cc \
	src/netlist/const.cc \
	src/netlist/design.cc \
	src/netlist/instance.cc \
	src/netlist/model.cc \
	src/netlist/models.cc \
	src/netlist/net.cc \
	src/netlist/node.cc \
	src/netlist/port.cc \
	src/parse/blif.cc \
	src/parse/pcf.cc \
	src/process/constant.cc \
	src/process/global.cc \
	src/process/io.cc \
	src/process/pack.cc \
	src/process/place.cc \
	src/process/route.cc \
	src/process/process.cc \
	src/arachne-pnr.cc \
	src/constraints.cc \
	src/designstate.cc \
	src/lexical_position.cc \
	src/line_parser.cc \
	src/util.cc

tools_gen_ice40_chipdb_gen_ice40_chipdb_SOURCES = \
	src/chipdb/chipdb.cc \
	src/chipdb/switch.cc \
	src/configuration/bit.cc \
	src/lexical_position.cc \
	src/line_parser.cc \
	src/util.cc \
	tools/gen-ice40-chipdb/chipdbparser.cc \
	tools/gen-ice40-chipdb/gen-ice40-chipdb.cc


CXXLINK_FOR_BUILD = $(CXX_FOR_BUILD) $(CXXFLAGS_FOR_BUILD) \
	$(CPPFLAGS_FOR_BUILD) $(LDFLAGS_FOR_BUILD) $(TARGET_ARCH_FOR_BUILD)

tools_gen_ice40_chipdb_gen_ice40_chipdb$(BUILD_EXEEXT) : \
	$(tools_gen_ice40_chipdb_gen_ice40_chipdb_OBJECTS)
	$(CXXLINK_FOR_BUILD) $^ $(LOADLIBS_FOR_BUILD) $(LDLIBS_FOR_BUILD) -o $@

$(tools_gen_ice40_chipdb_gen_ice40_chipdb_OBJECTS) : \
	CXX=$(CXX_FOR_BUILD)
$(tools_gen_ice40_chipdb_gen_ice40_chipdb_OBJECTS) : \
	CXXFLAGS=$(CXXFLAGS_FOR_BUILD)
$(tools_gen_ice40_chipdb_gen_ice40_chipdb_OBJECTS) : \
	CPPFLAGS=$(CPPFLAGS_FOR_BUILD)

tests_test_bv_SOURCES = tests/test_bv.cc
tests_test_us_SOURCES = tests/test_us.cc

arachne_pnr_datadir = $(datadir)/arachne-pnr
arachne_pnr_data_DATA = \
	share/arachne-pnr/chipdb-1k.bin \
	share/arachne-pnr/chipdb-8k.bin

share/arachne-pnr/chipdb-1k.bin: $(ICEBOX_DIR)/chipdb-1k.txt \
	./tools/gen-ice40-chipdb/gen-ice40-chipdb
	@$(MKDIR_P) share/arachne-pnr
	$(AM_V_GEN)./tools/gen-ice40-chipdb/gen-ice40-chipdb $< $@

share/arachne-pnr/chipdb-8k.bin: $(ICEBOX_DIR)/chipdb-8k.txt \
	./tools/gen-ice40-chipdb/gen-ice40-chipdb
	@$(MKDIR_P) share/arachne-pnr
	$(AM_V_GEN)./tools/gen-ice40-chipdb/gen-ice40-chipdb $< $@

if ENABLE_TESTS

simpletest: all ./tests/test_bv ./tests/test_us
	./tests/test_bv
	./tests/test_us
	cd tests/simple && bash run-test.sh
	cd tests/regression && bash run-test.sh
	cd tests/blif && bash run-test.sh
	cd tests/error && bash run-test.sh
	@echo
	@echo 'All tests passed.'
	@echo

test: all tests/test_bv tests/test_us
	./tests/test_bv
	./tests/test_us
	$(MAKE) -C examples/rot clean && $(MAKE) -C examples/rot
	cd tests/simple && bash run-test.sh
	cd tests/regression && bash run-test.sh
	cd tests/blif && bash run-test.sh
	cd tests/error && bash run-test.sh
	cd tests/fsm && bash run-test.sh
	cd tests/combinatorial && bash run-test.sh
	@echo
	@echo 'All tests passed.'
	@echo

endif

if ENABLE_DEVELOPER_MODE

testvg:
	cd tests/simple && bash run-valgrind-test.sh
	@echo
	@echo 'All tests passed.'
	@echo

endif

.PHONY: clean-local
clean-local:
	rm -f share/arachne-pnr/*.bin
