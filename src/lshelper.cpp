#include "../includes/lshelper.hpp"

namespace Ls::detail {

    std::string DateLastModToString(TimeSpec ts) {
        namespace krn = std::chrono;

        std::tm t = {};
        t.tm_year = 2025 - 1900; // Years since 1900
        t.tm_mon = 7 - 1;   // Months since January (0-11)
        t.tm_mday = 1;        // Day of the month (1-31)
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        t.tm_isdst = -1;

        std::time_t time_since_epoch_local = std::mktime(&t);

        if (time_since_epoch_local == -1) {
            std::cerr << "Error converting time." << std::endl;
            return {};
        }

        auto time_point = std::chrono::system_clock::from_time_t(time_since_epoch_local);
        auto duration_since_epoch = time_point.time_since_epoch();
        auto nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(duration_since_epoch);

        auto total_ns = krn::nanoseconds(ts.tv_sec * 1000000000LL + ts.tv_nsec);

        auto tp = krn::time_point<krn::system_clock>(krn::duration_cast<krn::system_clock::duration>(total_ns));

        std::time_t tt = krn::system_clock::to_time_t(tp);
        std::tm* tm = std::localtime(&tt);

        std::stringstream ss;
        std::string format_date = (nanoseconds_since_epoch > total_ns) ? "%b %e %Y" : "%b %e %H:%M";

        ss << std::put_time(tm, format_date.c_str());
    
        return ss.str();
    }

    std::string PermsToString(ModeFile mode) {
        std::string result;

        result += ((mode & S_IRUSR) ? 'r' : '-');
        result += ((mode & S_IWUSR) ? 'w' : '-');
        result += ((mode & S_IXUSR) ? 'x' : '-');
        result += ((mode & S_IRGRP) ? 'r' : '-');
        result += ((mode & S_IWGRP) ? 'w' : '-');
        result += ((mode & S_IXGRP) ? 'x' : '-');
        result += ((mode & S_IROTH) ? 'r' : '-');
        result += ((mode & S_IWOTH) ? 'w' : '-');
        result += ((mode & S_IXOTH) ? 'x' : '-');

        return result;
    }

    char FileTypesToString(ModeFile mode) {
        char result;

        result = (S_ISDIR(mode) ? 'd' : S_ISBLK(mode) ? 'b' 
                    : S_ISCHR(mode) ? 'c' : S_ISLNK(mode) ? 'l' 
                        : S_ISREG(mode) ? '-' : '\0');

        return result;
    }

    void DumpPerms(std::ostream& out, ModeFile mode) {
        out << PermsToString(mode);
    }

    void DumpTypeFiles(std::ostream& out, ModeFile type) {
        out << FileTypesToString(type);
    }

    void DumpDate(std::ostream& out, TimeSpec ts) {
        out << DateLastModToString(ts);
    }

    bool CheckErrno(std::ostream& out, const std::string& method_call) {
        if (errno == EBADF) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        } else if (errno == EACCES) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        } else if (errno = EBADF) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        } else if (errno = EIO) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        } else if (errno = EINTR) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        } else if (errno = EMFILE) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        } else if (errno = ENFILE) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        } else if (errno = ERANGE) {
            out << method_call << ' ' << std::strerror(errno) << '\n';
            return true;
        }
        
        return false;
    }

    void ResetErrno() {
        errno = 0;
    }

    uint8_t OptionByteToUint8(OptionByte option_byte) {
        return static_cast<uint8_t>(option_byte);
    }

    bool operator|(Option_t option, OptionByte option_byte) {
        return option | OptionByteToUint8(option_byte);
    }

    bool operator&(Option_t option, OptionByte option_byte) {
        return option & OptionByteToUint8(option_byte);
    }

}