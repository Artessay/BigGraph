#include "Node.h"

#include <assert.h>
#include <iostream>

Node::Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value) {
    assert(prop_string == "id");

    int64_t id = value->toLLong();
    node_ = Graph::getInstance().findNode(label_string, id);
}

Node::Node(unsigned node_id) {
    node_ = Graph::getInstance().findNode(node_id);
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
        auto it = node_->neighbors.find(property_string);
        if (it != node_->neighbors.end()) {
            std::vector<GPStore::Value>& neighbors = it->second;
            assert(neighbors.size() == 1);
            return &neighbors[0];
        }
    }

    std::cerr << "Can not find property: " << property_string << " in node: " << node_->id << std::endl;
    assert(false && "Property not found");
    return nullptr;
}

void Node::GetLinkedNodes(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, unsigned& list_len, char edge_dir) {
}

void Node::GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                       unsigned& prop_len, unsigned& list_len, char edge_dir) {
}