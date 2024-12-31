#include "Value.h"
#include <cassert>
#include <math.h>
#include <set>
#include <sstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <limits.h>
using namespace std;

/**
 * @brief Constructs a Value object which is of type NO_VALUE.
 * This default constructor will not allocate any memory, and the type of the Value object is NO_VALUE.
 * @remark 默认构造函数
 */
GPStore::Value::Value(){
    type_ = NO_VALUE;
}

/**
 * @brief Constructs a Value object from a pointer to another Value.
 * 
 * If the provided pointer is null, the type of this Value is set to NO_VALUE.
 * Otherwise, it initializes this Value using the data from the pointed Value.
 * 
 * @param value_ptr A pointer to a GPStore::Value object. If null, the type is set to NO_VALUE.
 * 
 * @remark 从指向另一个Value的指针构造
 */
GPStore::Value::Value(GPStore::Value *value_ptr) {
  if (!value_ptr) {type_ = NO_VALUE;}
  else ConstructFrom(*value_ptr);
}

/**
 * @brief Constructs a Value object with the given type.
 * 
 * This constructor is used to construct a Value object with a specific type.
 * If the type is STRING, PATH, LIST, MAP, DATE_TIME, the object will allocate
 * memory for that type. Otherwise, the type is set to NO_VALUE.
 * 
 * @param _type_ The type of the Value object to be constructed.
 * 
 * @remark 根据类型构造
 * 
 */
GPStore::Value::Value(Type _type_):type_(_type_){
    if(_type_ == STRING){
        data_.String = new string();
    } else if(_type_ == PATH){
        data_.Path = new PathContent();
    }else if(_type_ == LIST){
        data_.List = new vector<Value*>;
    }else if(_type_ == MAP){
        data_.Map = new map<std::string, Value *>();
    }else if(_type_ == DATE_TIME){
        data_.Datetime = new DateTime();
    }else if(_type_ == NO_VALUE){
        data_.Long = 0;
    }
}

/**
 * @brief Copy constructor for GPStore::Value.
 * 
 * This constructor initializes a new Value object as a copy of another Value object.
 * It utilizes the ConstructFrom method to copy the data from the provided Value object.
 *
 * @param other The Value object to be copied.
 * @remark 拷贝构造函数
 */
GPStore::Value::Value(const Value &other){
    ConstructFrom(other);
}

/**
 * @brief Constructs a Value object from a 32-bit integer.
 * 
 * This constructor is used to construct a Value object from a 32-bit integer.
 * The type of this Value object is set to INTEGER.
 * 
 * @param int_ The 32-bit integer value used to construct the Value object.
 * 
 * @remark 从32位整数构造
 */
GPStore::Value::Value(int_32 int_){
    type_ = INTEGER;
    data_.Int = int_;
}

/**
 * @brief Constructs a Value object from a 64-bit integer.
 * 
 * This constructor is used to construct a Value object from a 64-bit integer.
 * The type of this Value object is set to LONG.
 * 
 * @param long_ The 64-bit integer value used to construct the Value object.
 * 
 * @remark 从64位整数构造
 */
GPStore::Value::Value(int_64 long_){
    type_ = LONG;
    data_.Long = long_;
}

/**
 * @brief Constructs a Value object from a floating point number.
 * 
 * This constructor is used to construct a Value object from a floating point number.
 * The type of this Value object is set to FLOAT.
 * 
 * @param float_ The floating point number used to construct the Value object.
 * 
 * @remark 从浮点数构造 
 */
GPStore::Value::Value(double float_):type_(FLOAT){
    data_.Float = float_;
}

/**
 * @brief Constructs a Value object from a string.
 * 
 * This constructor is used to construct a Value object from a given string.
 * The type of this Value object is set to STRING.
 * 
 * @param str The string used to construct the Value object.
 * 
 * @remark 从字符串构造
 */
GPStore::Value::Value(const std::string& str):type_(STRING){
    data_.String = new string(str);
}

/**
 * @brief Constructs a Value object from a C-style string.
 * 
 * This constructor is used to construct a Value object from a C-style string.
 * The type of this Value object is set to STRING.
 * 
 * @param s The C-style string used to construct the Value object.
 * 
 * @remark 从C风格字符串构造
 */
GPStore::Value::Value(const char* s):type_(STRING){
    data_.String = new string(s);
}

/**
 * @brief Constructs a Value object from a boolean value.
 * 
 * This constructor is used to construct a Value object from a boolean value.
 * The type of this Value object is set to BOOLEAN.
 * 
 * @param b The boolean value used to construct the Value object.
 * 
 * @remark 从布尔值构造
 */
GPStore::Value::Value(bool b):type_(BOOLEAN){
    data_.Boolean = b;
}

/**
 * @brief Constructs a Value object from a type and a 32-bit identifier.
 * 
 * This constructor is used to construct a Value object from a specified type
 * and a 32-bit identifier. Depending on the type, the identifier is either
 * stored as a Node or an Edge.
 * 
 * @param _type The type of the Value object, which determines how the identifier is stored.
 * @param _id The 32-bit identifier used to construct the Value object.
 * 
 * @remark 根据类型和32位标识符构造
 */
GPStore::Value::Value(Type _type, uint_32 _id):type_(_type){
    if(_type == NODE)
        data_.Node = (uint_32)_id;
    else
        data_.Edge = (uint_64)_id;
}

/**
 * @brief Constructs a Value object from a type and a 64-bit identifier.
 * 
 * This constructor is used to construct a Value object from a specified type
 * and a 64-bit identifier. Depending on the type, the identifier is either
 * stored as a Node or an Edge.
 * 
 * @param _type The type of the Value object, which determines how the identifier is stored.
 * @param _id The 64-bit identifier used to construct the Value object.
 * 
 * @remark 根据类型和64位标识符构造
 */
GPStore::Value::Value(Type _type, uint_64 _id):type_(_type){
    if(_type == NODE)
        data_.Node = (uint_32)_id;
    else
        data_.Edge = (uint_64)_id;
}

/**
 * @brief Constructs a Value object from a PathContent instance.
 * 
 * This constructor initializes a Value object of type PATH by deep copying
 * the node and edge identifiers from the provided PathContent instance.
 * 
 * @param path_ The PathContent instance containing node and edge identifiers
 *              to be copied into the new Value object.
 * @remark 从路径构造
 */
