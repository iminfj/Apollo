%option noyywrap nodefault yylineno

%x IF_EXPR IF_TMP IF_BLOCK
%x ELSE_TMP ELSE_BLOCK

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

    # include <map>
    # include <vector>

    # include <apollo\Acm\acm.hpp>
    
    map<int, string>    memory_map;

    extern string cs_loc;

    vector<int>         bracket;

    int __pc_index = -1;

    map<int, string>    snippets;
    map<int, string>    itiswhat;
    map<int, bool>      whoisits;
    map<int, bool>      cdtblock;


    string                 block;
    string              nonblock;

    string f_read (string);
    void f_write (string, string);
%}


BO                              ("("|"["|"{")
BC                              (")"|"]"|"}")

%%

[ \t\n\r\n]*                    { /* ignore */ }

"if"[ \t\r\n]*"("               {
                                    ++__pc_index;
                                    block.clear(); nonblock.clear();
                                    bracket.push_back(0);
                                    itiswhat[__pc_index] = "if";
                                    BEGIN IF_EXPR;
                                }

<IF_EXPR>{BO}                   {
                                    bracket.push_back(0);
                                }
<IF_EXPR>{BC}                   { 
                                    bracket.pop_back();

                                    if (bracket.empty()) BEGIN IF_TMP;
                                }
<IF_EXPR>.                      { /* ignore */ }

<IF_TMP>[ \t\n\r\n]*            {
                                    nonblock += string(yytext);
                                }
<IF_TMP>";"                     {
                                    nonblock += string(yytext);
                                    snippets[__pc_index] = nonblock;
                                    whoisits[__pc_index] = false;
                                    cdtblock[__pc_index] = true;
                                    BEGIN INITIAL;
                                }
<IF_TMP>"{"                     {
                                    bracket.push_back(0);
                                    BEGIN IF_BLOCK;
                                }
<IF_TMP>.                       {
                                    nonblock += string(yytext);
                                }

<IF_BLOCK>[ \t\n\r\n]*          {
                                    block += string(yytext);
                                }
<IF_BLOCK>{BO}                  {
                                    bracket.push_back(0);
                                    block += string(yytext);
                                }
<IF_BLOCK>{BC}                  {
                                    bracket.pop_back();

                                    if (bracket.empty()) 
                                    {
                                        snippets[__pc_index] = block;
                                        whoisits[__pc_index] = true;
                                        cdtblock[__pc_index] = true;
                                        BEGIN INITIAL;
                                    }
                                    block += string(yytext);
                                }
<IF_BLOCK>.                     {
                                    block += string(yytext);
                                }



"else"[ \t\r\n]+                {
                                    ++__pc_index;
                                    block.clear(); nonblock.clear();
                                    itiswhat[__pc_index] = "else";
                                    cdtblock[__pc_index] = false;
                                    BEGIN ELSE_TMP;
                                }

<ELSE_TMP>[ \t\n\r\n]+          {
                                    nonblock += string(yytext);
                                }
<ELSE_TMP>"if"[ \t\r\n]*"("     {
                                    itiswhat[__pc_index] = "elif";
                                    cdtblock[__pc_index] = true;
                                }
<ELSE_TMP>";"                   {
                                    nonblock += string(yytext);
                                    snippets[__pc_index] = nonblock;
                                    whoisits[__pc_index] = false;
                                    BEGIN INITIAL;
                                }
<ELSE_TMP>"{"                   {
                                    bracket.push_back(0);
                                    BEGIN ELSE_BLOCK;
                                }
<ELSE_TMP>.                     {
                                    nonblock += string(yytext);
                                }

<ELSE_BLOCK>[ \t\n\r\n]         {
                                    block += string(yytext);
                                }
<ELSE_BLOCK>{BO}                {
                                    bracket.push_back(0);
                                    block += string(yytext);
                                }
<ELSE_BLOCK>{BC}                {
                                    bracket.pop_back();

                                    if (bracket.empty()) 
                                    {
                                        snippets[__pc_index] = block;
                                        whoisits[__pc_index] = true;
                                        BEGIN INITIAL;
                                    }
                                    block += string(yytext);
                                }
<ELSE_BLOCK>.                   {
                                    block += string(yytext);
                                }



.                               { /* ignore */ }

<<EOF>>                         { /* { ... } */

                                # ifdef AMM_2023

                                    /* 11 11.cdt .__pc_index .mode .file pc.py */

                                    // 11       : code block
                                    // 11.cat   : condition value
                                    // .__pc_index   : 2
                                    // .mode    : s
                                    // .file    : <fn>
                                    // pc.py    : conversion


                                    string addr;

                                    for (int i = 0; i <= __pc_index; ++i) 
                                    {
                                        addr = ACMC::__memory_alloc(ACMC::_M__ALLOC_PROCESS_CONTROL);

                                        // code             --> memory
                                        ACMC::__acm_volatile_load_store(addr, snippets[i]);
                                        // condition value  --> memory.cdt
                                        // ACMC::__acm_volatile_load_store(addr, (cdtblock[i] ? "true" : "false"), ".cdt");

                                        memory_map[i] = addr;
                                    }

                                    f_write(cs_loc + ".mode", "s");
                                # elif defined(AMM_2022)
                                    cerr << "Update AMM standards to at least version 2023" << endl;
                                    exit(-1);
                                # endif /* AMM */

                                    return 0;
                                }

%%

string f_read (string path)
{
    char buffer[4096]; memset(buffer, 0, sizeof(buffer));

    FILE * fp = fopen(path.c_str(), "r");
        fread(buffer, sizeof(char), 4096, fp);
    fclose(fp);

    return string((const char *)buffer);
}

void f_write (string file, string data)
{
    FILE * fp = fopen(file.c_str(), "w");
        fprintf(fp, "%s", data.c_str());
    fclose(fp);
}

