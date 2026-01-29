#pragma once

#include "lshelper.hpp"

namespace Ls {
    struct MaxLength {
        size_t max_n_link         = 0;
        size_t max_size           = 0;
        size_t max_date_day       = 0;
        size_t max_date_month     = 0;
        size_t max_date_year     = 0;
        size_t max_name           = 0;
    };

    class Directory final {
    public:
        using Entries = std::vector<detail::FileInfo>;

        Directory() = delete;
        explicit Directory(const std::string& path);

        ~Directory() = default;

        void ParseDir(); 
        void ParseEntryAttribute(bool human_output);

        const Entries& GetEntries() const noexcept;
        size_t GetMaxLengthEntry() const noexcept;
        MaxLength GetMaxLengthEntries() const noexcept;

        void SetSortCallback(std::function<bool(const detail::FileInfo& lhs, 
                            const detail::FileInfo& rhs)> callback) noexcept;

    private:
        void MaxElements(detail::FileInfo& file_info);
        std::string NormalizePath(const std::string& path) noexcept;
        std::string SearchUserNameByUid(detail::Uid uid);
        std::string SearchGroupNameByGid(detail::Gid gid);
        std::string HumanReadableSize(uint64_t size);

        void FillingFileInfo(detail::FileInfo& file_info, detail::Stat& stat_info);

        void SortEntries();

    private:
        MaxLength max_length_entries_;
        std::string path_;

        Entries entries_;
        std::unique_ptr<DIR, detail::DirCloser> dir_;

        std::function<bool(const detail::FileInfo& lhs, const detail::FileInfo& rhs)> callback_sort_;
    };

    class Ls final {
    public:
        Ls() = delete;
        Ls(std::ostream& out, const std::string& path, detail::Option_t option);

        Ls(const Ls& other) = delete;
        Ls(Ls&& other) = delete;
        Ls& operator=(const Ls& other) = delete;
        Ls& operator=(Ls&& other) = delete;

        void Parse(); 
        void Display() const noexcept;

        ~Ls() = default;

    private:
        detail::Option_t format_options_;
        Directory directory_;
        std::ostream& out_;

        void SimpeDisplay() const noexcept;
        void LongFormatDisplay() const noexcept;
        void AddPaddingToOut(size_t num_padding) const noexcept;
    };
}

