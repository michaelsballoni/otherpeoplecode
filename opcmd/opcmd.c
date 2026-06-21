#include "otherpeoplecode.h"

#include <stdio.h>
#include <stdlib.h>

int wmain(int argc, wchar_t* argv[])
{
	if (argc != 2)
	{
		printf("Usage: opcmd <DLL URL>\n");
		return 0;
	}

	wchar_t* url = argv[1];

	while (TRUE)
	{
		HMODULE module = LoadLibraryWebEx(url, L"", stdout);
		if (module == NULL)
			printf("\nModule not loaded.\n");
		else
			printf("\nModule loaded.\n");
		if (module != NULL)
			FreeLibrary(module);

		printf("\nHit CTRL-C to quit, any other key to load again\n");
		(void)getc(stdin);
	}

	return 0;
}
