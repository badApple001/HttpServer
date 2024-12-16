#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING


#include "Http/httplib.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include "GetHostAddress.hpp"
#include "CharConvert.hpp"
#include <windows.h> // Windows 平台特定头文件
#include <locale>
#include <codecvt>
namespace fs = std::filesystem;

const std::string ROOT_DIR = "./shared"; // 根目录路径

// 生成文件列表的 HTML
std::string generate_file_list(const std::string& dir_path) {
	std::string html = "<html><head><title>File Browser</title></head><body>";
	html += "<h1>Directory: " + dir_path + "</h1>";
	html += "<ul>";

	for (const auto& entry : fs::directory_iterator(dir_path)) {
		auto filename = entry.path().filename().string();
		if (entry.is_directory()) {
			html += "<li><a href='" + filename + "/'>" + filename + "/</a></li>";
		}
		else {
			html += "<li><a href='" + filename + "'>" + filename + "</a></li>";
		}
	}

	html += "</ul></body></html>";
	return html;
}

// 读取文件内容
std::string read_file(const std::string& file_path) {
	std::ifstream file(file_path, std::ios::binary);
	if (!file.is_open()) {
		return "";
	}
	std::string content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return content;
}

std::wstring wread_file(const std::wstring& wfile_path) {

	std::wifstream wfile(wfile_path, std::ios::binary);

	if (!wfile.is_open()) {
		return L"Error: Unable to open file.";
	}

	//wfile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>)); // 设置 UTF-8 读取
	std::wstring content((std::istreambuf_iterator<wchar_t>(wfile)),
		std::istreambuf_iterator<wchar_t>());
	wfile.close();
	return content;
}



struct FileInfo {

public:
	std::wstring name;
	size_t size;
};


