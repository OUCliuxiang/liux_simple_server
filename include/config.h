// 配置模块

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <string>
#include <cstring>
#include <algorithm> // std::transform()
#include <boost/lexical_cast.hpp> // 数值（类型）转换
#include <yaml-cpp/yaml.h>
#include <sstream> // stringstream 字符串流

#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <exception>

#include "log.h"
#include "mutex.h"
#include "util.h"

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
    virtual bool fromString(const std::string& val) = 0;
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
    std::vector<T> operator()(const std::string& val) {
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
        std::stringstream ss;
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
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 和上面类似，完成 set 和YAML String 类型的相互转换
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        std::stringstream ss;
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
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 哈希集合
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        std::stringstream ss;
        typename std::unordered_set<T> st;
        for (size_t i = 0; i < node.size(); i ++) {
            ss.str("");
            ss << node[i];
            // unordered_set 是无序容器，使用 insert 插入元素到集合中
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
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// map 和 unordered_map ，YAML::Node 的 key 需要单独存储，有点不同。
template<class T> 
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        typename std::map<std::string, T> mm;
        std::stringstream ss;
        for (auto it = node.begin(); it != node.end(); it ++) {
            ss.str("");
            // 此处的理解是无法保证 node 中的元素类型是 string，使用字符串流进行格式化转换。
            ss << it -> second;
            // YAML::Node::Scalar() 元素标量化。
            // map 是无序容器，使用 insert 插入。
            mm[it -> first.Scalar()] = LexicalCast<std::string, T>()(ss.str());
        }
        return mm;
    }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& v: val) {
            node[v.first] = YAML::Load(LexicalCast<T, std::string>()(v.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T> 
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        typename std::unordered_map<std::string, T> mm;
        std::stringstream ss;
        for (auto it = node.begin(); it != node.end(); it ++) {
            ss.str("");
            // 此处的理解是无法保证 node 中的元素类型是 string，使用字符串流进行格式化转换。
            ss << it -> second;
            // YAML::Node::Scalar() 元素标量化。
            // map 是无序容器，使用 insert 插入。
            mm[it -> first.Scalar()] = LexicalCast<std::string, T>()(ss.str());
        }
        return mm;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& val) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& v: val) {
            node[v.first] = YAML::Load(LexicalCast<T, std::string>()(v.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


static Logger::ptr rootlogger = LIUX_LOG_ROOT(); 

template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase {
public:
    using RWMutexType = RWMutex;
    using ptr = std::shared_ptr<ConfigVar>;
    using on_change_cb = std::function<void(const T& old_value, const T& new_value)>;
    
    ConfigVar(const std::string &name, const T &default_value, const std::string &description = "")
        : ConfigVarBase(name, description), m_val(default_value) {
    }

    // T 类型参数值转为 YAML String 
    std::string toString() override {
        try {
            // 加读锁，析构自动解锁
            RWMutexType::ReadLock lock(m_mutex);
            return ToStr()(m_val);
        } catch (std::exception &e) {
            LIUX_LOG_ERROR(rootlogger) << "ConfigVar::toString exception "
                                              << e.what() << " convert: " << TypeToName<T>() << " to string"
                                              << " name=" << m_name;
        }
        return "";
    }

    // 从 YAML String 获取参数
    bool fromString(const std::string &val) override {
        try {
            setValue(FromStr()(val));
        } catch (std::exception &e) {
            LIUX_LOG_ERROR(rootlogger) << "ConfigVar::fromString exception "
                                              << e.what() << " convert: string to " << TypeToName<T>()
                                              << " name=" << m_name
                                              << " - " << val;
        }
        return false;
    }

    // 获取当前参数的值
    const T getValue() {
        RWMutexType::ReadLock lock(m_mutex);
        return m_val;
    }

    // 从此往后，难以理解，先复制过来保证项目可以运行，后面根据运行结果慢慢理解。
    void setValue(const T &v) {
        {
            RWMutexType::ReadLock lock(m_mutex);
            if (v == m_val) {
                return;
            }
            // m_cbs 回调函数 unordered_map 回调函数作用：old_value --> new_value
            for (auto &i : m_cbs) {
                i.second(m_val, v);
            }
        }
        RWMutexType::WriteLock lock(m_mutex);
        m_val = v;
    }

    /**
     * @brief 返回参数值的类型名称(typeinfo)
     */
    std::string getTypeName() const override { return TypeToName<T>(); }

    /**
     * @brief 添加变化回调函数
     * @return 返回该回调函数对应的唯一id,用于删除回调
     */
    uint64_t addListener(on_change_cb cb) {
        static uint64_t s_fun_id = 0;
        RWMutexType::WriteLock lock(m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }

    /**
     * @brief 删除回调函数
     * @param[in] key 回调函数的唯一id
     */
    void delListener(uint64_t key) {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.erase(key);
    }

    /**
     * @brief 获取回调函数
     * @param[in] key 回调函数的唯一id
     * @return 如果存在返回对应的回调函数,否则返回nullptr
     */
    on_change_cb getListener(uint64_t key) {
        RWMutexType::ReadLock lock(m_mutex);
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    /**
     * @brief 清理所有的回调函数
     */
    void clearListener() {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.clear();
    }

private:
    RWMutexType m_mutex;
    T m_val;
    //变更回调函数组, uint64_t key,要求唯一，一般可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

/**
 * @brief ConfigVar的管理类
 * @details 提供便捷的方法创建/访问ConfigVar
 */
class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex RWMutexType;

    /**
     * @brief 获取/创建对应参数名的配置参数
     * @param[in] name 配置参数名称
     * @param[in] default_value 参数默认值
     * @param[in] description 参数描述
     * @details 获取参数名为name的配置参数,如果存在直接返回
     *          如果不存在,创建参数配置并用default_value赋值
     * @return 返回对应的配置参数,如果参数名存在但是类型不匹配则返回nullptr
     * @exception 如果参数名包含非法字符[^0-9a-z_.] 抛出异常 std::invalid_argument
     */
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name,
                                             const T &default_value, const std::string &description = "") {
        RWMutexType::WriteLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it != GetDatas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (tmp) {
                LIUX_LOG_INFO(rootlogger) << "Lookup name=" << name << " exists";
                return tmp;
            } else {
                LIUX_LOG_ERROR(rootlogger) << "Lookup name=" << name << " exists but type not "
                                                  << TypeToName<T>() << " real_type=" << it->second->getTypeName()
                                                  << " " << it->second->toString();
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
            LIUX_LOG_ERROR(rootlogger) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas()[name] = v;
        return v;
    }

    /**
     * @brief 查找配置参数
     * @param[in] name 配置参数名称
     * @return 返回配置参数名为name的配置参数
     */
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name) {
        RWMutexType::ReadLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    /**
     * @brief 使用YAML::Node初始化配置模块
     */
    static void LoadFromYaml(const YAML::Node &root);

    /**
     * @brief 加载path文件夹里面的配置文件
     */
    static void LoadFromConfDir(const std::string &path, bool force = false);

    /**
     * @brief 查找配置参数,返回配置参数的基类
     * @param[in] name 配置参数名称
     */
    static ConfigVarBase::ptr LookupBase(const std::string &name);

    /**
     * @brief 遍历配置模块里面所有配置项
     * @param[in] cb 配置项回调函数
     */
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);

private:
    /**
     * @brief 返回所有的配置项
     */
    static ConfigVarMap &GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }

    /**
     * @brief 配置项的RWMutex
     */
    static RWMutexType &GetMutex() {
        static RWMutexType s_mutex;
        return s_mutex;
    }
};


} // end namespace liux

#endif // __CONFIG_H__