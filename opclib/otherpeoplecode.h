#pragma once

#include <windows.h>
#include <stdio.h>

#include <string>
#include <vector>

namespace otherpeoplecode
{
	//
	// LoadLibraryWeb() is the central function
	// Pass in a URL and it downloads and caches the DLL on disk, 
	// then calls LoadLibrary with the path into cache
	// FORNOW - Explain each parameter
	// 
	HMODULE LoadLibraryWeb
	(
		std::wstring url, 
		std::wstring cacheDirectory = L"",
		FILE* progressLog = nullptr
	);
}
