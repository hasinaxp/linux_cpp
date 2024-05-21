#pragma once
#include <string>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

namespace sp {

    struct ServerConfig
    {
        uint32_t domain = AF_INET;
        uint16_t port = 4600;
        uint16_t threadCount = 8;
        uint32_t backlogCount = 32;
        uint32_t socketType = SOCK_STREAM;
        uint32_t protocol = 0;
        uint32_t address = INADDR_ANY;
    };



    
    struct Server
    {
        FILE * _logStream = stdout;
        ServerConfig _config = {};
        sockaddr_in _address = {};
        int32_t _socket;

        bool create(const ServerConfig & config)
        {

            _config = config;

            _address = {};
            _address.sin_addr.s_addr = _config.address;
            _address.sin_port = htons(_config.port);
            _address.sin_family = _config.domain;

            _socket = socket(_config.domain, _config.socketType, _config.protocol);
            if(_socket < 0)
            {
                fprintf(_logStream, "err:: unable to create socket\n");
                return false;
            }

            if(bind(_socket,(sockaddr *)&_address, sizeof(sockaddr_in)) < 0)
            {
                fprintf(_logStream,"err:: unable to bind socket\n");
                return false;
            }
           
            return true;
        }

        void start()
        {
            
            if(listen(_socket, _config.backlogCount) < 0)
            {
                fprintf(_logStream, "err:: unable to listen to port: %d\n", _config.port);
                return;
            }                                 
        }

        bool acceptClient(int32_t & clientSocket, sockaddr_in & clientAddress)
        {
            socklen_t clientAddressLength = sizeof(sockaddr_in);
            clientSocket = accept(_socket, (sockaddr *)&clientAddress, &clientAddressLength);
            if(clientSocket < 0)
            {
                fprintf(_logStream, "err:: unable to accept client\n");
                return false;
            }
            return true;
        }

        void destroy()
        {
            if(_socket > 0) {
                close(_socket);
                _socket = 0;
            }
        }

        std::string getLocalAddress()
        {
            return std::string(inet_ntoa(_address.sin_addr));
        }

        uint16_t getLocalPort()
        {
            return ntohs(_address.sin_port);
        }


    };


    struct Client
    {
        FILE * _logStream = stdout;
        int32_t _socket;
        sockaddr_in _address = {};

        bool create(const char * address, uint16_t port)
        {
            _address = {};
            _address.sin_addr.s_addr = inet_addr(address);
            _address.sin_port = htons(port);
            _address.sin_family = AF_INET;

            _socket = socket(AF_INET, SOCK_STREAM, 0);
            if(_socket < 0)
            {
                fprintf(_logStream, "err:: unable to create socket\n");
                return false;
            }

            if(connect(_socket, (sockaddr *)&_address, sizeof(sockaddr_in)) < 0)
            {
                fprintf(_logStream, "err:: unable to connect to server\n");
                return false;
            }

            return true;
        }

        void destroy()
        {
            if(_socket > 0) {
                close(_socket);
                _socket = 0;
            }
        }

        bool send(const char * buffer, uint32_t length)
        {

            if(::send(_socket, buffer, length, 0) < 0)
            {
                fprintf(_logStream, "err:: unable to send data\n");
                return false;
            }

            return true;
        }
        
        bool receive(char * buffer, uint32_t length)
        {
            if(::recv(_socket, buffer, length, 0) < 0)
            {
                fprintf(_logStream, "err:: unable to receive data\n");
                return false;
            }

            return true;
        }

        std::string getRemoteAddress()
        {
            return std::string(inet_ntoa(_address.sin_addr));
        }

        uint16_t getRemotePort()
        {
            return ntohs(_address.sin_port);
        }

    };


};
