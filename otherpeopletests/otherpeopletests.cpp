#include "pch.h"
#include "CppUnitTest.h"

#include "../otherpeoplecode.h"

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
			std::string source_location_str = std::source_location::current().file_name();
			Assert::IsTrue(opc::Utils::FileExists(std::wstring(source_location_str.begin(), source_location_str.end())));
			Assert::IsFalse(opc::Utils::FileExists(L"asd;lfjas;lfdj;lasdfj"));

			Assert::IsTrue(opc::Utils::StartsWith(L"foo", L"fo"));
			Assert::IsFalse(opc::Utils::StartsWith(L"foo", L"bar"));

			Assert::AreEqual(std::wstring(L"empty"), opc::Utils::SafePathStr(L""));
			Assert::AreEqual(std::wstring(L"abc"), opc::Utils::SafePathStr(L"abc"));
			Assert::AreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a.c"));
			Assert::AreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a^(!@c"));
			Assert::AreEqual(std::wstring(L"a.c"), opc::Utils::SafePathStr(L"a^c%*#"));
			Assert::AreEqual(std::wstring(L"empty"), opc::Utils::SafePathStr(L"^%*#"));
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
				opc::UrlParse parse;
				opc::UrlParts parts;
				std::wstring url = L"";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string("not url"), std::string(err_str));
			}

			{
				opc::UrlParse parse;
				opc::UrlParts parts;
				std::wstring url = L"http://";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string("server"), std::string(err_str));
			}

			{
				opc::UrlParse parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo:bar";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string("port"), std::string(err_str));
			}

			{
				opc::UrlParse parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string(""), std::string(err_str));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L""), parts.request);
				Assert::AreEqual(80, parts.port);
			}

			{
				opc::UrlParse parse;
				opc::UrlParts parts;
				std::wstring url = L"https://foo:914";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string(err_str), std::string(""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L""), parts.request);
				Assert::AreEqual(914, parts.port);
			}

			{
				opc::UrlParse parse;
				opc::UrlParts parts;
				std::wstring url = L"http://foo:924/request";
				const char* err_str = parse.Parse(url, parts);
				Assert::AreEqual(std::string(err_str), std::string(""));
				Assert::AreEqual(std::wstring(L"foo"), parts.server);
				Assert::AreEqual(std::wstring(L"request"), parts.request);
				Assert::AreEqual(924, parts.port);
			}

			{
				opc::UrlParse parse;
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
				opc::UrlParse parse;
				opc::UrlParts url_parts;
				const char* url_parse = parse.Parse(L"https://laksdfhaskdfsadlfsadf.com", url_parts);
				Assert::AreEqual(std::string(""), std::string(url_parse));

				opc::Download download;
				unsigned int status_code = 0;
				const char* dl_err_str = download.Load(url_parts, L"bad_request.html", status_code);
				Assert::AreEqual(std::string("send_request"), std::string(dl_err_str));
			}

			{
				opc::UrlParse parse;
				opc::UrlParts url_parts;
				const char* url_err_str = parse.Parse(L"https://michaelballoni.com/bad-url-part", url_parts);
				
				opc::Download download;
				unsigned int status_code = 0;
				const char* dl_err_str = download.Load(url_parts, L"mballoni-bad_url_part.html", status_code);
				Assert::IsTrue(status_code / 100 != 2);
			}

			{
				opc::UrlParse parse;
				opc::UrlParts url_parts;
				const char* url_err_str = parse.Parse(L"https://michaelballoni.com/stackglasses/index.html", url_parts);

				opc::Download download;
				unsigned int status_code = 0;
				const char* dl_err_str = download.Load(url_parts, L"stack-glasses.html", status_code);
				Assert::AreEqual(std::string(""), std::string(dl_err_str));
				std::vector<uint8_t> dl_bytes;
				const char* open_file_err_str = opc::Utils::LoadFileIntoMemory(L"stack-glasses.html", dl_bytes);
				Assert::AreEqual(std::string(""), std::string(open_file_err_str));
				std::wstring dl_str = opc::Utils::AsciiBytesToWStr(dl_bytes);
				Assert::IsTrue(dl_str.find(L"Glasses can be stacked") != std::wstring::npos);
			}
		}
	};
}
