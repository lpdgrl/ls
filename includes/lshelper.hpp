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

#include <iostream>

namespace ls::detail {
    using Options = uint8_t;
    using ModeFile = mode_t;
    using TimeSpec = timespec;
    using Gid = gid_t;
    using Uid = uid_t;
    using Nlink = nlink_t;

    using DirCloser = int (*)(DIR*);

    struct FileInfo {
        std::string fi_name;
        std::string fi_date_last_mod;
        std::uint64_t fi_size;
        ModeFile fi_type_or_mode;
        Gid fi_gid;
        Uid fi_uid;
        Nlink fi_nlink_count;
    };

    void DumpPerms(std::ostream& out, ModeFile mode);
    void DumpTypeFiles(std::ostream& out, ModeFile mode);
    void DumpDate(std::ostream& out, TimeSpec timespec);
}