#include "dsalgo.hpp"
#include "datetime.hpp"
#include "syscommands.hpp"
#include <cstdio>






using namespace sp;

int main() {
    char isoDateString[30];


    printf("ISO 8601 Date String: %s\n", ISODateString().c_str());
    
    printf("Date String: %s\n", datestr().c_str());


    printf("Time String: %s\n", timestr().c_str());


    printf("Date String: %s\n", datestr(time(NULL), "%A, %B %d, %Y").c_str());


    printf("ls: \n%s\n", cmd_ls().c_str());

    printf("OS Name: %s\n", OS_NAME);

    printf("env: %s\n", getenv("HOME"));


    std::string fileContent = "hello world! my name is spandan mondal";
    std::string zippedString = compress(fileContent.c_str(), fileContent.size());
    printf("Zipped String: %s\n", zippedString.c_str());
    std::string unzippedString = decompress(zippedString.c_str(), zippedString.size());
    printf("Unzipped String: %s\n", unzippedString.c_str());




    return 0;
}


