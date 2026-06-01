#include "pch.h"
#include "CppUnitTest.h"
#include "otherpeoplecode.h"
#include "utils.h"
#include "loader.h"

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
			Assert::IsFalse(opc::Utils::FileExists(L"asd;lfjas;lfdj;lasdfj"));

			Assert::IsTrue(opc::Utils::StartsWith(L"foo", L"fo"));
			Assert::IsFalse(opc::Utils::StartsWith(L"foo", L"bar"));

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
			const char* file_load_err = opc::Utils::LoadFileIntoMemory(L"..\\pch.h", file_bytes);
			Assert::IsFalse(file_load_err != nullptr);
			Assert::IsFalse(file_bytes.empty());
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
			Assert::AreEqual(size_t(1), un3_splitted.size());
			Assert::AreEqual(std::wstring(L""), un3_splitted[0]);

			auto one_splitted = opc::Utils::Split(L"foo:bar", L":");
			Assert::AreEqual(size_t(2), one_splitted.size());
			Assert::AreEqual(std::wstring(L"foo"), one_splitted[0]);
			Assert::AreEqual(std::wstring(L"bar"), one_splitted[1]);

			auto two_splitted = opc::Utils::Split(L"foo\r\nbar\r\n", L"\r\n");
			Assert::AreEqual(size_t(2), two_splitted.size());
			Assert::AreEqual(std::wstring(L"foo"), two_splitted[0]);
			Assert::AreEqual(std::wstring(L"bar"), two_splitted[1]);
		}

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
			} catch (const std::exception& exp) {
				Assert::AreEqual(std::string("msg"), std::string(exp.what()));
			}

			try {
				opc::Utils::RaiseItemError("msg", L"item");
				Assert::Fail();
			} catch (const std::exception& exp) {
				Assert::AreEqual(std::string("msg: item"), std::string(exp.what()));
			}
		}

		TEST_METHOD(TestUrlParse)
		{
			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string("not url"), std::string(err_str));
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string("server"), std::string(err_str));
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo:bar";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string("port"), std::string(err_str));
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string(""), std::string(err_str));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L""), parts.request);
				Assert::AreEqual(80, parts.port);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"https://foo:914";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string(err_str), std::string(""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L""), parts.request);
				Assert::AreEqual(914, parts.port);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo:924/request";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string(err_str), std::string(""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L"request"), parts.request);
				Assert::AreEqual(924, parts.port);
			}

			{
				opc::UrlParts parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo/request/more/some.dll";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string(err_str), std::string(""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L"request/more/some.dll"), parts.request);
			}
		}

		TEST_METHOD(TestDownload)
		{
			{
				opc::UrlParts url_parts;
				const char* url_parse = opc::UrlParts::Parse(L"https://laksdfhaskdfsadlfsadf.com", url_parts);
				Assert::AreEqual(std::string(""), std::string(url_parse));

				opc::HttpRequest request(url_parts, L"GET", L"bad_request.html");
				opc::HttpResponse response = opc::Loader().Load(request);
				Assert::AreEqual(std::wstring(L"send_request"), response.ErrorMessage);
			}

			{
				opc::UrlParts url_parts;
				const char* url_parse = opc::UrlParts::Parse(L"https://localhost/opctest/bad-url-part", url_parts);
				Assert::AreEqual(std::string(""), std::string(url_parse));

				opc::HttpRequest request(url_parts, L"GET", L"mballoni-bad_url_part.html");
				opc::HttpResponse response = opc::Loader().Load(request);
				Assert::IsTrue(response.StatusCode / 100 != 2);
			}

			{
				opc::UrlParts url_parts;
				const char* url_parse = opc::UrlParts::Parse(L"https://localhost/opctest/index.html", url_parts);
				Assert::AreEqual(std::string(""), std::string(url_parse));

				opc::HttpRequest request(url_parts, L"GET", L"local-index.html");
				opc::HttpResponse response = opc::Loader().Load(request);
				Assert::AreEqual(DWORD(200), response.StatusCode);
				Assert::AreEqual(std::wstring(L""), std::wstring(response.ErrorMessage));

				std::vector<uint8_t> dl_bytes;
				const char* open_file_err_str = opc::Utils::LoadFileIntoMemory(L"local-index.html", dl_bytes);
				Assert::AreEqual(std::string(""), std::string(open_file_err_str));
				std::wstring dl_str = opc::Utils::AsciiBytesToWStr(dl_bytes);
				Assert::IsTrue(dl_str.find(L"foo") != std::wstring::npos);
			}
		}
	};
}
