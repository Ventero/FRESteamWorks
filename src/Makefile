include ../config.sh

CXXFLAGS = -std=c++0x -m32 -fvisibility=hidden
CXXFLAGS += -Wall -Wextra -pedantic -Wno-unused-parameter -Wmissing-declarations
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

dir := FRESteamWorks
wdir := APIWrapper
obj := $(dir)/FRESteamWorks.o $(dir)/CSteam.o

.PHONY: all lib wrapper obj clean dist-clean
all: lib wrapper
lib: $(dir)/FRESteamWorks.so
wrapper: $(wdir)/APIWrapper
obj: SteamSDK

SteamSDK:
	ln -sf $(STEAM_SDK) $@

$(dir)/FRESteamWorks.o: $(dir)/functions.h
$(dir)/FRESteamWorks.so: $(obj)
	$(CXX) -shared $^ $(LIB) -o $@

$(wdir)/APIWrapper: $(wdir)/APIWrapper.o $(dir)/CSteam.o $(wdir)/libamf.a
	$(CXX) $^ $(LIB) -o $@

$(wdir)/APIWrapper.o: $(wdir)/APIWrapper.cpp $(wdir)/APIWrapper.h $(dir)/functions.h SteamSDK
	$(CXX) -c $(CXXFLAGS) -isystem $(wdir)/amf-cpp -I$(dir) $< -o $@

$(wdir)/libamf.a: $(wdir)/amf-cpp
	$(MAKE) -C $(wdir)/amf-cpp 32bit
	cp $(wdir)/amf-cpp/libamf.a $@

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(obj) $(wdir)/FRESteamWorks.o $(wdir)/APIWrapper $(wdir)/libamf.a
	rm -f SteamSDK

dist-clean: clean
	$(MAKE) -C $(wdir)/amf-cpp clean
