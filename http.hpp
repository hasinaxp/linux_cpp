#pragma once
#include "server.hpp"
#include "threadpool.hpp"
#include "dsalgo.hpp"
#include <cstdio>
#include <iterator>
#include <utility>
#include <regex>

#define HTTP_MAX_HEADER_SIZE 16 * 1024        // 16KB
#define HTTP_RESPONSE_BUFFER_SIZE 1024 * 1024 // 1MB
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

    struct HTTPHeaders
    {
        char *_content;
        uint64_t _size;
        std::vector<char *> _headerPtrs;

        void create(char *content)
        {
            _content = content;
            char *contentEnd = strstr(content, HTTP_SEPARATOR);
            strcpy(contentEnd, "\0");
            _size = sizeof(content);
            if (_size == 0)
                return;
            char *line = content;

            while (line)
            {
                char *lineend = strstr(line, HTTP_HEADER_SEPARATOR);
                if (lineend == nullptr)
                    break;
                char *key = strtok(line, ":");
                char *value = strtok(nullptr, HTTP_HEADER_SEPARATOR);
                _headerPtrs.push_back(key);
                _headerPtrs.push_back((char *)ltrim(value));
                line = lineend + 2;
            }
        }
        char *get(const std::string &key)
        {
            for (uint32_t i = 0; i < _headerPtrs.size(); i += 2)
            {
                if (strcmp(_headerPtrs[i], key.c_str()) == 0)
                    return _headerPtrs[i + 1];
            }
            return nullptr;
        }

        void set(char *key, char *&value)
        {
            for (uint32_t i = 0; i < _headerPtrs.size(); i += 2)
            {
                if (strcmp(_headerPtrs[i], key) == 0)
                {
                    _headerPtrs[i + 1] = value;
                    return;
                }
            }

            _headerPtrs.push_back((char *)key);
            _headerPtrs.push_back((char *)value);
        }

        uint32_t print(char *buffer, uint32_t size)
        {
            uint32_t offset = 0;
            for (uint32_t i = 0; i < _headerPtrs.size(); i += 2)
            {
                offset += sprintf(buffer + offset, "%s: %s\r\n", _headerPtrs[i], _headerPtrs[i + 1]);
                if (offset >= size - 2)
                    break;
            }
            offset += sprintf(buffer + offset, "\r\n");
            return offset;
        }

        int32_t count() const
        {
            return _headerPtrs.size() / 2;
        };
    };

    struct HTTPRequest
    {
        uint32_t _method = -1;
        char *_version = nullptr;
        uint32_t _contentLength = 0;
        char *_contentType = nullptr;
        char *_path = nullptr;
        char *_userAgent = nullptr;
        char clientIP[16] = {0};

        HTTPHeaders _headers;

        int32_t _clientSocket = 0;
        sockaddr_in _clientAddress = {};

        void create(const int32_t &clientSocket, const sockaddr_in &clientAddress)
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
            char *header_token = lineend + 2;
            _headers.create(header_token);
            char *cl = _headers.get("Content-Length");
            if (cl != nullptr)
            {
                _contentLength = atoi(cl);
            }
            _contentType = _headers.get("Content-Type");
            _userAgent = _headers.get("User-Agent");

            // url.create(_path);

            printf("%.7s [ %5.2f kb ]-> %s\n", method, (float)_contentLength / 1024, _path);
        };

        void print() {
        };
    };

    struct HTTPResponse
    {
        char *_responseProcessBuffer = nullptr;
        bool _headerDoneSending = false;
        bool _doneSending = false;
        int32_t _clientSocket = 0;
        uint32_t _statusCode = 200;
        uint32_t _contentLength = 0;
        uint32_t _responseProcessingBufferSize = 0;
        std::string _contentType = "text/plain";
        HTTPHeaders _headers;

        void create(int clientSocket)
        {
            _clientSocket = clientSocket;
        }

        void setHeader(char *key, char *value)
        {
            _headers.set(key, value);
        }

        void send(const char *data, uint32_t size)
        {
            char *dp = (char *)data;
            int writeSize = 0;
            if (!_headerDoneSending)
            {
                writeSize = sprintf(_responseProcessBuffer, "HTTP/1.1 %d OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n", _statusCode, _contentType.c_str(), size);

                writeSize += _headers.print(_responseProcessBuffer + writeSize, _responseProcessingBufferSize - writeSize);

                char *body = _responseProcessBuffer + writeSize;
                int canWrite = _responseProcessingBufferSize - writeSize;
                if (size > canWrite)
                {
                    memcpy(body, dp, canWrite);
                    dp += canWrite;
                    size -= canWrite;
                    writeSize += canWrite;
                }
                else
                {
                    memcpy(body, dp, size);
                    writeSize += size;
                    size = 0;
                }

                _responseProcessBuffer[writeSize] = '\0';
                printf("Sending data\n%s\n", _responseProcessBuffer);

                ::send(_clientSocket, _responseProcessBuffer, writeSize, 0);
                _headerDoneSending = true;
                return;
            }
            while (size > 0)
            {
                if (size > _responseProcessingBufferSize)
                {
                    ::send(_clientSocket, dp, _responseProcessingBufferSize, 0);
                    dp += _responseProcessingBufferSize;
                    size -= _responseProcessingBufferSize;
                }
                else
                {
                    ::send(_clientSocket, dp, size, 0);
                    size = 0;
                }
            }
        }

        void end()
        {
            if (_clientSocket > 0 && !_doneSending)
            {
                close(_clientSocket);
                _doneSending = true;
            }
        };

        void setStatus(uint32_t statusCode)
        {
            _statusCode = statusCode;
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
        std::function<void(HTTPRequest &, HTTPResponse &)> _routerFunction = nullptr;

        bool create(uint16_t port, uint32_t threadCount = std::thread::hardware_concurrency())
        {
            sp::ServerConfig config;
            config.port = port;
            if (!_server.create(config))
                return false;

            if (!_routerFunction)
            {
                fprintf(stderr, "err:: router function not set\n");
                return false;
            }

            _threadpool._onInit = [](std::vector<void *> &dataPtrs)
            {
                dataPtrs.push_back(malloc(HTTP_MAX_HEADER_SIZE));
                dataPtrs.push_back(malloc(HTTP_RESPONSE_BUFFER_SIZE));

                char *buffer = (char *)(dataPtrs[0]);
                strcpy(buffer, "Hello");
                buffer[HTTP_MAX_HEADER_SIZE - 1] = '\0';
                char *responseBuffer = (char *)(dataPtrs[1]);
                responseBuffer[HTTP_RESPONSE_BUFFER_SIZE - 1] = '\0';
            };

            _threadpool._onDestroy = [](std::vector<void *> &dataptr)
            {
                for (auto ptr : dataptr)
                    free(ptr);
            };

            _threadpool.create([&](HTTPJob &job, const std::vector<void *> &dataPtrs)
                               {
                    char *buffer = (char *)(dataPtrs[0]);
                    job.request.__processRequest(buffer, HTTP_MAX_HEADER_SIZE - 1);
                    job.request.print();
                    char * responseBuffer = (char *)(dataPtrs[1]);
                    job.response._responseProcessBuffer = responseBuffer;
                    job.response._responseProcessingBufferSize = HTTP_RESPONSE_BUFFER_SIZE -1;
                    _routerFunction(job.request, job.response);
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
