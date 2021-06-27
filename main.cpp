#include <iostream>

#include <boost/program_options.hpp>

#include "duolicate_finder.hpp"

using namespace std;

using namespace boost::program_options;
int main(int argc, const char* argv[])
{
    try 
    {
        options_description desc{ "Options" };
        desc.add_options()
            ("help,h", "This screen")
            ("dir,d", value<vector<string>>(), "directory for scaning")
            ("edir,e", value<vector<string>>(), "directory excluded from scanning")
            ("level,l", value<bool>()->default_value(true), "scaning level")
            ("size,s", value<size_t>()->default_value(1), "minimum file size for scaning")
            ("mask,m", value<std::string>()->default_value("*"), "file mask")
            ("block,b", value<size_t>()->default_value(1024), "block size")
            ("hash", value<string>()->default_value("crc32"), "hash algorithm: crc32, md5");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help") || !vm.count("dir"))
        {
            std::cout << desc << '\n';
            return 0;
        }

        DuplicateFinder finder(vm);

        finder.Find();
        
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
    }

	return 0;
}
