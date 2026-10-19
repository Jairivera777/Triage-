
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
        struct FileResult{
             std::string content; 
        };
        FileResult get_file_value(const char* name) const { return { "CC" }; } };
    struct Response { void set_content(const std::string& s, const char* t) { std::cout << "Página cargada.\n"; } };
    struct Server {
        void Get(const char* r, std::function<void(const Request&, Response&)> f) {}
        void Post(const char* r, std::function<void(const Request&, Response&)> f) {
            
        }
        void listen(const char* h, int p) { 
            std::cout << "Servidor de Triage iniciado en http://localhost:8080\nSimulación activa. Presiona Ctrl+C para salir.\n"; 
            while(true); 
        }
    };
}
#endif
