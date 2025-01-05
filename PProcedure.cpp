#include "PProcedure.h"
#include <cassert>
#include <iostream>
using namespace std;

const unsigned LIMIT_NUM = 20;
const char EDGE_IN = 'i';
const char EDGE_OUT = 'o';

void ProcessMessageLikes(Node &message, long long message_id, long long message_creation_date,
const std::string &message_content, Node &other_person, std::map<TYPE_ENTITY_LITERAL_ID, long long> &person_id_map,
std::map<long long, std::pair<long long, long long> > &candidates_index, std::map<std::pair<long long, long long>, std::tuple<long long, long long, std::string, int> > &candidates) {
    std::shared_ptr<const TYPE_ENTITY_LITERAL_ID[]> person_friends = nullptr;
    unsigned friends_num = 0;
    std::shared_ptr<const long long[]> creation_date_list = nullptr;
    unsigned creation_data_width = 0;
    message.GetLinkedNodesWithEdgeProps("LIKES", person_friends, creation_date_list, creation_data_width, friends_num, EDGE_IN);
    for (unsigned j = 0; j < friends_num; ++j) {
        auto person_vid = person_friends[j];
        long long like_creation_date = creation_date_list[j];
        auto it = candidates_index.find(person_vid);
        if (it != candidates_index.end()) {
            auto &key = it->second;
            if (like_creation_date < 0 - key.first) {
                continue;
            }
            if (like_creation_date == 0 - key.first) {
                auto cit = candidates.find(key);
                long long old_message_id = std::get<1>(cit->second);
                if (message_id > old_message_id) {
                    continue;
                }
            }
            candidates.erase(key);
            key.first = 0 - like_creation_date;
            candidates.emplace(key, std::make_tuple(person_vid, message_id, message_content,
                                (like_creation_date - message_creation_date) / 1000 / 60));
        } else {
            long long person_id;
            auto pit = person_id_map.find(person_vid);
            if (pit != person_id_map.end()) {
                person_id = pit->second;
            } else {
                other_person.Goto(person_vid);
                person_id = other_person["id"]->toLLong();
                person_id_map[person_vid] = person_id;
            }
            auto key = std::make_pair(0 - like_creation_date, person_id);
            if (candidates.size() >= LIMIT_NUM && candidates.lower_bound(key) == candidates.end()) {
                continue;
            }
            candidates.emplace(key, std::make_tuple(person_vid, message_id, message_content,
                                (like_creation_date - message_creation_date) / 1000 / 60));
            candidates_index.emplace(person_vid, key);
            if (candidates.size() > LIMIT_NUM) {
                auto cit = --candidates.end();
                candidates_index.erase(candidates_index.find(std::get<0>(cit->second)));
                candidates.erase(cit);
            }
        }
    }
}

