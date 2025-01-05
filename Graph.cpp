#include "Graph.h" // 包含自定义 Graph 类的头文件
#include <mutex>
#include <cassert>    // 提供 assert 宏用于调试时的条件断言
#include <fstream>    // 提供文件输入/输出功能
#include <sstream>    // 提供字符串流操作
#include <iostream>   // 提供输入输出流功能
#include <filesystem> // 提供文件系统操作功能


namespace fs = std::filesystem; // 将 std::filesystem 命名空间重命名为 fs，便于简化代码

extern std::vector<std::string> split(const std::string &str, char delimiter);
// 声明外部函数 split，用于根据分隔符将字符串拆分为多个子字符串

void Graph::init(const std::string &sf)
{
    // 初始化图结构，加载节点和边数据
    std::string baseDir = "social_network-csv_composite-longdateformatter-sf" + sf;
    // 根据输入参数 sf 构造基础目录路径

    loadGraphNode(baseDir, "dynamic"); // 加载动态类型的节点
    loadGraphNode(baseDir, "static");  // 加载静态类型的节点

    loadGraphEdge(baseDir, "dynamic"); // 加载动态类型的边
    loadGraphEdge(baseDir, "static");  // 加载静态类型的边
}

void Graph::loadGraphNode(const std::string &baseDir, const std::string &schemaType)
{
    // 加载节点的模式和数据
    std::string headersDir = baseDir + "/headers/" + schemaType; // 模式文件所在目录
    std::string dataDir = baseDir + "/" + schemaType;            // 数据文件所在目录

    for (const auto &entry : fs::directory_iterator(headersDir))
    {
        // 遍历 headersDir 目录中的每个文件
        if (entry.is_regular_file())
        {
            // 检查文件是否为常规文件
            std::string filename = entry.path().filename().string(); // 获取文件名

            assert(filename.find(".csv") != std::string::npos); // 确保文件是 .csv 格式
            std::string schemaName = filename.substr(0, filename.find(".csv"));
            // 提取 schema 名称（去掉 .csv 后缀）

            Schema schema;
            parseSchema(entry.path().string(), schema);
            // 解析 schema 文件，将结果存入 schema 对象

            if (schemaName.find("_") == std::string::npos)
            {
                // 如果 schema 名称不包含下划线，则认为是节点模式
                nodeSchemas[schemaName] = schema; // 保存模式
                loadNodes(schemaName, dataDir);   // 加载节点数据
            }
            else
            {
                // 否则跳过（处理边时会处理）
                continue;
            }
        }
    }
}

void Graph::loadGraphEdge(const std::string &baseDir, const std::string &schemaType)
{
    // 加载边的模式和数据
    std::string headersDir = baseDir + "/headers/" + schemaType; // 模式文件所在目录
    std::string dataDir = baseDir + "/" + schemaType;            // 数据文件所在目录

    for (const auto &entry : fs::directory_iterator(headersDir))
    {
        // 遍历 headersDir 目录中的每个文件
        if (entry.is_regular_file())
        {
            // 检查文件是否为常规文件
            std::string filename = entry.path().filename().string(); // 获取文件名

            assert(filename.find(".csv") != std::string::npos); // 确保文件是 .csv 格式
            std::string schemaName = filename.substr(0, filename.find(".csv"));
            // 提取 schema 名称（去掉 .csv 后缀）

            Schema schema;
            parseSchema(entry.path().string(), schema);
            // 解析 schema 文件，将结果存入 schema 对象

            if (schemaName.find("_") == std::string::npos)
            {
                // 如果 schema 名称不包含下划线，则认为是节点模式，跳过
                continue;
            }
            else
            {
                // 否则认为是边模式
                std::vector<std::string> schemaParts = split(schemaName, '_');
                // 根据下划线拆分 schema 名称，应该得到三个部分
                assert(schemaParts.size() == 3);

                std::string edgeType = schemaParts[1]; // 第二部分是边的类型

                edgeSchemas[edgeType] = schema; // 保存边的模式
                loadEdges(edgeType, dataDir);   // 加载边数据
            }
        }
    }
}