GPStore::Value::Value(const PathContent &path_):type_(PATH){
    data_.Path = new PathContent();
    data_.Path->node_id_ = path_.node_id_;
    data_.Path->edge_id_ = path_.edge_id_;
}

/**
 * @brief Constructs a Value object from node and edge identifiers.
 * 
 * This constructor initializes a Value object of type PATH by assigning
 * the provided vectors of node and edge identifiers to the internal
 * PathContent structure.
 * 
 * @param node_id A vector of unsigned integers representing node identifiers.
 * @param edge_id A vector of 64-bit integers representing edge identifiers.
 * @remark 从节点和边标识符构造
 */
GPStore::Value::Value(const std::vector<unsigned >& node_id, const std::vector<uint_64> &edge_id):type_(PATH){
    data_.Path = new PathContent();
    data_.Path->node_id_ = node_id;
    data_.Path->edge_id_ = edge_id;
}

/**
 * @brief Constructs a Value object from a vector of Value pointers.
 * 
 * This constructor is used to construct a Value object from a vector of Value
 * pointers. If the deep_copy parameter is true, the Value pointers in the
 * vector are deep copied into the new Value object. Otherwise, only the
 * pointers are copied, meaning that the Value pointers in the vector and the
 * new Value object point to the same objects.
 * 
 * @param list_ The vector of Value pointers used to construct the new Value
 *              object.
 * @param deep_copy If true, the Value pointers in the vector are deep copied,
 *                  otherwise only the pointers are copied.
 * 
 * @remark 构造列表
 */
GPStore::Value::Value(const std::vector<Value *> &list_, bool deep_copy):type_(LIST){
    data_.List = new vector<Value *>();
    for(Value *v : list_){
        if(deep_copy)
            data_.List->push_back(ValueDeepCopy(v));
        else
            data_.List->push_back(v);
    }
}

/**
 * @brief Constructs a Value object from a vector of strings and a vector of Value pointers.
 * 
 * This constructor is used to construct a Value object from a vector of strings
 * and a vector of Value pointers. If the deep_copy parameter is true, the Value
 * pointers in the vector are deep copied into the new Value object. Otherwise,
 * only the pointers are copied, meaning that the Value pointers in the vector
 * and the new Value object point to the same objects.
 * 
 * @param keys The vector of strings used as keys in the map.
 * @param values The vector of Value pointers used as values in the map.
 * @param deep_copy If true, the Value pointers in the vector are deep copied,
 *                  otherwise only the pointers are copied.
 * 
 * @remark 构造字典
 */
GPStore::Value::Value(const std::vector<std::string> &keys, const std::vector<Value *> &values, bool deep_copy):type_(MAP) {
    data_.Map = new map<std::string, Value *>();
    uint_64 n = keys.size();
    assert(values.size() == n);
    for(uint_64 i = 0; i < n; ++i){
        if(deep_copy)
            data_.Map->insert(make_pair(keys[i], ValueDeepCopy(values[i])));
        else
            data_.Map->insert(make_pair(keys[i], values[i]));
    }
}

/**
 * @brief Constructs a GPStore::Value object with a DateTime parameter.  
 * Initializes the internal Datetime structure by copying values  
 * from the provided DateTime object.
 * 
 * @param datetime_ The DateTime object used to initialize the new 
 * GPStore::Value instance.
 * @remark 日期时间构造函数
 */
GPStore::Value::Value(const DateTime &datetime_):type_(DATE_TIME){
    data_.Datetime = new DateTime();
    data_.Datetime->year = datetime_.year;
    data_.Datetime->month = datetime_.month;
    data_.Datetime->day = datetime_.day;
    data_.Datetime->hour = datetime_.hour;
    data_.Datetime->minute = datetime_.minute;
    data_.Datetime->second = datetime_.second;
    data_.Datetime->millisecond = datetime_.millisecond;
    data_.Datetime->microsecond = datetime_.microsecond;
    data_.Datetime->nanosecond = datetime_.nanosecond;
    data_.Datetime->timezone = datetime_.timezone;
}

/**
 * @brief Sets the datetime value by converting the given time parameters.  
 * The function extracts various time units (year, month, day, hour, etc.)  
 * from the provided `sumtime` and `nanosecond_` using chrono utilities.  
 * Adjusts for timezone differences, with specific handling for hour shifts.
 * 
 * @param sumtime The total milliseconds since epoch.
 * @param nanosecond_ Additional nanoseconds to be added.
 * @remark 设置日期时间
 */
void GPStore::Value::SetDatetime(long long sumtime, long long nanosecond_){
    std::chrono::milliseconds timestamp(sumtime);
    std::chrono::nanoseconds nanosecondsPart(nanosecond_);
    std::time_t timestampSec = std::chrono::duration_cast<std::chrono::seconds>(timestamp).count();
    // std::tm* localTime = std::localtime(&timestampSec); // ! localtime is not threadsafe: The result is stored in static storage and a pointer to that static storage is returned
    // https://stackoverflow.com/questions/38034033/c-localtime-this-function-or-variable-may-be-unsafe
    // https://en.cppreference.com/w/c/chrono/localtime
    std::tm localTime;
    localtime_r(&timestampSec, &localTime); // uses user-provided storage buf for the result.

    data_.Datetime->year = localTime.tm_year + 1900;    
    data_.Datetime->month = localTime.tm_mon + 1;  
    if(localTime.tm_hour >= 8){
        data_.Datetime->day = localTime.tm_mday;
        data_.Datetime->hour = localTime.tm_hour - 8 ;
    }
    else{
        data_.Datetime->day = localTime.tm_mday-1;
        data_.Datetime->hour = localTime.tm_hour + 16 ;
    }     
    data_.Datetime->minute = localTime.tm_min;
    data_.Datetime->second = localTime.tm_sec;

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp);
    data_.Datetime->millisecond = duration.count() % 1000;
    data_.Datetime->microsecond = nanosecondsPart.count() / 1000;
    data_.Datetime->nanosecond = nanosecondsPart.count() % 1000;
}

/**
 * @brief Sets the datetime value using another GPStore::Value object.  
 * The function retrieves the total milliseconds and nanoseconds from  
 * the `other` object. If `epochMillis` is not found, it falls back  
 * to using `epochSeconds` and converts it to milliseconds. 
 * 
 * @param other The source GPStore::Value object from which datetime 
 * information is retrieved.
 * @remark 从另一个对象设置日期时间
 */
