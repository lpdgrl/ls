#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#include <cstring>
#include <cerrno>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cstdint>
#include <chrono>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>

namespace Ls::detail {
    using Option_t = uint8_t;
    using ModeFile = mode_t;
    using TimeSpec = timespec;
    using Gid = gid_t;
    using Uid = uid_t;
    using Nlink = nlink_t;
    using DirCloser = int (*)(DIR*);
    using Stat = struct stat;

    enum class OptionByte : uint8_t {
        LongFormat      = 0b10000000,
        ReverseFormat   = 0b00010000,
        HumanFormat     = 0b00000010,
        SimpleFormat    = 0b00000001,
    };

    enum class TypeDate {
        Day = 0,
        Month = 1,
        Year  = 2,
        AllFormat = 3,
    };

    struct ColorFile {
        static constexpr const char* BLUE  = "\033[34m";
        static constexpr const char* MAGNETA   = "\033[35m";
        static constexpr const char* RESET = "\033[0m";
    };
    
    struct FileInfo {
        char fi_type_file;
        bool is_directory = false;
        bool is_symbol_link = false;
        std::string fi_nlink_count;
        std::string fi_size;
        std::string fi_name;
        std::string fi_date_lm_day;
        std::string fi_date_lm_month;
        std::string fi_date_lm_year;
        std::string fi_perms;
        std::string fi_gid;
        std::string fi_uid;
    };

    char FileTypesToString(ModeFile mode);

    std::string PermsToString(ModeFile mode);
    std::string DateLastModToString(TimeSpec ts, TypeDate td);
    std::string DateFormat(const std::chrono::nanoseconds& ns_sice_epoch,
                           const std::chrono::nanoseconds& total_ns, TypeDate td);

    void DumpPerms(std::ostream& out, ModeFile mode);
    void DumpTypeFiles(std::ostream& out, ModeFile mode);
    void DumpDate(std::ostream& out, TimeSpec timespec, TypeDate td);    

    bool CheckErrno(std::ostream& out, const std::string& method_call);
    void ResetErrno();

    bool operator|(Option_t option, OptionByte option_byte);
    bool operator&(Option_t option, OptionByte option_byte);

    uint8_t OptionByteToUint8(OptionByte option_byte);
}