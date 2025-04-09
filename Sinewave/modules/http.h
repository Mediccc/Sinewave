#pragma once
#pragma warning(disable: 4996)
#include <iostream>
#include <curl/curl.h>
#include <unordered_map>
#include <optional>
#include "logger.h"

struct HttpResponse {
	std::unordered_map<std::string, std::string> headers;
	std::string content;
};

class Http
{
public:
	static HttpResponse newRequest(const std::string& url, const std::string& method, std::optional<std::unordered_map<std::string, std::string>> headers = std::nullopt);
	static void downloadFile(const std::string& url, const std::string& fileName);
};