void Graph::parseSchema(const std::string &schemaFile, Schema &schema)
{
    // 解析 schema 文件内容
    std::ifstream infile(schemaFile);
    if (!infile.is_open())
    {
        // 如果文件无法打开，输出错误信息并返回
        std::cerr << "Can not open file: " << schemaFile << std::endl;
        return;
    }

    std::string line;
    if (std::getline(infile, line))
    {
        // 读取文件的第一行（模式定义）
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, '|'))
        {
            // 根据竖线分隔每个字段
            size_t colon = field.find(':');
            if (colon != std::string::npos)
            {
                // 查找冒号，将其分为属性名称和属性类型
                std::string attrName = field.substr(0, colon);
                std::string attrType = field.substr(colon + 1);
                schema.attributes.emplace_back(attrName, attrType);
                // 将属性名称和类型存入 schema 对象的 attributes 列表中
            }
        }
    }
}

void Graph::loadNodes(const std::string &schemaName, const std::string &dataDir)
{
    // 加载节点数据
    std::string prefixLower = schemaName;
    if (!prefixLower.empty())
    {
        prefixLower[0] = std::tolower(prefixLower[0]);
        // 将模式名称的首字母转为小写
    }

    std::string filename = dataDir + "/" + prefixLower + "_0_0.csv";
    // 构造数据文件名，假设命名规则为 [模式名称]_0_0.csv

    std::ifstream infile(filename);
    if (!infile.is_open())
    {
        // 如果文件无法打开，输出错误信息并返回
        std::cerr << "Can not open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        // 逐行读取数据文件内容
        std::stringstream ss(line);
        std::string field;
        GraphNode node;       // 定义 GraphNode 对象表示节点
        size_t attrIndex = 0; // 属性索引

        while (std::getline(ss, field, '|'))
        {
            // 根据竖线分隔每个属性值
            if (attrIndex >= nodeSchemas[schemaName].attributes.size())
            {
                assert(false && "Attribute index out of range");
                break;
            }

            const auto &[attrName, attrType] = nodeSchemas[schemaName].attributes[attrIndex];
            // 获取模式中对应的属性名称和类型

            GPStore::Value value; // 用于存储属性值
            if (attrName == "id")
            {
                // 如果属性名称是 id
                assert(attrType.find("ID") != std::string::npos);
                node.id = std::stoll(field); // 转换为 64 位整数作为节点 ID
                value = GPStore::Value(static_cast<GPStore::int_64>(node.id));
            }
            else if (attrType == "LONG")
            {
                // 如果是长整型
                value = GPStore::Value(std::stoll(field));
            }
            else if (attrType == "DOUBLE")
            {
                // 如果是双精度浮点型
                value = GPStore::Value(std::stod(field));
            }
            else if (attrType == "STRING" || attrType == "LABEL")
            {
                // 如果是字符串类型
                value = GPStore::Value(field);
            }
            else if (attrType == "STRING[]")
            {
                // 如果是字符串数组
                std::vector<std::string> tokens = split(field, ';');
                std::vector<GPStore::Value *> values;
                for (const auto &token : tokens)
                {
                    values.push_back(new GPStore::Value(token));
                }

                value = GPStore::Value(values, true /* deep_copy */);

                // 释放动态分配的内存
                for (auto v : values)
                {
                    delete v;
                }
            }
            else
            {
                // 如果是其他类型，报错
                std::cerr << "Unsupported attribute type: " << attrType << " in file: " << prefixLower << std::endl;
                assert(false && "Unsupported attribute type ");
                value = GPStore::Value(field);
            }

            node.attributes[attrName] = value; // 存储属性值到节点的 attributes 中
            attrIndex++;                       // 更新属性索引
        }

        nodes[schemaName][node.id] = std::move(node); // 将节点存入 nodes 容器
    }

#ifdef DEBUG
    // 如果定义了 DEBUG 宏，输出加载信息
    std::cout << "Loaded " << nodes[schemaName].size() << " nodes of type " << schemaName << std::endl;
#endif
}

std::string extractContent(const std::string &str)
{
    // 从格式化字符串中提取括号内的内容
    std::size_t start = str.find('('); // 找到第一个左括号的位置
    std::size_t end = str.find(')');   // 找到第一个右括号的位置
    if (start != std::string::npos && end != std::string::npos && start < end)
    {
        // 如果左右括号都存在，并且左括号在右括号之前
        return str.substr(start + 1, end - start - 1); // 提取括号内的内容
    }
    return ""; // 如果格式不正确，返回空字符串
}

