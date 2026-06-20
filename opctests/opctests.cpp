#include "pch.h"
#include "CppUnitTest.h"
#include "otherpeoplecode.h"

#include <filesystem>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace otherpeopletests
{
	TEST_CLASS(otherpeopletests)
	{
	public:
		TEST_METHOD(TestOpcUtils)
		{
			Assert::IsTrue(TestLoadLibraryWebUtils()); // have the DLL do its own testing
		}

		TEST_METHOD(TestLoadLibraryWeb)
		{
			std::wstring cache_path = L"TestOpcCache";
			if (std::filesystem::exists(cache_path))
				std::filesystem::remove_all(cache_path);

			{
				HMODULE module = LoadLibraryWebEx(L"http://localhost/opctest/winhttp.dll", cache_path.c_str(), stdout);
				Assert::IsTrue(module != nullptr);
				if (module != nullptr)
				{
					Assert::IsTrue(::GetProcAddress(module, "WinHttpAddRequestHeadersEx") != nullptr);
					::FreeLibrary(module);
				}
			}

			{
				HMODULE module = LoadLibraryWebEx(L"http://localhost/opctest/winhttp.dll", cache_path.c_str(), stdout);
				if (module != nullptr)
				{
					Assert::IsTrue(::GetProcAddress(module, "WinHttpAddRequestHeadersEx") != nullptr);
					::FreeLibrary(module);
				}
			}

			{
				HMODULE module = LoadLibraryWebEx(L"http://localhost/opctest/winhttp.dll", cache_path.c_str(), stdout);
				Assert::IsTrue(module != nullptr);
				if (module != nullptr)
				{
					Assert::IsTrue(::GetProcAddress(module, "WinHttpAddRequestHeadersEx") != nullptr);
					::FreeLibrary(module);
				}
			}
		}
	};
}
