INC = -I../../AdobeAIRSDK/include -I../../SteamSDK/public/steam
LIB = -L../../SteamSDK/redistributable_bin/linux32
LINK = -lsteam_api
CXXFLAGS = -m32 -fvisibility=hidden -Wall -Wextra -Wno-unused-parameter

dir := FRESteamWorks
obj := $(dir)/FRESteamWorks.o $(dir)/CSteam.o

all: lib
lib: $(dir)/FRESteamWorks.so
.SUFFIXES:

$(dir)/FRESteamWorks.so: $(obj)
	$(CXX) -shared -m32 $(LIB) $(LINK) $< -o $@

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -DLINUX $(INC) $< -o $@

clean:
	rm -f $(dir)/*.o $(dir)/*.so
