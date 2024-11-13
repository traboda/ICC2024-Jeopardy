#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <netdb.h>

#include "./runtime.hpp"

int main(){

    std::cout << "\033[1;31m [-] RUDRA [-] \033[0m \033[1;32mOne stop website tester\033[0m\n";
    rudra_network::initiate();
    while(0x1337){
        std::string inp;
        std::cout << "\033[1;31mCommand: \033[0m";
        std::cin >> inp;
        if(!strcmp(inp.substr(0, 4).c_str(), "help")){
            std::cout << "\033[1;31m [+] Help Menu\033[0m\n";
            std::cout << "\033[1;32m [-] ip_hack        : \033[0m Give your domain, get the info Regarding the same.\n";
            std::cout << "\033[1;32m [-] website_hack   : \033[0m Give me the ip address, I will give you the information.\n";
            std::cout << "\033[1;32m [-] header         : \033[0m Give me the Domain, I will give you the Content it has.\n";
            std::cout << "\033[1;32m [-] content        : \033[0m Give me the Domain, I will give you the headers it contains.\n";
            std::cout << "\033[1;32m [-] url_glob(vip)  : \033[0m Get the content of multiple websites.\n";
            std::cout << "\033[1;32m [-] register       : \033[0m Register the account for yourself\n";
            std::cout << "\033[1;32m [-] history        : \033[0m Get all the websites you visited.\n";
            std::cout << "\033[1;32m [-] help           : \033[0m You know what it is :>\n";
        }
        else if(!strcmp(inp.substr(0, 7).c_str(), "ip_hack")){
            std::string website_name;
            std::cout << "\033[1;32mWebsite: \033[0m";
            std::cin >> website_name;
            rudra_network::GetIpFromHost(website_name.c_str());
        }
        else if(!strcmp(inp.substr(0, 12).c_str(), "website_hack")){
            std::string website_name;
            std::cout << "\033[1;32mWebsite: \033[0m";
            std::cin >> website_name;
            rudra_network::GetHostNameByIp(website_name.c_str());
        }
        else if(!strcmp(inp.substr(0, 6).c_str(), "header")){
            std::string website_name, website_path;
            std::cout << "\033[1;32mWebsite: \033[0m";
            std::cin >> website_name;
            std::cout << "\033[1;32mPath to file: \033[0m";
            std::cin >> website_path;
            rudra_network::GetHeaderFromWebsite(website_name.c_str(), website_path.c_str());
        }
        else if(!strcmp(inp.substr(0, 7).c_str(), "content")){
            std::string website_name, website_path;
            std::cout << "\033[1;32mWebsite: \033[0m";
            std::cin >> website_name;
            std::cout << "\033[1;32mPath to file: \033[0m";
            std::cin >> website_path;
            rudra_network::GetContentFromWebsite(website_name.c_str(), website_path.c_str());
        }
        else if(!strcmp(inp.substr(0, 8).c_str(), "register")){
            std::string register_name;
            std::cout << "\033[1;32mUsername: \033[0m";
            std::cin >> register_name;
            rudra_network::RegsiterUserName(register_name.c_str());
        }
        else if(!strcmp(inp.substr(0, 7).c_str(), "history")){
            rudra_network::printWebsiteHistory();
        }
        else if(!strcmp(inp.substr(0, 8).c_str(), "url_glob")){
            std::string website_name;
            std::cout << "\033[1;32mWebsite: \033[0m";
            std::cin >> website_name;
            rudra_network::url_globbing(website_name);
        }
        else if(!strcmp(inp.substr(0, 8).c_str(), "feedback")){
            rudra_network::feedback();
        }
        else{            
            rudra_network::terror("Know your options!");
        }
    }
}