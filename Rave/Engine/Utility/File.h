#pragma once
#include <filesystem>

namespace rv
{
	bool FileExists(const std::filesystem::path& file);
	std::filesystem::path RelativeToSolution(const std::filesystem::path& path);

	extern std::filesystem::path solution_path;
}