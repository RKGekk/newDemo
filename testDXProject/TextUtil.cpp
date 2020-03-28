#include "TextUtil.h"

std::string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
    return std::string(buffer).substr( 0, pos);
}

std::wstring s2ws(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::string ws2s(const std::wstring& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0); 
    char* buf = new char[len];
    WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0); 
    std::string r(buf);
    delete[] buf;
    return r;
}

std::vector<std::string> splitR(const std::string& input, const std::string& regex) {
    std::regex re(regex);
    std::sregex_token_iterator first(input.begin(), input.end(), re, -1);
	std::sregex_token_iterator last;
    return std::vector<std::string>(first, last);
}

void split(const std::string &in, std::vector<std::string> &out, std::string token) {
	out.clear();
	std::string temp;
	int sz = int(in.size());
	for (int i = 0; i < sz; ++i) {
		std::string test = in.substr(i, token.size());
		if (test == token) {
			if (!temp.empty()) {
				out.push_back(temp);
				temp.clear();
				i += (int)token.size() - 1;
			}
			else {
				out.push_back("");
			}
		}
		else if (i + token.size() >= in.size()) {
			temp += in.substr(i, token.size());
			out.push_back(temp);
			break;
		}
		else {
			temp += in[i];
		}
	}
}