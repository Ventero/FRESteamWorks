include ../config.sh

CXXFLAGS = -std=c++0x -m32 -fvisibility=hidden
CXXFLAGS += -Wall -Wextra -pedantic -Wno-unused-parameter -Wmissing-declarations
CXXFLAGS += -Wconversion -Wcast-qual -Wno-sign-conversion
CXXFLAGS += -DUSE_BREAKPAD_HANDLER -DLINUX
CXXFLAGS += -isystem $(AIR_SDK)/include -isystem $(STEAM_SDK)/public/steam
LIB = -m32 -L$(STEAM_SDK)/redistributable_bin/linux32 -lsteam_api

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
wrapperobj := APIWrapper/APIWrapper.o APIWrapper/WrapperIO.o CSteam/CSteam.o APIWrapper/libamf.a

# Targets
.PHONY: all lib wrapper clean dist-clean
all: lib wrapper
lib: FRESteamWorks/FRESteamWorks.so
wrapper: APIWrapper/APIWrapper

clean:
	rm -f $(obj) $(wrapperobj)
	rm -f APIWrapper/libamf.a
	rm -f SteamSDK

dist-clean: clean
	$(MAKE) -C APIWrapper/amf-cpp clean

# Rules
# Output objects
FRESteamWorks/FRESteamWorks.so: $(obj)
	$(CXX) -shared $^ $(LIB) -o $@

APIWrapper/APIWrapper: $(wrapperobj)
	$(CXX) $^ $(LIB) -o $@

APIWrapper/libamf.a: APIWrapper/amf-cpp
	$(MAKE) -C APIWrapper/amf-cpp 32bit
	cp APIWrapper/amf-cpp/libamf.a $@

# Generic rules
SteamSDK:
	ln -sf $(STEAM_SDK) $@

%.o: %.cpp %.h functions.h SteamSDK
	$(CXX) -c $(CXXFLAGS) -I. $< -o $@

# Additional dependencies/flags
CSteam/CSteam.o: CSteam/ResponseTypes.h
FRESteamWorks/FRESteamWorks.o: FRESteamWorks/FREConverters.h CSteam/CSteam.h
APIWrapper/APIWrapper.o: APIWrapper/WrapperIO.h CSteam/CSteam.h
APIWrapper/APIWrapper.o: CXXFLAGS += -Wno-missing-declarations
APIWrapper/%.o: CXXFLAGS += -isystem APIWrapper/amf-cpp
