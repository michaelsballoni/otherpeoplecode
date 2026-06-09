#include "pch.h"
#include "CppUnitTest.h"
#include "otherpeoplecode.h"
#include "inifile.h"
#include "utils.h"
#include "urlparts.h"
#include "loader.h"

#include <iostream>
#include <source_location>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace opc = otherpeoplecode;

namespace otherpeopletests
{
	TEST_CLASS(otherpeopletests)
	{
	public:
		TEST_METHOD(TestUtils)
		{
			Assert::AreEqual(std::wstring(L""), opc::Utils::ToLower(L""));
			Assert::AreEqual(std::wstring(L"foobar"), opc::Utils::ToLower(L"FooBar"));

			std::string source_location_str = std::source_location::current().file_name();
			Assert::IsTrue(opc::Utils::FileExists(std::wstring(source_location_str.begin(), source_location_str.end())));
			Assert::IsTrue(!opc::Utils::FileExists(L"asd;lfjas;lfdj;lasdfj"));

			Assert::IsTrue(opc::Utils::StartsWith(L"foo", L"fo"));
			Assert::IsTrue(!opc::Utils::StartsWith(L"foo", L"bar"));

			Assert::AreEqual(std::wstring(L""), opc::Utils::Trim(L""));
			Assert::AreEqual(std::wstring(L""), opc::Utils::Trim(L" "));
			Assert::AreEqual(std::wstring(L"a"), opc::Utils::Trim(L"a"));
			Assert::AreEqual(std::wstring(L"ab"), opc::Utils::Trim(L"ab"));
			Assert::AreEqual(std::wstring(L"a"), opc::Utils::Trim(L" a"));
			Assert::AreEqual(std::wstring(L"a"), opc::Utils::Trim(L"a "));
			Assert::AreEqual(std::wstring(L"a"), opc::Utils::Trim(L" a "));

			Assert::AreEqual(std::wstring(L"empty"), opc::Utils::SafePathStr(L""));
			Assert::AreEqual(std::wstring(L"abc"), opc::Utils::SafePathStr(L"abc"));
			Assert::AreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a.c"));
			Assert::AreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a^(!@c"));
			Assert::AreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a^c%*#"));
			Assert::AreEqual(std::wstring(L"empty"), opc::Utils::SafePathStr(L"^%*#"));

			std::vector<uint8_t> file_bytes;
			std::wstring file_load_err = opc::Utils::LoadFileIntoMemory(opc::Utils::ToWideStr(__FILE__), file_bytes);
			Assert::IsTrue(file_load_err.empty());
			Assert::IsTrue(!file_bytes.empty());
			file_bytes.push_back(0);
			Assert::IsTrue(strstr((char*)file_bytes.data(), "#include") != nullptr);

			std::vector< uint8_t> ascii_bytes{ 'f', 'o', 'o' };
			Assert::AreEqual(std::wstring(L"foo"), opc::Utils::AsciiBytesToWStr(ascii_bytes));

			Assert::AreEqual(std::string("foobar"), opc::Utils::WstringToUtf8(L"foobar"));

			auto no_splitted = opc::Utils::Split(L"", L"");
			Assert::AreEqual(size_t(1), no_splitted.size());
			Assert::AreEqual(std::wstring(L""), no_splitted[0]);

			auto un1_splitted = opc::Utils::Split(L"a", L"");
			Assert::AreEqual(size_t(1), un1_splitted.size());
			Assert::AreEqual(std::wstring(L"a"), un1_splitted[0]);

			auto un3_splitted = opc::Utils::Split(L"", L"b");
			Assert::AreEqual(size_t(0), un3_splitted.size());

			auto one_splitted = opc::Utils::Split(L"foo:bar", L":");
			Assert::AreEqual(size_t(2), one_splitted.size());
			Assert::AreEqual(std::wstring(L"foo"), one_splitted[0]);
			Assert::AreEqual(std::wstring(L"bar"), one_splitted[1]);

			auto two_splitted = opc::Utils::Split(L"foo\r\nbar\r\n", L"\r\n");
			Assert::AreEqual(size_t(2), two_splitted.size());
			Assert::AreEqual(std::wstring(L"foo"), two_splitted[0]);
			Assert::AreEqual(std::wstring(L"bar"), two_splitted[1]);

			Assert::IsTrue(!opc::Utils::IsUrl(L""));
			Assert::IsTrue(!opc::Utils::IsUrl(L"foobar.blet"));
			Assert::IsTrue(opc::Utils::IsUrl(L"http://hfoobar.blet"));
			Assert::IsTrue(opc::Utils::IsUrl(L"https://sfoobar.blet"));
		}

		/* FORNOW - Unused
		TEST_METHOD(TestHeaderCompare)
		{
			opc::HttpResponse response(L"");
			Assert::IsTrue(response.DoHeadersMatch({ {L"", L""} }));

			response.Headers[L"foo"] = L"bar";
			Assert::IsTrue(response.DoHeadersMatch({ {L"", L""} }));
			Assert::IsTrue(response.DoHeadersMatch({ {L"foo", L"bar"} }));
			Assert::IsTrue(!response.DoHeadersMatch({ {L"foo", L"blet"} }));
			Assert::IsTrue(response.DoHeadersMatch({ {L"foomonk", L""} }));
			Assert::IsTrue(response.DoHeadersMatch({ {L"FOO", L"blet"} }));
			Assert::IsTrue(response.DoHeadersMatch({ {L"FOO", L"bar"} }));
			Assert::IsTrue(!response.DoHeadersMatch({ {L"foo", L"BAR"} }));
		}
		*/

		TEST_METHOD(TestRaiseItemError)
		{
			try {
				opc::Utils::RaiseItemError(nullptr, L"item");
				Assert::Fail();
			}
			catch (const std::exception& exp) {
				Assert::AreEqual(std::string("Invalid error message"), std::string(exp.what()));
			}

			try {
				opc::Utils::RaiseItemError("", L"item");
				Assert::Fail();
			}
			catch (const std::exception& exp) {
				Assert::AreEqual(std::string("Invalid error message"), std::string(exp.what()));
			}

			try {
				opc::Utils::RaiseItemError("msg", L"");
				Assert::Fail();
			}
			catch (const std::exception& exp) {
				Assert::AreEqual(std::string("msg"), std::string(exp.what()));
			}

			try {
				opc::Utils::RaiseItemError("msg", nullptr);
				Assert::Fail();
			}
			catch (const std::exception& exp) {
				Assert::AreEqual(std::string("msg"), std::string(exp.what()));
			}

			try {
				opc::Utils::RaiseItemError("msg", L"");
				Assert::Fail();
			}
			catch (const std::exception& exp) {
				Assert::AreEqual(std::string("msg"), std::string(exp.what()));
			}

			try {
				opc::Utils::RaiseItemError("msg", L"item");
				Assert::Fail();
			}
			catch (const std::exception& exp) {
				Assert::AreEqual(std::string("msg: item"), std::string(exp.what()));
			}
		}

		TEST_METHOD(TestUrlParse)
		{
			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"";
				std::wstring err_str = opc::UrlParts::Parse(url, parts);
				Assert::AreEqual(std::wstring(L"not url"), err_str);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://";
				std::wstring err_str = opc::UrlParts::Parse(url, parts);
				Assert::AreEqual(std::wstring(L"server"), err_str);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo:bar";
				std::wstring err_str = opc::UrlParts::Parse(url, parts);
				Assert::AreEqual(std::wstring(L"port"), err_str);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo";
				std::wstring err_str = opc::UrlParts::Parse(url, parts);
				Assert::AreEqual(std::wstring(L""), err_str);
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L""), parts.request);
				Assert::AreEqual(80, parts.port);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"https://foo:914";
				std::wstring err_str = opc::UrlParts::Parse(url, parts);
				Assert::AreEqual(std::wstring(err_str), std::wstring(L""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L""), parts.request);
				Assert::AreEqual(914, parts.port);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo:924/request";
				std::wstring err_str = opc::UrlParts::Parse(url, parts);
				Assert::AreEqual(std::wstring(err_str), std::wstring(L""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L"request"), parts.request);
				Assert::AreEqual(924, parts.port);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo/request/more/some.dll";
				std::wstring err_str = opc::UrlParts::Parse(url, parts);
				Assert::AreEqual(std::wstring(err_str), std::wstring(L""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L"request/more/some.dll"), parts.request);
			}
		}

		TEST_METHOD(TestIniFile)
		{
			auto src = std::map<std::wstring, std::wstring>();
			src[L"fOO"] = L"bar";
			src[L"blet "] = L"MONKey";
			Assert::IsTrue(opc::IniFile::PutEntries(L"test_opc_file.ini", src));
			auto entries_opt = opc::IniFile::GetEntries(L"test_opc_file.ini");
			Assert::IsTrue(entries_opt.has_value());
			auto entries_dict = entries_opt.value();
			Assert::AreEqual(size_t(2), entries_dict.size());
			Assert::AreEqual(std::wstring(L"bar"), entries_dict[L"foo"]);
			std::wstring blet_str = entries_dict[L"blet"];
			Assert::AreEqual(std::wstring(L"MONKey"), entries_dict[L"blet"]);
		}

		TEST_METHOD(TestDownload)
		{
			{
				// bad URL
				opc::HttpRequest request(L"http://)(*&^)*(^)*(&^)*(&.com", L"GET", L"mballoni-bad_request.html");
				opc::HttpResponse response = opc::Loader().Load(request);
				Assert::AreEqual(std::wstring(L"connect"), response.ErrorMessage);
				Assert::IsTrue(response.StatusCode / 100 != 2);
				Assert::AreEqual(DWORD(0), response.StatusCode);
			}

			{
				// bad request
				opc::HttpRequest request(L"http://localhost/opctest/bad-url-part", L"GET", L"mballoni-bad_url_part.html");
				opc::HttpResponse response = opc::Loader().Load(request);
				Assert::AreEqual(std::wstring(L""), response.ErrorMessage);
				Assert::AreEqual(DWORD(404), response.StatusCode);
			}

			{
				// good request
				opc::HttpRequest request(L"http://localhost/opctest/index.html", L"GET", L"mballoni-local-index.html");
				opc::HttpResponse response = opc::Loader().Load(request);
				Assert::AreEqual(std::wstring(L""), response.ErrorMessage);
				Assert::AreEqual(DWORD(200), response.StatusCode);
				std::vector<uint8_t> file_bytes;
				std::wstring file_error = opc::Utils::LoadFileIntoMemory(L"mballoni-local-index.html", file_bytes);
				Assert::AreEqual(std::wstring(L""), file_error);
				std::wstring file_str = opc::Utils::AsciiBytesToWStr(file_bytes);
				Assert::AreEqual(std::wstring(L"foo"), file_str);
			}
		}
	};
}
