#include "pch.h"
#include "otherpeoplecode.h"
#include "inifile.h"
#include "utils.h"
#include "urlparts.h"
#include "loader.h"

#include <filesystem>
#include <iostream>
#include <source_location>

namespace opc = otherpeoplecode;

#define AssertAreEqual(x, y) \
	if (!((x) == (y))) { \
		printf("AssertAreEqual fails: %d %s\n", __LINE__, __FILE__); \
		return false; \
	}

#define AssertIsTrue(x) \
	if (!(x)) { \
		printf("AssertIsTrue fails: %d %s\n", __LINE__, __FILE__); \
		return false; \
	}

#define AssertFail() \
	{ \
		printf("AssertFail: %d %s\n", __LINE__, __FILE__); \
		return false; \
	}

BOOL TestLoadLibraryWebUtils()
{
	printf("Utils...\n");
	AssertAreEqual(std::wstring(L""), opc::Utils::ToLower(L""));
	AssertAreEqual(std::wstring(L"foobar"), opc::Utils::ToLower(L"FooBar"));

	std::string source_location_str = std::source_location::current().file_name();
	AssertIsTrue(opc::Utils::FileExists(std::wstring(source_location_str.begin(), source_location_str.end())));
	AssertIsTrue(!opc::Utils::FileExists(L"asd;lfjas;lfdj;lasdfj"));

	AssertIsTrue(opc::Utils::StartsWith(L"foo", L"fo"));
	AssertIsTrue(!opc::Utils::StartsWith(L"foo", L"bar"));

	AssertAreEqual(std::wstring(L""), opc::Utils::Trim(L""));
	AssertAreEqual(std::wstring(L""), opc::Utils::Trim(L" "));
	AssertAreEqual(std::wstring(L"a"), opc::Utils::Trim(L"a"));
	AssertAreEqual(std::wstring(L"ab"), opc::Utils::Trim(L"ab"));
	AssertAreEqual(std::wstring(L"a"), opc::Utils::Trim(L" a"));
	AssertAreEqual(std::wstring(L"a"), opc::Utils::Trim(L"a "));
	AssertAreEqual(std::wstring(L"a"), opc::Utils::Trim(L" a "));

	AssertAreEqual(std::wstring(L"empty"), opc::Utils::SafePathStr(L""));
	AssertAreEqual(std::wstring(L"abc"), opc::Utils::SafePathStr(L"abc"));
	AssertAreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a.c"));
	AssertAreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a^(!@c"));
	AssertAreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a^c%*#"));
	AssertAreEqual(std::wstring(L"empty"), opc::Utils::SafePathStr(L"^%*#"));

	std::vector<uint8_t> file_bytes;
	std::wstring file_load_err = opc::Utils::LoadFileIntoMemory(opc::Utils::ToWideStr(__FILE__), file_bytes);
	AssertIsTrue(file_load_err.empty());
	AssertIsTrue(!file_bytes.empty());
	file_bytes.push_back(0);
	AssertIsTrue(strstr((char*)file_bytes.data(), "#include") != nullptr);

	std::vector< uint8_t> ascii_bytes{ 'f', 'o', 'o' };
	AssertAreEqual(std::wstring(L"foo"), opc::Utils::AsciiBytesToWStr(ascii_bytes));

	AssertAreEqual(std::string("foobar"), opc::Utils::WstringToUtf8(L"foobar"));

	auto no_splitted = opc::Utils::Split(L"", L"");
	AssertAreEqual(size_t(1), no_splitted.size());
	AssertAreEqual(std::wstring(L""), no_splitted[0]);

	auto un1_splitted = opc::Utils::Split(L"a", L"");
	AssertAreEqual(size_t(1), un1_splitted.size());
	AssertAreEqual(std::wstring(L"a"), un1_splitted[0]);

	auto un3_splitted = opc::Utils::Split(L"", L"b");
	AssertAreEqual(size_t(0), un3_splitted.size());

	auto one_splitted = opc::Utils::Split(L"foo:bar", L":");
	AssertAreEqual(size_t(2), one_splitted.size());
	AssertAreEqual(std::wstring(L"foo"), one_splitted[0]);
	AssertAreEqual(std::wstring(L"bar"), one_splitted[1]);

	auto two_splitted = opc::Utils::Split(L"foo\r\nbar\r\n", L"\r\n");
	AssertAreEqual(size_t(2), two_splitted.size());
	AssertAreEqual(std::wstring(L"foo"), two_splitted[0]);
	AssertAreEqual(std::wstring(L"bar"), two_splitted[1]);

	AssertIsTrue(!opc::Utils::IsUrl(L""));
	AssertIsTrue(!opc::Utils::IsUrl(L"foobar.blet"));
	AssertIsTrue(opc::Utils::IsUrl(L"http://hfoobar.blet"));
	AssertIsTrue(opc::Utils::IsUrl(L"https://sfoobar.blet"));

	AssertAreEqual(std::wstring(L"unsafe_string"), opc::Utils::ToSafeStr(L""));
	AssertAreEqual(std::wstring(L"f"), opc::Utils::ToSafeStr(L"f"));
	AssertAreEqual(std::wstring(L"foo"), opc::Utils::ToSafeStr(L"foo"));
	AssertAreEqual(std::wstring(L"foo_bar"), opc::Utils::ToSafeStr(L"foo.bar"));
	AssertAreEqual(std::wstring(L"foo_bar22"), opc::Utils::ToSafeStr(L"foo.bar22"));
	AssertAreEqual(std::wstring(L"foo_bar2_2_extra"), opc::Utils::ToSafeStr(L"foo.bar2.2-extra"));
	AssertAreEqual(std::wstring(L"foo_bar2_2_trail"), opc::Utils::ToSafeStr(L"foo.bar2.2..-trail..."));

	printf("RaiseItemError...\n");
	try {
		opc::Utils::RaiseItemError(nullptr, L"item");
		AssertFail();
	}
	catch (const std::exception& exp) {
		AssertAreEqual(std::string("Invalid error message"), std::string(exp.what()));
	}

	try {
		opc::Utils::RaiseItemError("", L"item");
		AssertFail();
	}
	catch (const std::exception& exp) {
		AssertAreEqual(std::string("Invalid error message"), std::string(exp.what()));
	}

	try {
		opc::Utils::RaiseItemError("msg", L"");
		AssertFail();
	}
	catch (const std::exception& exp) {
		AssertAreEqual(std::string("msg"), std::string(exp.what()));
	}

	try {
		opc::Utils::RaiseItemError("msg", nullptr);
		AssertFail();
	}
	catch (const std::exception& exp) {
		AssertAreEqual(std::string("msg"), std::string(exp.what()));
	}

	try {
		opc::Utils::RaiseItemError("msg", L"");
		AssertFail();
	}
	catch (const std::exception& exp) {
		AssertAreEqual(std::string("msg"), std::string(exp.what()));
	}

	try {
		opc::Utils::RaiseItemError("msg", L"item");
		AssertFail();
	}
	catch (const std::exception& exp) {
		AssertAreEqual(std::string("msg: item"), std::string(exp.what()));
	}


	printf("UrlParse...\n");
	{
		opc::UrlParts parse;
		opc::UrlParts parts;
		std::wstring url = L"";
		std::wstring err_str = opc::UrlParts::Parse(url, parts);
		AssertAreEqual(std::wstring(L"not url"), err_str);
	}
	{
		opc::UrlParts parse;
		opc::UrlParts parts;
		std::wstring url = L"http://";
		std::wstring err_str = opc::UrlParts::Parse(url, parts);
		AssertAreEqual(std::wstring(L"server"), err_str);
	}
	{
		opc::UrlParts parse;
		opc::UrlParts parts;
		std::wstring url = L"http://foo:bar";
		std::wstring err_str = opc::UrlParts::Parse(url, parts);
		AssertAreEqual(std::wstring(L"port"), err_str);
	}
	{
		opc::UrlParts parse;
		opc::UrlParts parts;
		std::wstring url = L"http://foo";
		std::wstring err_str = opc::UrlParts::Parse(url, parts);
		AssertAreEqual(std::wstring(L""), err_str);
		AssertAreEqual(std::wstring(L"foo"), parts.server);
		AssertAreEqual(std::wstring(L""), parts.request);
		AssertAreEqual(80, parts.port);
	}
	{
		opc::UrlParts parse;
		opc::UrlParts parts;
		std::wstring url = L"https://foo:914";
		std::wstring err_str = opc::UrlParts::Parse(url, parts);
		AssertAreEqual(std::wstring(err_str), std::wstring(L""));
		AssertAreEqual(std::wstring(L"foo"), parts.server);
		AssertAreEqual(std::wstring(L""), parts.request);
		AssertAreEqual(914, parts.port);
	}
    {
		opc::UrlParts parse;
		opc::UrlParts parts;
		std::wstring url = L"http://foo:924/request";
		std::wstring err_str = opc::UrlParts::Parse(url, parts);
		AssertAreEqual(std::wstring(err_str), std::wstring(L""));
		AssertAreEqual(std::wstring(L"foo"), parts.server);
		AssertAreEqual(std::wstring(L"request"), parts.request);
		AssertAreEqual(924, parts.port);
	}
	{
		opc::UrlParts parse;
		opc::UrlParts parts;
		std::wstring url = L"http://foo/request/more/some.dll";
		std::wstring err_str = opc::UrlParts::Parse(url, parts);
		AssertAreEqual(std::wstring(err_str), std::wstring(L""));
		AssertAreEqual(std::wstring(L"foo"), parts.server);
		AssertAreEqual(std::wstring(L"request/more/some.dll"), parts.request);
	}

	printf("IniFile...\n");
	auto src = std::map<std::wstring, std::wstring>();
	src[L"fOO"] = L"bar";
	src[L"blet "] = L"MONKey";
	AssertIsTrue(opc::IniFile::PutEntries(L"test_opc_file.ini", src));
	auto entries_opt = opc::IniFile::GetEntries(L"test_opc_file.ini");
	AssertIsTrue(entries_opt.has_value());
	auto entries_dict = entries_opt.value_or(std::map<std::wstring, std::wstring>());
	AssertAreEqual(size_t(2), entries_dict.size());
	AssertAreEqual(std::wstring(L"bar"), entries_dict[L"foo"]);
	std::wstring blet_str = entries_dict[L"blet"];
	AssertAreEqual(std::wstring(L"MONKey"), entries_dict[L"blet"]);

	printf("Download...\n");
	{
		// bad URL
		opc::HttpRequest request(L"http://)(*&^)*(^)*(&^)*(&.com", L"GET", L"mballoni-bad_request.html");
		opc::HttpResponse response = opc::Loader().Load(request);
		AssertAreEqual(std::wstring(L"connect"), response.ErrorMessage);
		AssertIsTrue(response.HttpStatusCode / 100 != 2);
		AssertAreEqual(DWORD(0), response.HttpStatusCode);
	}
	{
		// bad request
		opc::HttpRequest request(L"http://localhost/opctest/bad-url-part", L"GET", L"mballoni-bad_url_part.html");
		opc::HttpResponse response = opc::Loader().Load(request);
		AssertAreEqual(std::wstring(L""), response.ErrorMessage);
		AssertAreEqual(DWORD(404), response.HttpStatusCode);
	}
	{
		// good request
		opc::HttpRequest request(L"http://localhost/opctest/index.html", L"GET", L"mballoni-local-index.html");
		opc::HttpResponse response = opc::Loader().Load(request);
		AssertAreEqual(std::wstring(L""), response.ErrorMessage);
		AssertAreEqual(DWORD(200), response.HttpStatusCode);
		std::vector<uint8_t> mballoni_file_bytes;
		std::wstring file_error = opc::Utils::LoadFileIntoMemory(L"mballoni-local-index.html", mballoni_file_bytes);
		AssertAreEqual(std::wstring(L""), file_error);
		std::wstring file_str = opc::Utils::AsciiBytesToWStr(mballoni_file_bytes);
		AssertAreEqual(std::wstring(L"foo"), file_str);
	}

	printf("All done.\n");
	return TRUE;
}
