#pragma once
#include "server.hpp"
#include "threadpool.hpp"
#include "dsalgo.hpp"
#include <cstdio>
#include <iterator>
#include <utility>
#include <regex>

#define HTTP_MAX_HEADER_SIZE 8 * 1024 // 8KB
#define HTTP_HEADER_SEPARATOR "\r\n"
#define HTTP_SEPARATOR "\r\n\r\n"

namespace sp
{

    constexpr const uint32_t HTTP_METHOD_GET = 0;
    constexpr const uint32_t HTTP_METHOD_POST = 1;
    constexpr const uint32_t HTTP_METHOD_PUT = 2;
    constexpr const uint32_t HTTP_METHOD_DELETE = 3;
    constexpr const uint32_t HTTP_METHOD_HEAD = 4;
    constexpr const uint32_t HTTP_METHOD_OPTIONS = 5;
    constexpr const uint32_t HTTP_METHOD_TRACE = 6;
    constexpr const uint32_t HTTP_METHOD_CONNECT = 7;
    constexpr const uint32_t HTTP_INVALID = -1;

    static uint32_t HTTPgetMethod(const char *method)
    {
        if (strcmp(method, "GET") == 0)
            return HTTP_METHOD_GET;
        if (strcmp(method, "POST") == 0)
            return HTTP_METHOD_POST;
        if (strcmp(method, "PUT") == 0)
            return HTTP_METHOD_PUT;
        if (strcmp(method, "DELETE") == 0)
            return HTTP_METHOD_DELETE;
        if (strcmp(method, "HEAD") == 0)
            return HTTP_METHOD_HEAD;
        if (strcmp(method, "OPTIONS") == 0)
            return HTTP_METHOD_OPTIONS;
        if (strcmp(method, "TRACE") == 0)
            return HTTP_METHOD_TRACE;
        if (strcmp(method, "CONNECT") == 0)
            return HTTP_METHOD_CONNECT;
        return HTTP_INVALID;
    }


    struct URL
    {
        char* protocol = nullptr;
        char* host = nullptr;
        char* path = nullptr;
        char* query = nullptr;
        char* fragment = nullptr;

        void create(const char* url)
        {
            char* urlcopy = strdup(url);
            char* protocolEnd = strstr(urlcopy, "://");
            if(protocolEnd != nullptr)
            {
                *protocolEnd = '\0';
                protocol = urlcopy;
                urlcopy = protocolEnd + 3;
            }
            char* pathStart = strchr(urlcopy, '/');
            if(pathStart != nullptr)
            {
                *pathStart = '\0';
                host = urlcopy;
                path = pathStart + 1;
            }
            char* queryStart = strchr(path, '?');
            if(queryStart != nullptr)
            {
                *queryStart = '\0';
                query = queryStart + 1;
            }
            char* fragmentStart = strchr(query, '#');
            if(fragmentStart != nullptr)
            {
                *fragmentStart = '\0';
                fragment = fragmentStart + 1;
            }
        }
        void print()
        {
            printf("Protocol: %s\n", protocol);
            printf("Host: %s\n", host);
            printf("Path: %s\n", path);
            printf("Query: %s\n", query);
            printf("Fragment: %s\n", fragment);
        }
    };

    struct HTTPHeaders
    {
        char *_content;
        uint64_t _size;
        std::vector<char *> _headerPtrs;

        void create(char *content)
        {
            _content = content;
            char* contentEnd = strstr(content, HTTP_SEPARATOR);
            strcpy(contentEnd, "\0");
            _size = sizeof(content);
            char *line = content;
            char * lastfour = content + _size - 4;

            while(line)
            {
                char *lineend = strstr(line, HTTP_HEADER_SEPARATOR);
                if(lineend == nullptr) 
                    break;
                char *key = strtok(line, ": ");
                char *value = strtok(nullptr, HTTP_HEADER_SEPARATOR);
                _headerPtrs.push_back(key);
                _headerPtrs.push_back((char*)ltrim(value));
                line = lineend + 2;
            }

        }
        char* get(const std::string &key)
        {
            for (uint32_t i = 0; i < _headerPtrs.size(); i += 2)
            {
                if (strcmp(_headerPtrs[i], key.c_str()) == 0)
                    return _headerPtrs[i + 1];
            }
            return nullptr;
        }

