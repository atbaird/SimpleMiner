#include "Engine/Core/FileUtils.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

FileUtils::FileUtils()
{

}

bool FileUtils::createDirectoryA(std::string directory_name)
{
	if ((CreateDirectoryA((LPCSTR)directory_name.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS))
	{
		return true;
	}
	return false;
}