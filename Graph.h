#pragma once

#include <string>

class Graph {
    public:
        Graph(const std::string& sf);

        void loadGraph(const std::string& baseDir, const std::string& schemaType);
};