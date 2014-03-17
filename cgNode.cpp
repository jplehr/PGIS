
#include "cgNode.h"

CgNode::CgNode(std::string function){
	this->functionName = function;
}


void CgNode::addCallsNode(std::shared_ptr<CgNode> functionWhichIsCalledNode){

	for(auto node : calledNodes){
		if(node->isSameFunction(functionWhichIsCalledNode))
			return;
	}

	calledNodes.push_back(functionWhichIsCalledNode);
}


void CgNode::addIsCalledByNode(std::shared_ptr<CgNode> functionByWhichItIsCalledNode){
	isCalledByNodes.push_back(functionByWhichItIsCalledNode);
}


bool CgNode::isSameFunction(std::shared_ptr<CgNode> cgNodeToCompareTo){
	if(this->functionName.compare(cgNodeToCompareTo->getFunctionName()) == 0)
		return true;

	return false;
}


std::string CgNode::getFunctionName(){
	return this->functionName;
}


void CgNode::dumpToDot(std::ofstream& outStream){
	for(auto calledNode : calledNodes){
		outStream << "\"" << this->functionName << "\" -> \"" << calledNode->getFunctionName() << "\";" << std::endl;
	}
}

std::vector<std::shared_ptr<CgNode> > CgNode::getCallers(){
	return isCalledByNodes;
}

std::vector<std::shared_ptr<CgNode> > CgNode::getCallees(){
	return calledNodes;
}

void CgNode::printMinimal(){
	std::cout << this->functionName << "\n";
}

void CgNode::print(){
	std::cout << this->functionName << "\n";
	for(auto n : calledNodes){
		std::cout << "--"<< n->getFunctionName() << "\n";
	}
}
