#include "Graph.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Graph::Graph(const std::string& sf) {
    std::string baseDir = "social_network-csv_composite-longdateformatter-sf" + sf;

    loadGraph(baseDir, "dynamic");
}

void Graph::loadGraph(const std::string& baseDir, const std::string& schemaType) {
    std::string headersDir = baseDir + "/headers/" + schemaType;
    std::string dataDir = baseDir + "/" + schemaType;

    for (const auto& entry : fs::directory_iterator(headersDir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            // std::cout << filename << std::endl;
            
            assert(filename.find(".csv") != std::string::npos);
            std::string schemaName = filename.substr(0, filename.find(".csv"));
            
            Schema schema;
            parseSchema(entry.path().string(), schema);

            // judge whether it is a node or an edge
            if (schemaName.find("_") == std::string::npos) {
                // node
                nodeSchemas[schemaName] = schema;
                loadNodes(schemaName, dataDir);
            } else {
                // edge
                edgeSchemas[schemaName] = schema;
                // loadEdges(schemaName, dataDir);
            }
        }
    }
}


void Graph::parseSchema(const std::string& schemaFile, Schema& schema) {
    std::ifstream infile(schemaFile);
    if (!infile.is_open()) {
        std::cerr << "Can not open file: " << schemaFile << std::endl;
        return;
    }

    std::string line;
    if (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, '|')) {
            size_t colon = field.find(':');
            if (colon != std::string::npos) {
                std::string attrName = field.substr(0, colon);
                std::string attrType = field.substr(colon + 1);
                schema.attributes.emplace_back(attrName, attrType);
            }
        }
    }
}


void Graph::loadNodes(const std::string& schemaName, const std::string& dataDir) {
    // 转换为小写开头以匹配文件名，例如 Comment -> comment
    std::string prefixLower = schemaName;
    if (!prefixLower.empty()) {
        prefixLower[0] = std::tolower(prefixLower[0]);
    }

    std::string filename = dataDir + "/" + prefixLower + "_0_0.csv";

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Can not open file: " << filename << std::endl;
        return;
    }
    // std::cerr << "Opened file: " << filename << std::endl;

    std::string line;
    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string field;
        GraphNode node;
        size_t attrIndex = 0;

        while (std::getline(ss, field, '|')) {
            if (attrIndex >= nodeSchemas[schemaName].attributes.size()) {
                assert(false && "Attribute index out of range");
                break;
            }
            const auto& [attrName, attrType] = nodeSchemas[schemaName].attributes[attrIndex];
            if (attrName == "id") {
                node.id = std::stoll(field);
            } else {
                GPStore::Value value;
                if (attrType.find("ID") != std::string::npos) {
                    // assume all IDs are 64-bit
                    value = GPStore::Value(std::stoll(field));
                }
                else if (attrType == "LONG") {
                    value = GPStore::Value(std::stoll(field));
                }
                else if (attrType == "DOUBLE") {
                    value = GPStore::Value(std::stod(field));
                }
                else if (attrType == "STRING" || attrType == "STRING[]") {
                    value = GPStore::Value(field);
                }
                else { // 其他类型
                    std::cerr << "Unsupported attribute type: " << attrType << std::endl;
                    // assert(false && "Unsupported attribute type ");
                    value = GPStore::Value(field);
                }
                node.attributes[attrName] = value;
            }
            attrIndex++;
        }

        nodes[schemaName].emplace_back(std::move(node));
    }

    std::cout << "Loaded " << nodes[schemaName].size() << " nodes of type " << schemaName << std::endl;
}

// void Graph::loadEdges(const std::string& schemaName, const std::string& dataDir) {
//     // 构建数据文件前缀
//     std::string prefix = schemaName;
//     // 转换为小写以匹配文件名，例如 Comment_hasCreator_Person -> comment_hascreator_person
//     std::string prefixLower = prefix;
//     std::transform(prefixLower.begin(), prefixLower.end(), prefixLower.begin(), ::tolower);

//     // 遍历dynamic目录下匹配的文件
//     for (const auto& entry : fs::directory_iterator(dataDir)) {
//         if (entry.is_regular_file()) {
//             std::string filename = entry.path().filename().string();
//             // 检查文件名是否以prefixLower_开头且以.csv结尾
//             if (filename.find(prefixLower + "_") == 0 && filename.substr(filename.find_last_of('.')) == ".csv") {
//                 std::ifstream infile(entry.path());
//                 if (!infile.is_open()) {
//                     std::cerr << "无法打开数据文件: " << entry.path() << std::endl;
//                     continue;
//                 }

//                 std::string line;
//                 // 读取每一行
//                 while (std::getline(infile, line)) {
//                     std::stringstream ss(line);
//                     std::string field;
//                     GraphEdge edge;
//                     size_t attrIndex = 0;

//                     while (std::getline(ss, field, '|')) {
//                         if (attrIndex >= edgeSchemas[schemaName].attributes.size()) break;
//                         const auto& [attrName, attrType] = edgeSchemas[schemaName].attributes[attrIndex];
//                         if (attrName == ":START_ID") {
//                             edge.start_id = std::stoll(field);
//                         }
//                         else if (attrName == ":END_ID") {
//                             edge.end_id = std::stoll(field);
//                         }
//                         else {
//                             GPStore::Value value;
//                             if (attrType.find("ID(") != std::string::npos) {
//                                 // 处理ID类型，假设都是64-bit
//                                 value = GPStore::Value(static_cast<GPStore::Value::Type>(GPStore::Value::NODE), std::stoll(field));
//                             }
//                             else if (attrType == "LONG") {
//                                 value = GPStore::Value(static_cast<GPStore::Value::Type>(GPStore::Value::LONG), std::stoll(field));
//                             }
//                             else if (attrType == "DOUBLE") {
//                                 value = GPStore::Value(std::stod(field));
//                             }
//                             else { // STRING 或其他类型
//                                 value = GPStore::Value(field);
//                             }
//                             edge.attributes[attrName] = value;
//                         }
//                         attrIndex++;
//                     }

//                     edges[schemaName].emplace_back(std::move(edge));
//                 }
//             }
//         }
//     }

//     std::cout << "Loaded " << edges[schemaName].size() << " edges of type " << schemaName << std::endl;
// }

void Graph::printInfo() const {
    std::cout << "Graph Information:" << std::endl;
    std::cout << "Node Types:" << std::endl;
    for (const auto& [type, nodeList] : nodes) {
        std::cout << "  " << type << ": " << nodeList.size() << " nodes" << std::endl;
    }
    std::cout << "Edge Types:" << std::endl;
    for (const auto& [type, edgeList] : edges) {
        std::cout << "  " << type << ": " << edgeList.size() << " edges" << std::endl;
    }
}