void ic1(const std::vector<GPStore::Value> &args, std::vector<std::vector<GPStore::Value>> &result)
{
    // 从参数提取 first_name
    string first_name = args[1].toString();
    const char *first_name_char = first_name.data(); // 获取 first_name 的 C 风格字符串
    unsigned first_name_size = first_name.size();    // 获取 first_name 的长度

    // 创建初始 Person 节点，使用 "id" 属性和 args[0] 提供的值
    Node person_node("Person", "id", &args[0]);
    if (person_node.node_id_ == -1) // 如果该节点不存在，则直接返回
        return;
    assert(person_node.node_ != nullptr); // 确保节点有效

    // 定义候选集合，存储符合条件的候选元组
    std::set<std::tuple<int, std::string, long long, uint64_t>> candidates;

    // 设置 BFS 的初始状态
    TYPE_ENTITY_LITERAL_ID start_vid = person_node.node_id_;        // 起始节点 ID
    std::vector<TYPE_ENTITY_LITERAL_ID> curr_frontier({start_vid}); // 当前搜索边界
    std::set<TYPE_ENTITY_LITERAL_ID> visited({start_vid});          // 已访问节点集合

    // 开始 BFS 搜索，最多扩展三层
    for (int distance = 0; distance <= 3; distance++)
    {
        std::vector<TYPE_ENTITY_LITERAL_ID> next_frontier; // 下一层边界节点集合

        // 遍历当前边界中的每个节点
        for (const auto &vid : curr_frontier)
        {
            Node froniter_person("Person", vid);      // 创建对应的 Person 节点
            assert(froniter_person.node_ != nullptr); // 确保节点有效

            // 检查条件：起始节点或 first_name 不匹配则跳过
            bool flag = vid == start_vid;
            flag = flag || (froniter_person["firstName"]->toString() != first_name);
            if (flag)
                continue;

            // 获取 lastName 和 id 并创建候选元组
            std::string last_name = froniter_person["lastName"]->toString();
            long long person_id = froniter_person["id"]->toLLong();
            auto tup = std::make_tuple(distance, last_name, person_id, vid);

            // 如果候选集合超过限制，维护大小限制
            if (candidates.size() >= LIMIT_NUM)
            {
                auto &candidate = *candidates.rbegin(); // 集合中最后一个（最大值）
                if (tup > candidate)
                    continue; // 跳过更大的候选元组
            }
            candidates.emplace(std::move(tup)); // 添加新候选元组
            if (candidates.size() > LIMIT_NUM)
            {
                candidates.erase(--candidates.end()); // 移除最大值以保持大小限制
            }
        }

        // 如果达到限制或最大深度，停止搜索
        if (candidates.size() >= LIMIT_NUM || distance == 3)
            break;

        // 扩展到下一层边界
        for (auto vid : curr_frontier)
        {
            Node froniter_person("Person", vid); // 当前边界的节点

            // 获取出度邻居
            std::vector<GPStore::Value> &friends_out = froniter_person.GetLinkedNodes("knows", EDGE_OUT);
            for (const auto &friend_node : friends_out)
            {
                TYPE_ENTITY_LITERAL_ID friend_vid = friend_node.toLLong();
                if (visited.find(friend_vid) == visited.end())
                {
                    visited.emplace(friend_vid);            // 标记访问
                    next_frontier.emplace_back(friend_vid); // 添加到下一层边界
                }
            }

            // 获取入度邻居
            std::vector<GPStore::Value> &friends_in = froniter_person.GetLinkedNodes("knows", EDGE_IN);
            for (const auto &friend_node : friends_in)
            {
                TYPE_ENTITY_LITERAL_ID friend_vid = friend_node.toLLong();
                if (visited.find(friend_vid) == visited.end())
                {
                    visited.emplace(friend_vid);            // 标记访问
                    next_frontier.emplace_back(friend_vid); // 添加到下一层边界
                }
            }
        }

        // 准备下一轮搜索
        std::sort(next_frontier.begin(), next_frontier.end());
        curr_frontier.swap(next_frontier);
    }

    // 将候选结果填充到 result
    for (const auto &tup : candidates)
    {
        int64_t vid = std::get<3>(tup);
        Node person("Person", vid); // 获取候选节点

        // 填充基础属性到结果中
        result.emplace_back();
        result.back().reserve(13);                    // 预留 13 个值
        result.back().emplace_back(std::get<2>(tup)); // id
        result.back().emplace_back(std::get<1>(tup)); // lastName
        result.back().emplace_back(std::get<0>(tup)); // distance
        result.back().emplace_back(*person["birthday"]);
        result.back().emplace_back(*person["creationDate"]);
        result.back().emplace_back(*person["gender"]);
        result.back().emplace_back(*person["browserUsed"]);
        result.back().emplace_back(*person["locationIP"]);
        result.back().emplace_back(*person["email"]);
        result.back().emplace_back(*person["language"]);
        result.back().emplace_back(*Node("Place", person["isLocatedIn"]->toLLong())["name"]);

        // 填充大学信息
        result.back().emplace_back(GPStore::Value::Type::LIST);
        std::vector<GPStore::Value> &universities = person.GetLinkedNodes("studyAt", EDGE_OUT);
        for (const auto &university : universities)
        {
            Node university_node("Organisation", university.toLLong());
            if (university_node["isLocatedIn"] == nullptr)
                continue;
            Node location_city("Place", university_node["isLocatedIn"]->toLLong());
            GPStore::Value *classYear = Node::GetEdgeProps("studyAt", person.node_id_, university_node.node_id_, "classYear");
            std::vector<GPStore::Value *> university_prop_vec{university_node["name"], classYear, location_city["name"]};
            result.back().back().data_.List->emplace_back(new GPStore::Value(university_prop_vec, true));
        }

        // 填充公司信息
        result.back().emplace_back(GPStore::Value::Type::LIST);
        std::vector<GPStore::Value> &companies = person.GetLinkedNodes("workAt", EDGE_OUT);
        for (const auto &company : companies)
        {
            Node company_node("Organisation", company.toLLong());
            if (company_node["isLocatedIn"] == nullptr)
                continue;
            Node location_country("Place", company_node["isLocatedIn"]->toLLong());
            GPStore::Value *work_from = Node::GetEdgeProps("workAt", person.node_id_, company_node.node_id_, "workFrom");
            std::vector<GPStore::Value *> company_prop_vec{company_node["name"], work_from, location_country["name"]};
            result.back().back().data_.List->emplace_back(new GPStore::Value(company_prop_vec, true));
        }
    }
}

void ic2(const std::vector<GPStore::Value> &args, std::vector<std::vector<GPStore::Value>> &result) {
}

void is1(const std::vector<GPStore::Value> &args, std::vector<std::vector<GPStore::Value>> &result) {
    Node person_node("Person", "id", &args[0]);
    if (person_node.node_id_ == -1)
        return;

    result.emplace_back();
    result.back().reserve(8);
    result.back().emplace_back(*person_node["firstName"]);
    result.back().emplace_back(*person_node["lastName"]);
    result.back().emplace_back(*person_node["birthday"]);
    result.back().emplace_back(*person_node["locationIP"]);
    result.back().emplace_back(*person_node["browserUsed"]);
    Node city_node("Place", person_node["isLocatedIn"]->toLLong());
    result.back().emplace_back(*city_node["id"]);
    result.back().emplace_back(*person_node["gender"]);
    result.back().emplace_back(*person_node["creationDate"]);
}