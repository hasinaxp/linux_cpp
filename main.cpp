#include "dsalgo.hpp"
#include "datetime.hpp"
#include "syscommands.hpp"
#include <cstdio>
#include "http.hpp"





using namespace sp;

int main() {
    
    HTTPServer server;
    server.create(7800);
    printf("server created successfilly\n");
    server.listen();

    return 0;
}


