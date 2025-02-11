#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Banner
void banner() {
    std::cout << R"(
 _______ _     _ ______  _     _
 |______ |     | |_____]  \___/ 
 ______| |_____| |_____] _/   \_
                                )" << std::endl;
    std::cout << "By CYSECTOOLS" << std::endl;


    std::cout << "*************************" << std::endl;
    std::cout << "* [1] - Continue        *" << std::endl;
    std::cout << "* [2] - Exit            *" << std::endl;
    std::cout << "*************************" << std::endl;
}

// Callback function to handle HTTP response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Function to fetch subdomains from crt.sh
std::vector<std::string> getSubdomainsFromCrtSh(const std::string& domain) {
    std::vector<std::string> subdomains;
    std::string url = "https://crt.sh/?q=%25." + domain + "&output=json";
    std::string response;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0"); // Avoid detection from bot
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
            return subdomains;
        }
    }

    try {
        auto jsonResponse = json::parse(response);
        for (const auto& entry : jsonResponse) {
            if (entry.contains("name_value")) {
                std::string subdomain = entry["name_value"];
                if (std::find(subdomains.begin(), subdomains.end(), subdomain) == subdomains.end()) {
                    subdomains.push_back(subdomain);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
    }

    return subdomains;
}

int main() {

    int choice;
    banner(); 
    std::cout << std::endl;
    std::cout << "Choose an option: ";
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "\n";
        std::string domain;
        std::cout << "Enter domain to search subdomains (or 'x' to exit): ";
        std::cin >> domain;

        if (domain == "x") return 0;

        std::vector<std::string> subdomains = getSubdomainsFromCrtSh(domain);
        std::cout << "\nFound Subdomains:\n";
        for (const auto& sub : subdomains) {
            std::cout << sub << std::endl;
        }
    }
    if (choice == 2) return 0;

    return 0;
}
