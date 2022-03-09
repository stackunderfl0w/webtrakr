//
// Created by Pat on 8/30/2021.
//

#ifndef WEBTRAKR_WEB_HELPER_H
#define WEBTRAKR_WEB_HELPER_H
#include <vector>
#include <string>
#include <curl/curl.h>
#include <regex>

std::vector<std::string> get_chapter_list(const char* address);
std::vector<std::string> get_chapter_list_wuxia(const char* address);
#endif //WEBTRAKR_WEB_HELPER_H
