# otherpeoplecode
Empower Windows C++ code to dynamically load other Windows C++ code via URLs.

## Libraries
Each library is a DLL file hosted on the internet or a local path.
The framework doesn't care which, which is good for development vs. production.

## Client
The client C++ runtime is told by applications about libraries to load by URLs/paths.
It downloads the DLL and caches it locally, which is key because DLLs can get big.

## Applications
C++ applications call the client to load a DLL with a URL, just like LoadLibrary, 
just dynamic.