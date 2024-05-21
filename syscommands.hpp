#pragma once
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <string>

namespace sp {


#ifdef __linux__
    static constexpr const char* OS_NAME = "Linux";
    static constexpr const uint32_t OS_TYPE = 0;
#elif __APPLE__
    static constexpr const char* OS_NAME = "MacOS";
    static constexpr const uint32_t OS_TYPE = 0;
#elif _WIN32
    static constexpr const char* OS_NAME = "Windows";
    static constexpr const uint32_t OS_TYPE = 1;
#endif



    static void execute_command(const char* command, char* buffer, size_t buffer_size) {
        FILE* pipe = popen(command, "r");
        if (!pipe) {
            return;
        }
        size_t bytes_read = fread(buffer, 1, buffer_size, pipe);
        buffer[bytes_read] = '\0';
        pclose(pipe);
    }

    static void wget(const char* url, const char* output_file) {
        char command[256];
    #ifdef __linux__
        snprintf(command, 256, "wget %s -O %s", url, output_file);
        system(command);
    #elif __APPLE__
        snprintf(command, 256, "curl %s -o %s", url, output_file);
        system(command);
    #elif _WIN32
        snprintf(command, 256, "powershell -command \"(new-object System.Net.WebClient).DownloadFile('%s', '%s')\"", url, output_file);
        system(command);
    #endif
    }


    static void unzip(const char* zip_file, const char* output_dir) {
        char command[256];
    #ifdef __linux__
        snprintf(command, 256, "unzip %s -d %s", zip_file, output_dir);
        system(command);
    #elif __APPLE__
        snprintf(command, 256, "unzip %s -d %s", zip_file, output_dir);
        system(command);
    #elif _WIN32
        snprintf(command, 256, "powershell -command \"Expand-Archive -Path %s -DestinationPath %s\"", zip_file, output_dir);
        system(command);
    #endif
    }

    static std::string cmd_ls( ) {
        char buffer[4 *1024];
    #ifdef __linux__
        execute_command("ls", buffer, 4 * 1024);
    #elif __APPLE__
        execute_command("ls", buffer, 4 * 1024);
    #elif _WIN32
        execute_command("dir", buffer, 4 * 1024);
    #endif
        return std::string(buffer); 
    }

    static std::string cmd_pwd( ) {
        char buffer[1024];
        execute_command("pwd", buffer, 1024);
        return std::string(buffer); 
    }

    static bool moveFile( const char* src, const char* dest ) 
    {
        char command[256];
    #ifdef __linux__
        snprintf(command, 256, "mv %s %s", src, dest);
        
    #elif __APPLE__
        snprintf(command, 256, "mv %s %s", src, dest);
    #elif _WIN32
        snprintf(command, 256, "move %s %s", src, dest);
    #endif
        execute_command(command, command, 256);
        if (command[0] == '\0') {
            return true;
        }
        return false;
    }

    static bool copyFile( const char* src, const char* dest ) 
    {
        char command[256];
    #ifdef __linux__
        snprintf(command, 256, "cp %s %s", src, dest);

    #elif __APPLE__
        snprintf(command, 256, "cp %s %s", src, dest);
    #elif _WIN32
        snprintf(command, 256, "copy %s %s", src, dest);
    #endif
        execute_command(command, command, 256);
        if (command[0] == '\0') {
            return true;
        }
        return false;
    }

    static bool removeFile( const char* file ) 
    {
        char command[256];
    #ifdef __linux__
        snprintf(command, 256, "rm %s", file);
    #elif __APPLE__
        snprintf(command, 256, "rm %s", file);
    #elif _WIN32
        snprintf(command, 256, "del %s", file);
    #endif
        execute_command(command, command, 256);
        if (command[0] == '\0') {
            return true;
        }
        return false;
    }

    static bool makeDir( const char* dir ) 
    {
        char command[256];
        snprintf(command, 256, "mkdir %s", dir);
        execute_command(command, command, 256);
        if (command[0] == '\0') {
            return true;
        }
        return false;
    }

    static bool removeDir( const char* dir ) 
    {
        char command[256];

    #ifdef __linux__
        snprintf(command, 256, "rm -r %s", dir);
    #elif __APPLE__
        snprintf(command, 256, "rm -r %s", dir);
    #elif _WIN32
        snprintf(command, 256, "rmdir /s /q %s", dir);
    #endif
        execute_command(command, command, 256);
        if (command[0] == '\0') {
            return true;
        }
        return false;
    }

    static std::string getEnv( const char* env ) 
    {
        char command[1024];
        snprintf(command, 1024, "echo %s", env);
        execute_command(command, command, 1024);
        return std::string(command);
    }

    static bool setEnv( const char* env, const char* value ) 
    {
        char command[1024];
        snprintf(command, 1024, "export %s=%s", env, value);
        execute_command(command, command, 1024);
        if (command[0] == '\0') {
            return true;
        }
        return false;
    }



    static std::string compress(const char* buffer, uint32_t size)
    {
        char * command = new char[size + 1024];
        
#ifdef __linux__
        snprintf(command, 2048, "echo \"%s\" | gzip -c | base64", buffer);
#elif __APPLE__
        snprintf(command, 2048, "echo \"%s\" | gzip -c | base64", buffer);
#elif _WIN32
        //use powershell
        snprintf(command, 1024, "powershell -command \"[System.Convert]::ToBase64String([System.IO.Compression.GzipStream]::new([System.IO.MemoryStream]::new([System.Text.Encoding]::UTF8.GetBytes('%s')), [System.IO.Compression.CompressionMode]::Compress))\"", buffer);
#endif
        execute_command(command, command, size + 1024);
        std::string v = std::string(command);
        delete[] command;
        return v;
    }

    static std::string decompress(const char* buffer, uint32_t size)
    {
        char * command = new char[size + 1024];

#ifdef __linux__
        snprintf(command, 256 + size, "echo \"%s\" | base64 -d | gunzip -c", buffer);
#elif __APPLE__
        snprintf(command, 2048, "echo \"%s\" | base64 -d | gunzip -c", buffer);
#elif _WIN32
        //use powershell
        snprintf(command, 1024, "powershell -command \"[System.Text.Encoding]::UTF8.GetString([System.IO.Compression.GzipStream]::new([System.IO.MemoryStream]::new([System.Convert]::FromBase64String('%s')), [System.IO.Compression.CompressionMode]::Decompress))\"", buffer);
#endif

        char * output = new char[size*10 + 1024];
        execute_command(command, output, size*10 + 1024);
        std::string v = std::string(output);
        delete[] command;
        delete[] output;
        return v;
    }






}; // namespace sp


