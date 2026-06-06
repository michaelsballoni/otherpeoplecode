#pragma once

#include <windows.h>

namespace otherpeoplecode
{
	//
	// LoadLibraryWeb() is the main function
	// Ppass in a HTTP(S) URL or a file path
	// and you get standard LoadLibary behavior, an HMODULE returned
	// that you must call FreeLibary() to clean up.
	// 
	// Internally if the url is an HTTP(S) URL,
	// the function caches the DLL on disk and calls LoadLibrary() 
	// on the cache location.
	// Otherwise it simply calls LoadLibrary() on the supposed file path.
	// 
	// The support for file "URLs" is so you can do 
	// testing against local DLLs without getting networking involved.
	//
	HMODULE LoadLibraryWeb(const TCHAR* url);

	//
	// Downloaded DLLs are stored in a cache
	// 
	// Where should DLLs be cached?
	void SetCachePath(const TCHAR* path);
}
