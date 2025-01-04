#pragma once

#include <string>
#include <unordered_map>

#include "Value.h"

class Graph {
    public:

        static Graph& getInstance() {
            static std::mutex mtx;
            std::lock_guard<std::mutex> lock(mtx);
            static Graph instance;
            return instance;
        }

        // ban copy and assignment
        Graph(const Graph&) = delete;
        Graph& operator=(const Graph&) = delete;

        struct GraphNode {
            int64_t id;
            std::unordered_map<std::string, GPStore::Value> attributes;
            std::unordered_map<std::string, std::vector<GPStore::Value>> neighbors; // edgeType -> endNodeIds
        };

        struct GraphEdge {
            int64_t start_id;
            int64_t end_id;
            std::unordered_map<std::string, GPStore::Value> attributes;
        };

        void init(const std::string& sf);

        GraphNode* findNode(int64_t nodeId);

        GraphNode* findNode(const std::string& nodeType, int64_t nodeId);

        void printInfo() const;

    private:
        struct Schema {
            std::vector<std::pair<std::string, std::string>> attributes; // name, type
        };

        std::unordered_map<std::string, Schema> nodeSchemas;

        std::unordered_map<std::string, Schema> edgeSchemas;

        // 存储节点，按类型分类
        std::unordered_map<std::string, std::unordered_map<int64_t, GraphNode>> nodes;

        // 存储边，按类型分类
        std::unordered_map<std::string, std::vector<GraphEdge>> edges;

        Graph() {};

        void loadGraph(const std::string& baseDir, const std::string& schemaType);

        void parseSchema(const std::string& schemaFile, Schema& schema);

        void loadNodes(const std::string& schemaName, const std::string& dataDir);
        
        void loadEdges(const std::string& schemaName, const std::string& dataDir);
};