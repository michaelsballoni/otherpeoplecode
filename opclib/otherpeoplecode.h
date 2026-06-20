#pragma once

#include <windows.h>

#include <stddef.h>
#include <stdio.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif
	// LoadLibraryWeb() is the central function
	// Pass in a URL and it downloads and caches the DLL on disk, 
	// then calls LoadLibrary with the path into cache.
	// DLLs are cached in a directory named OtherPeopleCode in the current directory.
	HMODULE __declspec(dllexport) LoadLibraryWeb(const wchar_t* url);

	// Use LoadLibraryWebEx() to specify the cache directory 
	// and a file to output trace output for troubleshooting.
	HMODULE __declspec(dllexport) LoadLibraryWebEx
	(
		const wchar_t* url,
		const wchar_t* cacheDirectory,
		FILE* progressLog
	);

	// Ensure that the implementation is functional.
	BOOL __declspec(dllexport) TestLoadLibraryWebUtils();

#ifdef __cplusplus
}
#endif
