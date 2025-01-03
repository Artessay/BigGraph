#pragma once

#include <string>
#include <unordered_map>

#include "Value.h"

class Graph {
    public:
        Graph(const std::string& sf);

        void loadGraph(const std::string& baseDir, const std::string& schemaType);

        void printInfo() const;

    private:
        struct Schema {
            std::vector<std::pair<std::string, std::string>> attributes; // name, type
        };

        std::unordered_map<std::string, Schema> nodeSchemas;

        std::unordered_map<std::string, Schema> edgeSchemas;

        struct GraphNode {
            int64_t id;
            std::unordered_map<std::string, GPStore::Value> attributes;
        };

        struct GraphEdge {
            int64_t start_id;
            int64_t end_id;
            std::unordered_map<std::string, GPStore::Value> attributes;
        };

        // 存储节点，按类型分类
        std::unordered_map<std::string, std::vector<GraphNode>> nodes;

        // 存储边，按类型分类
        std::unordered_map<std::string, std::vector<GraphEdge>> edges;


        void parseSchema(const std::string& schemaFile, Schema& schema);

        void loadNodes(const std::string& schemaName, const std::string& dataDir);
        
        // void loadEdges(const std::string& schemaName, const std::string& dataDir);
};