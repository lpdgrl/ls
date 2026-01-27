#include "../includes/ls.hpp"

std::vector<std::string> ListDir(const std::string& path, size_t& max) {
    std::vector<std::string> entries;
    DIR* dir = opendir(path.c_str());

    if (!dir) {
        return entries;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name(entry->d_name);
        if (name != "." &&  name != ".." && name[0] != '.') {
            entries.push_back(entry->d_name);
            max = std::max(name.size(), max);
        }
    }

    std::cout << "Max: " << max << '\n';

    closedir(dir);
    return entries;
}

void Usage() {
    std::cerr << "Try \'ls --help\' for more information" << '\n';
}

int main(int argc, char* argv[]) {
    try {
        ls::Ls ls(".", 1);
    } catch (std::system_error& err) {
        std::cerr << err.what() << '\n';
        return -1;
    }
    

    char cwd[1024];

    std::string options{"lrh"};
    winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    std::cout << "lines: " << w.ws_row << " columns: " << w.ws_col << '\n';
    size_t max = 0;

    char ch;
    while ((ch = getopt(argc, argv, options.c_str())) != -1) {
        switch (ch) {
            case 'l':
                break;
            case 'r':
                break;
            case 'h':
                break;
            default:
                Usage();
                return 2;
        }
    }
    argc -= optind;
    argv += optind;

    // if (argv != nullptr) {
    //     std::cout << *argv << '\n';
    // }

    if (errno == ERANGE) {
        std::cout << "size is small" << '\n';
    }

    std::string path;
    std::vector<std::string> result;
    if (argc) {
        path = *argv;
        result = ListDir(*argv, max);
    } else {
        result = ListDir(".", max);
    }

    if (result.empty()) {
        return -1;
    }
    
    size_t num_entries = result.size();
    
    size_t n_column = w.ws_col / num_entries;
    if (w.ws_col % num_entries) {
        ++n_column;
    }
    
    size_t n_row_in_column = num_entries / n_column;
    if (num_entries % n_column) {
        ++n_row_in_column;
    }

    std::cout << "size result: " << num_entries << " size n_column: " << n_column << " n_row:" << n_row_in_column << '\n';

    std::sort(result.begin(), result.end());
    
    struct stat fileinfo;
    
    
    for (const auto& entry : result) {
        errno = 0;
        std::string new_path(path);
        new_path.append(entry);

        int stat_err = stat(new_path.c_str(), &fileinfo);
        // std::cout << "stat_err: " << stat_err << '\n';
        if (stat_err != 0) {
            if (errno == EACCES) {
                std::cerr << "Permission is denied" << '\n';
            } else if (errno = EBADF) {
                std::cerr << "stat EBADF" << '\n';
            }
            std::cout << errno << '\n';
        }

        ls::detail::DumpTypeFiles(std::cout, fileinfo.st_mode);
        ls::detail::DumpPerms(std::cout, fileinfo.st_mode);
        std::cout << ' ' << fileinfo.st_nlink;
        std::cout << ' ' << getpwuid(fileinfo.st_uid)->pw_name;
        std::cout << ' ' << getgrgid(fileinfo.st_gid)->gr_name;
        std::cout << ' ' << fileinfo.st_size / 1024;
        std::cout << ' ';
        ls::detail::DumpDate(std::cout, fileinfo.st_mtim);
        std::cout << ' ' << entry << '\n';
        
        // std::cout << fileinfo.st_size << '\n';
        // std::cout << fileinfo.st_uid << '\n';b
        // passwd* pwd = getpwuid(fileinfo.st_uid);
        // std::cout << pwd->pw_name << '\n';
    }

    // for (size_t row = 0; row < n_row_in_column; ++row) {
    //     for (size_t base = row, col = 0; ;) {
    //         const auto& entry = result[base];
    //         size_t num_space = 2;
    //         if (entry.size() >= max) {
    //             num_space = 2;
    //         } 
    //         std::cout << entry; 
    //         size_t i = 0;
    //         while (i++ < num_space) {
    //             putchar(' ');
    //         }
    //         if ((base += n_row_in_column) >= num_entries) {
    //             break;
    //         }
    //         if (col++ == n_column) {
    //             break;
    //         }
    //     }
    //     std::cout << '\n';
    // }

    return 0;
}