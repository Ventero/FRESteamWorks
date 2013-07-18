include ../config.sh

INC = -isystem $(AIR_SDK)/include -isystem $(STEAM_SDK)/public/steam
LIB = -L$(STEAM_SDK)/redistributable_bin/linux32
LINK = -m32 -lsteam_api
CXXFLAGS = -std=c++0x -m32 -fvisibility=hidden -Wall -Wextra -pedantic -Wno-unused-parameter -DUSE_BREAKPAD_HANDLER

# comma separated list of whitelisted app ids
ifneq ($(APPIDS),)
WHITELIST = -DWHITELIST=$(APPIDS)
endif

.SUFFIXES:

dir := FRESteamWorks
wdir := APIWrapper
obj := $(dir)/FRESteamWorks.o $(dir)/CSteam.o

all: lib wrapper
lib: $(dir)/FRESteamWorks.so
wrapper: $(wdir)/APIWrapper
obj: SteamSDK

SteamSDK:
	ln -sf $(STEAM_SDK) $@

$(dir)/FRESteamWorks.so: $(obj)
	$(CXX) -shared $(LIB) $^ $(LINK) -o $@

$(wdir)/APIWrapper: $(wdir)/APIWrapper.o $(dir)/CSteam.o $(wdir)/amf-cpp/libamf.a
	$(CXX) $(LIB) $^ $(LINK) -o $@

$(wdir)/APIWrapper.o: $(wdir)/APIWrapper.cpp $(wdir)/APIWrapper.h SteamSDK
	$(CXX) $(CXXFLAGS) -c -DLINUX $(WHITELIST) $(INC) -isystem $(wdir)/amf-cpp -IFRESteamWorks $< -o $@

$(wdir)/amf-cpp/libamf.a: $(wdir)/amf-cpp
	$(MAKE) -C $(wdir)/amf-cpp 32bit

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -DLINUX $(INC) $< -o $@

clean:
	rm -f $(dir)/*.o $(dir)/*.so $(wdir)/*.o $(wdir)/APIWrapper $(wdir)/amf-cpp/libamf.a
	rm -f SteamSDK
	$(MAKE) -C $(wdir)/amf-cpp clean
