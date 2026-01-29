#include "../includes/ls.hpp"

void Usage();
Ls::detail::Option_t ParseOptions(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    const char* dot_root = ".";

    auto format_options = ParseOptions(argc, argv);

    argc -= optind;
    argv += optind;

    std::string path;
    path = (argc) ? *argv : dot_root;

    try {
        Ls::Ls ls(std::cout, path, format_options);
        ls.Parse();
        ls.Display();
    } catch (std::system_error& err) {
        std::cerr << err.what() << '\n';
        return -1;
    }

    return 0;
}

void Usage() {
    std::cerr << "Try \'ls\' for more information" << '\n';
}

Ls::detail::Option_t ParseOptions(int argc, char* argv[]) {
    const char* str_options = "lrh";
    Ls::detail::Option_t format_options = Ls::detail::OptionByteToUint8(Ls::detail::OptionByte::SimpleFormat);

    char ch;
    while ((ch = getopt(argc, argv, str_options)) != -1) {
        switch (ch) {
            case 'l':
                format_options |= Ls::detail::OptionByteToUint8(Ls::detail::OptionByte::LongFormat);
                break;
            case 'r':
                format_options |= Ls::detail::OptionByteToUint8(Ls::detail::OptionByte::ReverseFormat);
                break;
            case 'h':
                format_options |= Ls::detail::OptionByteToUint8(Ls::detail::OptionByte::HumanFormat);
                break;
            default:
                Usage();
                return 2;
        }
    }



    return format_options;
}