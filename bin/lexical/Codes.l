
%option noyywrap nodefault yylineno

%x xidentifier xlanguage xsnippet
%x xslcmt xmlcmt

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

    # include <apollo\Codes\codes.hpp>

    # include <vector>
    vector<int> stack_brackets;
    # define push_brct ( stack_brackets.push_back(0) )
    # define pop_brct ( stack_brackets.pop_back() )

    # define token  yytext
    # define jump   BEGIN

    int logi = 0;
    # define alog ( cout << "logi: " << ++logi << " at " << __LINE__ << " in the " << __FILE__ << endl )

    string code = EMPTY_STR;
    # define code_need ( code += string(token) )

    extern string fn, codes_loc;
%}



IDENT       (("$"|"_")?[a-zA-Z"_"][a-zA-Z0-9"_"]*)
ENDCODES    ("}"[ ]?"~")
IGNORES     ([ \t\r\n]+)


%%

[ \t\n\r\n]*                            { /* ignore */ }

"//"                                    {
                                            jump xslcmt;
                                        }
<xslcmt>[\r\n]                          {
                                            jump INITIAL;
                                        }
<xslcmt>.                               { /* ignore */ }

"/*"                                    {
                                            jump xmlcmt;
                                        }
<xmlcmt>{IGNORES}                       { /* ignore */ }
<xmlcmt>"*/"                            {
                                            jump INITIAL;
                                        }
<xmlcmt>.                               { /* ignore */ }



"code"[ \t\n]+                          {
                                            jump xidentifier;
                                        }
<xidentifier>{IGNORES}                  { /* ignore */ }
<xidentifier>{IDENT}                    {
                                            IDENT_PUT(string(token));
                                        }
<xidentifier>"->"{IGNORES}              {
                                            jump xlanguage;
                                        }


<xlanguage>{IGNORES}                    { /* ignore */ }
<xlanguage>{IDENT}                      {
                                            LANG_PUT(string(token));
                                            lid_match(string(token));
                                        }
<xlanguage>"{"                          {
                                            push_brct;
                                            jump xsnippet;
                                        }


<xsnippet>{IGNORES}                     {
                                            code_need;
                                        }
<xsnippet>"{"                           {
                                            push_brct;
                                            code_need;
                                        }
<xsnippet>"}"                           {
                                            pop_brct;
                                            if (stack_brackets.empty()) 
                                            {
                                                SNIPPET_PUT(code);
                                                code.clear();
                                                NEXT;

                                                jump INITIAL;
                                            } else 
                                            {
                                                code_need;
                                            }
                                        }
<xsnippet>.                             {
                                            code_need;
                                        }


.                                       {
                                            /* ignore */ 
                                        }

<<EOF>>                                 {{}
                                            co_compilation();
                                            return 0;
                                        }

%%


