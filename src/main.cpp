#include "graph.h"
#include "output.h"
#include "tarjan.h"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: depgraph < input.txt\n"
                     "Input: module count, module names, dependency count, then pairs A B.\n"
                     "A B means A depends on B. Names cannot contain whitespace.\n";
        return 0;
    }
    if (argc != 1) {
        std::cerr << "Usage: depgraph < input.txt (or depgraph --help)\n";
        return 1;
    }
    try {
        const auto modules = depgraph::readGraph(std::cin);
        const auto result = depgraph::findSCCs(modules.graph);
        depgraph::printAnalysis(std::cout, modules, result);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
