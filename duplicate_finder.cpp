#include "duolicate_finder.hpp"

#include <iostream>
#include <set>
#include <fstream>
#include <numeric>

#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>

using namespace std;
using namespace boost::filesystem;
using boost::uuids::detail::md5;

void DuplicateFinder::Find()
{
    vector<path> files;

    for (const auto& dir : vm_["dir"].as<vector<string>>())
    {
        try
        {
            recursive_directory_iterator begin(dir);
            recursive_directory_iterator end;

            for (auto itr = begin; itr != end; ++itr)
            {
                auto fs = itr->status();

                if (fs.type() == boost::filesystem::directory_file)
                {
                    if (vm_["level"].as<bool>() == false)
                        itr.no_push();

                    if (vm_.count("edir"))
                    {
                        auto v = vm_["edir"].as<vector<string>>();
                        if (find(v.cbegin(), v.cend(), itr->path().filename()) != v.cend())
                            itr.no_push();
                    }
                }

                if (fs.type() == boost::filesystem::regular_file)
                {
                    if (vm_.count("mask"))
                    {
                        string mask = vm_["mask"].as<string>();
                        ReplaceAll(mask, ".", "[.]");
                        ReplaceAll(mask, "*", ".*");
                        ReplaceAll(mask, "?", ".");

                        boost::smatch what;
                        boost::regex filter(mask);

                        if (!boost::regex_match(itr->path().filename().string(), what, filter))
                            continue;
                    }

                    if (file_size(itr->path()) < vm_["size"].as<size_t>())
                        continue;

                    files.push_back(itr->path());
                }

            } // for itr

        } // try

        catch (const std::exception& e)
        {
            std::cerr << "exception: " << e.what() << std::endl;
        }

    } // for dirs

    set<path> duplicates;

    for (auto first_itr = files.cbegin(); first_itr != files.cend(); ++first_itr)
    {
        if (duplicates.find(*first_itr) != duplicates.cend())
            continue;

        for (auto second_itr = first_itr + 1; second_itr != files.cend(); ++second_itr)
        {
            if (duplicates.find(*second_itr) != duplicates.cend())
                continue;

            if (CompareFiles(*first_itr, *second_itr))
            {
                if (duplicates.find(*first_itr) == duplicates.cend())
                {
                    cout << *first_itr << endl;
                    duplicates.insert(*first_itr);
                }

                cout << *second_itr << endl;
                duplicates.insert(*second_itr);
            }
        }

        cout << endl;
    }
}

bool DuplicateFinder::GetBlockHash(const path& file, const size_t block_number, int& block_hash)
{
    auto pos = files_hash_.find(file);

    if (pos != files_hash_.cend())
    {
        if (pos->second.size() > block_number)
        {
            block_hash = pos->second[block_number];
            return true;
        }
    }

    ifstream stream;
    vector<char> buffer(vm_["block"].as<size_t>(), 0);

    stream.open(file.string(), ifstream::binary);
    stream.seekg(buffer.size() * block_number);
    stream.read(buffer.data(), buffer.size());

    if (stream.gcount() == 0)
        return false;

    if (vm_["hash"].as<string>() == "crc32")
    {
        boost::crc_32_type result;
        result.process_bytes(buffer.data(), buffer.size());
        block_hash = result.checksum();
    }
    else
    {
        md5 hash;
        md5::digest_type digest;

        hash.process_bytes(buffer.data(), buffer.size());
        hash.get_digest(digest);
        block_hash = digest[0];
    }

    files_hash_[file].push_back(block_hash);

    return true;
}

bool DuplicateFinder::CompareFiles(const path& file1, const path& file2)
{
    for (size_t block_number = 0;; ++block_number)
    {
        int hash1, hash2;

        if (GetBlockHash(file1, block_number, hash1) && GetBlockHash(file2, block_number, hash2))
        {
            if (hash1 != hash2)
                return false;
        }
        else
            return true;
    }
}

void DuplicateFinder::ReplaceAll(string& inout, string_view what, string_view with)
{
    size_t count = 0;
    for (string::size_type pos = 0;
        inout.npos != (pos = inout.find(what.data(), pos, what.length()));
        pos += with.length(), ++count) 
    {
        inout.replace(pos, what.length(), with.data(), with.length());
    }
}
