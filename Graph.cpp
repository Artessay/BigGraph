#include "Graph.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Graph::Graph(const std::string& sf) {
    std::string baseDir = "social_network-csv_composite-longdateformatter-sf" + sf;

    loadGraph(baseDir, "dynamic");
}

void Graph::loadGraph(const std::string& baseDir, const std::string& schemaType) {
    std::string headersDir = baseDir + "/headers/" + schemaType;
    std::string bodyDir = baseDir + "/" + schemaType;

    for (const auto& entry : fs::directory_iterator(headersDir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::cout << filename << std::endl;
            // std::ifstream infile(entry.path());
            // if (!infile.is_open()) {
            //     std::cerr << "无法打开文件: " << entry.path() << std::endl;
            //     continue;
            // }

            // std::string schemaName = filename.substr(0, filename.find(".csv"));
            // Schema schema;
            // parseSchema(entry.path().string(), schema);

            // // 判断是节点还是边
            // if (schemaName.find("_has") == std::string::npos) {
            //     // 节点
            //     nodeSchemas[schemaName] = schema;
            //     loadNodes(schemaName, dynamicDir);
            // } else {
            //     // 边
            //     edgeSchemas[schemaName] = schema;
            //     loadEdges(schemaName, dynamicDir);
            // }
        }
    }
}