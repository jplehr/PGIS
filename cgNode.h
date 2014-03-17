
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

class CgNode {

public:
	CgNode(std::string function);
	void addCallsNode(std::shared_ptr<CgNode> functionWhichIsCalledNode);
	void addIsCalledByNode(std::shared_ptr<CgNode> functionByWhichItIsCalledNode);

	bool isSameFunction(std::shared_ptr<CgNode> cgNodeToCompareTo);

	std::string getFunctionName();

	std::vector<std::shared_ptr<CgNode> > getCallers();
	std::vector<std::shared_ptr<CgNode> > getCallees();

	void dumpToDot(std::ofstream& outputStream);

	void print();
	void printMinimal();

private:
	std::vector<std::shared_ptr<CgNode> > calledNodes;
	std::vector<std::shared_ptr<CgNode> > isCalledByNodes;
	std::string functionName;
};
