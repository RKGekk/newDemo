#ifndef TEXTUTIL_H
#define TEXTUTIL_H

#include <regex>
#include <string>
#include <windows.h>

std::string ExePath();

std::wstring s2ws(const std::string& s);

std::string ws2s(const std::wstring& s);

std::vector<std::string> splitR(const std::string& input, const std::string& regex);

void split(const std::string &in, std::vector<std::string> &out, std::string token);

#endif