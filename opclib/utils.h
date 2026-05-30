#pragma once

namespace otherpeoplecode
{
	class Setup
	{
	public:
		std::wstring CachePath;
		std::wstring UrlBasePath;
		std::wstring FileBasePath;

		static Setup& GetObj();
	};

	class Utils
	{
	public:
		static bool StartsWith(const std::wstring& str, const wchar_t* starter);
		static std::wstring Trim(const std::wstring& str);
		static bool FileExists(const std::wstring& path);
		static std::wstring SafePathStr(const std::wstring& un);
		static void RaiseItemError(const char* msg, const wchar_t* item);
		static const char* LoadFileIntoMemory(const std::wstring& filePath, std::vector<uint8_t>& output);
		static std::wstring AsciiBytesToWStr(const std::vector<uint8_t>& bytes);
		static std::string WstringToUtf8(const std::wstring& str);
		static std::vector<std::wstring> Split(const std::wstring& str, const std::wstring& seperator);
	};
}