void GPStore::Value::SetDatetime(GPStore::Value &other){
    // other is Map
    long long sumtime = 0 ,nanosecond_ = 0;
    auto iter = other.search("nanosecond");  
    if(iter != nullptr ) nanosecond_ = iter->toLLong() ;
    iter = other.search("epochMillis");  
    if(iter != nullptr ) sumtime = iter -> toLLong() ;
    else sumtime = (other.search("epochSeconds") -> toLLong()) *1000 ;
    SetDatetime(sumtime, nanosecond_);
}

std::pair<GPStore::uint_16, GPStore::uint_16> GPStore::Value::getMonthDay()
{
    if(type_ != Type::DATE_TIME) return {};
    return {data_.Datetime->month, data_.Datetime->day};
}


/**
 * @brief Destroys the Value object.
 * 
 * The destructor simply calls Destruct() to free any allocated memory.
 * 
 * @remark 析构
 */
GPStore::Value::~Value(){
    Destruct();
}

/**
 * @brief Returns true if the Value object is null or represents an invalid entity or edge.
 * 
 * Null values are represented by the special value NO_VALUE.
 * Invalid entities and edges are represented by the special value INVALID_ENTITY_LITERAL_ID and INVALID_EDGE_ID respectively.
 * 
 * @return true if the Value object is null or represents an invalid entity or edge.
 * @remark 返回值是否为空
 */
bool GPStore::Value::isNull() const{
    return type_ == NO_VALUE || (type_ == NODE && data_.Node == UINT_MAX) || (type_ == EDGE && data_.Edge == 0xffffffffffffffff);
}

bool GPStore::Value::isErrorValue() const {
    return type_ == ERROR_VALUE;
}

bool GPStore::Value::isNumber() const{
    return type_ == FLOAT || type_ == INTEGER || type_ == LONG;
}

bool GPStore::Value::isInteger() const {
    return type_ == INTEGER || type_ == LONG;
}

bool GPStore::Value::storable() const{
    return type_ == INTEGER || type_ == FLOAT || type_ == STRING || type_ == BOOLEAN || type_ == LONG ||
           isIntArray() || isLongArray() || isFloatArray() || isBooleanArray() || isStringArray();
}

bool GPStore::Value::isIntArray() const{
     if(type_ != LIST) return false;

     for(const Value *v : *data_.List){
         if(v->type_ != INTEGER)
             return false;
     }

     return true;
 }

bool GPStore::Value::isLongArray() const{
    if(type_ != LIST) return false;

    for(const Value *v : *data_.List){
        if(v->type_ != INTEGER && v->type_ != LONG)
            return false;
    }

    return true;
}


bool GPStore::Value::isFloatArray() const{
    if(type_ != LIST) return false;

    for(const Value *v : *data_.List){
        if(v->type_ != INTEGER && v->type_ != LONG && v->type_ != FLOAT)
            return false;
    }

    return true;
}

bool GPStore::Value::isStringArray() const{
    if(type_ != LIST) return false;

    for(const Value *v : *data_.List){
        if(v->type_ != STRING)
            return false;
    }

    return true;
}

bool GPStore::Value::isBooleanArray() const{
    if(type_ != LIST) return false;

    for(const Value *v : *data_.List){
        if(v->type_ != BOOLEAN)
            return false;
    }

    return true;
}

GPStore::uint_64 GPStore::Value::convertToBytes(void * ptr){
    //TODO
    return 0;
}

void GPStore::Value::constructFromBytes(const void * ptr, uint_64 n){
    // TODO
    return;
}


GPStore::Value::Type GPStore::Value::getType() const{
    return type_;
}

GPStore::int_64 GPStore::Value::hashCode() const {
    int_64 hash = 0, seed = 131, mask = 0x7FFFFFFFFFFFFFFF;
    uint_64 n = 0;
    switch (type_)
    {
        case Value::INTEGER:
            return data_.Int & mask;
        case Value::FLOAT:
            return *(int_64 *) (&data_.Float) & mask;
        case Value::STRING:
            n = data_.String->size();
            for(uint_64 i = 0; i < n; ++i){
                hash = hash * seed + (data_.String->at(i));
            }
            return (hash & mask);
        case Value::BOOLEAN:
            return data_.Boolean ? 1 : 0;
        case Value::NODE:
            return data_.Node & mask;
        case Value::EDGE:
            return data_.Edge & mask;
        case Value::PATH:
            return 7; // Unable to hash a path.
        case Value::LIST:
            n = data_.List->size();
            for(uint_64 i = 0; i < n; ++i){
                hash = hash * seed + (data_.List->at(i)->hashCode());
            }
            return (hash & mask);
        case Value::MAP:
            return 13; // Unable to hash a map
        case Value::NO_VALUE:
            return 17;
        case Value::DATE_TIME:
            return 19;
    }
    return 131; //make g++ happy
}

/**
 * @brief Compare two Value objects
 * This function compares two Value objects. Currently, it checks whether the two
 * Value objects are equal based on their type and content. For example, if the two
 * Value objects are of type INTEGER, it will compare the integer values. If the two
 * Value objects are of type STRING, it will compare the string values.
 *
 * @return true if the two Value objects are equal, false otherwise
 * 
 * @remark 比较两个值
 */
bool GPStore::Value::operator==(const Value& other) const{
    switch (type_)
    {
        case Value::INTEGER:
            return IntEqualTo(other);
        case Value::LONG:
            return LongEqualTo(other);
        case Value::FLOAT:
            return FloatEqualTo(other);
        case Value::STRING:
            return StringEqualTo(other);
        case Value::BOOLEAN:
            return BooleanEqualTo(other);
        case Value::NODE:
            return NodeEqualTo(other);
        case Value::EDGE:
            return EdgeEqualTo(other);
        case Value::PATH:
            return PathEqualTo(other);
        case Value::DATE_TIME:
            return DatetimeEqualTo(other);
        case Value::LIST:
            return ListEqualTo(other);
        case Value::MAP:
            return MapEqualTo(other);
        case Value::NO_VALUE:
            return NoValueEqualTo(other);
    }
    return false; //make g++ happy
}


/**
 * @brief Compare two Value objects
 * This function compares two Value objects. It will return true if the first value
 * is less than the second value, false otherwise.
 * 
 * @param other The second Value object to compare with.
 * 
 * @return true if the first Value object is less than the second Value object, false otherwise
 * 
 * @remark 对两个值进行比较
 */
