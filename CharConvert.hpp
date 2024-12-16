#include <windows.h>
#include <string>

// �� std::wstring ת��Ϊ UTF-8 ����� std::string
std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(size_needed - 1, '\0'); // ȥ��β���� '\0'
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size_needed, nullptr, nullptr);

    return result;
}

// �� UTF-8 ����� std::string ת��Ϊ std::wstring
std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(size_needed - 1, '\0'); // ȥ��β���� '\0'
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size_needed);

    return result;
}
