#pragma once
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <regex>

namespace rudra_network{

    // Class Defenition
    class webHist{
    public:
        webHist(){
            header = NULL;  
            content = NULL;
        }
        char* name;
        char* header;
        char* content;
    };

    class user{
    public:
        user(){
            vip = false;
            *username = 0;
        }
        char username[0x50];
        bool vip;
    };

    // Helper functions 
    user MainUser;
    std::vector<webHist> entireHistory;
    std::vector<std::string> betaTester;

    void initiate(){
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stdout, NULL, _IONBF, 0);

        
        betaTester.push_back("BOB1");
        betaTester.push_back("BOB2");
        betaTester.push_back("BOB3");
        betaTester.push_back("BOB4");
    }

    bool checkBetaTester(std::string username){
        for(auto tester: betaTester){
            if(!username.compare(tester)){
                return true;
            }
        }
        return false;
    }

    void terror(std::string str){
        std::cout << str << std::endl;
        exit(0x1337);
    }

    void colored_print(std::string str, int num){
        if(num == 1){
            std::cout << "\033[1;32m" << str << "\033[0m";
            return;
        }
        else if(num == 2){
            std::cout << "\033[1;31m" << str << "\033[0m";
            return;
        }
        std::cout << str ;
    }

    void newline(){
        std::cout << std::endl;
    }

    void colored_print_size(const char* str, uint size, int num ){
        size = (((size) + ( 16 - 1)) & ~0x7);
        for(uint i=0;i<size;i++){
            if(str[i]){
                printf("%c", str[i]);
            }
        }
        newline();
    }

    bool check_size(const char * str, int size){
        if(strlen(str) <= size){
            return true;
        }
        return false;
    }

    std::string parse_request(std::string req_path, std::string req_host ){
        std::string request = "GET " + req_path + " HTTP/1.1\r\nHost: " + req_host + "\r\nConnection: close\r\n\r\n";
        return request;
    }

    int checkString(const char* given_string, const char* to_search_string){
        int j = 0;
        while(true){
            int i = 0;
            for(;i<strlen(to_search_string);i++){
                if((to_search_string[i] != given_string[j+i]) && (to_search_string[i] != std::tolower(given_string[j+i]))){
                    break;
                }
            }
            if(i == strlen(to_search_string)){
                return j;
            }
            j++;
        }
        return -1;
    }

    // Main functionalities
    // Register
    void RegsiterUserName(const char* name){
        if(strlen(name) >= 0x50){
            terror("Kindly, get a smaller name.");
        }
        if(*MainUser.username == 0){
            memcpy(MainUser.username, name, strlen(name));
            MainUser.vip = false;
        }
        else{
            char* name_dup = (char*)malloc(0x300);
            memcpy(name_dup, name, 0x100);
            // TODO: Give vip users name change option, this project is becoming huge, gotta hire some dudes.
        }
    }

    // website_hack
    void GetHostNameByIp(const char* ip){
        struct hostent *he;
        struct in_addr **addr_list;
        struct in_addr addr;

        inet_aton(ip, &addr);
        he = gethostbyaddr(&addr, sizeof(addr), AF_INET);

        colored_print("---------------------------------------\n", 1);
        colored_print(" [+] Host Information \n", 2);
        colored_print("---------------------------------------\n", 1);
        colored_print("Host name: ", 1);
        std::string som = he->h_name;
        colored_print(som, 0);
        newline();
    }


    // Function to expand sets in the format {value1,value2,...}
    std::vector<std::string> expandSet(const std::string &set) {
        std::vector<std::string> expanded;
        std::string value;
        for (char c : set) {
            if (c == ',') {
                expanded.push_back(value);
                value.clear();
            } else if(c != '{' && c != '}'){
                value.push_back(c);
            }
        }
        expanded.push_back(value); // Add the last value
        return expanded;
    }

    // Function to generate URLs based on the globbing pattern
    std::vector<std::string> generateURLs(const std::string &url) {
        std::vector<std::string> urls;
        std::regex setPattern(R"(\{([^{}]+)\})");

        std::smatch match;
        std::string currentUrl = url;

        // Sanity Check
        std::string::difference_type n1 = std::count(url.begin(), url.end(), '{');
        std::string::difference_type n2 = std::count(url.begin(), url.end(), '}');
        if(n1 > 1 || n1 != n2){
            terror("Something seems off. I am not as good as curl yet!");
        }

        // Expand sets like {value1,value2}
        while (std::regex_search(currentUrl, match, setPattern)) {
            std::string prefix = match.prefix();
            std::string suffix = match.suffix();
            auto expandedSet = expandSet(match.str());
            for (const auto &s : expandedSet) {
                urls.push_back(prefix + s + suffix);
            }
            currentUrl = urls[0];  
        }
        if (urls.empty()) {
            urls.push_back(url);
        }

        return urls;
    }

    // ip_hack
    void GetIpFromHost(const char* hostname){
        struct hostent *he;
        struct in_addr **addr_list;
        struct in_addr addr;

        he = gethostbyname(hostname);
        if (he == NULL) { 
            terror("Something did'nt go right");
        }

        colored_print("---------------------------------------\n", 1);
        colored_print(" [+] IP Information \n", 2);
        colored_print("---------------------------------------\n", 1);

        colored_print(" [+] Official name is: ", 1);
        colored_print(he->h_name, 0);
        newline();
        colored_print(" [+] IP address: ", 1); 
        colored_print(inet_ntoa(*(struct in_addr*)he->h_addr), 2);
        newline();
        colored_print(" [+] All IP addresses:\n", 1);
        addr_list = (struct in_addr **)he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++) {
            colored_print(" (", 1);
            colored_print(std::to_string(i+1), 1);
            colored_print(") ", 1);
            colored_print(inet_ntoa(*addr_list[i]), 2);
            newline();
        }
    }

    // content
    void GetContentFromWebsite(const char* hostname, const char* path){
        if(strlen(hostname) >= 0x50 || *path == (char)(0)){
            terror("Something deffo ain't right. ");
        }
        int soc_fd = socket(AF_INET, SOCK_STREAM, 0);
        struct hostent *server;
        struct sockaddr_in serv_addr;
        webHist tmp; 
        char buffer[0x500];
        
        if (soc_fd < 0){
            terror("Something did not go right! ");
        }
        
        server = gethostbyname(hostname);
        memset((char*)&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(80);
        memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length); 
        
        if (connect(soc_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            terror("Something did not go right! ");
        }

        std::string req_path(path);
        std::string req_host(hostname);
        std::string request(parse_request(req_path, req_host));
        if (send(soc_fd, request.c_str(), request.size(), 0) < 0){
            terror("Something did not go right! ");
        }
        int n;  
        std::string raw_string;
        while ((n = recv(soc_fd, buffer, sizeof(buffer), 0)) > 0){
            raw_string.append(buffer, n);
        }
        close(soc_fd);

        colored_print("---------------------------------------\n", 1);
        colored_print(" [+] Content Information \n", 2);
        colored_print("---------------------------------------\n", 1);
        
        std::string endingString("<html");
        int size = checkString(raw_string.c_str(), endingString.c_str());
        colored_print_size(raw_string.substr(size, raw_string.length()-size).c_str(), (raw_string.length()-size), 1);

        tmp.header = NULL;
        tmp.name = (char*)malloc(0x50);
        memset(tmp.name, 0x0, 0x50);
        tmp.content = (char*) malloc(raw_string.length()-size);
        memcpy(tmp.name, hostname, strlen(hostname));
        memcpy(tmp.content, raw_string.substr(size+6, raw_string.length()-size-6).c_str(), (raw_string.length()-size));
        entireHistory.push_back(tmp);
    }

    // header
    void GetHeaderFromWebsite(const char* hostname, const char* path){
        if(strlen(hostname) >= 0x50 || *path == (char)(0)) {
            terror("Something did not go right! ");
        }

        int soc_fd = socket(AF_INET, SOCK_STREAM, 0);
        struct hostent *server;
        struct sockaddr_in serv_addr;
        webHist tmp;
        int size = 0;

        char buffer[0x5000];

        if (soc_fd < 0){
            terror("Something did not go right! ");
        }

        server = gethostbyname(hostname);
        memset((char*)&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(80);
        memcpy((char*)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length); 
        
        if (connect(soc_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            terror("Something did not go right! ");
        }

        std::string req_path(path);
        std::string req_host(hostname);
        std::string request(parse_request(req_path, req_host));
        if (send(soc_fd, request.c_str(), request.size(), 0) < 0){
            terror("Something did not go right! ");
        }
        int n;
        std::string raw_string("");
        while ((n = recv(soc_fd, buffer, sizeof(buffer), 0)) > 0){
            raw_string.append(buffer, n);
        }        
        tmp.header = (char*)malloc(raw_string.length());
        memset(tmp.header, 0x0, raw_string.length());
        tmp.name = (char*)malloc(0x50);
        memset(tmp.name, 0x0, 0x50);
        close(soc_fd);
        std::string endingString("<html");

        size = checkString(raw_string.c_str(), endingString.c_str());

        colored_print("---------------------------------------\n", 1);
        colored_print(" [+] Header Information\n", 2);
        colored_print("---------------------------------------\n", 1);
        colored_print_size(raw_string.c_str(), size, 0); 

        if(raw_string.length() < 0x5000){
            memcpy(tmp.name, hostname, strlen(hostname));
            memcpy(tmp.header, raw_string.c_str(), size);
            tmp.content = NULL;
            entireHistory.push_back(tmp);
        }    
        else{
            terror("Give me lesser header information.");
        }    
    }
   
    void url_globbing(std::string req_url){
        if(*MainUser.username == (char)(0) ||  !checkBetaTester(MainUser.username)){
            terror("Become a VIP member today at just $1.99.");
        }
        std::vector<std::string> urls = generateURLs(req_url);
        for(auto url : urls){
            rudra_network::GetContentFromWebsite(url.c_str(), "/");
        }
    }

    void printWebsiteHistory(){
        for(auto webs: entireHistory){
            colored_print("Name: ", 1);
            colored_print(webs.name, 2);
            newline();

            if(webs.header != NULL){
                colored_print("---------------------------------------\n", 1);
                colored_print(" [+] Header Information\n", 2);
                colored_print("---------------------------------------\n", 1);
                colored_print(webs.header, 0);
                newline();
            }
            if(webs.content != NULL){
                colored_print("---------------------------------------\n", 1);
                colored_print(" [+] Content Information\n", 2);
                colored_print("---------------------------------------\n", 1);
                colored_print(webs.content, 0);
                newline();
            }
        }
    }

    void feedback(){

        // TODO: Allow multiple feedbacks and store them in cloud. Give some prompt to ask feedback.
        char* name = (char*)malloc(0x180);
        char* will_you_recomend = (char*)malloc(0x300);
        char* like_our_service = (char*)malloc(0x180);
        char* will_use_this_instead_of_curl = (char*)malloc(0x300);
        char* should_we_opensource = (char*)malloc(0x180);
        char* complaints = (char*)malloc(0x300); // No input required.

        scanf("%69s", name);
        scanf("%69s", like_our_service);
        scanf("%420s", will_you_recomend);
        scanf("%420s", will_use_this_instead_of_curl);

        free(will_you_recomend);
        free(should_we_opensource);
        free(complaints);
        free(like_our_service);
        free(will_use_this_instead_of_curl);
        free(name);
    }
}

/*
TODO:
- Hire Software Dev to build VIP features.
- Hire a Security Guy to test the functionality.
- Make it unexploitable for people :D
*/