bool GPStore::Value::operator<(const Value& other) const{
    switch (type_)
    {
        case Value::INTEGER:
            return CompareIntWith(other) == -1;
        case Value::LONG:
            return CompareLongWith(other) == -1;
        case Value::FLOAT:
            return CompareFloatWith(other) == -1;
        case Value::STRING:
            return CompareStringWith(other) == -1;
        case Value::BOOLEAN:
            return CompareBooleanWith(other) == -1;
        case Value::NODE:
            return CompareNodeWith(other) == -1;
        case Value::EDGE:
            return CompareEdgeWith(other)== -1;
        case Value::PATH:
            return ComparePathWith(other) == -1;
        case Value::DATE_TIME:
            return CompareDatetimeWith(other) == -1;
        case Value::LIST:
            return CompareListWith(other) == -1;
        case Value::MAP:
            return CompareMapWith(other) == -1;
        case Value::NO_VALUE:
            return CompareNoValueWith(other) == -1;
    }
    return false;   //make g++ happy
}



/**
 * @brief Compare two Value objects.
 * This function compares two Value objects. It will return a negative integer if
 * the first Value object is less than the second Value object, 0 if they are equal,
 * and a positive integer otherwise.
 * 
 * @param other The second Value object to compare with.
 * 
 * @return -1 if the first Value object is less than the second Value object,
 * 0 if they are equal, 1 otherwise.
 * 
 * @remark 两个值的比较
 */
int GPStore::Value::comp(const Value & other) const{
    switch (type_)
    {
        case Value::INTEGER:
            return CompareIntWith(other);
        case Value::LONG:
            return CompareLongWith(other);
        case Value::FLOAT:
            return CompareFloatWith(other);
        case Value::STRING:
            return CompareStringWith(other);
        case Value::BOOLEAN:
            return CompareBooleanWith(other);
        case Value::NODE:
            return CompareNodeWith(other);
        case Value::EDGE:
            return CompareEdgeWith(other);
        case Value::PATH:
            return ComparePathWith(other);
        case Value::DATE_TIME:
            return CompareDatetimeWith(other);
        case Value::LIST:
            return CompareListWith(other);
        case Value::MAP:
            return CompareMapWith(other);
        case Value::NO_VALUE:
            return CompareNoValueWith(other);
    }
    return 1;
}

/**
 * @brief Copy assignment operator for class Value.
 * This function copies the content of the other Value object to the current
 * Value object.
 * @param other The other Value object to copy from.
 * @remark 赋值运算符
*/
GPStore::Value& GPStore::Value::operator=(const Value&other){
    if(this == &other){
        return *this;
    }
    Destruct();
    ConstructFrom(other);
    return *this;
}

/**
 * @brief Gets the content of a list-typed Value object.
 * 
 * This function returns the content of a list-typed Value object. If the Value
 * object is not of type LIST, this function returns nullptr.
 * 
 * @return The content of the list-typed Value object, or nullptr if the Value
 * object is not of type LIST.
 * 
 * @remark 得到列表内容
 */
const std::vector<GPStore::Value*>* GPStore::Value::getListContent(){
    if(type_ != LIST) return nullptr;
    return this->data_.List;
}

/**
 * @brief Appends a Value object to the list-typed Value object.
 * 
 * This function appends a Value object to the list-typed Value object. If the
 * Value object is not of type LIST, this function does nothing.
 * 
 * @param value The Value object to append.
 * 
 * @remark 向列表中添加元素
 */
void GPStore::Value::append(const GPStore::Value& value){
    data_.List->push_back(ValueDeepCopy(&value));
    return;
}

/**
 * @brief Appends a Value pointer to the list-typed Value object.
 * 
 * This function appends a Value pointer to the list-typed Value object. If the
 * Value object is not of type LIST, this function does nothing.
 * 
 * @param value The Value pointer to append.
 * 
 * @remark 向列表中添加Value指针
 */
void GPStore::Value::append(Value * value){
    data_.List->push_back(value);
}

/**
 * @brief Accesses an element in the list-typed Value object.
 * 
 * This function returns a reference to the element at the specified index in the
 * list-typed Value object. If the Value object is not of type LIST, this function
 * throws an exception.
 * 
 * @param index The index of the element to access.
 * @return A reference to the element at the specified index.
 * @remark 访问列表中的元素
*/
GPStore::Value& GPStore::Value::operator[](uint_64 index){
    assert(type_ == LIST and data_.List->size() > index);
    return *data_.List->at(index);
}

/**
 * @brief Inserts a key-value pair into the map-typed Value object.
 * 
 * This function inserts a key-value pair into the map-typed Value object. If the
 * Value object is not of type MAP, this function does nothing.
 * 
 * @param key The key of the pair to insert.
 * @param value The value of the pair to insert.
 * 
 * @remark 插入键值对
*/
void GPStore::Value::insert(const std::string &key, const GPStore::Value &value){
    assert(type_ == MAP);
    data_.Map->insert(make_pair(key, ValueDeepCopy(&value)));
}

/**
 * @brief Inserts a key-value pair into the map-typed Value object.
 * 
 * This function inserts a key-value pair into the map-typed Value object. If the
 * Value object is not of type MAP, this function does nothing. The value is stored
 * by pointer, so the caller is responsible for ensuring the lifetime of the
 * pointed-to object.
 * 
 * @param key The key of the pair to insert.
 * @param value The value of the pair to insert.
 * 
 * @remark 插入键值对
*/
void GPStore::Value::insert(const std::string &key, Value *val_ptr){
    assert(type_ == MAP);
    data_.Map->insert(std::make_pair(key, val_ptr));
}

/**
 * @brief Searches for a key in the map-typed Value object.
 * 
 * This function searches for the specified key in the map-typed Value object.
 * If the key is found, it returns a pointer to the associated Value object.
 * If the key is not found or the Value object is not of type MAP, it returns nullptr.
 * 
 * @param key The key to search for in the map.
 * @return A pointer to the Value object associated with the key, or nullptr if the key is not found.
 * 
 * @remark 在映射中搜索键
 */
GPStore::Value * GPStore::Value::search(const std::string &key){
    assert(type_ == MAP);
    auto end = data_.Map->end();
    auto it = data_.Map->find(key);
    if(it != end){
        return it->second;
    } else {
        return nullptr;
    }
}

