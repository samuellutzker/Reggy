HEADERS= win.h reggy.h
SRC= win.cpp reggy.cpp
COMPFLAGS= -std=c++20 `wx-config --cxxflags`
LINKFLAGS= -std=c++20 -lyajl `wx-config --libs richtext,std`

reggy: $(HEADERS) $(SRC)
	g++ $(SRC) $(COMPFLAGS) $(LINKFLAGS) -o reggy
clean:
	rm reggy