#include "../includes/ls.hpp"

void Usage() {
    std::cerr << "Try \'ls --help\' for more information" << '\n';
}

int main(int argc, char* argv[]) {
    const char* str_options = "lrh";
    Ls::detail::Option_t format_options = Ls::detail::OptionByteToUint8(Ls::detail::OptionByte::SimpleFormat);
    using bin8 = std::bitset<8>;

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

    argc -= optind;
    argv += optind;

    std::cout << bin8(format_options) << '\n';
    
    std::string path;
    path = (argc) ? *argv : ".";

    try {
        Ls::Ls ls(path, format_options);
        ls.Parse();
        ls.Display();
    } catch (std::system_error& err) {
        std::cerr << err.what() << '\n';
        return -1;
    }

    return 0;
}