/**
 * @brief Returns the size of the list or map-typed Value object.
 * 
 * This function returns the size of the list or map-typed Value object. If the
 * Value object is not of type LIST or MAP, this function returns 0.
 * @return The size of the list or map-typed Value object.
 * @remark 获取列表或映射的大小
*/
GPStore::uint_64 GPStore::Value::size(){
    if(type_ == STRING){
        return data_.String->size();
    }else if(type_ == LIST){
        return data_.List->size();
    }else if(type_ == MAP){
        return data_.Map->size();
    }else{
        assert(false);
    }
}

/**
 * @brief Converts the Value object to an int.
 * 
 * This function will convert the Value object to an int if it can be converted.
 * If the Value object is of type INTEGER, FLOAT, LONG, or STRING, it will be converted
 * to int. If the Value object is of any other type, 0 will be returned.
 * 
 * @return The integer representation of the Value object.
 * 
 * @remark 将Value对象转换为32位整数
*/
int GPStore::Value::toInt() const{
    switch (type_){
    case INTEGER:
        return data_.Int;
        break;
    case FLOAT:
        return static_cast<int>(data_.Float);
        break;   
    case LONG:
        return static_cast<int>(data_.Long);
        break; 
    case STRING:
        return std::stoi(*data_.String);
        break; 
    default:
        return 0;
        break;       
    }
    return 0;  
}

/**
 * @brief Converts the Value object to a long long.
 * 
 * This function will convert the Value object to a long long if it can be converted.
 * If the Value object is of type INTEGER, FLOAT, LONG, or STRING, it will be converted
 * to long long. If the Value object is of any other type, 0 will be returned.
 * 
 * @return The long long representation of the Value object.
 * 
 * @remark 将Value对象转换为64位整数
*/
long long GPStore::Value::toLLong() const{
    switch (type_){
    case INTEGER:
        return static_cast<long long>(data_.Int);
        break;
    case FLOAT:
        return static_cast<long long>(data_.Float);
        break;   
    case LONG:
        return data_.Long;
        break; 
    case STRING:
        return std::stoll(*data_.String);
        break; 
    default:
        return 0;
        break;       
    }
    return 0;  
}

/**
 * @brief Converts the Value object to a string.
 *
 * This function will convert the Value object to a string if it can be converted.
 * If the Value object is of type INTEGER, FLOAT, LONG, STRING, BOOLEAN, NODE,
 * EDGE, PATH, LIST, or MAP, it will be converted to a string. If the Value object
 * is of any other type, an empty string will be returned.
 *
 * @return The string representation of the Value object.
 *
 * @remark 将Value对象转换为字符串
 */
std::string GPStore::Value::toString() const {
    std::stringstream  buf;
    bool first = true;
    switch (type_)
    {
        case GPStore::Value::INTEGER:
            return to_string(data_.Int);
        case GPStore::Value::LONG:
            return to_string(data_.Long);
        case GPStore::Value::FLOAT:
            return to_string(data_.Float);
        case GPStore::Value::STRING:
            return *data_.String;
        case GPStore::Value::BOOLEAN:
            return data_.Boolean ? "true" : "false";
        case GPStore::Value::NODE:
            return string("Node(") + to_string(data_.Node) + ")";
        case GPStore::Value::EDGE:
            return string("Edge(") + to_string(data_.Edge) + ")";
        case GPStore::Value::PATH:
            return "PATH";
        case GPStore::Value::LIST:
            buf << "[";
            first = true;
            for(auto v : *data_.List){
                if(!first){buf << ", ";}
                else first = false;
                buf << v->toString();
            }
            buf << "]";
            return buf.str();
        case GPStore::Value::MAP:
            buf << "{";
            first = true;
            for(auto & p : *data_.Map){
                if(!first){buf << ", ";}
                else first = false;
                buf << p.first << ": " << p.second->toString();
            }
            buf << "}";
            return buf.str();
        case GPStore::Value::DATE_TIME:
            buf << std::setfill('0');
            buf << std::setw(4) << data_.Datetime->year << '-';
            buf << std::setw(2) << data_.Datetime->month << '-';
            buf << std::setw(2) << data_.Datetime->day << 'T';
            buf << std::setw(2) << data_.Datetime->hour << ':';
            buf << std::setw(2) << data_.Datetime->minute << ':';
            buf << std::setw(2) << data_.Datetime->second << '.';
            buf << std::setw(3) << data_.Datetime->millisecond ;
            buf << std::setw(3) << data_.Datetime->microsecond ;
            buf << std::setw(3) << data_.Datetime->nanosecond << 'Z';
            return buf.str();
        case GPStore::Value::NO_VALUE:
            return "null";
    }
    return "";   //make g++ happy
}

/**
 * @brief Creates a deep copy of a Value object.
 *
 * This function creates a deep copy of the given Value object. The function
 * handles various types of Value objects, such as INTEGER, LONG, FLOAT,
 * STRING, BOOLEAN, NODE, EDGE, PATH, DATE_TIME, LIST, and MAP. For LIST and
 * MAP types, it recursively deep copies the elements or pairs contained
 * within them. If the Value object is of type NO_VALUE, a new Value object
 * of the same type is created.
 *
 * @param value The Value object to be deep copied.
 * @return A pointer to the newly created deep copy of the Value object.
 *
 * @remark 生成Value对象的深拷贝
 */
