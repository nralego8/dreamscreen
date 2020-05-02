#include <stdio.h>
#include <curl/curl.h>
 
int main(int argc, char **argv)
{
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;
	
	char *url = "http://192.168.1.32:16021/api/v1/Wl2aru89oF8d7eket1cqqsv0fyEcgYhc/state";
	char *json = "{\"on\": {\"value\":true}, \"hue\":{\"value\":199}, \"sat\":{\"value\":100}}";
	curl = curl_easy_init();
	if(curl) {
		headers = curl_slist_append(headers, "Content-Type: application/json");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);
	return 0;
}
