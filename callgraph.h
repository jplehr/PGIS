#include <map>
#include <string>
#include <queue>

#include "cgNode.h"

class Callgraph {

public:
	Callgraph();
	int putFunction(std::string fullQualifiedNameCaller, std::string fullQualifiedNameCallee);

	void print();
	void printDOT();

	std::shared_ptr<CgNode> findMain();

	std::vector<std::shared_ptr<CgNode> > getNodesToMark();

private:
	std::map<std::string, std::shared_ptr<CgNode> > graph;
};
