#include "vars.mk"
INC = -I../../AdobeAIRSDK/include -I../../SteamSDK/public/steam
LIB = -L../..//SteamSDK/redistributable_bin/linux32
LINK = -lsteam_api
CXXFLAGS = -m32 -Wall -Wextra -Wno-unused-parameter

dir := FRESteamWorks

all: lib
lib: $(dir)/FRESteamWorks.so
.SUFFIXES:

$(dir)/FRESteamWorks.so: $(dir)/FRESteamWorks.o
	$(CXX) -shared -m32 $(LIB) $(LINK) $< -o $@

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -DLINUX $(INC) $< -o $@

clean:
	rm -f $(dir)/*.o $(dir)/*.so
