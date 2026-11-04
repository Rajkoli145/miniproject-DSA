#include "graph.h"

#include <climits>
#include <cctype>
#include <istream>
#include <stdexcept>
#include <unordered_map>

namespace depgraph {

ModuleGraph buildGraph(
    const std::vector<std::string>& names,
    const std::vector<std::pair<std::string, std::string>>& dependencies) {
    if (names.size() > INT_MAX) {
        throw std::invalid_argument("Too many modules for integer IDs.");
    }
    ModuleGraph result{names, Graph(names.size())};
    std::unordered_map<std::string, int> ids;
    for (std::size_t i = 0; i < names.size(); ++i) {
        const auto& name = names[i];
        if (name.empty()) throw std::invalid_argument("Module names must not be empty.");
        for (unsigned char ch : name) {
            if (std::isspace(ch)) {
                throw std::invalid_argument("Module names must not contain whitespace.");
            }
        }
        if (!ids.emplace(name, static_cast<int>(i)).second) {
            throw std::invalid_argument("Duplicate module name: " + name);
        }
    }
    for (const auto& edge : dependencies) {
        auto source = ids.find(edge.first);
        auto target = ids.find(edge.second);
        if (source == ids.end()) throw std::invalid_argument("Unknown module: " + edge.first);
        if (target == ids.end()) throw std::invalid_argument("Unknown module: " + edge.second);
        result.graph[source->second].push_back(target->second);
    }
    return result;
}

namespace {
int readCount(std::istream& input, const std::string& label) {
    std::string token;
    if (!(input >> token) || token.empty()) {
        throw std::invalid_argument("Missing " + label + ".");
    }
    int value = 0;
    for (char ch : token) {
        if (ch < '0' || ch > '9' || value > (INT_MAX - (ch - '0')) / 10) {
            throw std::invalid_argument("Invalid " + label + ": expected a nonnegative integer.");
        }
        value = value * 10 + (ch - '0');
    }
    return value;
}
}  // namespace

ModuleGraph readGraph(std::istream& input) {
    const int vertices = readCount(input, "module count");
    std::vector<std::string> names;
    for (int i = 0; i < vertices; ++i) {
        std::string name;
        if (!(input >> name)) throw std::invalid_argument("Missing module name.");
        names.push_back(name);
    }
    const int edges = readCount(input, "dependency count");
    std::vector<std::pair<std::string, std::string>> dependencies;
    for (int i = 0; i < edges; ++i) {
        std::string source, target;
        if (!(input >> source >> target)) {
            throw std::invalid_argument("Incomplete dependency pair.");
        }
        dependencies.emplace_back(source, target);
    }
    std::string extra;
    if (input >> extra) throw std::invalid_argument("Unexpected trailing input: " + extra);
    if (input.bad()) throw std::invalid_argument("Failed to read input.");
    return buildGraph(names, dependencies);
}

}  // namespace depgraph
