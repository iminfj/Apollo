%option noyywrap nodefault yylineno

%x define_head def_ident params tail body
%x def_modifier dm_ident 

%{
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
    
    # define YY_NO_UNISTD_H
    # include <iostream>
    using namespace std;

    # include <vector>
    # include <string>

    # include <Windows.h>
    # include <io.h>

    # include <apollo\Utils\HashTable.hpp>

    /**/
    /*   Separation list      */
    int _s_list = 0;
    /* End of Separation list */
    string          name;
    string          code;
    string          list;

    vector<int>     bracket;
    HashTable       funcs;

    /**/

    extern string   apo_env_loc;
    extern string   memory_env_loc;
    extern string   fn;

    const char * f_read (const char *);


    int             write           (const char *, const char *);
    const char *    combstr         (const char *, const char *);
    string          gen_signature   (const string&, string);
%}


ident               (($|_)?[a-zA-Z_][a-zA-Z0-9_]*)
modifier            ("static"|"single"|"const"|"neuron")


%%

[ \n\n]*                    { /* ignore */ }


"def"[ \t\n]+               {
                                name = "";
                                code = "";
                                list = "";
                                BEGIN define_head;
                            }

<define_head>{modifier}     { BEGIN def_modifier;   }


<def_modifier>[ \t\n]+      { BEGIN dm_ident;       }

<define_head>{ident}        {
                                name = (string)yytext;
                                BEGIN def_ident;
                            }
<dm_ident>{ident}           {
                                name = (string)yytext;
                                BEGIN def_ident;
                            }


<def_ident>[ \t\n]*         { /* ignore */  }
<def_ident>"("              { BEGIN params; }


<params>"("                 { /* ignore */  }
<params>")"                 { BEGIN tail;   }
<params>.                   { list = (string)combstr(list.c_str(), yytext); }

<tail>[ \t\n]*              { /* ignore */  }
<tail>"->"                  { /* ignore */  }
<tail>"{"                   {
                                bracket.push_back(12);
                                BEGIN body;
                            }
<tail>.                     { /* ignore */ }

<body>[\t]*                 { code += string(yytext); }
<body>[\n]*                 { code = (string)combstr(code.c_str(), yytext); }
<body>[ ]*                  { code = (string)combstr(code.c_str(), yytext); }
<body>"{"                   {
                                if (bracket.size() >= 1) 
                                    code = (string)combstr(code.c_str(), yytext);
                                bracket.push_back(12);
                            }
<body>"}"                   {
                                bracket.pop_back();
                                
                                if (bracket.size() >= 1) 
                                    code = (string)combstr(code.c_str(), yytext);
                                
                                if (bracket.empty()) 
                                {
                                    funcs.put(gen_signature(name, list), code);
                                    string loc = (string)combstr(memory_env_loc.c_str(), combstr("F\\F\\F\\F\\F\\F\\F\\F\\", fn.c_str()));
                                    if (_access(combstr(loc.c_str(), "\\"), 0) == -1) 
                                    {
                                        string cmd("mkdir ");
                                        system(combstr(cmd.c_str(), combstr(loc.c_str(), "\\")));
                                    }
                                    funcs.keys_write((string)combstr(loc.c_str(), "\\code-function.table.k"));
                                    funcs.vals_write((string)combstr(loc.c_str(), "\\code-function.table.v"));                                    

                                    /*    Separation     */
                                    ++_s_list;
                                    string sprt_loc = (string)combstr(apo_env_loc.c_str(), "separation\\func\\");
                                    string n = combstr(to_string(_s_list).c_str(), ".n");
                                    string o = combstr(to_string(_s_list).c_str(), ".o");
                                    
                                    write(combstr(sprt_loc.c_str(), n.c_str()), gen_signature(name, list).c_str());
                                    write(combstr(sprt_loc.c_str(), o.c_str()), code.c_str());
                                    /* End of Separation */
                                    BEGIN INITIAL;
                                }
                            }
<body>.                     {   code = (string)combstr(code.c_str(), yytext); }

.                           { /* ignore */ }

<<EOF>>                     { 
                                /*{     Separation    }*/
                                    
                                string sprt_loc = (string)combstr(apo_env_loc.c_str(),  "separation\\func\\");
                                string slist    = (string)combstr(sprt_loc.c_str(),     "s_list");
                                string smode    = (string)combstr(sprt_loc.c_str(),     "s_mode");

                                write(combstr(sprt_loc.c_str(), "apollo_env_loc"), apo_env_loc.c_str());
                                write(slist.c_str(), to_string(_s_list).c_str());
                                write(smode.c_str(), "s");
                                
                                /* python separation.py */
                                string cmd = (string)combstr("python ", combstr(sprt_loc.c_str(), "sprt.py"));
                                system(cmd.c_str());
                                /* End of Separation */
                                exit(0);
                            }

%%



int write (const char * _address, const char * _stream) 
{
    FILE * fp = fopen(_address, "w");
    fprintf(fp, "%s", _stream);
    fclose(fp);
    return 1;
}

const char * combstr (const char * s1, const char * s2) 
{
	string s = s1;
	s.append(s2);
	return strdup(s.c_str());
}

const char * f_read (const char * _path) 
{
	FILE * fp = fopen(_path, "rb");
	char _bf[4096];
	memset(_bf, 0, sizeof(_bf));
	fread(_bf, sizeof(char), 4096, fp);
	fclose(fp);
	return strdup(string((const char *)_bf).c_str());
}

string gen_signature (const string &_name, string _list) 
{
    string s (combstr("_F__", combstr(_name.c_str(), "_"))), t;
    
    _list += '/';

    vector<string> E;

    for (auto c : _list) {
        if (c == ',' || c == '/') 
        {
            E.push_back(t);
            t.clear();
            continue;
        } else if (c != ' ') {
            t += c;
        }
    }
    for (auto const& e : E) {
        for (auto c : e) 
        {
            if (c == 's' || c == 'i' || c == 'd' || c == 'b') 
            {
                s += c;
                break;
            } else 
                s.append("u0");
        }
    }
    
    return s;
}