GPStore::Value * GPStore::Value::ValueDeepCopy(const Value *value){
    Value::Type ty = value->type_;
    Value *new_elem = nullptr;
    switch (ty)
    {
        case GPStore::Value::INTEGER:
            new_elem = new GPStore::Value(value->data_.Int);
            break;
        case GPStore::Value::LONG:
            new_elem = new GPStore::Value(value->data_.Long);
            break;
        case GPStore::Value::FLOAT:
            new_elem = new GPStore::Value(value->data_.Float);
            break;
        case GPStore::Value::STRING:
            new_elem = new GPStore::Value(*value->data_.String);
            break;
        case GPStore::Value::BOOLEAN:
            new_elem = new GPStore::Value(value->data_.Boolean);
            break;
        case GPStore::Value::NODE:
            new_elem = new GPStore::Value(Value::NODE, value->data_.Node);
            break;
        case GPStore::Value::EDGE:
            new_elem = new GPStore::Value(Value::EDGE, value->data_.Edge);
            break;
        case GPStore::Value::PATH:
            new_elem = new GPStore::Value(value->data_.Path->node_id_, value->data_.Path->edge_id_);
            break;
        case GPStore::Value::DATE_TIME:
            new_elem = new GPStore::Value(value->data_.Datetime);
            break;
        case GPStore::Value::LIST:
            new_elem = new GPStore::Value(Value::LIST);
            for(const Value * v: *value->data_.List){
                new_elem->data_.List->push_back(ValueDeepCopy(v));
            }
            break;
        case GPStore::Value::MAP:
            new_elem = new GPStore::Value(GPStore::Value::MAP);
            for(const auto &p : *value->data_.Map){
                new_elem->data_.Map->insert(make_pair(p.first, ValueDeepCopy(p.second)));
            }
            break;
        case GPStore::Value::NO_VALUE:
            new_elem = new GPStore::Value(GPStore::Value::NO_VALUE);
            break;
    }
    return new_elem;
}

/**
 * @brief Moves the current Value object's data and type to another Value object.
 *
 * This function transfers ownership of the current Value object's data
 * and type to the specified target Value object. The target's existing
 * data is destructed before the transfer. After the transfer, the current
 * Value object is set to a NO_VALUE state.
 *
 * @param that The target Value object to which the data and type are moved.
 * @remark 将当前Value对象的数据和类型移动到另一个Value对象
 */
void GPStore::Value::moveTo(GPStore::Value & that){
    that.Destruct();
    that.type_ = type_;
    that.data_ = data_;
    this->type_ = NO_VALUE;
    this->data_.Long = 0;
}

/**
 * @brief Swap the contents of two Value objects.
 *
 * This function swaps the contents of the current Value object with the
 * specified Value object. The swap is done in a non-throwing manner, i.e.,
 * no exceptions are thrown regardless of whether the swap is successful or
 * not.
 *
 * @param that The Value object with which the current Value object is swapped.
 * @remark 交换两个Value对象的内容
 */
void GPStore::Value::swap(GPStore::Value &that) noexcept{
    GPStore::Value::Type ty;
    GPStore::Value::ValueUnion val;

    ty = that.type_;
    that.type_ = this->type_;
    this->type_ = ty;

    val = that.data_;
    that.data_ = this->data_;
    this->data_ = val;
}

//--------------------Private--------------------//

/**
 * @brief Constructs the current Value object from another Value object.
 *
 * This function initializes the current Value object by copying the data
 * and type from the provided Value object. It handles various types such
 * as INTEGER, LONG, FLOAT, STRING, BOOLEAN, NODE, EDGE, PATH, LIST,
 * DATE_TIME, and MAP. For complex types like STRING, PATH, LIST, DATE_TIME,
 * and MAP, it performs deep copies of the necessary elements or members.
 * The function assumes that no string, path, list, or map is already
 * allocated in the current object, so no deallocation is performed.
 *
 * @param other The Value object from which to copy the data and type.
 * @remark 从另一个Value对象构建当前Value对象
 */
void GPStore::Value::ConstructFrom(const GPStore::Value& other){
    // called when no string, path, list, map is allocated.
    // we neednot free them
    type_ = other.type_;
    switch (type_)
    {
        case GPStore::Value::INTEGER:
            data_.Int = other.data_.Int;
            break;
        case GPStore::Value::LONG:
            data_.Long = other.data_.Long;
            break;
        case GPStore::Value::FLOAT:
            data_.Float = other.data_.Float;
            break;
        case GPStore::Value::STRING:
            data_.String = new string(*other.data_.String);
            break;
        case GPStore::Value::BOOLEAN:
            data_.Boolean = other.data_.Boolean;
            break;
        case GPStore::Value::NODE:
            data_.Node = other.data_.Node;
            break;
        case GPStore::Value::EDGE:
            data_.Edge = other.data_.Edge;
            break;
        case GPStore::Value::PATH:
            data_.Path = new PathContent();
            data_.Path->node_id_ = other.data_.Path->node_id_;
            data_.Path->edge_id_ = other.data_.Path->edge_id_;
            break;
        case GPStore::Value::LIST:
            data_.List = new vector<Value *>();
            for(const GPStore::Value *v : *other.data_.List){
                data_.List->push_back(ValueDeepCopy(v));
            }
            break;
        case GPStore::Value::DATE_TIME:
            data_.Datetime = new DateTime();
            data_.Datetime->year = other.data_.Datetime->year;
            data_.Datetime->month = other.data_.Datetime->month;
            data_.Datetime->day = other.data_.Datetime->day;
            data_.Datetime->hour = other.data_.Datetime->hour;
            data_.Datetime->minute = other.data_.Datetime->minute;
            data_.Datetime->second = other.data_.Datetime->second;
            data_.Datetime->millisecond = other.data_.Datetime->millisecond;
            data_.Datetime->microsecond = other.data_.Datetime->microsecond;
            data_.Datetime->nanosecond = other.data_.Datetime->nanosecond;
            data_.Datetime->timezone = other.data_.Datetime->timezone;  
            break; 
        case GPStore::Value::MAP:
            data_.Map = new map<std::string, Value *>();
            for(const auto &p : *other.data_.Map){
                data_.Map->insert(make_pair(p.first, ValueDeepCopy(p.second)));
            }
            break;
        case GPStore::Value::NO_VALUE:
            break;
    }
    return;
}

int GPStore::Value::CompareIntWith(const GPStore::Value &other) const{
    Type ty = other.getType();
    if(ty == INTEGER ){
        if(data_.Int < other.data_.Int)
            return -1;
        else if(data_.Int == other.data_.Int)
            return 0;
        return 1;
    }
    else if(ty == LONG){
        if(data_.Int < other.data_.Long)
            return -1;
        else if(data_.Int == other.data_.Long)
            return 0;
        return 1;
    }
    else if( ty == FLOAT) {
        if(isinf(other.data_.Float) == 1 || isnan(other.data_.Float))
            return -1;
        if(isinf(other.data_.Float) == -1)
            return 1;

        if( (double)data_.Int < other.data_.Float){
            return -1;
        } else if((double)data_.Int == other.data_.Float){
            return 0;
        }
        return 1;

    } else if(ty == NO_VALUE){
        return -1;
    } else {
        return 1;
    }
}

