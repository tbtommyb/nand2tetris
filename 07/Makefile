CXX=clang++
CXXFLAGS=-Wall -std=c++1z
LIBS = -lboost_system -lboost_filesystem

vm: *.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS)

clean:
	rm -f vm
