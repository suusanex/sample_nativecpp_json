// sample_nativecpp_json_nlohmann.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <fstream>
#include <Windows.h>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct User {
	std::wstring name;
    int id;
};

struct Data {
    int value;
    std::wstring str;
    std::vector<User> info;
};

struct Config {
	Data data;
};



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

template<typename T>
bool get_value(const json& j, LPCSTR key, T& valueBuf) {
    if (!j.contains(key)) {
        return false;
    }

    valueBuf = j.at(key).get<T>();
    return true;
}

bool get_value(const json& j, LPCSTR key, std::wstring& valueBuf) {
    if (!j.contains(key)) {
        return false;
    }

    valueBuf = ConvertUtf8ToUtf16(j.at(key).get<std::string>().c_str());
    return true;
}

template<typename T>
bool get_array(const json& j, LPCSTR key, std::vector<T>& valueBuf) {
	if (!j.contains(key) || !j[key].is_array()) {
		return false;
	}

	valueBuf.clear();
    for (const auto& item : j[key]) {
        try {
            valueBuf.emplace_back(item.get<T>());
        }
        catch (const json::type_error& e) {
            std::cerr << "要素の型変換に失敗しました: " << e.what() << std::endl;
        }
    }

	return true;
}
void from_json(const json& j, User& u) {

	get_value(j, "id", u.id);
	get_value(j, "name", u.name);
}
void from_json(const json& j, Config& c) {

	get_value(j, "str", c.data.str);
	get_value(j, "value", c.data.value);
	get_array(j, "info", c.data.info);
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


        std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs.is_open()) {
            std::wcerr << L"file open failed: " << filePath << std::endl;
            return 1;
        }

		json j;
		ifs >> j;

        auto config = j.at("Data").get<Config>();
		// 読み込んだデータを表示
		std::wcout << L"str: " << config.data.str.c_str() << std::endl;
		std::wcout << L"value: " << config.data.value << std::endl;
        for (const auto & info : config.data.info)
        {
			std::wcout << L"id: " << info.id << std::endl;
			std::wcout << L"name: " << info.name.c_str() << std::endl;	        
        }


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
