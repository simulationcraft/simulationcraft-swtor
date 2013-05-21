# ==========================================================================
# SimulationCraft for Star Wars: The Old Republic
# http://code.google.com/p/simulationcraft-swtor/
# ==========================================================================

# To build on Unix/Mac: make OS=UNIX
# To build on Windows with MinGW: c:\mingw\bin\mingw32-make

# To build debuggable executable, add OPTS=-g to cmd-line invocation
# By default, 32-bit binary is built.  To build a 64-bit binary, add BITS=64 to the cmd-line invocation
# Override MODULE on the cmd-line invocation if you want to build a custom named executable, e.g. 'simc64'
# Override OBJ_DIR if you want your object files built somewhere other than the local directory

OS         =

CXX        = g++ -std=c++0x
CPP_FLAGS  = -Wall -Wextra -Woverloaded-virtual -I.
OPTS       = -O3 -fomit-frame-pointer -ffast-math
BITS       = 32

-include ../build.conf

ifeq (${OS},)
ifdef SystemRoot
OS         = WINDOWS
else
OS         = UNIX
endif
endif

ifeq (64,${BITS})
CPP_FLAGS += -m64
LINK_FLAGS+= -m64
endif

ifneq (${NO_DEBUG},)
  CPP_FLAGS += -DNDEBUG
endif
# Windows platform with MinGW32
ifeq (${OS},WINDOWS)
MODULE_EXT = .exe
CPP_FLAGS += -DUNICODE
LINK_LIBS  = -lws2_32 -lwininet
ifeq ($(NO_STATIC),)
LINK_FLAGS += -static
endif
COPY       = copy
REMOVE     = del /f
PATHSEP    = \\
AR         = ar
# MinGW W32
ifeq (32,${BITS})
CPP_FLAGS += -malign-double
endif
endif

# Any UNIX platform
ifeq (${OS},UNIX)
LINK_LIBS += -lpthread
AR         = ar
COPY       = cp
REMOVE     = rm -f
PATHSEP    = /
ifneq (64,${BITS})
ifeq (i386,${HOSTTYPE})
CPP_FLAGS  += -malign-double
endif
ifeq (i686,${HOSTTYPE})
CPP_FLAGS  += -malign-double
endif
endif
endif

MODULE     = simc$(MODULE_EXT)

ifneq ($(OPENSSL_PATH),)
CPP_FLAGS += -DUSE_OPENSSL -I$(OPENSSL_PATH)/include
LINK_LIBS += -L$(OPENSSL_PATH)/lib -lssleay32
endif

SRC = \
	simulationcraft.hpp \
	utf8.h \
	sc_action.cpp \
	sc_buff.cpp \
	sc_chart.cpp \
	sc_consumable.cpp \
	sc_dot.cpp \
	sc_enchant.cpp \
	sc_expressions.cpp \
	sc_event.cpp \
	sc_gear_stats.cpp \
	sc_heal.cpp \
	sc_http.cpp \
	sc_item.cpp \
	sc_js.cpp \
	sc_knotor.cpp \
	sc_log.cpp \
	sc_main.cpp \
	sc_mrrobot.cpp \
	sc_option.cpp \
	sc_pet.cpp \
	sc_player.cpp \
	sc_plot.cpp \
	sc_raid_event.cpp \
	sc_rating.cpp \
	sc_reforge_plot.cpp \
	sc_report_html_player.cpp \
	sc_report_html_sim.cpp \
	sc_report_text.cpp \
	sc_report_xml.cpp \
	sc_report.cpp \
	sc_rng.cpp \
	sc_sample_data.cpp \
	sc_scaling.cpp \
	sc_sequence.cpp \
	sc_set_bonus.cpp \
	sc_sim.cpp \
	sc_stats.cpp \
	sc_talent.cpp \
	sc_target.cpp \
	sc_thread.cpp \
	sc_torhead.cpp \
	sc_unique_gear.cpp \
	sc_util.cpp \
	sc_weapon.cpp \
	sc_xml.cpp \
	class_modules$(PATHSEP)agent_smug.hpp \
	class_modules$(PATHSEP)sc_agent_smug.cpp \
	class_modules$(PATHSEP)bount_troop.hpp \
	class_modules$(PATHSEP)cons_inq.hpp \
	class_modules$(PATHSEP)warr_knight.hpp \
	class_modules$(PATHSEP)sc_warr_knight.cpp \
	class_modules$(PATHSEP)sc_commando_mercenary.cpp \
	class_modules$(PATHSEP)sc_gunslinger_sniper.cpp \
	class_modules$(PATHSEP)sc_juggernaut_guardian.cpp \
	class_modules$(PATHSEP)sc_sage_sorcerer.cpp \
	class_modules$(PATHSEP)sc_scoundrel_operative.cpp \
	class_modules$(PATHSEP)sc_sentinel_marauder.cpp \
	class_modules$(PATHSEP)sc_shadow_assassin.cpp \
	class_modules$(PATHSEP)sc_vanguard_powertech.cpp

SRC_H   := $(filter %.h, $(SRC)) $(filter %.hh, $(SRC)) $(filter %.hpp, $(SRC))
SRC_CPP := $(filter %.cpp, $(SRC))
OBJ_DIR = .
OBJ_EXT = o
SRC_OBJ	:= $(SRC_CPP:%.cpp=$(OBJ_DIR)$(PATHSEP)%.$(OBJ_EXT))

.PHONY:	all mostlyclean clean

all: $(MODULE)

install: all
	$(COPY) $(MODULE) ..

$(MODULE): $(SRC_OBJ)
	-@echo [$(MODULE)] Linking $@
	@$(CXX) $(OPTS) $(LINK_FLAGS) $^ $(LINK_LIBS) -o $@

sc_http$(MODULE_EXT): sc_http.cpp sc_util.cpp
	-@echo [$@] Linking
	@$(CXX) $(CPPFLAGS) -DUNIT_TEST $(OPTS) $(LINK_FLAGS) $^ $(LINK_LIBS) -o $@

$(OBJ_DIR)$(PATHSEP)%.$(OBJ_EXT): %.cpp $(SRC_H)
	-@echo [$(MODULE)] Compiling $(notdir $<)
	@$(CXX) $(CPP_FLAGS) $(OPTS) -c $< -o $@

%.s: %.cpp $(SRC_H)
	-@echo [$(MODULE)] Compiling $(notdir $<)
	@$(CXX) $(CPP_FLAGS) $(OPTS) -S $< -o $@


mostlyclean:
	-@echo [$(MODULE)] Cleaning intermediate files
	@$(REMOVE) $(SRC_OBJ)

clean: mostlyclean
	-@echo [$(MODULE)] Cleaning target files
	@$(REMOVE) $(MODULE) sc_http$(MODULE_EXT)

# Deprecated targets

unix windows mac:
	$(error unix/windows/mac are no longer a valid targets, please read the docs at the top of Makefile)

echo:
	@echo OS=$(OS)
	@echo BOOST_PATH=$(BOOST_PATH)
