#include "../includes/lshelper.hpp"

namespace ls::detail {
    void DumpPerms(std::ostream& out, ModeFile mode) {
        out << ((mode & S_IRUSR) ? 'r' : '-')
            << ((mode & S_IWUSR) ? 'w' : '-')
            << ((mode & S_IXUSR) ? 'x' : '-')
            << ((mode & S_IRGRP) ? 'r' : '-')
            << ((mode & S_IWGRP) ? 'w' : '-')
            << ((mode & S_IXGRP) ? 'x' : '-')
            << ((mode & S_IROTH) ? 'r' : '-')
            << ((mode & S_IWOTH) ? 'w' : '-')
            << ((mode & S_IXOTH) ? 'x' : '-');
    }

    void DumpTypeFiles(std::ostream& out, ModeFile type) {
        out << ((S_ISDIR(type) ? 'd' : S_ISBLK(type) ? 'b' 
                    : S_ISCHR(type) ? 'c' : S_ISLNK(type) ? 'l' 
                        : S_ISREG(type) ? '-' : '\0'));
    }

    void DumpDate(std::ostream& out, TimeSpec ts) {
        namespace krn = std::chrono;

        auto total_ns = krn::nanoseconds(ts.tv_sec * 1000000000LL + ts.tv_nsec);

        auto tp = krn::time_point<krn::system_clock>(krn::duration_cast<krn::system_clock::duration>(total_ns));

        std::time_t tt = krn::system_clock::to_time_t(tp);
        std::tm* tm = std::localtime(&tt);

        out << std::put_time(tm, "%b %a %d %H:%M");
    }
}