std::string get_content_type(const std::string& file_path);
std::vector<FileInfo> get_filtered_files(const std::wstring& directory) {

	std::vector<FileInfo> files;

	try {
		// 检查路径是否存在且是目录
		if (!fs::exists(directory)) {
			std::wcerr << L"Error: Directory does not exist: " << directory << std::endl;
			return files;
		}
		if (!fs::is_directory(directory)) {
			std::wcerr << L"Error: Path is not a directory: " << directory << std::endl;
			return files;
		}

		// 遍历目录，过滤 .exe 和 .apk 文件
		for (const auto& entry : fs::directory_iterator(directory)) {
			if (entry.is_regular_file()) {
				std::wstring extension = entry.path().extension().wstring();
				if (extension == L".exe" || extension == L".apk") {
					std::wstring filename = entry.path().filename().wstring();
					FileInfo fileInfo;
					fileInfo.name = filename;
					fileInfo.size = entry.file_size();
					files.push_back(fileInfo);
				}
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		// 捕获并打印文件系统相关异常
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		// 捕获并打印其他异常
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return files;
}


std::string get_api_path(const std::string& path) {
	// 找到 "/api/" 的位置
	size_t pos = path.find("api/");

	// 如果没有找到"/api/"，返回空字符串
	if (pos != std::string::npos) {
		// 返回"/api/"之后的字符串部分
		return path.substr(pos + 4); // +5 是为了跳过 "/api/" 部分
	}

	// 如果没有找到"/api/"，返回空字符串
	return "";
}

void to_lower(std::string& str) {
	for (char& c : str) {
		c = std::tolower(static_cast<unsigned char>(c));  // 转换每个字符为小写
	}
}
 
//
//std::string wstring_to_u8(const std::wstring& wstr) {
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//	return converter.to_bytes(wstr);
//}

//
//// 将 std::wstring 转换为 UTF-8 编码的 std::string
//std::string wstring_to_utf8(const std::wstring& wstr) {
//	if (wstr.empty()) return std::string();
//
//	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
//	std::string result(size_needed - 1, '\0'); // 去掉尾部的 '\0'
//	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
//
//	return result;
//}

size_t getFileSize(const std::string& filePath) {
	try {
		return std::filesystem::file_size(filePath);
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error getting file size: " << e.what() << std::endl;
		return 0;
	}
}


int main() {
	// 设置控制台代码页为 UTF-8
	SetConsoleOutputCP(CP_UTF8);
	std::setlocale(LC_ALL, ".UTF-8");



	httplib::Server server;

	// 文件列表和访问
	server.Get(R"(/(.*))", [](const httplib::Request& req, httplib::Response& res) {

		std::string relative_path = req.matches.size() > 1 ? req.matches[1].str() : "";
		if (relative_path.empty()) relative_path = ".";

		std::string path = ROOT_DIR + "/" + relative_path;
		if (path.back() == '/') path.pop_back();

		// 打印访问者信息
		std::cout << req.remote_addr << "\t";
		std::cout << path << "\n";

		//存在api字样
		if (relative_path.find("api/") != std::string::npos) {

			auto api = get_api_path(relative_path);
			to_lower(api);  // 转换为小写

			std::cout << "call api: " << api << std::endl;
			if (api == "files") {

				std::vector<FileInfo> files = get_filtered_files(L"./shared/bin");
				if (files.size() > 0) {
					for (auto file : files) {
						std::wcout << "\t" << file.name << std::endl;
					}
				}

				std::wstring json_response = L"{ \"files\": [";
				for (size_t i = 0; i < files.size(); ++i) {

					json_response += L"{";
					json_response += L"\"name\" : \"" + files[i].name + L"\",";


					std::ostringstream oss;
					oss << files[i].size / 1024;
					std::string text = oss.str();
					std::wstring wtext = utf8_to_wstring(text);
					json_response += L"\"size\": " + wtext;

					json_response += L"}";
					if (i < files.size() - 1) json_response += L", ";
				}
				json_response += L"] }";
				res.status = 200;

				//std::string utf8text = wstring_to_u8(json_response);
				std::string utf8text = wstring_to_utf8(json_response);
				res.set_content(utf8text, "text/plain; charset=utf-8"); // 生成目录列表
			}
		}
		else {

			if (fs::is_directory(path)) {
				// 检查是否存在 index.html
				std::string index_file = path + "/index.html";

				if (fs::exists(index_file) && fs::is_regular_file(index_file)) {
					res.set_content(read_file(index_file), "text/html"); // 加载并展示 index.html
				}
				else {
					res.set_content(generate_file_list(path), "text/html"); // 生成目录列表
				}
			}
			else if (path == "./shared/download") {

				if (req.has_param("file")) {

					auto filename = req.get_param_value("file");
					std::cout << "download: " << filename << std::endl;

					// 如果是文件，返回其内容
					std::string content_type = get_content_type(path);
					std::string content_data = read_file("./shared/bin/" + filename);

					if (content_data == "") {

						auto content = wread_file(L"./shared/bin/" + utf8_to_wstring(filename));
						content_data = wstring_to_utf8(content);
					}
					res.set_content(content_data, content_type);
				}
			}
			else if (fs::is_regular_file(path)) {

				// 如果是文件，返回其内容
				std::string content_type = get_content_type(path);
				res.set_content(read_file(path), content_type);
			}
			else {
				res.status = 404;
				res.set_content("404 Not Found", "text/plain");
			}
		}

		});


	// 启动服务器
	std::string ip = get_local_address();
	std::cout << "Starting server on " << ip << ":8080" << std::endl;
	server.listen("0.0.0.0", 8080);

	return 0;
}

std::string get_content_type(const std::string& file_path) {
	if (file_path.ends_with(".html")) return "text/html";
	if (file_path.ends_with(".css")) return "text/css";
	if (file_path.ends_with(".js")) return "application/javascript";
	if (file_path.ends_with(".png")) return "image/png";
	if (file_path.ends_with(".jpg") || file_path.ends_with(".jpeg")) return "image/jpeg";
	return "text/plain"; // 默认类型
}