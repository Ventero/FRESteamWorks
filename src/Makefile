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
dir := FRESteamWorks
obj := $(dir)/FRESteamWorks.o $(dir)/CSteam.o
# APIWrapper objects
wdir := APIWrapper
wrapperobj := $(wdir)/APIWrapper.o $(wdir)/WrapperIO.o $(dir)/CSteam.o $(wdir)/libamf.a

# Targets
.PHONY: all lib wrapper obj clean dist-clean
all: lib wrapper
lib: $(dir)/FRESteamWorks.so
wrapper: $(wdir)/APIWrapper

# Rules
SteamSDK:
	ln -sf $(STEAM_SDK) $@

$(dir)/FRESteamWorks.so: $(obj)
	$(CXX) -shared $^ $(LIB) -o $@

$(dir)/%.o: $(dir)/%.cpp $(dir)/%.h $(dir)/functions.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(wdir)/APIWrapper: $(wrapperobj)
	$(CXX) $^ $(LIB) -o $@

$(wdir)/libamf.a: $(wdir)/amf-cpp
	$(MAKE) -C $(wdir)/amf-cpp 32bit
	cp $(wdir)/amf-cpp/libamf.a $@

$(wdir)/%.o: $(wdir)/%.cpp $(wdir)/%.h $(dir)/functions.h SteamSDK
	$(CXX) -c $(CXXFLAGS) -isystem $(wdir)/amf-cpp -I$(dir) $< -o $@

clean:
	rm -f $(obj) $(wrapperobj)
	rm -f SteamSDK

dist-clean: clean
	$(MAKE) -C $(wdir)/amf-cpp clean

# Dependencies
$(wdir)/APIWrapper.o: $(wdir)/WrapperIO.h $(dir)/CSteam.h
$(wdir)/APIWrapper.o: CXXFLAGS += -Wno-missing-declarations
