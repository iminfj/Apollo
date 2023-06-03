/*
 * This file is part of the [Apollo] project.
 * Copyright [2023] [Rui Zhang] and other contributors.
 * See the LICENSE file in the project root directory for the full license information.
 * 
 * The sharing rights for this project belong to all those who have contributed to the project.
 * 
 * Thanks to the following contributors for their dedication and support of this project:
 *   - Rui Zhang 
*/


# include <iostream>
using namespace std;

# include <vector>
# include <map>

# include <windows.h>
# include <io.h>

extern FILE * yyin;
extern int yylex();

# define __0xffffffff   "F\\F\\F\\F\\F\\F\\F\\F\\"
# define analysis       yylex()



string memory_root, codes_loc, fn, include_path;


int main (int args, char * argv[]) 
{
    memory_root = string((const char *)getenv("APOLLO_MEMORY"));
    if (yyin = fopen(argv[1], "r")) 
    {
        fn = (string)argv[1];
        codes_loc = memory_root + __0xffffffff + fn + "\\code-snippets";

        if (_access(codes_loc.c_str(), 0) == -1) 
            system(string("mkdir " + codes_loc).c_str());
        include_path = memory_root + __0xffffffff + fn + "\\include";
        if (_access(include_path.c_str(), 0) == -1) 
            system(string("mkdir " + include_path).c_str());

        analysis;
    } else 
    {
        cerr << ">> You need to write a command line parameter that specifies the file to be parsed." << endl;
        return -1;
    }
    return 0;
}


