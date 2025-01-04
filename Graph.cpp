#include "Graph.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

extern std::vector<std::string> split(const std::string& str, char delimiter);

void Graph::init(const std::string& sf) {
    std::string baseDir = "social_network-csv_composite-longdateformatter-sf" + sf;

    loadGraphNode(baseDir, "dynamic");
    loadGraphNode(baseDir, "static");

    loadGraphEdge(baseDir, "dynamic");
    loadGraphEdge(baseDir, "static");
}

void Graph::loadGraphNode(const std::string& baseDir, const std::string& schemaType) {
    std::string headersDir = baseDir + "/headers/" + schemaType;
    std::string dataDir = baseDir + "/" + schemaType;

    // load node first and then load edge
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
                continue;
            }
        }
    }
}

void Graph::loadGraphEdge(const std::string& baseDir, const std::string& schemaType) {
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
                continue;
            } else {
                // edge
                std::vector<std::string> schemaParts = split(schemaName, '_');
                assert(schemaParts.size() == 3);

                std::string edgeType = schemaParts[1];

                edgeSchemas[edgeType] = schema;
                loadEdges(edgeType, dataDir);
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

            GPStore::Value value;
                if (attrName == "id") {
                assert(attrType.find("ID") != std::string::npos);

                // assume all IDs are 64-bit
                node.id = std::stoll(field);
                value = GPStore::Value(node.id);
            }
            else if (attrType == "LONG") {
                value = GPStore::Value(std::stoll(field));
            }
            else if (attrType == "DOUBLE") {
                value = GPStore::Value(std::stod(field));
            }
            else if (attrType == "STRING" || attrType == "LABEL") {
                value = GPStore::Value(field);
            }
            else if (attrType == "STRING[]") {
                std::vector<std::string> tokens = split(field, ';');
                std::vector<GPStore::Value *> values;
                for (const auto& token : tokens) {
                    values.push_back(new GPStore::Value(token));
                }
                
                value = GPStore::Value(values, true /* deep_copy */);

                // free memory
                for (auto v : values) {
                    delete v;
                }
            }
            else { // 其他类型
                std::cerr << "Unsupported attribute type: " << attrType << " in file: " << prefixLower << std::endl;
                assert(false && "Unsupported attribute type ");
                value = GPStore::Value(field);
            }
            node.attributes[attrName] = value;
                
            attrIndex++;
        }

        nodes[schemaName][node.id] = std::move(node);
    }

#ifdef DEBUG
    std::cout << "Loaded " << nodes[schemaName].size() << " nodes of type " << schemaName << std::endl;
#endif
}

std::string extractContent(const std::string& str) {
    std::size_t start = str.find('(');
    std::size_t end = str.find(')');
    if (start != std::string::npos && end != std::string::npos && start < end) {
        return str.substr(start + 1, end - start - 1);
    }
    return "";  // 如果没有找到或格式不正确，则返回空字符串
}

void Graph::loadEdges(const std::string& edgeType, const std::string& dataDir) {
    const auto& edgeSchema = edgeSchemas[edgeType];
    assert(edgeSchema.attributes.size() >= 2);

    std::string startAttrName = edgeSchema.attributes[0].second;
    std::string endAttrName = edgeSchema.attributes[1].second;
    assert(startAttrName.find("START_ID") != std::string::npos);
    assert(endAttrName.find("END_ID") != std::string::npos);

    std::string startNodeSchema = extractContent(startAttrName);
    std::string endNodeSchema = extractContent(endAttrName);
    assert(!startNodeSchema.empty());
    assert(!endNodeSchema.empty());

    // 转换为首字母小写以匹配文件名，例如 Comment_hasCreator_Person -> comment_hasCreator_person
    std::string prefixLower = startNodeSchema + "_" + edgeType + "_" + endNodeSchema;
    for (size_t i = 0; i < prefixLower.length(); ++i) {
        if (i == 0 || prefixLower[i - 1] == '_') {
            // 如果是每部分的第一个字符，则转换为小写
            prefixLower[i] = std::tolower(prefixLower[i]);
        }
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
        GraphEdge edge;
        size_t attrIndex = 0;

        while (std::getline(ss, field, '|')) {
            if (attrIndex >= edgeSchema.attributes.size()) {
                assert(false && "Attribute index out of range");
                break;
            }
            const auto& [attrName, attrType] = edgeSchema.attributes[attrIndex];
            
            if (attrType.find("START_ID") != std::string::npos) {
                edge.start_id = std::stoll(field);
            }
            else if (attrType.find("END_ID") != std::string::npos) {
                edge.end_id = std::stoll(field);
            }
            else {
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
                else if (attrType == "STRING") {
                    value = GPStore::Value(field);
                }
                else if (attrType == "STRING[]") {
                    std::vector<std::string> tokens = split(field, ';');
                    std::vector<GPStore::Value *> values;
                    for (const auto& token : tokens) {
                        values.push_back(new GPStore::Value(token));
                    }
                    
                    value = GPStore::Value(values, true /* deep_copy */);

                    // free memory
                    for (auto v : values) {
                        delete v;
                    }
                }
                else { // 其他类型
                    std::cerr << "Unsupported attribute type: " << attrType << " in file: " << prefixLower << std::endl;
                    assert(false && "Unsupported attribute type ");
                    value = GPStore::Value(field);
                }
                edge.attributes[attrName] = value;
            }
            attrIndex++;
        }

        edges[edgeType].emplace_back(std::move(edge));

        // add neighbors
        GraphNode* startNode = findNode(startNodeSchema, edge.start_id);
        if (startNode != nullptr) {
            startNode->neighborsOut[edgeType].push_back(edge.end_id);
        } else {
            // std::cerr << "Can not find start node: " << edge.start_id << " with schema " << startNodeSchema << " for edge: " << edgeType << std::endl;
            
            // create a new node
            GraphNode node;
            node.id = edge.start_id;
            node.neighborsOut[edgeType].push_back(edge.end_id);
            nodes[startNodeSchema][node.id] = std::move(node);
        }

        GraphNode* endNode = findNode(endNodeSchema, edge.end_id);
        if (endNode != nullptr) {
            endNode->neighborsIn[edgeType].push_back(edge.start_id);
        } else {
            // std::cerr << "Can not find end node: " << edge.end_id << " with schema " << endNodeSchema << " for edge: " << edgeType << std::endl;
            
            // create a new node
            GraphNode node;
            node.id = edge.end_id;
            node.neighborsIn[edgeType].push_back(edge.start_id);
            nodes[endNodeSchema][node.id] = std::move(node);
        }
    }

#ifdef DEBUG
    std::cout << "Loaded " << edges[edgeType].size() << " edges of type " << edgeType << std::endl;
#endif
}

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


Graph::GraphNode* Graph::findNode(int64_t nodeId) {
    for (auto& [type, nodeList] : nodes) {
        auto nodeIt = nodeList.find(nodeId);
        if (nodeIt != nodeList.end()) {
            return &(nodeIt->second);
        }
    }

    return nullptr;
}

Graph::GraphNode* Graph::findNode(const std::string& nodeType, int64_t nodeId) {
    auto it = nodes.find(nodeType);
    if (it == nodes.end()) {
        std::cerr << "Can not find node type: " << nodeType << std::endl;
        return nullptr;
    }

    auto nodeIt = it->second.find(nodeId);
    if (nodeIt != it->second.end()) {
        return &(nodeIt->second);
    }

    // std::cerr << "Can not find node: " << nodeId << " with schema " << nodeType << std::endl;
    return nullptr;
}