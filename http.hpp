#pragma once
#include "server.hpp"
#include "threadpool.hpp"
#include <utility>


namespace sp {


    struct HTTPRequest
    {

    };

    struct HTTPResponse
    {

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

        void create(uint16_t port, uint32_t threadCount = std::thread::hardware_concurrency())
        {
            ServerConfig config;
            config.port = port;
            _server.create(config);

            _threadpool.create([&](HTTPJob & job){

                printf("Processing job\n");

            }, threadCount);
        };

        void destroy()
        {
            _threadpool.destroy();
            _server.destroy();
        };

        void listen()
        {
            _server.start();
            while(true)
            {
                int32_t clientSocket = 0;
                sockaddr_in clientAddress = {};
                if(_server.acceptClient(clientSocket, clientAddress))
                {
                    HTTPJob job;
                    _threadpool.push(std::move(job));

                }
            }
        };

    };

};
