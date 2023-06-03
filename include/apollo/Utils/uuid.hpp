/*
 * This file is part of the [Apollo-LS] project.
 * Copyright [2023] [Rui Zhang] and other contributors.
 * See the LICENSE file in the project root directory for the full license information.
 * 
 * The sharing rights for this project belong to all those who have contributed to the project.
 * 
 * Thanks to the following contributors for their dedication and support of this project:
 *   - Rui Zhang 
*/

# include <iostream>
# include <random>
# include <sstream>
# include <iomanip>

# ifdef _WIN32
    # include <windows.h>
# else
    # include <unistd.h>
# endif

inline std::string uuid_gen() 
{
# ifdef _WIN32
    DWORD pid = GetCurrentProcessId();
# else
    pid_t pid = getpid();
# endif

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << pid;
    for (int i = 0; i < 8; ++i) {
        ss << dis(gen);
    }

    return ss.str();
}