void Graph::loadEdges(const std::string &edgeType, const std::string &dataDir)
{
    // 加载边数据
    const auto &edgeSchema = edgeSchemas[edgeType];
    // 获取当前边类型的模式
    assert(edgeSchema.attributes.size() >= 2);
    // 边模式中至少需要两个属性：起始节点和结束节点的 ID

    std::string startAttrName = edgeSchema.attributes[0].second;
    std::string endAttrName = edgeSchema.attributes[1].second;
    // 获取第一个和第二个属性的名称，它们应表示起始和结束节点 ID
    assert(startAttrName.find("START_ID") != std::string::npos);
    assert(endAttrName.find("END_ID") != std::string::npos);
    // 确保这些属性名包含 "START_ID" 和 "END_ID"

    std::string startNodeSchema = extractContent(startAttrName);
    std::string endNodeSchema = extractContent(endAttrName);
    // 提取括号内的节点模式名称
    assert(!startNodeSchema.empty());
    assert(!endNodeSchema.empty());
    // 确保提取出的模式名称非空

    // 转换为文件名格式：将模式名称拼接为小写形式的前缀
    std::string prefixLower = startNodeSchema + "_" + edgeType + "_" + endNodeSchema;
    for (size_t i = 0; i < prefixLower.length(); ++i)
    {
        if (i == 0 || prefixLower[i - 1] == '_')
        {
            // 每个部分的第一个字符转换为小写
            prefixLower[i] = std::tolower(prefixLower[i]);
        }
    }

    std::string filename = dataDir + "/" + prefixLower + "_0_0.csv";
    // 构造数据文件名，假设命名规则为 [起始节点模式]_[边类型]_[结束节点模式]_0_0.csv

    std::ifstream infile(filename);
    if (!infile.is_open())
    {
        // 如果文件无法打开，输出错误信息并返回
        std::cerr << "Can not open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        // 逐行读取边数据文件内容
        std::stringstream ss(line);
        std::string field;
        GraphEdge edge;       // 定义 GraphEdge 对象表示边
        size_t attrIndex = 0; // 属性索引

        while (std::getline(ss, field, '|'))
        {
            // 根据竖线分隔每个属性值
            if (attrIndex >= edgeSchema.attributes.size())
            {
                assert(false && "Attribute index out of range");
                break;
            }

            const auto &[attrName, attrType] = edgeSchema.attributes[attrIndex];
            // 获取模式中对应的属性名称和类型

            if (attrType.find("START_ID") != std::string::npos)
            {
                // 如果属性类型是起始节点 ID
                edge.start_id = std::stoll(field); // 转换为 64 位整数作为起始节点 ID
            }
            else if (attrType.find("END_ID") != std::string::npos)
            {
                // 如果属性类型是结束节点 ID
                edge.end_id = std::stoll(field); // 转换为 64 位整数作为结束节点 ID
            }
            else
            {
                // 处理其他属性
                GPStore::Value value; // 用于存储属性值
                if (attrType.find("ID") != std::string::npos)
                {
                    // 如果是 ID 类型，转换为 64 位整数
                    value = GPStore::Value(std::stoll(field));
                }
                else if (attrType == "LONG")
                {
                    // 如果是长整型
                    value = GPStore::Value(std::stoll(field));
                }
                else if (attrType == "DOUBLE")
                {
                    // 如果是双精度浮点型
                    value = GPStore::Value(std::stod(field));
                }
                else if (attrType == "STRING")
                {
                    // 如果是字符串类型
                    value = GPStore::Value(field);
                }
                else if (attrType == "STRING[]")
                {
                    // 如果是字符串数组
                    std::vector<std::string> tokens = split(field, ';');
                    std::vector<GPStore::Value *> values;
                    for (const auto &token : tokens)
                    {
                        values.push_back(new GPStore::Value(token));
                    }

                    value = GPStore::Value(values, true /* deep_copy */);

                    // 释放动态分配的内存
                    for (auto v : values)
                    {
                        delete v;
                    }
                }
                else
                {
                    // 如果是其他类型，报错
                    std::cerr << "Unsupported attribute type: " << attrType << " in file: " << prefixLower << std::endl;
                    assert(false && "Unsupported attribute type ");
                    value = GPStore::Value(field);
                }
                edge.attributes[attrName] = value; // 存储属性值到边的 attributes 中
            }
            attrIndex++; // 更新属性索引
        }

        edges[edgeType][edge.start_id][edge.end_id] = std::move(edge);
        // 将边存入 edges 容器，按边类型和起始/结束节点 ID 组织

        // 添加出度和入度邻居信息
        GraphNode *startNode = findNode(startNodeSchema, edge.start_id);
        if (startNode != nullptr)
        {
            startNode->neighborsOut[edgeType].push_back(static_cast<GPStore::int_64>(edge.end_id));
            // 如果起始节点存在，添加出度邻居
        }
        else
        {
            // 如果起始节点不存在，创建一个新节点并添加邻居
            GraphNode node;
            node.id = edge.start_id;
            node.neighborsOut[edgeType].push_back(static_cast<GPStore::int_64>(edge.end_id));
            nodes[startNodeSchema][node.id] = std::move(node);
        }

        GraphNode *endNode = findNode(endNodeSchema, edge.end_id);
        if (endNode != nullptr)
        {
            endNode->neighborsIn[edgeType].push_back(static_cast<GPStore::int_64>(edge.start_id));
            // 如果结束节点存在，添加入度邻居
        }
        else
        {
            // 如果结束节点不存在，创建一个新节点并添加邻居
            GraphNode node;
            node.id = edge.end_id;
            node.neighborsIn[edgeType].push_back(static_cast<GPStore::int_64>(edge.start_id));
            nodes[endNodeSchema][node.id] = std::move(node);
        }
    }

#ifdef DEBUG
    // 如果定义了 DEBUG 宏，输出加载信息
    std::cout << "Loaded " << edges[edgeType].size() << " edges of type " << edgeType << std::endl;
#endif
}

