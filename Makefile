
CXXFLAGS=-std=c++11

INCLUDEFLAGS=-I. -I/home/ci24amun/myRoot/gcc/openmpi/cube/4.2.1/include/cube/

LDFLAGS+=-L/home/ci24amun/myRoot/gcc/openmpi/cube/4.2.1/lib -lcube4 -lz

CubeCallGraphTool:
	$(CXX) $(CXXFLAGS) $(INCLUDEFLAGS) -o CubeCallgraphTool.exe main.cpp cgNode.cpp callgraph.cpp $(LDFLAGS)

clean:
	rm -rf *.o
