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
	// There is no trace output.
	HMODULE __declspec(dllexport) LoadLibraryWeb(const wchar_t* url);

	// Use LoadLibraryWebEx() to specify the cache directory 
	// and a FILE* (like stdout) for trace output for troubleshooting.
	HMODULE __declspec(dllexport) LoadLibraryWebEx
	(
		const wchar_t* url,
		const wchar_t* cacheDirectory,
		FILE* progressLog
	);

	// Unit test routine to ensure the implementation functions.
	BOOL __declspec(dllexport) TestLoadLibraryWebUtils();

#ifdef __cplusplus
}
#endif
