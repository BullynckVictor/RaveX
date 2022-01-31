#include "Engine/Utility/File.h"

std::filesystem::path rv::solution_path = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().parent_path();

bool rv::FileExists(const std::filesystem::path& file)
{
	return std::filesystem::is_regular_file(file);
}

std::filesystem::path rv::RelativeToSolution(const std::filesystem::path& path)
{
	return std::filesystem::relative(path, solution_path);
}
