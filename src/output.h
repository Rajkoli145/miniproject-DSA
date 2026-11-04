#pragma once

#include "tarjan.h"
#include <iosfwd>

namespace depgraph {
// Expects the unmodified result of findSCCs(modules.graph).
void printAnalysis(std::ostream& output, const ModuleGraph& modules, const SCCResult& result);
}
