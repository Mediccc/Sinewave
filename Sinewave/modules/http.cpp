#include "http.h"


size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
	return written;
}


HttpResponse Http::newRequest(const std::string& url, const std::string& method, std::optional<std::unordered_map<std::string, std::string>> headers) {
	CURL* curl = curl_easy_init();
	HttpResponse response;
	CURLcode res;

	std::string readBuffer;

	struct curl_slist* chunk = NULL;

	if (headers) {
		for (auto it : headers.value()) {
			std::string h = it.first + ": " + it.second;
			curl_slist_append(chunk, h.c_str());
		}
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.content);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	res = curl_easy_perform(curl);

	curl_slist_free_all(chunk);
	curl_easy_cleanup(curl);

	return response;
}

void Http::downloadFile(const std::string& url, const std::string& fileName) {
	CURL* curl = curl_easy_init();
	FILE* file;
	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

	file = fopen(fileName.c_str(), "wb");
	if (file) {
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
		curl_easy_perform(curl);
		fclose(file);
		Logger::log(Logger::SUCCESS, "Finished installing " + fileName);
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();
}
