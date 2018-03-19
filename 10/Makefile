CXX=clang++
CXXFLAGS=-Wall -std=c++1z
LIBS = -lboost_system -lboost_filesystem

JackAnalyzer: *.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LIBS)

clean:
	rm -f JackAnalyzer
