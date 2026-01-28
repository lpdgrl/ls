#include "../includes/ls.hpp" 

namespace Ls {
    Directory::Directory(const std::string& path)
        : path_(NormalizePath(path))
        , dir_(opendir(path.c_str()), &closedir)
    { 
        if (!dir_) {
            throw std::system_error(errno, std::generic_category(), "ls: cannot access '" + path + "'");
        }
    }

    void Directory::ParseDir() {
        dirent* entry;
        
        // Clear errno if we want check error after readdir
        errno = 0;
        while ((entry = readdir(dir_.get())) != nullptr) {
            detail::FileInfo file_info;
            file_info.fi_name = entry->d_name;
            if (file_info.fi_name != "." &&  file_info.fi_name != ".." && file_info.fi_name[0] != '.') {
                entries_.emplace_back(file_info);
                max_length_entry_ = std::max(file_info.fi_name.size(), max_length_entry_);
            }
        }
        std::cout << path_ << '\n';
        [[maybe_unused]] bool res = detail::CheckErrno(std::cerr, "ParseDir, call readdir:");

        SortEntries();
    }

    void Directory::ParseEntryAttribute(bool human_output) {
        for (auto& entry : entries_) {
            std::string new_path(path_);
            new_path.append(entry.fi_name);
            
            detail::Stat stat_file_info;
            detail::ResetErrno();

            int stat_err = stat(new_path.c_str(), &stat_file_info);

            if (stat_err != 0) {
                detail::CheckErrno(std::cerr, "ParseEntryAttribute, call stat:"); 
                // return;
            }

            FillingFileInfo(entry, stat_file_info);

            if (entry.fi_type_file == 'd') {
                entry.fi_name += '/';
            }
            
            if (human_output) {
                entry.fi_size /= 1024;
            }
        }
    }

    void Directory::FillingFileInfo(detail::FileInfo& file_info, detail::Stat& stat_file_info) {
        file_info.fi_size           = stat_file_info.st_size; 
        file_info.fi_nlink_count    = stat_file_info.st_nlink;
        file_info.fi_type_file      = detail::FileTypesToString(stat_file_info.st_mode);
        file_info.fi_perms          = detail::PermsToString(stat_file_info.st_mode);
        file_info.fi_date_last_mod  = detail::DateLastModToString(stat_file_info.st_mtim);
        file_info.fi_gid            = SearchGroupNameByGid(stat_file_info.st_gid);
        file_info.fi_uid            = SearchUserNameByUid(stat_file_info.st_uid);
    }

    void Directory::SetSortCallback(std::function<bool(const detail::FileInfo& lhs, 
                                    const detail::FileInfo& rhs)> callback) noexcept 
    {
        callback_sort_ = callback;
    }

    const Directory::Entries& Directory::GetEntries() const noexcept {
        return entries_;
    }

    size_t Directory::GetMaxLengthEntry() const noexcept {
        return max_length_entry_;
    }

    std::string Directory::NormalizePath(const std::string& path) noexcept {
        if (path.empty()) {
            return path;
        }

        std::string result;
        result.append(path);

        if (path.find('/', path.size() - 1) == std::string::npos) {
            result.append("/");
        }

        return result;
    }

    std::string Directory::SearchUserNameByUid(detail::Uid uid) {
        detail::ResetErrno();

        std::string result;
        passwd* pwd;

        pwd = getpwuid(uid);

        if (pwd == nullptr) {
            detail::CheckErrno(std::cerr, "SearchUsernameByUid call getpwuid:");
            return {};
        }

        result = pwd->pw_name;
        return result;
    }

    std::string Directory::SearchGroupNameByGid(detail::Gid gid) {
        detail::ResetErrno();

        std::string result;
        group* grp;

        grp = getgrgid(gid);

        if (grp == nullptr) {
            detail::CheckErrno(std::cerr, "SearchGroupNameByGid call getgrgid:");
            return {};
        }

        result = grp->gr_name;
        return result;
    }

    void Directory::SortEntries() {
        std::sort(entries_.begin(), entries_.end(), callback_sort_);
    }

    Ls::Ls(const std::string& path, detail::Option_t options)
        : format_options_(options)
        , directory_(path.c_str()) 
    {}


    void Ls::Parse() {
        if (format_options_ & detail::OptionByte::ReverseFormat) {
            directory_.SetSortCallback([] (const detail::FileInfo& lhs, const detail::FileInfo& rhs) -> bool {
                return lhs.fi_name > rhs.fi_name; }
            );
        } else {
            directory_.SetSortCallback([] (const detail::FileInfo& lhs, const detail::FileInfo& rhs) -> bool {
                return lhs.fi_name < rhs.fi_name; }
            );
        }

        directory_.ParseDir();

        if (format_options_ & detail::OptionByte::LongFormat) {
            bool human_output = format_options_ & detail::OptionByte::HumanFormat;
            directory_.ParseEntryAttribute(human_output);
        }
    }

    void Ls::Display() const noexcept {
        if (format_options_ & detail::OptionByte::LongFormat) {
            LongFormatDisplay();
        }
        else if (format_options_ & detail::OptionByte::SimpleFormat) {
            SimpeDisplay();
        }
    }

    void Ls::LongFormatDisplay() const {
        const Directory::Entries& entries = directory_.GetEntries();

        for (const auto& entry : entries) {
            std::cout 
                << entry.fi_type_file   
                << entry.fi_perms 
                << ' ' << entry.fi_nlink_count 
                << ' ' << entry.fi_uid
                << ' ' << entry.fi_gid
                << ' ' << entry.fi_size
                << ' ' << entry.fi_date_last_mod
                << ' ' << entry.fi_name;
                std::cout << '\n';
        }
    }

    void Ls::SimpeDisplay() const noexcept {
        winsize w;

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        std::cout << "lines: " << w.ws_row << " columns: " << w.ws_col << '\n';

        const Directory::Entries& entries = directory_.GetEntries();

        size_t num_entries = entries.size();
        size_t max_length_enry = directory_.GetMaxLengthEntry();

        size_t n_column = w.ws_col / num_entries;
        if (w.ws_col % num_entries) {
            ++n_column;
        }
        
        size_t n_row_in_column = num_entries / n_column;
        if (num_entries % n_column) {
            ++n_row_in_column;
        }

        for (size_t row = 0; row < n_row_in_column; ++row) {
            for (size_t base = row, col = 0; ;) {
                const auto& entry = entries[base];
                size_t num_space = 2;
                if (entry.fi_name.size() >= max_length_enry) {
                    num_space = 2;
                } 
                std::cout << entry.fi_name; 
                size_t i = 0;
                while (i++ < num_space) {
                    putchar(' ');
                }
                if ((base += n_row_in_column) >= num_entries) {
                    break;
                }
                if (col++ == n_column) {
                    break;
                }
            }
            std::cout << '\n';
        }
    }
}