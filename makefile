CXX := g++
CXXFLAGS := -std=c++11 -O2  `Magick++-config --cppflags --cxxflags`
LINKFLAGS := -lpthread `Magick++-config --ldflags --libs`
.PHONY: all
all: main.o 
	$(CXX) $(CXXFLAGS) $^ $(LINKFLAGS)
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
.PHONY: clean
clean:
	rm -f *.o a.out
