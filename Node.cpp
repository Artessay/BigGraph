#include "Node.h" // 包含自定义 Node 类的头文件

#include <assert.h> // 提供 assert 宏用于调试时的条件断言
#include <iostream> // 提供输入输出流功能

// 构造函数：通过属性字符串和值创建节点
Node::Node(const std::string &label_string, const std::string &prop_string, const GPStore::Value *value)
{
    assert(prop_string == "id");
    // 确保属性字符串是 "id"，这是约定的节点标识属性

    int64_t node_id = value->toLLong();
    // 将 GPStore::Value 转换为 64 位长整型，表示节点 ID

    node_ = Graph::getInstance().findNode(label_string, node_id);
    // 在图实例中查找具有指定标签和 ID 的节点
    node_id_ = node_id;
    // 存储节点 ID
}

// 构造函数：通过标签字符串和节点 ID 创建节点
Node::Node(const std::string &label_string, int64_t node_id)
{
    node_ = Graph::getInstance().findNode(label_string, node_id);
    // 在图实例中查找具有指定标签和 ID 的节点
    node_id_ = node_id;
    // 存储节点 ID
}

// 构造函数：仅通过节点 ID 创建节点（不关心标签）
Node::Node(int64_t node_id)
{
    node_ = Graph::getInstance().findNode(node_id);
    // 在图实例中查找具有指定 ID 的节点（忽略标签）
    node_id_ = node_id;
    // 存储节点 ID
}

// 重载操作符 []：通过属性名访问节点的属性值
GPStore::Value *Node::operator[](const std::string &property_string)
{
    assert(node_ != nullptr);
    // 确保节点指针非空

    {
        auto it = node_->attributes.find(property_string);
        if (it != node_->attributes.end())
        {
            // 在节点的属性中查找指定属性名
            return &it->second;
            // 如果找到，返回属性值的指针
        }
    }

    {
        auto it = node_->neighborsOut.find(property_string);
        if (it != node_->neighborsOut.end())
        {
            // 在节点的出度邻居中查找指定属性名
            std::vector<GPStore::Value> &neighbors = it->second;
            assert(neighbors.size() == 1);
            // 假设只有一个匹配的邻居
            return &neighbors[0];
            // 返回邻居节点的值指针
        }
    }

    std::cerr << "Can not find property: " << property_string << " in node: " << node_->id << std::endl;
    // 如果未找到属性或邻居，输出错误信息
    return nullptr;
    // 返回空指针
}

// 获取与当前节点通过特定类型的边相连的节点
std::vector<GPStore::Value> &Node::GetLinkedNodes(const std::string &pre_str, char edge_dir)
{
    assert(node_ != nullptr);
    // 确保节点指针非空

    if (edge_dir == 'o')
    {
        // 如果边的方向是出度
        return node_->neighborsOut[pre_str];
        // 返回出度邻居的列表
    }
    else if (edge_dir == 'i')
    {
        // 如果边的方向是入度
        return node_->neighborsIn[pre_str];
        // 返回入度邻居的列表
    }
    else
    {
        assert(false && "Invalid edge direction");
        // 如果边的方向无效，触发断言失败
    }
}

// 获取特定边的属性值
GPStore::Value *Node::GetEdgeProps(const std::string &edge_type, int64_t start_id, int64_t end_id, const std::string &prop_name)
{
    Graph::GraphEdge *graphEdge = Graph::getInstance().findEdge(edge_type, start_id, end_id);
    // 在图实例中查找具有指定起始和结束节点的边
    if (graphEdge == nullptr)
    {
        assert(false && "Edge not found");
        // 如果未找到边，触发断言失败
        return nullptr;
    }

    auto it = graphEdge->attributes.find(prop_name);
    // 查找边的属性中指定的属性名
    if (it != graphEdge->attributes.end())
    {
        return &it->second;
        // 如果找到，返回属性值的指针
    }
    else
    {
        assert(false && "Property not found");
        // 如果未找到属性，触发断言失败
        return nullptr;
    }
}

// 获取与当前节点通过特定类型的边相连的节点列表（未实现）
void Node::GetLinkedNodes(const std::string &pre_str, std::shared_ptr<const int64_t[]> &nodes_list, unsigned &list_len, char edge_dir)
{
    // 占位函数，未实现具体逻辑
}

// 获取与当前节点通过特定类型的边相连的节点及其边的属性值（未实现）
void Node::GetLinkedNodesWithEdgeProps(const std::string &pre_str, std::shared_ptr<const int64_t[]> &nodes_list, std::shared_ptr<const long long[]> &prop_list,
                                       unsigned &prop_len, unsigned &list_len, char edge_dir)
{
    // 占位函数，未实现具体逻辑
}

// 输出节点的信息
void Node::printInfo() const
{
    std::cout << "Node Information:" << std::endl;
    std::cout << "Node ID: " << node_id_ << std::endl;
    // 输出节点 ID

    assert(node_ != nullptr);
    // 确保节点指针非空

    std::cout << "Attributes: " << std::endl;
    for (const auto &[attr, val] : node_->attributes)
    {
        // 遍历节点的属性
        std::cout << "  " << attr << ": " << val.toString() << std::endl;
        // 输出属性名称和值
    }
}
