%option noyywrap nodefault yylineno
%option noinput
%option nounput

%x comment comment2

%x compiler compiler_call compiler_call_memory compiler_call_cdtblock

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
    # include <string>
    # include <iostream>
    using namespace std;
    # include <Windows.h>
    # include <io.h>
    # include "syntax.hpp"

    # define text_token yytext
    # define jump       BEGIN

    # define SAVE_TOKEN     yylval.string           = new std::string(text_token, yyleng)
    # define BASE_TYPE      yylval.base_type        = new std::string(text_token, yyleng)
    # define ACSS_MODI      yylval.access_m         = new std::string(text_token, yyleng)
    # define BINC_OP        yylval.binc_op          = new std::string(text_token, yyleng)  
    # define SAVE_HEX       yylval.hex              = new std::string(text_token, yyleng)
    # define CONST_TEXT(text) (yylval.constant_text = new std::string(text))
   
    # define TOKEN(t)       (yylval.token = t)

    int switch_tips = -1;

    string pc_index_tmp;

    extern const char * combstr(const char *, const char *);
    extern string   apo_env_loc,
                    pc_true_index, 
                    pc_false_index,
                    fn
    ;
    extern bool __DEFINE_FUNCTION_SWITCH;

    // extern string fn;

    bool IF_STATE = false;
    bool ALLOW_EXECUTION = false;

    string  EXECUTION;

# define IF_IN_PROCESS  true
# define IF_NONE        false
# define IF_SET(state)  ( IF_STATE = state )

// @{
    enum compiler_call_params 
    {
        CALL_PROCESS_CONTROL = 1
    };
    using cpl_calls = enum compiler_call_params;

    cpl_calls cpl_calle;
// @}
%}

EXP		([eE][+-]?[0-9]+)
ANAME   (($|_)?[a-zA-Z_][a-zA-Z0-9_]*)
HEX     ([0][xX][a-fA-F0-9]{8,8})
INT     ([0-9]+)
BOOL    ("true"|"false")


BLS     ([ \t\n\r\n]+)
BLX     ([ \t\n\r\n]*)

PC      ("(pc)"|"(process-control)")

TTT     ([a-zA-Z0-9.]*)

%%

" "                         { /* ignore */ }

\"[^\"]+\"                  {
                                std::string tmp (yytext + 1, yyleng - 2);
                                CONST_TEXT(tmp); return CONST_T;
                            }
'[^']*'                     {
                                std::string tmp (yytext + 1, yyleng - 2);
                                CONST_TEXT(tmp); return CONST_T;
                            }

[ \t]                       { /* ignore */ }
[\r\n]                      { /* ignore */ }

{HEX}                       { SAVE_HEX; return HEX; }

"def"                       { __DEFINE_FUNCTION_SWITCH = true; return TOKEN(DEF); }
"ret"                       |
"return"                    { return TOKEN(RET); }
"true"                      |
"false"                     { SAVE_TOKEN; return BOOLVAL; }

"out"                       { return TOKEN(FOUT); }
"exit"                      { return TOKEN(FEXIT); }
"address"                   { return TOKEN(FADDRESS); }
"wait"                      { return TOKEN(FWAIT); }
"pause"                     { return TOKEN(FPAUSE); }
"mview"                     { return TOKEN(FMVIEW); }

"+"	|
"-"	|
"*"	|
"/"	|
"="	|
"|"	|
"." |
","	|
";"	|
":"	|
"{"	|
"}"	|
"("	|
"#" |
"^" |
")"	|
"[" |
"]" |
"!" |
"_" |
"&" |
"\\" |
"@" |
"'" |
"\"" |
"~" 	{ return TOKEN(text_token[0]); }

"\|"    { return TOKEN(text_token[0]); }



"//"                                    {
                                            jump comment;
                                        }
<comment>.                              { /* ignore */ }
<comment>\n                             { 
                                            jump INITIAL;
                                        }

"/*"                                    {
                                            jump comment2;
                                        }
<comment2>\n                            { /* ignore */ }
<comment2>"@compiler"{BLS}              {
                                            jump compiler;
                                        }
<comment2>"*/"                          {
                                            jump INITIAL;
                                        }
<comment2>.                             {
                                            // if (ALLOW_EXECUTION) 
                                            // {
                                            //     cout << "(" << fn << ") will execute -> " << EXECUTION << endl;
                                            //     system(string("apollo --func " + EXECUTION + " --memory-binding " + fn).c_str());
                                            //     ALLOW_EXECUTION = false;
                                            //     IF_STATE = false;
                                            //     EXECUTION.clear();
                                            // }
                                        }

