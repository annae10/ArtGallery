#define CURL_STATICLIB
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include <htmlcxx/html/ParserDom.h>

using namespace std;
using namespace htmlcxx;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int main()
{
	HTML::ParserDom parser;
	string url = "https://www.veranda.com/travel/g1436/best-museums-in-the-world/";
	string page;

    //load page into variable page
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = curl_easy_init();
	if (curl)
    {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page);

		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			//cout<< page <<endl;
		}
		else
		{
			cerr<<"Error: "<<curl_easy_strerror(res)<<endl;
		}
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

    //page parsing
	tree<HTML::Node> dom = parser.parseTree(page);
	vector<string>titles;
	for(auto it = dom.begin();it != dom.end(); ++it)
	{
		if(it->tagName() == "h2" )
		{
			string title = "";
			++it;
			while (it != dom.end() && !it->isTag())
			{
				title += it->text();
				++it;
			}	
			titles.push_back(title);
		}
	}

	for (const auto& title: titles)
	{
        if (title != ""){
		cout<<"Title: "<<title<<endl;
        }
	}	

	return 0;
}
