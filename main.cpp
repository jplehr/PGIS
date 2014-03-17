#include <fstream>
#include "callgraph.h"
#include "Cube.h"
#include <cstdlib>

#include <cassert>

int main(int argc, char** argv){


Callgraph cg;
// The cube documentation says one should always use a try/catch thing around
try{
	// Create cube instance
	cube::Cube cube;
	// Read our cube file
	cube.openCubeReport( argv[1] );
	std::cout << "Reading input.. done." << std::endl;


	const std::vector<cube::Cnode*>& cnodes = cube.get_cnodev();

	std::cout << cnodes[0]->get_callee()->get_name() << std::endl;
//	std::cout << cnodes[0]->get_callee()->get_mod() << std::endl;

	for(auto node : cnodes){
		if(node->get_parent() == NULL)
			continue;
		cg.putFunction(node->get_parent()->get_callee()->get_name(), node->get_callee()->get_name());
//		cg.putFunction(node->get_callee()->get_name(), node->get_parent()->get_callee()->get_name());
	}


	cg.print();
	cg.printDOT();

	for(auto node : cg.getNodesToMark())
		node->printMinimal();

	

} catch(cube::RuntimeError e){
	std::cout << e.get_msg() << std::endl;
}

return EXIT_SUCCESS;
}
