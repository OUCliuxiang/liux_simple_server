// 配置模块理解困难，先复制过来保证后续程序可以运行，随后根据运行结果慢慢理解。

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace liux {

static liux::Logger::ptr g_logger = LIUX_LOG_NAME("system");

ConfigVarBase::ptr Config::LookupBase(const std::string &name) {
    RWMutexType::ReadLock lock(GetMutex());
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}

//"A.B", 10
//A:
//  B: 10
//  C: str

static void ListAllMember(const std::string &prefix,
                          const YAML::Node &node,
                          std::list<std::pair<std::string, const YAML::Node>> &output) {
    if (prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos) {
        LIUX_LOG_ERROR(g_logger) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    output.push_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
        for (auto it = node.begin();
             it != node.end(); ++it) {
            ListAllMember(prefix.empty() ? it->first.Scalar()
                                         : prefix + "." + it->first.Scalar(),
                          it->second, output);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node &root) {
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);

    for (auto &i : all_nodes) {
        std::string key = i.first;
        if (key.empty()) {
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);

        if (var) {
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

// /// 记录每个文件的修改时间
// static std::map<std::string, uint64_t> s_file2modifytime;
// /// 是否强制加载配置文件，非强制加载的情况下，如果记录的文件修改时间未变化，则跳过该文件的加载
// static liux::Mutex s_mutex;

// void Config::LoadFromConfDir(const std::string &path, bool force) {
//     std::string absoulte_path = liux::EnvMgr::GetInstance()->getAbsolutePath(path);
//     std::vector<std::string> files;
//     FSUtil::ListAllFile(files, absoulte_path, ".yml");

//     for (auto &i : files) {
//         {
//             struct stat st;
//             lstat(i.c_str(), &st);
//             liux::Mutex::Lock lock(s_mutex);
//             if (!force && s_file2modifytime[i] == (uint64_t)st.st_mtime) {
//                 continue;
//             }
//             s_file2modifytime[i] = st.st_mtime;
//         }
//         try {
//             YAML::Node root = YAML::LoadFile(i);
//             LoadFromYaml(root);
//             LIUX_LOG_INFO(g_logger) << "LoadConfFile file="
//                                      << i << " ok";
//         } catch (...) {
//             LIUX_LOG_ERROR(g_logger) << "LoadConfFile file="
//                                       << i << " failed";
//         }
//     }
// }

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
    RWMutexType::ReadLock lock(GetMutex());
    ConfigVarMap &m = GetDatas();
    for (auto it = m.begin();
         it != m.end(); ++it) {
        cb(it->second);
    }
}

} // end namespace liux 