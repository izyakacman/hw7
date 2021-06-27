#pragma once

#include <vector>
#include <map>
#include <string_view>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>

class DuplicateFinder
{
public:

	DuplicateFinder() = delete;
	DuplicateFinder(boost::program_options::variables_map vm) : vm_{ std::move(vm) } {};
	~DuplicateFinder() = default;

	void Find();

private:

	bool GetBlockHash(const boost::filesystem::path& file, const size_t block_number, int& block_hash);
	bool CompareFiles(const boost::filesystem::path& file1, const boost::filesystem::path& file2);
	void ReplaceAll(std::string& inout, std::string_view what, std::string_view with);

	boost::program_options::variables_map vm_;
	std::map<boost::filesystem::path, std::vector<int>> files_hash_;
};