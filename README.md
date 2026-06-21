# otherpeoplecode
Dynamically load DLLs from remote sources in your Win32 C/C++ code. \
This project unites Win32's `LoadLibrary()` function with the `WinHttp` library as a basic caching HTTP client. \
The client performs content length, timestamp, and ETag cache validation on each load.

## Usage
otherpeoplecode is delivered as a DLL, opclib.dll, that exports three functions:
1. `LoadLibraryWeb()`
1. `LoadLibraryWebEx()`
1. `TestLoadLibraryWebUtils()`

### LoadLibraryWeb()
`HMODULE __declspec(dllexport) LoadLibraryWeb(const wchar_t* url)` \
Loads a library given a URL.  
The cache directory is managed internally, and there is no trace output.

### LoadLibraryWebEx()
```
HMODULE __declspec(dllexport) LoadLibraryWebEx
(
	const wchar_t* url,
	const wchar_t* cacheDirectory,
	FILE* progressLog
);
```
Loads a library given a URL, with optional cache directory and stream to trace output to.  

### TestLoadLibraryWebUtils()
`BOOL __declspec(dllexport) TestLoadLibraryWebUtils()` \
Runs unit tests for the library code.
The `opctest` library calls this to ensure test coverage.

## Sample output
The `opcmd` program takes a URL on the command line and loads a DLL from it.
Here is its output loading winhttp.dll twice, first with an empty cache, second loading from the cache:
```
> .\opcmd.exe "http://localhost/opctest/winhttp.dll"
LoadLibraryWeb: http://localhost/opctest/winhttp.dll
LoadLibraryWeb: Cache directory: OtherPeopleCode
LoadLibraryWeb: DLL not found in cache
LoadLibraryWeb: GET
Load...
Load: Opening output file: OtherPeopleCode\localhost_opctest_winhttp_dll_80.dll
Load: Connecting...
Load: Opening...
Load: Sending request...
Load: Receiving response...
Load: Reading status code...
Load: Status code: 200
Load: Reading all headers (size)...
Load: Reading all headers...
Load: Processing headers...
Load: Response body...
Load: All done.
LoadLibraryWeb: GET Headers:
 - content-length: 1228760
 - etag: "c044d7d8b5f7dc1:0"
 - last-modified: Tue, 09 Jun 2026 02:15:31 GMT
LoadLibraryWeb: Writing INI
LoadLibraryWeb: GET success, LoadLibrary from cache!

Module loaded.

Hit CTRL-C to quit, any other key to load again

LoadLibraryWeb: http://localhost/opctest/winhttp.dll
LoadLibraryWeb: Cache directory: OtherPeopleCode
LoadLibraryWeb: Local file and INI exist; will make HEAD request to check for changes
Load...
Load: Not reading response body for verb HEAD
Load: Connecting...
Load: Opening...
Load: Sending request...
Load: Receiving response...
Load: Reading status code...
Load: Status code: 200
Load: Reading all headers (size)...
Load: Reading all headers...
Load: Processing headers...
Load: All done.
HEAD Headers:
 - content-length: 1228760
 - etag: "c044d7d8b5f7dc1:0"
 - last-modified: Tue, 09 Jun 2026 02:15:31 GMT
LoadLibraryWeb: HEAD and INI match...LoadLibrary cached DLL!

Module loaded.

Hit CTRL-C to quit, any other key to load again
```

## Implementation
The solution is divided into...
1. `opclib`: the DLL, a C interface to a C++ implementation
1. `opcmd`: a cmd line POC, an example of consuming the `opclib` DLL
1. `opctest`: unit tests to validate the implementation
    1. Most of the unit tests are in `opclib` itself, in `tests.cpp`
