INC = -I../../AdobeAIRSDK/include -I../../SteamSDK/public/steam
LIB = -L../../SteamSDK/redistributable_bin/linux32
LINK = -m32 -lsteam_api
CXXFLAGS = -std=c++0x -m32 -fvisibility=hidden -Wall -Wextra -Wno-unused-parameter

# comma separated list of whitelisted app ids
ifneq ($(APPIDS),)
WHITELIST = -DWHITELIST=$(APPIDS)
endif

dir := FRESteamWorks
wdir := APIWrapper
obj := $(dir)/FRESteamWorks.o $(dir)/CSteam.o

all: lib wrapper
lib: $(dir)/FRESteamWorks.so
wrapper: $(wdir)/APIWrapper
.SUFFIXES:

$(dir)/FRESteamWorks.so: $(obj)
	$(CXX) -shared $(LIB) $^ $(LINK) -o $@

$(wdir)/APIWrapper: $(wdir)/APIWrapper.o $(dir)/CSteam.o
	$(CXX) $(LIB) $^ $(LINK) -o $@

$(wdir)/APIWrapper.o: $(wdir)/APIWrapper.cpp $(wdir)/APIWrapper.h
	$(CXX) $(CXXFLAGS) -c -DLINUX $(WHITELIST) $(INC) -IFRESteamWorks $< -o $@

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -DLINUX $(INC) $< -o $@

clean:
	rm -f $(dir)/*.o $(dir)/*.so $(wdir)/*.o $(wdir)/APIWrapper
