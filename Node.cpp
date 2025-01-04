#include "Node.h"

#include <assert.h>
#include <iostream>

Node::Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value) {
    assert(prop_string == "id");
    int64_t node_id = value->toLLong();

    node_ = Graph::getInstance().findNode(label_string, node_id);
    node_id_ = node_id;
}

Node::Node(const std::string& label_string, int64_t node_id) {
    node_ = Graph::getInstance().findNode(label_string, node_id);
    node_id_ = node_id;
}

Node::Node(int64_t node_id) {
    node_ = Graph::getInstance().findNode(node_id);
    node_id_ = node_id;
}

GPStore::Value* Node::operator[](const std::string& property_string) {
    assert (node_ != nullptr);

    {
        auto it = node_->attributes.find(property_string);
        if (it != node_->attributes.end()) {
            return &it->second;
        }
    }

    {
        auto it = node_->neighborsOut.find(property_string);
        if (it != node_->neighborsOut.end()) {
            std::vector<GPStore::Value>& neighbors = it->second;
            assert(neighbors.size() == 1);
            return &neighbors[0];
        }
    }

    std::cerr << "Can not find property: " << property_string << " in node: " << node_->id << std::endl;
    assert(false && "Property not found");
    return nullptr;
}

std::vector<GPStore::Value>& Node::GetLinkedNodes(const std::string& pre_str, char edge_dir) {
    assert(node_ != nullptr);
    if (edge_dir == 'o') {
        return node_->neighborsOut[pre_str];
    } else if (edge_dir == 'i') {
        return node_->neighborsIn[pre_str];
    } else {
        assert(false && "Invalid edge direction");
    }
}

void Node::GetLinkedNodes(const std::string& pre_str, std::shared_ptr<const int64_t[]>& nodes_list, unsigned& list_len, char edge_dir) {
}

void Node::GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const int64_t[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                       unsigned& prop_len, unsigned& list_len, char edge_dir) {
}


void Node::printInfo() const {
    std::cout << "Node Information:" << std::endl;
    std::cout << "Node ID: " << node_id_ << std::endl;
    assert(node_ != nullptr);
    std::cout << "Attributes: " << std::endl;
    for (const auto& [attr, val] : node_->attributes) {
        std::cout << "  " << attr << ": " << val.toString() << std::endl;
    }
}