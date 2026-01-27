#include "../includes/ls.hpp" 

namespace ls {
    Directory::Directory(const std::string& path)
        : path_(path)
        , dir_(opendir(path.c_str()), &closedir)
    { 
        if (!dir_) {
            throw std::system_error(errno, std::generic_category(), "ls: cannot access '" + path + "'");
        }
    }


    Ls::Ls(const std::string& path, detail::Options options)
        : options_display_(options)
        , directory_(path.c_str()) 
    {}


    void Ls::Parse() {
        // DIR* dir = opendir(path_.c_str());

        // if (!dir) {
        //     return entries;
        // }

        // struct dirent* entry;
        // while ((entry = readdir(dir)) != nullptr) {
        //     std::string name(entry->d_name);
        //     if (name != "." &&  name != ".." && name[0] != '.') {
        //         entries.push_back(entry->d_name);
        //         max = std::max(name.size(), max);
        //     }

        // std::cout << "Max: " << max << '\n';

        // closedir(dir);
    }

    void Ls::Display() const noexcept {

    }
}