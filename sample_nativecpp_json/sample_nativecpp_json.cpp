// sample_nativecpp_json.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <codecvt>
#include <Windows.h>
#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

using namespace boost::property_tree;

std::wstring ConvertUtf8ToUtf16(LPCSTR utf8Str) {

    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
    if (sizeNeeded == 0)
    {
        const auto win32Err = GetLastError();
        std::cerr << "WideCharToMultiByte failed, Win32Err=" << win32Err << ", InputStr=" << utf8Str << std::endl;
        throw std::exception("WideCharToMultiByte failed");
    }

    std::wstring utf16Str(sizeNeeded - 1, 0);
    if (MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, &utf16Str[0], sizeNeeded) == 0)
    {
        const auto win32Err = GetLastError();
        std::cerr << "WideCharToMultiByte failed, Win32Err=" << win32Err << ", InputStr=" << utf8Str << std::endl;
        throw std::exception("WideCharToMultiByte failed");
    }

    return utf16Str;
}

std::string ConvertUtf16ToUtf8(LPCWSTR utf16Str) {

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8, 0, utf16Str, -1, nullptr, 0, nullptr, nullptr
    );
    if (sizeNeeded == 0)
    {
        const auto win32Err = GetLastError();
        std::wcerr << L"WideCharToMultiByte failed, Win32Err=" << win32Err << L", InputStr=" << utf16Str << std::endl;
        throw std::exception("WideCharToMultiByte failed");
    }

    std::string utf8Str(sizeNeeded - 1, 0);
    if (WideCharToMultiByte(CP_UTF8, 0, utf16Str, -1, &utf8Str[0], sizeNeeded, nullptr, nullptr) == 0)
    {
        const auto win32Err = GetLastError();
		std::wcerr << L"WideCharToMultiByte failed, Win32Err=" << win32Err << L", InputStr=" << utf16Str << std::endl;
		throw std::exception("WideCharToMultiByte failed");
    }

	return utf8Str;
}

int main()
{

    try {
        //コンソール出力用のUTF-8設定。コンソールアプリでなければ不要
        SetConsoleOutputCP(CP_UTF8);

        //Win32API用のロケール設定。Win10以降で環境を問わず使えるようにUTF-8とする
        if (setlocale(LC_ALL, ".UTF8") == nullptr)
        {
            std::wcerr << L"setlocale failed" << std::endl;
            return -1;
        }

        LPCWSTR filePath = L"サンプル.json";

        //UTF-8のjsonを読み込むためのロケール設定をする
        std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>());
        wptree pt;
        read_json(ConvertUtf16ToUtf8(filePath), pt, utf8_locale);

        //以下、各値の読み込みのテスト

        // Data.value
        if (boost::optional<int> value = pt.get_optional<int>(L"Data.value")) {
            std::wcout << L"value : " << value.get() << std::endl;
        }
        else {
            std::wcout << L"value is nothing" << std::endl;
        }

        // Data.str
        if (boost::optional<std::wstring> str = pt.get_optional<std::wstring>(L"Data.str")) {
            std::wcout << L"str : " << str.get() << std::endl;
        }
        else {
            std::wcout << L"str is nothing" << std::endl;
        }

        // Data.info
        BOOST_FOREACH(const wptree::value_type & child, pt.get_child(L"Data.info")) {
            const wptree& info = child.second;

            // Data.info.id
            if (boost::optional<int> id = info.get_optional<int>(L"id")) {
                std::wcout << L"id : " << id.get() << std::endl;
            }
            else {
                std::wcout << L"id is nothing" << std::endl;
            }

            // Data.info.name
            if (boost::optional<std::wstring> name = info.get_optional<std::wstring>(L"name")) {
                std::wcout << L"name : " << name.get() << std::endl;
            }
            else {
                std::wcout << L"name is nothing" << std::endl;
            }
        }

        return 0;
    }
    catch (std::exception& e)
    {
		std::cerr << "Exception: " << e.what() << std::endl;
		return -1;
	}
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
