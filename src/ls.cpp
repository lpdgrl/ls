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
            }
            
        }
        // bool res = detail::CheckErrno(std::cerr, "ParseDir, call readdir:");
        SortEntries();
    }

    void Directory::ParseEntryAttribute(bool human_output) {
        for (auto& entry : entries_) {
            std::string new_path(path_);
            new_path.append(entry.fi_name);
            
            detail::Stat stat_file_info;
            detail::ResetErrno();

            int stat_err = lstat(new_path.c_str(), &stat_file_info);

            if (stat_err != 0) {
                bool res = detail::CheckErrno(std::cerr, "ParseEntryAttribute, call stat:"); 
                // return;
            }

            FillingFileInfo(entry, stat_file_info);

            if (entry.fi_type_file == 'd') {
                entry.is_directory = true;
            }

            if (entry.fi_type_file == 'l') {
                entry.is_symbol_link = true;
            }
            
            entry.fi_size = (human_output) ? HumanReadableSize(stat_file_info.st_size) 
                    : std::to_string(stat_file_info.st_size);

            MaxElements(entry);

        }
    }

    std::string Directory::HumanReadableSize(uint64_t size) {
        std::string result;
        std::string sym;

        // Number bits in Byte
        static uint64_t size_byte = 8;

        // Number bytes in KB, MB, GB
        static uint64_t size_kb = 1024;
        static uint64_t size_mb = 1048576;  
        static uint64_t size_gb = 1073741824;

        static const char sym_K = 'K';
        static const char sym_M = 'M';
        static const char sym_G = 'G';
        
        // Because division two number in C++ is truncating fractional part so we have use 
        // formula a = a / b + (a % b != 0) 
        // from https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
        // Standard idiom "int a = (a + b - 1) / b" 
        // but if used less size of type we have to can get overflow when addition two number  - (INT_MAX + b - 1) / b
        if (size < size_mb) {
            size = size / size_kb + (size % size_kb != 0);
            sym += sym_K;
        } else if (size > size_mb && size < size_gb) {
            size = size / size_mb + (size % size_mb != 0);
            sym = sym_M;
        } else if (size >= size_gb) {
            size = size / size_gb + (size % size_gb != 0);
            sym += sym_G;
        }

        result = std::to_string(size) + ((size < 10) ? (".0" + sym) : sym);
        return result;
    }

    
    void Directory::MaxElements(detail::FileInfo& file_info) {
        max_length_entries_.max_size        = std::max(max_length_entries_.max_size, file_info.fi_size.size());
        max_length_entries_.max_n_link      = std::max(max_length_entries_.max_n_link, file_info.fi_nlink_count.size());
        max_length_entries_.max_date_day    = std::max(max_length_entries_.max_date_day, file_info.fi_date_lm_day.size());
        max_length_entries_.max_date_month  = std::max(max_length_entries_.max_date_month, file_info.fi_date_lm_month.size());
        max_length_entries_.max_date_year   = std::max(max_length_entries_.max_date_year, file_info.fi_date_lm_year.size());
        max_length_entries_.max_name        = std::max(max_length_entries_.max_name, file_info.fi_name.size());
    }

    void Directory::FillingFileInfo(detail::FileInfo& file_info, detail::Stat& stat_file_info) {
        file_info.fi_nlink_count    = std::to_string(stat_file_info.st_nlink);
        file_info.fi_type_file      = detail::FileTypesToString(stat_file_info.st_mode);
        file_info.fi_perms          = detail::PermsToString(stat_file_info.st_mode);
        file_info.fi_date_lm_day    = detail::DateLastModToString(stat_file_info.st_mtim, detail::TypeDate::Day);
        file_info.fi_date_lm_month  = detail::DateLastModToString(stat_file_info.st_mtim, detail::TypeDate::Month);
        file_info.fi_date_lm_year   = detail::DateLastModToString(stat_file_info.st_mtim, detail::TypeDate::Year);
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
        return max_length_entries_.max_name;
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
            bool res = detail::CheckErrno(std::cerr, "SearchUsernameByUid call getpwuid:");
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
            bool res = detail::CheckErrno(std::cerr, "SearchGroupNameByGid call getgrgid:");
            return {};
        }

        result = grp->gr_name;
        return result;
    }

    void Directory::SortEntries() {
        std::sort(entries_.begin(), entries_.end(), callback_sort_);
    }
    
    MaxLength Directory::GetMaxLengthEntries() const noexcept {
        return max_length_entries_;
    }

    Ls::Ls(std::ostream& out, const std::string& path, detail::Option_t options)
        : out_(out)
        , format_options_(options)
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

        bool human_output = format_options_ & detail::OptionByte::HumanFormat;
        directory_.ParseEntryAttribute(human_output);
    }

    void Ls::Display() const noexcept {
        if (format_options_ & detail::OptionByte::LongFormat) {
            LongFormatDisplay();
        }
        else if (format_options_ & detail::OptionByte::SimpleFormat) {
            SimpeDisplay();
        }
    }

    void Ls::LongFormatDisplay() const noexcept {
        auto max_lengths = directory_.GetMaxLengthEntries();
        const Directory::Entries& entries = directory_.GetEntries();

        for (const auto& entry : entries) {
            out_ << entry.fi_type_file << entry.fi_perms;

            if (entry.fi_nlink_count.size() < max_lengths.max_n_link) {
                size_t pad = max_lengths.max_n_link - entry.fi_nlink_count.size();
                AddPaddingToOut(pad);
                out_ << entry.fi_nlink_count;
            } else {
                out_ << ' ' << entry.fi_nlink_count;
            }
            
            out_ << ' ' << entry.fi_uid;
            out_ << ' ' << entry.fi_gid; 

            if (entry.fi_size.size() < max_lengths.max_size) {
                size_t pad = max_lengths.max_size - entry.fi_size.size();
                AddPaddingToOut(pad);
                out_ << entry.fi_size;
            } else {
                out_ << ' ' << entry.fi_size;
            }

            if (entry.fi_date_lm_month.size() < max_lengths.max_date_month) {
                size_t pad = max_lengths.max_date_month - entry.fi_date_lm_month.size();
                AddPaddingToOut(pad);
                out_ << entry.fi_date_lm_month;
            } else {
                out_ << ' ' << entry.fi_date_lm_month;
            }

            if (entry.fi_date_lm_day.size() < max_lengths.max_date_day) {
                size_t pad = max_lengths.max_date_day - entry.fi_date_lm_day.size();
                AddPaddingToOut(pad);
                out_ << entry.fi_date_lm_day;
            } else {
                out_ << ' ' << entry.fi_date_lm_day;
            }

            if (entry.fi_date_lm_year.size() < max_lengths.max_date_year) {
                size_t pad = max_lengths.max_date_year - entry.fi_date_lm_year.size();
                AddPaddingToOut(pad);
                out_ << entry.fi_date_lm_year;
            } else {
                out_ << ' ' << entry.fi_date_lm_year;
            }
            
            if (entry.is_directory) {
                out_ << detail::ColorFile::BLUE;
            }

            if (entry.is_symbol_link) {
                out_ << detail::ColorFile::MAGNETA;
            }

            out_ << ' ' << entry.fi_name;
            out_ << detail::ColorFile::RESET;
            out_ << '\n';
        }
    }

    void Ls::AddPaddingToOut(size_t num_padding) const noexcept {
        for (size_t i = 0; i <= num_padding; ++i) {
            out_ << ' ';
        }
    }

    void Ls::SimpeDisplay() const noexcept {
        //This piece of code made me really hurt. And then I didn't come close to hitting the exact output like ls. 
        // I'm going to study the ls sources. :)

        winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        const Directory::Entries& entries = directory_.GetEntries();
        size_t num_entries = entries.size();
        size_t max_length_entry = directory_.GetMaxLengthEntry();
        size_t k_padding = 2;

        size_t column_width = max_length_entry + k_padding;

        size_t n_columns = w.ws_col / column_width;
        if (n_columns == 0) {
            n_columns = 1;
        }

        size_t n_rows = (num_entries + n_columns - 1) / n_columns;

        for (size_t row = 0; row < n_rows; ++row) {
            for (size_t col = 0; col < n_columns; ++col) {
                size_t idx = col * n_rows + row;
                
                if (idx >= num_entries) {
                    continue;
                }
                
                const auto& entry = entries[idx];

                if (entry.is_directory) {
                    out_ << detail::ColorFile::BLUE;
                }

                if (entry.is_symbol_link) {
                    out_ << detail::ColorFile::MAGNETA;
                }

                out_ << entry.fi_name;
                out_ << detail::ColorFile::RESET;
                
                size_t pad = column_width - entry.fi_name.size();
                for (size_t i = 0; i < pad; ++i) {
                    out_ << ' ';
                }
            }
            out_ << '\n';
        }
    }
}