<compiler>"%call"{BLS}                  {
                                            jump compiler_call;
                                        }
<compiler_call>"*/"                     {
                                            jump INITIAL;
                                        }
<compiler_call>{PC}{BLX}                {
                                            cpl_calle = CALL_PROCESS_CONTROL;
                                            jump compiler_call_memory;
                                        }
<compiler_call>.                        { /* ignore */ }

<compiler_call_memory>{HEX}             {
                                            pc_index_tmp = string(text_token);
                                            // if (!IF_STATE) 
                                            // {
                                            //     ALLOW_EXECUTION = true;
                                            //     EXECUTION = pc_index_tmp;
                                            //     jump comment2;
                                            // }
                                            jump compiler_call_cdtblock;
                                        }
<compiler_call_memory>.                 { /* ignore */ }

<compiler_call_cdtblock>[ \t\n\r\n]     { /* ignore */ }
<compiler_call_cdtblock>{BOOL}          {
                                            if (string(text_token) == "true") 
                                            {
                                                pc_true_index = pc_index_tmp;
                                            }
                                            else 
                                            if (string(text_token) == "false") 
                                            {
                                                pc_false_index = pc_index_tmp;
                                            }
                                            
                                            jump comment2;
                                        }
<compiler_call_cdtblock>.               { /* ignore */ }

">"		{ SAVE_TOKEN; return CMP; }
"<"		{ SAVE_TOKEN; return CMP; }
"!="	{ SAVE_TOKEN; return CMP; }
"=="	{ SAVE_TOKEN; return CMP; }
">="	{ SAVE_TOKEN; return CMP; }
"<="	{ SAVE_TOKEN; return CMP; }

"&&"    { SAVE_TOKEN; return LOP; }
"||"    { SAVE_TOKEN; return LOP; }

"++"    { SAVE_TOKEN; return SINDE; }
"--"    { SAVE_TOKEN; return SINDE; }

"^="    { BINC_OP; return BOP; }
"~="    { BINC_OP; return BOP; }
"*="    { BINC_OP; return BOP; }
"/="    { BINC_OP; return BOP; }
"+="    { BINC_OP; return BOP; }
"-="    { BINC_OP; return BOP; }



"<<"    { return TOKEN(ML); }
">>"    { return TOKEN(MR); }

"->"    { return TOKEN(PTR); }

"if"	    { /* IF_SET(IF_IN_PROCESS); */ return TOKEN(IF); }
"else"	    { return TOKEN(ELSE);	}
"for"       { return TOKEN(FOR);    }
"while"	    { return TOKEN(WHILE);  }
"do"	    { return TOKEN(DO);     }
"switch"    { return TOKEN(SWITCH); }
"case"      { return TOKEN(CASE);   }
"break"     { return TOKEN(BREAK);  }

"code"      { return TOKEN(CODE); }

"import"    { return TOKEN(IMPORT); }

"public"    { ACSS_MODI; return ACCESSM; }
"private"   { ACSS_MODI; return ACCESSM; }
"protected" { ACSS_MODI; return ACCESSM; }

"single"    { return TOKEN(SINGLE); }

"void"      { BASE_TYPE; return TYPE; }
"int"       { BASE_TYPE; return TYPE; }
"str"       { BASE_TYPE; return TYPE; }
"double"    { BASE_TYPE; return TYPE; }
"float"     { BASE_TYPE; return TYPE; }
"bool"      { BASE_TYPE; return TYPE; }
"char"      { BASE_TYPE; return TYPE; }
"var"       { BASE_TYPE; return TYPE; }
"obj"       { BASE_TYPE; return TYPE; }
"struct"    { BASE_TYPE; return TYPE; }
"union"     { BASE_TYPE; return TYPE; }
"reg"       { BASE_TYPE; return TYPE; }
"neuron"    { BASE_TYPE; return TYPE; }

"class"     { return TOKEN(CLASS); }

"static"    { return TOKEN(STATIC); }
"onlyread"  { return TOKEN(ONLYREAD); }

"program"   { return TOKEN(PROGRAM); }
"mode"      { return TOKEN(CPLMODE); }

"print"     { return TOKEN(PRINT); }

"%?blk?%"   { return TOKEN(BLK); }
"`"         { return TOKEN(BLK); }


{ANAME}                 { SAVE_TOKEN; return NAME; }
{INT}                   { yylval.integer = atoi(text_token); return INTVAL; }
[0-9]+"."[0-9]*{EXP}?   |
"."?[0-9]+{EXP}?		{ yylval.d = atof(text_token); return NUMBER; }
.		                { /* ignore */ }

<<EOF>>                 { return EOF; }

%%


