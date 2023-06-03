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
# include "string.h"

using namespace std;

extern FILE * yyin;
extern int yylex();
extern const char * f_read  (const char *);
extern const char * combstr (const char *, const char *);

string apo_env_loc      = string((const char *)getenv("APOLLO_HOME"));
string memory_env_loc   = string((const char *)getenv("APOLLO_MEMORY"));

string fn;

int main (int args, char ** argv) 
{	
    if (yyin = fopen(argv[1], "r")) {
		fn = (string)argv[1];
        string s_file_loc = (string)combstr(apo_env_loc.c_str(),  "separation\\func\\s_file");
        FILE * w = fopen(s_file_loc.c_str(), "w");
            fprintf(w, "%s", fn.c_str());
        fclose(w);
        
        yylex();
    }
    return 0;
}

