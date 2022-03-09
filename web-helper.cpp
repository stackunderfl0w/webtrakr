//
// Created by Pat on 8/30/2021.
//

#include <iostream>
#include "web-helper.h"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::vector<std::string> get_chapter_list(const char* address){
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, address);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        std::cout<<res<<std::endl;
        curl_easy_cleanup(curl);

        //std::cout << readBuffer << std::endl;
    }
    std::regex_iterator<std::string::iterator>::regex_type rx(
            "https?:\\/\\/(www\\.)?"\
  "[-a-zA-Z0-9@:%._\\+~#=]{1,256}"\
  "\\.[a-zA-Z0-9()]{1,6}\\b([-a-z"\
  "A-Z0-9()@:%_\\+.~#?&//=]*)");

    // stl iterator for looping through
    // all the links found in the string
    std::regex_iterator<std::string::iterator> n(readBuffer.begin(), readBuffer.end(), rx), e;

    // link counter
    std::vector<std::string> links;
    int lnkCounter = 0;

    while (n != e) {

        //std::cout << "Link# " << ++lnkCounter<<" "<<n->str().c_str()<<std::endl;

        //std::cout << "====\r\n";
        if (strstr(n->str().c_str(), "chapter")) {
            //links.emplace_back(n->str());
            //std::cout<<"accepted\n";
            links.emplace_back(n->str());
        }
        //std::cout << n->str() << "\r\n" << std::endl;
        /*char* x = strstr(n->str().c_str(), "chapter");
        if (x) {
            //links.emplace_back(n->str());
            links.emplace_back(x);
        }

        x = strstr(n->str().c_str(), ".jpg");
        if (x) {
            //links.emplace_back(n->str());
            std::cout<<n->str()<<std::endl;
        }*/

        n++;

    }
    return links;
}
std::vector<std::string> get_chapter_list_wuxia(const char* address){
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, address);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        std::cout<<res<<std::endl;
        curl_easy_cleanup(curl);

        //std::cout << readBuffer << std::endl;
    }
    std::regex_iterator<std::string::iterator>::regex_type rx(
            "\\/(novel)\\/[a-zA-Z]+\\/.+?(?=\")");

    // stl iterator for looping through
    // all the links found in the string
    std::regex_iterator<std::string::iterator> n(readBuffer.begin(), readBuffer.end(), rx), e;

    // link counter
    std::vector<std::string> links;
    int lnkCounter = 0;

    while (n != e) {

        //std::cout << "Link# " << ++lnkCounter;

        //std::cout << "====\r\n";
        if (strstr(n->str().c_str(), "chapter")) {
            //links.emplace_back(n->str());
            links.emplace_back("https://www.wuxiaworld.com"+n->str());
        }
        //std::cout << n->str() << "\r\n" << std::endl;
        /*char* x = strstr(n->str().c_str(), "chapter");
        if (x) {
            //links.emplace_back(n->str());
            links.emplace_back(x);
        }

        x = strstr(n->str().c_str(), ".jpg");
        if (x) {
            //links.emplace_back(n->str());
            std::cout<<n->str()<<std::endl;
        }*/

        n++;

    }
    return links;
}