void Graph::printInfo() const
{
    // 输出图的基本信息，包括节点和边的类型及数量
    std::cout << "Graph Information:" << std::endl;
    std::cout << "Node Types:" << std::endl;
    for (const auto &[type, nodeList] : nodes)
    {
        // 遍历节点容器，输出每种节点类型的数量
        std::cout << "  " << type << ": " << nodeList.size() << " nodes" << std::endl;
    }
    std::cout << "Edge Types:" << std::endl;
    for (const auto &[type, edgeList] : edges)
    {
        // 遍历边容器，输出每种边类型的数量
        std::cout << "  " << type << ": " << edgeList.size() << " edges" << std::endl;
    }
}

Graph::GraphNode *Graph::findNode(int64_t nodeId)
{
    // 在所有节点类型中查找具有指定 ID 的节点
    for (auto &[type, nodeList] : nodes)
    {
        auto nodeIt = nodeList.find(nodeId);
        if (nodeIt != nodeList.end())
        {
            return &(nodeIt->second); // 如果找到节点，返回指针
        }
    }
    return nullptr; // 如果未找到，返回空指针
}

Graph::GraphNode *Graph::findNode(const std::string &nodeType, int64_t nodeId)
{
    // 在指定节点类型中查找具有指定 ID 的节点
    auto it = nodes.find(nodeType);
    if (it == nodes.end())
    {
        // 如果节点类型不存在，输出错误信息并返回空指针
        std::cerr << "Can not find node type: " << nodeType << std::endl;
        return nullptr;
    }

    auto nodeIt = it->second.find(nodeId);
    if (nodeIt != it->second.end())
    {
        // 如果找到节点，返回指针
        return &(nodeIt->second);
    }

    return nullptr; // 如果未找到，返回空指针
}

Graph::GraphEdge *Graph::findEdge(const std::string &edgeType, int64_t startId, int64_t endId)
{
    // 查找具有指定起始和结束节点 ID 的边
    auto it = edges.find(edgeType);
    if (it == edges.end())
    {
        // 如果边类型不存在，输出错误信息并返回空指针
        std::cerr << "Can not find edge type: " << edgeType << std::endl;
        return nullptr;
    }

    auto startIt = it->second.find(startId);
    if (startIt == it->second.end())
    {
        // 如果起始节点 ID 不存在，输出错误信息并返回空指针
        std::cerr << "Can not find start node: " << startId << " for edge: " << edgeType << std::endl;
        return nullptr;
    }

    auto endIt = startIt->second.find(endId);
    if (endIt == startIt->second.end())
    {
        // 如果结束节点 ID 不存在，输出错误信息并返回空指针
        std::cerr << "Can not find end node: " << endId << " for edge: " << edgeType << std::endl;
        return nullptr;
    }

    return &(endIt->second); // 如果找到边，返回指针
}