        static std::string make(const char *keys, const char *values, uint32_t count)
        {
            std::string result;
            for (uint32_t i = 0; i < count; ++i)
            {
                result += keys[i];
                result += ": ";
                result += values[i];
                result += HTTP_HEADER_SEPARATOR;
            }
            return result;
        }

             int32_t count() const
        {
            return _headerPtrs.size() / 2;
        };
    };

    struct HTTPRequest
    {
        uint32_t _method = -1;
        char* _version = nullptr;
        uint32_t _contentLength = 0;
        char* _contentType = nullptr;
        char* _path = nullptr;
        char* _userAgent = nullptr;
        char clientIP[16] = {0};
        URL url;



        HTTPHeaders _headers;

        int32_t _clientSocket = 0;
        sockaddr_in _clientAddress = {};

        void create(const int32_t & clientSocket,const sockaddr_in & clientAddress)
        {
            _clientSocket = clientSocket;
            _clientAddress = clientAddress;
            inet_ntop(AF_INET, &_clientAddress.sin_addr, clientIP, sizeof(clientIP));
        };

        void __processRequest(char *buffer, uint32_t size)
        {
            int32_t bytesReceived = read(_clientSocket, buffer, size - 1);
            if (bytesReceived == -1)
            {
                fprintf(stderr, "err:: Failer to receive data from client\n");
                return;
            }
            buffer[size] = '\0';
            char *start = buffer;
            char *lineend = strstr(buffer, HTTP_HEADER_SEPARATOR);
            char *method = strtok(start, " ");
            _method = HTTPgetMethod(method);
            _path = strtok(nullptr, " ");
            _version = strtok(nullptr, HTTP_SEPARATOR);
            char* header_token = lineend + 2;
            _headers.create(header_token);
            char * cl = _headers.get("Content-Length"); 
            if(cl != nullptr)
            {
                _contentLength = atoi(cl);
            }
            _contentType = _headers.get("Content-Type");
            _userAgent = _headers.get("User-Agent");
            
            url.create(_path);

            printf("%.7s [ %5.2f kb ]-> %s\n", method, (float)_contentLength/1024, _path);
            
        };

        void print()
        {
        };
    };

    struct HTTPResponse
    {
        int _clientSocket = 0;

        void create(int clientSocket)
        {
            _clientSocket = clientSocket;
        };

        void send(const char *data, uint32_t size)
        {
            ::send(_clientSocket, data, size, 0);
        };

        void end()
        {
            close(_clientSocket);
        };

        void sendHelloWorld()
        {
            const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nHello World!";
            send(response, strlen(response));
        };
    };

    struct HTTPJob
    {
        HTTPRequest request;
        HTTPResponse response;
    };

    struct HTTPServer
    {
        Server _server;
        ThreadPool<HTTPJob> _threadpool;

        bool create(uint16_t port, uint32_t threadCount = std::thread::hardware_concurrency())
        {
            sp::ServerConfig config;
            config.port = port;
            if(!_server.create(config))
                return false;

            _threadpool._onInit = [](void **dataptr)
            {
                *dataptr = malloc(HTTP_MAX_HEADER_SIZE);

                char *buffer = (char *)(*dataptr);
                strcpy(buffer, "Hello");
                buffer[HTTP_MAX_HEADER_SIZE - 1] = '\0';
            };

            _threadpool._onDestroy = [](void *dataptr)
            {
                free(dataptr);
            };

            _threadpool.create([&](HTTPJob &job, void *dataptr)
                    {
                    char *buffer = (char *)dataptr;
                    job.request.__processRequest(buffer, HTTP_MAX_HEADER_SIZE - 1);
                    job.request.print();


                    job.response.sendHelloWorld();
                    job.response.end(); },
                    threadCount);
            return true;
        };

        void destroy()
        {
            _threadpool.destroy();
            _server.destroy();
        };

        void listen()
        {
            _server.start();
            printf("server started successfully.\n");
            while (true)
            {
                int32_t clientSocket = 0;
                sockaddr_in clientAddress = {};
                if (_server.acceptClient(clientSocket, clientAddress))
                {
                    HTTPJob job;
                    job.request.create(clientSocket, clientAddress);
                    job.response.create(clientSocket);
                    _threadpool.push(std::move(job));
                }
            }
        };
    };
};
