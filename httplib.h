#ifndef _CPPHTTPLIB_HTTPLIB_H_
#define _CPPHTTPLIB_HTTPLIB_H_

#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib")

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
            return str == "true";
        }
    };    

    struct Response { 
        std::string body;
        std::string content_type;
        std::map<std::string, std::string> headers;

        void set_header(const std::string& key, const std::string& value) {
            headers[key] = value;
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

        std::string urlDecode(std::string str) {
            std::string ret;
            char ch;
            int ii;
            for (size_t i = 0; i < str.length(); i++) {
                if (str[i] == '+') {
                    ret += ' ';
                } else if (str[i] == '%' && i + 2 < str.length()) {
                    std::sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
                    ch = static_cast<char>(ii);
                    ret += ch;
                    i += 2;
                } else {
                    ret += str[i];
                }
            }
            return ret;
        }

        void listen(const char* host, int puerto) { 
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cout << "Error al inicializar Winsock\n";
                return;
            }

            SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket == INVALID_SOCKET) {
                std::cout << "Error al crear el socket\n";
                WSACleanup();
                return;
            }

            // Permitir reutilizar el puerto inmediatamente si se reinicia
            int opt = 1;
            setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

            sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = INADDR_ANY;
            serverAddr.sin_port = htons(puerto);

            if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                std::cout << "Error en bind (el puerto " << puerto << " podria estar ocupado)\n";
                closesocket(serverSocket);
                WSACleanup();
                return;
            }

            ::listen(serverSocket, SOMAXCONN);

            std::cout << "==========================================" << std::endl;
            std::cout << "Servidor C++ activo en http://localhost:" << puerto << std::endl;
            std::cout << "==========================================" << std::endl;

            while (true) {
                SOCKET clientSocket = accept(serverSocket, NULL, NULL);
                if (clientSocket == INVALID_SOCKET) continue;

                char buffer[8192] = {0};
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead <= 0) {
                    closesocket(clientSocket);
                    continue;
                }

                std::string peticion(buffer, bytesRead);
                Request req;
                Response res;

                // 1. Petición Preflight OPTIONS (CORS)
                if (peticion.find("OPTIONS") == 0) {
                    std::string resp = 
                        "HTTP/1.1 200 OK\r\n"
                        "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With\r\n"
                        "Content-Length: 0\r\n"
                        "Connection: close\r\n\r\n";
                    send(clientSocket, resp.c_str(), static_cast<int>(resp.length()), 0);
                }
                // 2. Petición POST
                else if (peticion.find("POST") != std::string::npos && handlerPost) {
                    size_t bodyPos = peticion.find("\r\n\r\n");
                    if (bodyPos != std::string::npos) {
                        req.body = peticion.substr(bodyPos + 4);
                        
                        std::stringstream ss(req.body);
                        std::string pareja;
                        while (std::getline(ss, pareja, '&')) {
                            size_t eqPos = pareja.find('=');
                            if (eqPos != std::string::npos) {
                                std::string clave = pareja.substr(0, eqPos);
                                std::string valor = pareja.substr(eqPos + 1);
                                req.params[clave] = urlDecode(valor);
                            }
                        }
                    }

                    handlerPost(req, res);

                    std::string resp = 
                        "HTTP/1.1 200 OK\r\n"
                        "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: Content-Type\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " + std::to_string(res.body.length()) + "\r\n"
                        "Connection: close\r\n\r\n" + res.body;

                    send(clientSocket, resp.c_str(), static_cast<int>(resp.length()), 0);
                }
                // 3. Petición GET
                else if (peticion.find("GET") != std::string::npos && handlerGet) {
                    handlerGet(req, res);

                    std::string resp = 
                        "HTTP/1.1 200 OK\r\n"
                        "Access-Control-Allow-Origin: *\r\n"
                        "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                        "Access-Control-Allow-Headers: Content-Type\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: " + std::to_string(res.body.length()) + "\r\n"
                        "Connection: close\r\n\r\n" + res.body;

                    send(clientSocket, resp.c_str(), static_cast<int>(resp.length()), 0);
                }

                closesocket(clientSocket);
            }
            WSACleanup();
        }
    };
}
#endif