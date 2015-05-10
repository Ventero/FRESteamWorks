include ../config.sh

CXXFLAGS = -std=c++0x -m32 -fvisibility=hidden -I.
CXXFLAGS += -Wall -Wextra -pedantic -Wno-unused-parameter -Wmissing-declarations
CXXFLAGS += -Wconversion -Wcast-qual -Wno-sign-conversion
CXXFLAGS += -DUSE_BREAKPAD_HANDLER -DVERSION_SAFE_STEAM_API_INTERFACES
CXXFLAGS += -isystem $(AIR_SDK)/include -isystem $(STEAM_SDK)/public
LIB = -m32 -lsteam_api

ifeq ($(shell uname), Darwin)
	CXXFLAGS += -DOSX
	LIB += -L$(STEAM_SDK)/redistributable_bin/osx32
else
	CXXFLAGS += -DLINUX
	LIB += -L$(STEAM_SDK)/redistributable_bin/linux32
endif

# comma separated list of whitelisted app ids
ifneq ($(APPIDS),)
CXXFLAGS += -DWHITELIST=$(APPIDS)
endif

ifdef DEBUG
CXXFLAGS += -g -DDEBUG
endif

.SUFFIXES:

# Native extension objects
obj := FRESteamWorks/FRESteamWorks.o FRESteamWorks/FREConverters.o CSteam/CSteam.o
# APIWrapper objects
wrapperobj := APIWrapper/APIWrapper.o APIWrapper/APIFunctions.o APIWrapper/CLISteam.o \
              CSteam/CSteam.o
# All source files
src := $(obj:.o=.cpp) $(wrapperobj:.o=.cpp)
# Destination for libamf
amf := APIWrapper/libamf.a

# Targets
.PHONY: all lib wrapper clean dist-clean
all: lib wrapper
lib: FRESteamWorks/FRESteamWorks.so
wrapper: APIWrapper/APIWrapper

clean:
	rm -f $(obj) $(wrapperobj) $(amf)
	rm -f SteamSDK

dist-clean: clean
	$(MAKE) -C APIWrapper/amf-cpp clean

# Rules
# Output objects
FRESteamWorks/FRESteamWorks.so: $(obj)
	$(CXX) -shared $^ $(LIB) -o $@

APIWrapper/APIWrapper: $(wrapperobj) $(amf)
	$(CXX) $^ $(LIB) -o $@

$(amf): APIWrapper/amf-cpp
	$(MAKE) -C APIWrapper/amf-cpp 32bit
	cp APIWrapper/amf-cpp/libamf.a $@

# Generic rules
SteamSDK:
	ln -sf $(STEAM_SDK) $@

%.o: | SteamSDK
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Additional flags
APIWrapper/%.o: CXXFLAGS += -isystem APIWrapper/amf-cpp/src

.dep: $(src)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -isystem APIWrapper/amf-cpp/src -MM $(src) | \
		sed '/^[^[:space:]]/s,^[^:]*: \([^[:space:]]*\)/,\1/&,;s,:, $@:,' > $@

ifeq ($(filter $(MAKECMDGOALS),clean dist-clean),)
-include .dep
endif