int GPStore::Value::CompareLongWith(const Value &other) const {
    Type ty = other.getType();
    if(ty == INTEGER ){
        if(data_.Long < other.data_.Int)
            return -1;
        else if(data_.Long == other.data_.Int)
            return 0;
        return 1;
    }
    else if(ty == LONG){
        if(data_.Long < other.data_.Long)
            return -1;
        else if(data_.Long == other.data_.Long)
            return 0;
        return 1;
    }
    else if( ty == FLOAT) {
        if(isinf(other.data_.Float) == 1 || isnan(other.data_.Float))
            return -1;
        if(isinf(other.data_.Float) == -1)
            return 1;

        if( (double)data_.Long < other.data_.Float){
            return -1;
        } else if((double)data_.Long == other.data_.Float){
            return 0;
        }
        return 1;

    } else if(ty == NO_VALUE){
        return -1;
    } else {
        return 1;
    }
}

int GPStore::Value::CompareFloatWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == INTEGER || ty == LONG){
        int_64 int_val = ty == INTEGER ? (int_64)other.data_.Int : other.data_.Long;
        if(isinf(data_.Float) == 1 || isnan(data_.Float))
            return 1;
        if(isinf(data_.Float) == -1)
            return -1;
        if( data_.Float < (double)int_val)
            return -1;
        else if( data_.Float == (double)int_val)
            return 0;
        return 1;
    } else if( ty == FLOAT) {
        if(isnan(other.data_.Float)){
            return isnan(data_.Float) ? 0 : -1;
        }
        if(isinf(other.data_.Float) == 1){
            if(isnan(data_.Float)) return  1;
            else if(isinf(data_.Float) == 1) return 0;
            return -1;
        }
        if(isinf(other.data_.Float) == -1) {
            return isinf(data_.Float) == -1 ? 0 : 1;
        }
        if(isnan(data_.Float) || isinf(data_.Float) == 1) return 1;
        if(isinf(data_.Float) == -1)return -1;
        if(data_.Float < other.data_.Float)
            return -1;
        else if(data_.Float == other.data_.Float)
            return 0;
        else return 1;
    } else if(ty == NO_VALUE){
        return -1;
    } else {
        return 1;
    }
}

int GPStore::Value::CompareStringWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == STRING){
        if(*data_.String  == *other.data_.String)
            return 0;
        return *data_.String < *other.data_.String ? -1 : 1;
    } else if (ty == FLOAT || ty == INTEGER || ty == NO_VALUE || ty == BOOLEAN){
        return -1;
    } else {
        return 1;
    }
}

int GPStore::Value::CompareBooleanWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == INTEGER || ty == FLOAT || ty == NO_VALUE) {
        return -1;
    } else if(ty != BOOLEAN){
        return 1;
    } else {
        return data_.Boolean ? (other.data_.Boolean ? 0 : 1)
                             : (other.data_.Boolean ? -1 : 0);
    }
}

int GPStore::Value::CompareNodeWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == MAP){
        return 1;
    } else if(ty == NODE){
        return data_.Node < other.data_.Node ? -1 : (data_.Node == other.data_.Node ? 0 : 1);
    } else {
        return -1;
    }
}

int GPStore::Value::CompareEdgeWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == MAP || ty == NODE){
        return 1;
    } else if(ty == EDGE){
        return data_.Edge < other.data_.Edge ? -1 : ( data_.Edge == other.data_.Edge ? 0 : 1);
    } else {
        return -1;
    }
}

int GPStore::Value::CompareDatetimeWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == MAP || ty == LIST || ty == NODE || ty == EDGE || ty == PATH){
        return 1;
    } else if(ty == DATE_TIME){
        if(other.data_.Datetime->year!=data_.Datetime->year)
            return data_.Datetime->year < other.data_.Datetime->year ? -1 : 1;
        if(other.data_.Datetime->month!=data_.Datetime->month)
            return data_.Datetime->month < other.data_.Datetime->month ? -1 : 1;
        if(other.data_.Datetime->day!=data_.Datetime->day)
            return data_.Datetime->day < other.data_.Datetime->day ? -1 : 1;
        if(other.data_.Datetime->second!=data_.Datetime->second)
            return data_.Datetime->second < other.data_.Datetime->second ? -1 : 1;
        if(other.data_.Datetime->millisecond!=data_.Datetime->millisecond)
            return data_.Datetime->millisecond < other.data_.Datetime->millisecond ? -1 : 1;
        if(other.data_.Datetime->microsecond!=data_.Datetime->microsecond)
            return data_.Datetime->microsecond < other.data_.Datetime->microsecond ? -1 : 1;
        if(other.data_.Datetime->nanosecond!=data_.Datetime->nanosecond)
            return data_.Datetime->nanosecond < other.data_.Datetime->nanosecond ? -1 : 1;
        return 0;
    } else {
        return -1;
    }
}

int GPStore::Value::ComparePathWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == MAP || ty == LIST || ty == NODE || ty == EDGE)
        return 1;
    else if(ty != PATH)
        return -1;
    int m = data_.Path->edge_id_.size(), n = other.data_.Path->edge_id_.size();
    int l = m < n ? m : n;
    for(int i = 0; i < l; ++i){
        if(data_.Path->node_id_[i] != other.data_.Path->node_id_[i])
            return data_.Path->node_id_[i] < other.data_.Path->node_id_[i] ? -1 : 1;
        if(data_.Path->edge_id_[i] != other.data_.Path->edge_id_[i])
            return data_.Path->edge_id_[i] < other.data_.Path->edge_id_[i] ? -1 : 1;
    }
    if(data_.Path->node_id_[l] != other.data_.Path->node_id_[l])
        return data_.Path->node_id_[l] < other.data_.Path->node_id_[l] ? -1 : 1;
    return m == n ? 0 : (m < n ? -1 : 1);
}

int GPStore::Value::CompareListWith(const Value &other) const{
    Type ty = other.getType();
    if(ty == MAP || ty == NODE || ty == EDGE){
        return 1;
    } else if(ty != LIST){
        return -1;
    }
    // ty is LIST
    uint_64 m = data_.List->size(), n = other.data_.List->size();
    uint_64 l = m > n ? n : m;
    for(uint_64 i = 0; i < l; ++i){
        if(*data_.List->at(i) == *other.data_.List->at(i)){
            continue;
        }
        return *data_.List->at(i) < *other.data_.List->at(i) ?  -1 : 1;
    }
    return m == n ? 0 : (m > n ? 1 : -1);
}

