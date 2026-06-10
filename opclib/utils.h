#pragma once

namespace otherpeoplecode
{
	// Various utilities, mostly for string processing
	class Utils
	{
	public:
		static std::wstring ToLower(const std::wstring& str);
		static bool StartsWith(const std::wstring& str, const wchar_t* starter);
		static std::wstring Trim(const std::wstring& str);
		static bool FileExists(const std::wstring& path);
		static std::wstring SafePathStr(const std::wstring& un);
		static void RaiseItemError(const char* msg, const wchar_t* item);
		static std::wstring LoadFileIntoMemory(const std::wstring& filePath, std::vector<uint8_t>& output);
		static std::wstring AsciiBytesToWStr(const std::vector<uint8_t>& bytes);
		static std::wstring Utf8ToWString(const std::string& str);
		static std::string WstringToUtf8(const std::wstring& str);
		static std::vector<std::wstring> Split(const std::wstring& str, const std::wstring& seperator);
		static std::wstring TcharToWString(const wchar_t* str);
		static std::wstring TcharToWString(const char* str);
		static std::wstring ToWideStr(const std::string& str);
		static std::string ToNarrowStr(const std::wstring& str);
		static bool IsUrl(const std::wstring& maybeUrl);
		static std::wstring ToSafeStr(const std::wstring& str);
	};
}
