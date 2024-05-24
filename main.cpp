#include "dsalgo.hpp"
#include "datetime.hpp"
#include "syscommands.hpp"
#include <cstdio>
#include "http.hpp"

using namespace sp;

int main()
{

    HTTPServer server;
    server._routerFunction = [](HTTPRequest &request, HTTPResponse &response)
    {
        printf("Request received\n");
        response._statusCode = 200;
        const char *data = "Hello World\n, this is a response from the server to you";
        uint32_t dataLength = strlen(data);
        response.send(data, dataLength);
    };
    if (server.create(7800))
    {
        printf("listening on port 7800\n");
        server.listen();
    }

    return 0;
}
