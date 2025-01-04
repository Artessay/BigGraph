#include <string>
#include <memory>
#include "Value.h"
#include "Graph.h"

class Node {
 public:
  int64_t node_id_;

  Graph::GraphNode* node_;
  
  Node()=default;
  Node(const std::string& label_string, const std::string& prop_string, const GPStore::Value* value);
  Node(int64_t node_id);
  Node(const std::string& label_string, int64_t node_id);
  void Goto(int64_t new_node_id) {
    node_id_ = new_node_id;
  }
  GPStore::Value* operator[](const std::string& property_string);
  void GetLinkedNodes(const std::string&, std::shared_ptr<const int64_t[]>& nodes_list, unsigned& list_len, char edge_dir);
  void GetLinkedNodesWithEdgeProps(const std::string& pre_str, std::shared_ptr<const int64_t[]>& nodes_list, std::shared_ptr<const long long[]>& prop_list,
                                   unsigned& prop_len, unsigned& list_len, char edge_dir);

  std::vector<GPStore::Value>& GetLinkedNodes(const std::string& pre_str, char edge_dir);

  void printInfo() const;
};
