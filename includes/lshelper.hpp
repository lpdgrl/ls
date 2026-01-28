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
#include <bitset>

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
    
    struct FileInfo {
        char fi_type_file;
        std::uint32_t fi_nlink_count;
        std::uint64_t fi_size;
        std::string fi_name;
        std::string fi_date_last_mod;
        std::string fi_perms;
        std::string fi_gid;
        std::string fi_uid;
    };

    std::string PermsToString(ModeFile mode);
    char FileTypesToString(ModeFile mode);
    std::string DateLastModToString(TimeSpec ts);

    void DumpPerms(std::ostream& out, ModeFile mode);
    void DumpTypeFiles(std::ostream& out, ModeFile mode);
    void DumpDate(std::ostream& out, TimeSpec timespec);    

    bool CheckErrno(std::ostream& out, const std::string& method_call);
    void ResetErrno();

    bool operator|(Option_t option, OptionByte option_byte);
    bool operator&(Option_t option, OptionByte option_byte);

    uint8_t OptionByteToUint8(OptionByte option_byte);
}