int GPStore::Value::CompareMapWith(const Value &other) const{
    Type ty = other.getType();
    if(ty != MAP) return -1;
    // Give my def of map order
    if(data_.Map->size() != other.data_.Map->size()){
        return data_.Map->size() < other.data_.Map->size() ?  -1 : 1;
    }
    std::vector<std::string> this_keys, other_keys;
    for(const auto & p : *(this->data_.Map)){
        this_keys.emplace_back(p.first);
    }
    for(const auto & p : *(other.data_.Map)){
        other_keys.emplace_back(p.first);
    }
    std::sort(this_keys.begin(), this_keys.end());
    std::sort(other_keys.begin(), other_keys.end());
    int n = (int)this_keys.size();
    for(int i = 0; i < n; ++i){
        if(this_keys[i] != other_keys[i])
            return this_keys[i] < other_keys[i] ? -1 : 1;
    }
    for(int i = 0; i < n; ++i){
        int comp_v = data_.Map->at(this_keys[i])->comp(data_.Map->at(other_keys[i])) ;
        if(comp_v != 0) return comp_v;
    }
    return false;
}

int GPStore::Value::CompareNoValueWith(const Value &other) const{
    return other.getType() == NO_VALUE ? 0 : 1;
}


bool GPStore::Value::IntEqualTo(const Value &other) const{
    return other.getType() == INTEGER && data_.Int == other.data_.Int ||
           other.getType() == FLOAT && data_.Int == other.data_.Float ||
           other.getType() == LONG && data_.Int == other.data_.Long;
}

bool GPStore::Value::LongEqualTo(const Value &other) const{
    return other.getType() == INTEGER && data_.Long == other.data_.Int ||
           other.getType() == FLOAT && data_.Long == other.data_.Float ||
           other.getType() == LONG && data_.Long == other.data_.Long;
}

bool GPStore::Value::FloatEqualTo(const Value &other) const{
    return other.getType() == FLOAT && data_.Float == other.data_.Float ||
           other.getType() == INTEGER && data_.Float == other.data_.Int ||
           other.getType() == LONG && data_.Float == other.data_.Long;
}

bool GPStore::Value::StringEqualTo(const Value &other) const{
    return other.getType() == STRING && *data_.String == *other.data_.String;
}

bool GPStore::Value::BooleanEqualTo(const Value &other) const{
    return other.getType() == BOOLEAN && data_.Boolean == other.BOOLEAN;
}

bool GPStore::Value::NodeEqualTo(const Value &other) const{
    return other.getType() == NODE && other.data_.Node == data_.Node;
}

bool GPStore::Value::EdgeEqualTo(const Value &other) const{
    return other.getType() == EDGE && data_.Edge == other.data_.Edge;
}

bool GPStore::Value::DatetimeEqualTo(const Value &other) const{
    if(other.getType() != GPStore::Value::DATE_TIME) return false;
    if((other.data_.Datetime->year!=data_.Datetime->year)||
       (other.data_.Datetime->month!=data_.Datetime->month)||
       (other.data_.Datetime->day!=data_.Datetime->day)||
       (other.data_.Datetime->hour!=data_.Datetime->hour)||
       (other.data_.Datetime->minute!=data_.Datetime->minute)||
       (other.data_.Datetime->second!=data_.Datetime->second)||
       (other.data_.Datetime->millisecond!=data_.Datetime->millisecond)||
       (other.data_.Datetime->microsecond!=data_.Datetime->microsecond)||
       (other.data_.Datetime->nanosecond!=data_.Datetime->nanosecond)||
       (other.data_.Datetime->timezone!=data_.Datetime->timezone))
       return false;
    return true;
}

bool GPStore::Value::PathEqualTo(const Value &other) const{
    if(other.getType() != GPStore::Value::PATH) return false;
    // just check [n0 e1 n1 e2 n2 .. ek nk]
    if(other.data_.Path->edge_id_.size() != data_.Path->edge_id_.size()) return false;
    uint_64 n = data_.Path->edge_id_.size();

    for(int i = 0; i < n; ++i){
        if(other.data_.Path->edge_id_[i] != data_.Path->edge_id_[i] ||
           other.data_.Path->node_id_[i] != data_.Path->node_id_[i]){
            return false;
        }
    }

    return other.data_.Path->node_id_[n] == data_.Path->node_id_[n];
}

bool GPStore::Value::ListEqualTo(const Value &other) const{
    if(other.getType() != GPStore::Value::LIST) return false;

    uint_64 n = data_.List->size();
    if(other.data_.List->size() != n){
        return false;
    }
    for(uint_64 i = 0; i < n; ++i){
        if( !( *other.data_.List->at(i) == *data_.List->at(i) ) ){
            return false;
        }
    }
    return true;
}

bool GPStore::Value::MapEqualTo(const Value &other) const{
    if(other.getType() != GPStore::Value::MAP) return false;

    uint_64 m = data_.Map->size(), n = other.data_.Map->size();
    if(m != n) {
        return false;
    }
    for(const auto &p : *data_.Map){
        const string & k = p.first;
        Value *v = p.second;
        if(other.data_.Map->find(k) == other.data_.Map->end() ||
           !(*other.data_.Map->at(k) == *v) ){
            return false;
        }
    }
    return true;
}

bool GPStore::Value::NoValueEqualTo(const Value &other) const{
    return other.getType() == NO_VALUE;
}

/**
 * @brief Destroys the Value object and releases its resources.
 *
 * This function should be called when a Value object is no longer needed.
 * It will delete all the resources allocated by the Value object and
 * set its type to NO_VALUE.
 *
 * @remark 销毁Value对象
 */
void GPStore::Value::Destruct(){
    switch (type_)
    {
        case Value::STRING:
            delete data_.String;
            break;
        case Value::PATH:
            delete data_.Path;
            break;
        case Value::DATE_TIME:
            delete data_.Datetime;
            break;
        case Value::LIST:
            for(Value *v : *data_.List){
                if(v != nullptr){
                    delete v;
                }
            }
            delete data_.List;
            break;
        case Value::MAP:
            for(const auto &p : *data_.Map){
                if(p.second != nullptr){
                    delete p.second;
                }
            }
            delete data_.Map;
            break;
        default:
            break;
    }
    type_ = NO_VALUE;
    return;
}

