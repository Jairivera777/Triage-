
#ifndef _CPPHTTPLIB_HTTPLIB_H_
#define _CPPHTTPLIB_HTTPLIB_H_
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <winsock2.h>
namespace httplib {
    struct Request { 
        std::string body; 
        std::map<std::string, std::string> params;
        bool tieneParametro(const std::string& key) const {
            return params.find(key) != params.end();
        }
        std::string getParametro(const std::string& key) const {
            auto it = params.find(key);
            if(it != params.end()) {
                return it->second;
            }
            return "";
        }
        bool pasarBool(const std::string str) const {
            return str=="true";
        }
    };    
    struct Response { 
        std::string body;
        std::string content_type;
        void set_header(const std::string& key, const std::string& value){

        }
        void set_content(const std::string& s, const char* type) { 
            body = s;
            content_type = type; 
        }
    };
    struct Server {
        std::function<void(const Request&, Response&)> handlerPost;
        std::function<void(const Request&, Response&)> handlerGet;

        void Get(const char* pattern, std::function<void(const Request&, Response&)> handler) {
            handlerGet = handler;
        }
        void Post(const char* pattern, std::function<void(const Request&, Response&)> handler) {
            handlerPost = handler;
        }
        void listen(const char* h, int p) { 
            std::cout << "Servidor de Triage iniciado en http://localhost:8080\nSimulación activa. Presiona Ctrl+C para salir.\n"; 
            while(true); 
        }
    };
}
#endif
