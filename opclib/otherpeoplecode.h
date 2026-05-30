#pragma once

namespace otherpeoplecode
{
	//
	// The main function, pass in a HTTP(S) URL or a file path
	// and you get standard LoadLibary behavior, and HMODULE returned
	// that you must call FreeLibary() to clean up.
	// 
	// Internally if the url is an HTTP(S) URL,
	// the function caches the DLL on disk and calls LoadLibrary() 
	// on the libary from the cache location.
	// Otherwise it simply calls LoadLibrary() on the file path.
	// 
	// The support for file "URLs" is so you can do 
	// testing against local DLLs without getting the internet involved.
	//
	HMODULE LoadLibraryWeb(const TCHAR* url);


	//
	// Downloaded DLLs are stored in a cache
	// 

	// Where should DLLs be cached?
	void SetCachePath(const TCHAR* path);


	//
	// URLs passed to LoadLibraryWeb can refer to HTTP URLs, or file paths
	// 

	// What base URL should be prepended to URLs before download?
	void SetUrlBasePath(const TCHAR* path);

	// What base file path should be appended to local file paths before access?
	void SetFileBasePath(const TCHAR* path);
}
