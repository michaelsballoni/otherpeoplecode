#include "otherpeoplecode.h"

#include <stdio.h>
#include <stdlib.h>

int wmain(int argc, wchar_t* argv[])
{
	if (argc <= 1 || argc > 3)
	{
		printf("opcmd <DLL URL> <Cache Directory> <Trace Log File>\n");
		printf("1. DLL URL: The URL where the DLL can be downloaded from\n");
		printf("2. Cache Directory: Where downloaded DLLs should be cached; specify \"\" to use default directory\n");
		printf("3. Trace Log File: Where output of the progress of this program should go; specify \"\" for standard output\n");
		return 0;
	}

	wchar_t* url = argv[1];
	wchar_t* cache_dir_path = argc > 2 && *(argv[2]) ? argv[2] : L"";
	wchar_t* trace_output_file_path = argc > 3 && *(argv[3]) ? argv[3] : L"";

	printf("URL:        %ls\n", url);
	printf("Cache Dir:  %ls\n", cache_dir_path);
	printf("Trace File: %ls\n", trace_output_file_path);

	while (TRUE)
	{
		HMODULE module = 
			LoadLibraryWebEx
			(
				url, 
				cache_dir_path, 
				*trace_output_file_path ? _wfopen(trace_output_file_path, L"wt") : stdout
			);
		if (module == NULL)
			printf("Module not loaded.\n");
		else
			printf("Module loaded.\n");
		if (module != NULL)
			FreeLibrary(module);

		printf("Hit CTRL-C to quit, any other key to load again");
		(void)getc(stdin);
	}

	return 0;
}
