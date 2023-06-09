﻿#define CURL_STATICLIB
#include <iostream>
#include <curl/curl.h>


static size_t getResponsetoString(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
  
    CURL* curl;
    CURLcode response;
    std::string str_response;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.museumsassociation.org/museums-journal/news/#");
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getResponsetoString);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_response);
    response = curl_easy_perform(curl);
    std::cout << response << "\n";
    curl_easy_cleanup(curl);

    return 0;
}

