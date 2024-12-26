/**
 * @file compatability.hpp
 * @author Alex (aleksandriliev05@gmail.com)
 * @brief Header for system specific includes, aliases, funcs, etc.
 * @version 0.1
 * @date 2024-12-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <string>
#else
#include <unistd.h>
#include <libgen.h>
#endif

// TODO: add func to set CWD to bin path. Otherwise, any relative paths in the code are a possible problem

/**
 * @brief Get the dir of the current binary
 * 
 * @return std::string 
 */
std::string getBinaryDir() {
    char path[1024];

#ifdef _WIN32
    if (GetModuleFileNameA(NULL, path, sizeof(path)) == 0) {
        std::cerr << "Error: Unable to get executable path!" << std::endl;
        return "";
    }
    std::string dir(path);
    dir = dir.substr(0, dir.find_last_of("\\/"));

#else
    size_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len == -1) {
        std::cerr << "Error: Unable to get executable path!" << std::endl;
        return "";
    }
    path[len] = '\0';
    std::string dir(path);
    dir = dir.substr(0, dir.find_last_of("/"));
#endif
    return dir;
}

