#include "callgraph.h"

Callgraph::Callgraph(){
}

int Callgraph::putFunction(std::string fullQualifiedNameCaller, std::string fullQualifiedNameCallee){

	std::shared_ptr<CgNode> caller;
	std::shared_ptr<CgNode> callee;

	int returnCode = 0;

	if(graph.find(fullQualifiedNameCaller) != graph.end()){
		// A node representing the caller already exists
		caller = graph.find(fullQualifiedNameCaller)->second;
	} else {
		// Create a new node representing the caller
		caller = std::make_shared<CgNode>(fullQualifiedNameCaller);
		graph.insert(std::pair<std::string, std::shared_ptr<CgNode> >(fullQualifiedNameCaller, caller));
		returnCode++;
	}

	if(graph.find(fullQualifiedNameCallee) != graph.end()){
		// A node representing the callee already exists
		callee = graph.find(fullQualifiedNameCallee)->second;
	} else {
		// Create a new node representing the callee
		callee = std::make_shared<CgNode>(fullQualifiedNameCallee);
		graph.insert(std::pair<std::string, std::shared_ptr<CgNode> >(fullQualifiedNameCallee, callee));
		returnCode++;
	}

	std::cout << "Caller: " << fullQualifiedNameCaller << "\nCalee: " << fullQualifiedNameCallee << std::endl;

	caller->addCallsNode(callee);
	callee->addIsCalledByNode(caller);
	return returnCode;
}

std::vector<std::shared_ptr<CgNode> > Callgraph::getNodesToMark(){

	std::vector<std::shared_ptr<CgNode> > nodesToMark;
	std::queue<std::shared_ptr<CgNode> > workQueue;

	workQueue.push(findMain());
	while(! workQueue.empty()){
		auto node = workQueue.front();
		workQueue.pop();
		if(node->getCallers().size() == 0 || node->getCallers().size() > 1){
			bool insert = true;
			for(auto refNode : nodesToMark){
				if(refNode == node)
					insert = false;
			}
			if(insert)
				nodesToMark.push_back(node);
		}
		for(auto n : node->getCallees())
			workQueue.push(n);
	}
	
	return nodesToMark;

}

void Callgraph::print(){

	auto mainNode = findMain();
	mainNode->print();

	std::cout << std::endl;
}

std::shared_ptr<CgNode> Callgraph::findMain(){

	for(auto node : graph){
		auto fName = node.second->getFunctionName();
		if(fName.find("main") != std::string::npos)
			return node.second;
	}

	return NULL;
}

void Callgraph::printDOT(){

	std::ofstream outfile("callgraph.dot", std::ofstream::out);

	outfile << "digraph callgraph {\nnode [shape=oval]\n";

	for( auto mapPair : graph){
		mapPair.second->dumpToDot(outfile);
	}

	outfile << "\n}" << std::endl;
	outfile.close();
	std::cout << "DOT file dumped." << std::endl;

}
