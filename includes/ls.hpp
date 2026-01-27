#pragma once

#include "lshelper.hpp"

namespace ls {
    class Directory {
    public:
        Directory() = delete;
        explicit Directory(const std::string& path);

        ~Directory() = default;


    private:
        size_t max_length_entry_ = 0;
        std::string path_;
        std::vector<detail::FileInfo> entries_;
        std::unique_ptr<DIR, detail::DirCloser> dir_;
    };

    class Ls {
    public:
        Ls() = delete;
        Ls(const std::string& path, detail::Options option);

        Ls(const Ls& other) = delete;
        Ls(Ls&& other) = delete;
        Ls& operator=(const Ls& other) = delete;
        Ls& operator=(Ls&& other) = delete;

        void Parse(); 
        void Display() const noexcept;

        ~Ls() = default;

    private:
        detail::Options options_display_;
        Directory directory_;
    };
}

