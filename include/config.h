// 配置模块

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <string>
#include <algorithm> // std::transform()
#include <boost/lexical_cast.hpp> // 数值（类型）转换
#include <sstream> // stringstream 字符串流

#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

namespace liux {

// 配置变量基类，存放公用属性    
class ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVarBase>;
    ConfigVarBase(const std::string& name, const std::string& description = ""):
        m_name(name), m_description(description) {
        // char uppper to lower, included in <algorithm>
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    // 虚基类，保证基类指针指向子类对象析构时可以调用释放子类对象资源
    virtual ~ConfigVarBase() {}

    const std::string& getName() const {return m_name;}
    const std::string& getDescription() const {return m_description;} 
    // 纯虚不可实现，子类根据自己的需求进行重写。
    // 纯虚函数，该类是抽象类，不可实例化。
    virtual std::string toString() = 0;
    // 从字符串初始化配置，纯虚，子类按照自己需求重写实现。     
    virtual bool formString(const std::string& val) = 0;
    // 返回配置参数的类型名   
    virtual std::string& getTypeName() const = 0; 

protected:
    std::string m_name;
    std::string m_description;
};

// 类型转换模板类， F 源类型， T 目标类型
// 重载 () 运算符对 boost::lexical_cast 进行封装。
template<class F, class T> 
class LexicalCast {
public:
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

// 从 YAML String 转换为 vector<T> 的类型转换模板类     
// 重载 () 运算符对 LexicalCast 进行再封装
template<class T> 
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator()(const string& val) {
        YAML::Node node = YAML::Load(val);
        std::stringstream ss; // 字符串流接收数据     
        // vector 容器元素类型在编译时不可知，要等到程序运行传入参数才可知道，所以用 typename 
        typename std::vector<T> vec;    
        for (size_t i = 0; i < node.size(); ++ i) {
            // sstream 调用 .str() 方法会将字符串流转储给一个字符串，同时清空自身流
            // 从而不给出接收字符串，调用 .str("") 可以用来清空 sstream 
            ss.str("");
            ss << node[i];
            // 我不理解，这个重载不是静态的啊，怎么能直接用？    
            // 而且运算符重载不可以静态。
            // 莫不是 LexicalCast<...>() 这一步实现了对象实例化？     
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};

// 上一个模板类的逆，从 vector<T> 转换成 YAML String，重载 () 对 LexicalCast 再封装
template<class T> 
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& v: val) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(v)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 和上面类似，通过重载 () 再封装分别从 LexicalCast 特化出两个 YAML String 和 std::list 相互转换的特例
template<class T> 
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator()(const std::string& val) {
        typename std::list<T> lst;
        stringstream ss;
        YAML::Node node = YAML::Load(val);
        for (size_t i = 0; i < node.size(); i ++) {
            ss.str("");
            ss << node[i];
            lst.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return lst;
    }
};

template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &v: val) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(v)));
        }
        stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 和上面类似，完成 set 和YAML String 类型的相互转换
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator()(const string& val) {
        YAML::Node node = YAML::Load(val);
        stringstream ss;
        typename std::set<T> st;
        for (size_t i = 0; i < node.size(); i ++) {
            ss.str("");
            ss << node[i];
            // set 是无序容器，使用 insert 插入元素到集合中
            st.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return st;
    }
};

template<class T> 
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &v: val) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(v)));
        }
        stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 哈希集合
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator()(const string& val) {
        YAML::Node node = YAML::Load(val);
        stringstream ss;
        typename std::unordered_set<T> st;
        for (size_t i = 0; i < node.size(); i ++) {
            ss.str("");
            ss << node[i];
            // set 是无序容器，使用 insert 插入元素到集合中
            st.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return st;
    }
};

template<class T> 
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &v: val) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(v)));
        }
        stringstream ss;
        ss << node;
        return ss.str();
    }
};

// map 和 哈希 map ，YAML::Node 的 key 需要单独存储，有点不同。




} // end namespace liux

#endif // __CONFIG_H__