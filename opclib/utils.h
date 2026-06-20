#pragma once

// Pacification / Troubleshooting
// progressLog is a FILE* which can be null to not do logging
// msg is assumed to be a ASCII string literal message to output
// Any parameters after the msg are variables to format into the output
#define OPCLOG(progressLog, msg, ...) \
	{ \
		if (progressLog) \
		{ \
			do \
			{ \
				::fprintf(progressLog, msg"\r\n", ##__VA_ARGS__); \
				::_flushall(); \
			} while (false); \
		} \
	}

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
		static std::wstring ToWideStr(const std::string& str);
		static std::string ToNarrowStr(const std::wstring& str);
		static bool IsUrl(const std::wstring& maybeUrl);
		static std::wstring ToSafeStr(const std::wstring& str);
	};
}
