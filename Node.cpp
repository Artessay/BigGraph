
#include "Node.h"

Node::Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value) {

}

Node::Node(unsigned node_id) {
    
}

GPStore::Value* Node::operator[](const std::string& property_string) {
}

void Node::GetLinkedNodes(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, unsigned& list_len, char edge_dir) {
}

void Node::GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const unsigned[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                       unsigned& prop_len, unsigned& list_len, char edge_dir) {
}