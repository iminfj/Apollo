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
	
	# include <apollo.hpp>

	# include <iostream>
	using namespace std;
	
	# include <cstdio>
	# include <cstdlib>
	# include <string.h>
	# include <vector>
	# include <map>
	
	bool __DEFINE_FUNCTION_SWITCH = false;
	extern bool IF_STATE;

	string __list_params_callf;

	apollo::identset * latest_ident = nullptr;

	int index_actual_argument = 0;

	map<string, string> calls_type_map;

	extern vector<string> cache_k;
	extern bool __is_func;

	string pc_true_index;
	string pc_false_index;

	bool elif_reject = false;

	extern FILE * yyin;
	extern int yylex();
	extern int yylineno;
	extern string fn;
	extern string memory_env_loc;
	extern string __func_fn;
	void yyerror (const char *, ...);

	extern int Main;
	extern int Function;
	extern int ProcessControl;

	extern int cplt_t;

	bool ident_is_refer_codes = false;
%}

%union 
{ 
	double d;
	int integer;
	int token;
	int icmp;
	void * v;
	std::string * string;
	std::string * base_type;
	std::string * access_m;
	std::string * binc_op;
	std::string * hex;
	std::string * constant_text;
}

/* Define Vt
 */
%token <d> NUMBER
%token <string> NAME
%token <hex> HEX
%token <integer> INTVAL
%token <string> BOOLVAL
%token EOL
%token <string> BLK

%token IF ELSE FOR WHILE DO SWITCH CASE BREAK DEF RET

/* functions */
%token <string> FOUT FEXIT FADDRESS FWAIT FPAUSE FMVIEW

%token <string> PUBLIC PRIVATE PROTECTED SINGLE IMPORT STATIC ONLYREAD CLASS
%token <string> PROGRAM CPLMODE ABLANK
%token <base_type> TYPE 
%token <access_m> ACCESSM
%token <string> PRINT
%token <string> CODE OCHAR PTR

%token <string> BOP SINDE TCOMMENT 

/* bit operator */
%token <string> ML MR

%token <string> TEXTA TEXTB
%token <constant_text> CONST_T
%token <string> SQT DQT

%token <string> CMP
%token <string> LOP

%right '='
%left '+' '-'
%left '*' '/'


/* Define Vn
 */
%type <v> stmts stmt block
%type <string> vardecl expr numeric
%type <string> call_args

%type <string> funcdef funcdef_params func_tail_position func_tp_after f_block f_stmts f_stmt
%type <string> return

%type <string> classdef c_instance

%type <string> member_access

%type <string> if nonblock_expr if_cdtblock
%type <string> for for_three_param
%type <string> while
%type <string> switch swt_block swt_cases swt_case swt_x_type swt_case_block

%type <v> preprocessing

%type <string> imppack

%type <string> ident memory_address
%type <string> type
%type <string> access_modif

%type <string> lot

%type <string> appobject
%type <string> ao_language

/* Self-increasing and decreasing (s in de)
 */
%type <string> sinde

%type <string> text text_fac
%type <constant_text> constant_text_string

%type <string> boolvals
%type <blank> blank

/* functions out */
%type <string> outmsage out_contents out_content out_link_sym 
%type <string> exitprog

/* Built-in function */
%type <string> built_in_function
/* Memory */
%type <string> address mview
/* Control */
%type <string> pause wait


%start program


%%

program: stmts 
		{
			/* ignore */
		}
	   ;

preprocessing: /* empty */
			 | '#' PROGRAM ':' '\'' ident '\'' 
			 	{
					$$ = $5;
					cout << "\tprogram entry point is '" << *$5 << "' -> ADDR " << $5 << endl;
				}
			 | IMPORT imppack ';'
			 	{
					cout << "\tI got'it : " << *$2 << endl;
					$$ = $2;
			 	}
			 ;

imppack: ident { $$ = $1; }
	   | ident '.' imppack
			{
				cout << "\timppack: " << *$3 << " -> ADDR " << $3 << endl;
				$$->append(".");
				$$->append(*$3);
			}
	   ;


stmts: stmt					{ }
	 | stmts stmt			{ }
	 ;
f_stmts: f_stmt				{ }
	   | f_stmts f_stmt		{ }
	   ;


stmt: vardecl ';' { /* ignore */ } 
	| funcdef 
	| expr ';' | return ';'
	| appobject		| preprocessing
	| classdef
	| c_instance	| member_access ';'
	| if | for | while | switch
	| outmsage ';'	| exitprog ';' | built_in_function ';'
	; 
f_stmt: vardecl ';' { /* ignore */ }
	  | expr ';' | return ';'
	  | c_instance	|	member_access ';'
	  | if	|	for	|	while	|	switch
	  | outmsage ';'|	exitprog ';' | built_in_function ';'
	  ;

return: RET expr 
{
	switch (cplt_t) 
	{
	case 1: /* Function */
	case 2: /* Process Control */
		$$ = apollo::__return__($2);
		break;
	
	default: /* Syntax error! */ /* Main */
		cout << "The keyword `return` should appear in the function definition." << endl;
		break;
	}
	return 0;
}
;

block: '{' stmts '}'		{ $$ = $2; } 
	 | '{' '}'				{ $$ = new string(to_string(0)); }
	 ;

f_block: '{' f_stmts '}'	{ $$ = $2; }
	   | '{' ident '}'		{ $$ = new string(*$2); }
	   | '{' '}'			{ $$ = new string(to_string(0)); }
	   ;

swt_case_block: /* empty */		{ $$ = new string(to_string(0)); }
			  | f_stmts		{ $$ = $1; }
			  ;

c_instance: type ident '(' ')' ';'		{ cout << "\tclass instance" << endl; }
		  ;

classdef: CLASS ident block		{ cout << "\t%class: " << *$2 << endl; }
		;

vardecl: type ident
			{
				apollo::vartab * vardecl;
				switch (apollo::statictype_checking(*$1)) {
					case apollo::_STRING:
					case apollo::_INT:
					case apollo::_DOUBLE:
					case apollo::_BOOL:
						vardecl = apollo::declaration_variable(apollo::ident(*$2), *$1);
						break;
					case apollo::NOSUPPORTTYPE:
						cout << "\n\tTemporarily unsupported types : " << *$1 << endl;
						break;

					default: 
						cout << "\n\tUnrecognizable static type enumeration returns." << endl;
						break;
				}

				if (!vardecl) 
					cout << "\n\tSorry, variable '" << *$2 << "' was not declared successfully" << endl;

				$$ = $2;
			}
	   | type ident '=' constant_text_string 
	   		{
				apollo::vartab * string_var;
				switch (apollo::statictype_checking(*$1)) {
					case apollo::_STRING:
						string_var = apollo::definition_variable(apollo::ident(*$2), *$1, *$4);
						break;			

					default:
						cout << "\n\tSorry, (static syntax) you cannot pass string values to " << *$1 << endl; 
						break;
				}

				if (!string_var) 
					cout << "\n\tSorry, variable '" << *$2 << "' was not declared successfully" << endl;
				
				$$ = $2;
			}
	   | type ident '=' numeric 
	   		{
				apollo::vartab * num_var;
				switch (apollo::statictype_checking(*$1)) {
					case apollo::_INT:
						num_var = apollo::definition_variable(apollo::ident(*$2), *$1, atoi($4->c_str()));
						break;
					case apollo::_DOUBLE:
						num_var = apollo::definition_variable(apollo::ident(*$2), *$1, (double)atof($4->c_str()));
						break;

					default:
						cout << "\n\tSorry, (static syntax) you cannot pass num values to " << *$1 << "." << endl; 
						break;
				}

				if (!num_var) 
					cout << "\n\tSorry, variable '" << *$2 << "' was not declared successfully." << endl;
				
				$$ = $2;
			}
	   | type ident '=' boolvals
	   		{
				apollo::vartab * bool_var;
				switch (apollo::statictype_checking(*$1)) {
					case apollo::_BOOL:
						if (*$4 == "true") {
							bool_var = apollo::definition_variable(apollo::ident(*$2), *$1, true);
						} else if (*$4 == "false") {
							bool_var = apollo::definition_variable(apollo::ident(*$2), *$1, false);
						} else {
							cout << "\n\tThe bool type only supports true, false as values." << endl;
							bool_var = nullptr;
						}
						break;

					default:
						cout << "\n\tSorry, (static syntax) you cannot pass num values to " << *$1 << "." << endl; 
						break;
				}

				if (!bool_var) 
					cout << "\n\tSorry, variable '" << *$2 << "' was not declared successfully." << endl;
				
				$$ = $2;
			}
	   | type ident '=' expr 
			{
				$$ = apollo::var_def_assign_expr(*$1, $2, $4);
			}
	   ;

funcdef: DEF ident '(' funcdef_params ')' func_tail_position f_block 
{ 
	vector<string> params_list = apollo::tmp_func_params;
	vector<apollo::identset *> member_list = apollo::tmp_func_members;

	apollo::functab * func = apollo::function(
		apollo::ident(*$2),
		params_list,
		member_list
	);

	if ($6 != nullptr) 
	{
		/* tail */
		func->return_type = *$6;
		func->return_value_address = acm::memory_write(
			acm::__memory_alloc(acm::_M__ALLOC_FUNCTION_RETURN_VALUE)
			,
			""
		);
		acm::memory_write(func->memory_address, func->return_value_address, ".retval");
		acm::memory_write(func->memory_address, func->return_type, ".type");
		// temp-uss-aggregation
		acm::memory_write("0xFFFFFFFF", func->return_type,
			fn + "\\temp-uss\\" + *func->funcsign->name + ".type"
		);
		apollo::log_debug(*$2 + " - Function Return Value: " + func->return_value_address);
	}

	apollo::ftis_init_mark		=
	__DEFINE_FUNCTION_SWITCH	= false;

	apollo::ftis_init(apollo::ident(*$2));
	apollo::set_scope(apollo::backup_scope);

	int i = 0;
	for (auto const& s : apollo::vec_formal) 
	{
		if (apollo::__fft.size() != 0) 
		{
			apollo::fft[s] = apollo::__fft.at(i);
		}
		++i;
		// f{α} -> ffa ∪ {β}, (α = *func->funcsign->name) ∈ symset, (β = s) ∈ vec_formal.
		apollo::ffa[*func->funcsign->name].push_back(s);
		apollo::ABT[*func->funcsign->name].insert(map<int, string>::value_type(i, s));
		acm::memory_write("0xFFFFFFFF", s, (fn + "\\temp-uss\\" + *func->funcsign->name + "-" + to_string(i) + ".abt"));
		acm::memory_write("0xFFFFFFFF", to_string(i), (fn + "\\temp-uss\\" + *func->funcsign->name + ".abt"));
	}
	apollo::__fft.clear();

	for (auto f : apollo::ffa[*func->funcsign->name]) 
	{
		ACMC::__acm_volatile_load_store(_0xffffffff, apollo::map_symset[apollo::var(apollo::ident(f))->ident], (fn + "\\SFR\\" + *func->funcsign->name + "." + f + "." + fn));
		ACMC::__acm_volatile_load_store(_0xffffffff, apollo::fft[f], (fn + "\\SFR\\" + *func->funcsign->name + "." + f + ".type"));
	}
	apollo::ffa.clear();
	apollo::fft.clear();
};

func_tail_position: /* empty */
{
	$$ = nullptr;
	apollo::__ALLOC_FUNCTION_REAL_PARAMETERS = false;
}| 
PTR func_tp_after
{
	$$ = $2;
	apollo::__ALLOC_FUNCTION_REAL_PARAMETERS = false;
};

func_tp_after: type 
{

	$$ = $1;
}| 
access_modif 
{ 
	$$ = $1;
}| 
access_modif ':' type 
{ 
	string * func_tp_info = new string(*$1); 
	func_tp_info->append(", ");
	func_tp_info->append(*$3);
	$$ = func_tp_info;
}| 
'(' func_tp_after ')' 
{
	$$ = $2;
};

funcdef_params: /* empty */ 
{
	latest_ident->what = "function";
	$$ = new string(to_string(0)); 
}| 
vardecl	
{ 
	latest_ident->what = "function";

	apollo::tmp_func_params.clear();
	apollo::tmp_func_members.clear();
	apollo::vec_formal.clear();

	apollo::vec_formal.push_back(*$1);
	apollo::put_func_param(apollo::var(apollo::ident(*$1))->type);
	apollo::__fft.push_back(apollo::var(apollo::ident(*$1))->type);
}
| funcdef_params ',' vardecl 
{ 
	apollo::vec_formal.push_back(*$3);
	apollo::put_func_param(apollo::var(apollo::ident(*$3))->type);
	apollo::__fft.push_back(apollo::var(apollo::ident(*$3))->type);
};


member_access: expr '.' expr 
{ 
	cout << "\tmember_access: " << *$1 << "." << *$3 << endl;
};


expr: ident '=' expr
{ 
	$$ = apollo::expr_assign_ident($1, $3);
}| 
ident '(' call_args ')'
{
	apollo::codes * code = nullptr;
	const string args = ($3 != nullptr ? *$3 : EMPTY_STR);
	string t1 = ACMC::__acm_volatile_load_store(reference_codes[*$1], nullptr, ".lid");
	switch (cplt_t) 
	{
	case 0:
		code = apollo::codes_volatile_get_new (*$1, nullptr);
		if (code != nullptr) 
		{
			switch (correspondence[__to_upper(code->language)]) 
			{
			case language::BAT:
				execute_code("", code->memory_address, (code->memory_address + ".bat"));
				break;

			case language::C:
				execute_code("", code->memory_address, (code->memory_address + ".exe"));
				break;
			case language::CXX:
				// cout << "cxx will be call..." << endl;
				execute_code("", code->memory_address, (code->memory_address + ".exe " + args));
				break;
			case language::CS:
				break;
			case language::CMD:
				execute_code("", code->memory_address, (code->memory_address + ".cmd"));
				break;
			
			case language::JAVA:
				execute_code_to("java", code->memory_address, *code->name);
				break;
			
			case language::PYC:
				execute_code("", code->memory_address, ("dist\\" + code->memory_address + ".exe"));
				break;
			case language::PYE:
				execute_code_to("python", code->memory_address, (code->memory_address + ".py"));
				break;
			case language::PWSH:
				execute_code("", code->memory_address, (code->memory_address + ".ps1"));
				break;

			case language::VBS:
				break;
			
			case language::T:
				/* ignore */
				break;

			default:
				/* ignore */
				break;
			}

			// $$ = new string(code->code_return);
			$$ = new string(ACMC::__acm_volatile_load_store(code->code_return, nullptr));
		}
		break;
	case 1:
		if (ident_is_refer_codes) 
		{
			ident_is_refer_codes = false;

			code = new apollo::codes;
			switch (correspondence[__to_upper(t1)]) 
			{
			case language::BAT:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".bat"));
				break;

			case language::C:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".exe"));
				break;
			case language::CXX:
				// cout << "cxx will be call..." << endl;
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".exe " + args));
				break;
			case language::CS:
				break;
			case language::CMD:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".cmd"));
				break;
			
			case language::JAVA:
				execute_code_to("java", reference_codes[*$1], *$1);
				break;
			
			case language::PYC:
				execute_code("", reference_codes[*$1], ("dist\\" + reference_codes[*$1] + ".exe"));
				break;
			case language::PYE:
				execute_code_to("python", reference_codes[*$1], (reference_codes[*$1] + ".py"));
				break;
			case language::PWSH:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".ps1"));
				break;

			case language::VBS:
				break;
			
			case language::T:
				/* ignore */
				break;

			default:
				/* ignore */
				break;
			}
		}
		break;
	case 2:
		if (ident_is_refer_codes) 
		{
			ident_is_refer_codes = false;

			code = new apollo::codes;
			switch (correspondence[__to_upper(t1)]) 
			{
			case language::BAT:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".bat"));
				break;

			case language::C:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".exe"));
				break;
			case language::CXX:
				// cout << "cxx will be call..." << endl;
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".exe " + args));
				break;
			case language::CS:
				break;
			case language::CMD:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".cmd"));
				break;
			
			case language::JAVA:
				execute_code_to("java", reference_codes[*$1], *$1);
				break;
			
			case language::PYC:
				execute_code("", reference_codes[*$1], ("dist\\" + reference_codes[*$1] + ".exe"));
				break;
			case language::PYE:
				execute_code_to("python", reference_codes[*$1], (reference_codes[*$1] + ".py"));
				break;
			case language::PWSH:
				execute_code("", reference_codes[*$1], (reference_codes[*$1] + ".ps1"));
				break;

			case language::VBS:
				break;
			
			case language::T:
				/* ignore */
				break;

			default:
				/* ignore */
				break;
			}
		}
		break;
	
	default:
		apollo::log_debug("cplt_t " + to_string(cplt_t) + " in ident(call_args)");
		break;
	}

	if (code == nullptr) 
	{
		apollo::log_debug("[call] Syntax Level Explanation - Function Calls");
		apollo::status_packets * value = apollo::function_calls(*$1, __list_params_callf, nullptr);
		if (!value) 
		{
			cout << "value false" << endl;
		}
		if (value->effective) 
		{
			// Function Return Value
			$$ = new string(value->_val_s);

			if (auto is_function_symbol = value->_sym_f) 
			{
				calls_type_map[*$$] = is_function_symbol->return_type;
				apollo::latest_fcalls(is_function_symbol->memory_address);
			} else 
			{
				cout << "auto false" << endl;
			}
		}
		else
		{
			cout << "value->effective false" << endl;
			$$ = nullptr;
		}
	} else 
		apollo::pre_action(apollo::CodesCall);
	// cout << "*****" << endl;
}| 
ident 
{
	apollo::pre_action(apollo::VarCall);
	apollo::vartab * id = apollo::var(apollo::ident(*$1));
	if (!id) 
	{
		apollo::status_packets * sp = apollo::ident_refer(*$1);
		if (sp && sp->effective) 
			$$ = sp->_sym_v->ident->name;
		else
		{
			cout << "\tInvalid identifier '" << *$1 << "', please check if it is defined." << endl;
			return -1;
		}
	}
	$$ = $1;
}| 
numeric 
{
	apollo::pre_action(apollo::NumCall);
	$$ = $1;
}| 
expr '*' expr 
{
	$$ = apollo::expr_operations(apollo::OP_MUL, $1, $3);
}| 
expr '/' expr 
{
	$$ = apollo::expr_operations(apollo::OP_DIV, $1, $3);
}| 
expr '+' expr 
{ 
	$$ = apollo::expr_operations(apollo::OP_ADD, $1, $3);
}| 
expr '-' expr 
{ 
	$$ = apollo::expr_operations(apollo::OP_SUB, $1, $3);
}| 
expr '^' expr 
{ 
	$$ = apollo::expr_operations(apollo::OP_POW, $1, $3); 
}| 
sinde 
{
	cout << "\texpr -> sinde" << endl;
}| 
expr CMP expr		
{
	apollo::ALL_OP op;
	if (*$2 == ">") 
		op = apollo::OP_GT;
	else
	if (*$2 == "<") 
		op = apollo::OP_LT;
	else
	if (*$2 == "!=") 
		op = apollo::OP_NE;
	else 
	if (*$2 == "==") 
		op = apollo::OP_EE;
	else 
	if (*$2 == ">=") 
		op = apollo::OP_GTE;
	else 
	if (*$2 == "<=") 
		op = apollo::OP_LTE;

	$$ = apollo::expr_operations(op, $1, $3);
}| 
expr LOP expr 
{
	/* expr = expr CMP expr */
	/* expr LOP expr == { (expr CMP expr) LOP (expr CMP expr) } */

	apollo::possact act_expr1 = apollo::pre_action(apollo::GET_PREACT).at(apollo::pact_size - 2), 
					act_expr2 = apollo::pre_action(apollo::GET_PREACT).at(apollo::pact_size - 1);
	bool			l = false, 
					r = false;

	switch (act_expr1) 
	{
	case apollo::NumCall:
	case apollo::ValueComparison: /* (0|1) */
		switch (act_expr2) 
		{
		case apollo::NumCall:
		case apollo::ValueComparison: /* (0|1) LOP (0|1) */
			l = atoi($1->c_str()) > 0 ? true : false;
			r = atoi($3->c_str()) > 0 ? true : false;
			break;
		case apollo::VarCall: /* (0|1) LOP bool or .. */
			if (apollo::var(apollo::ident(*$1))->type == "bool") 
			{
				l = atoi($1->c_str()) > 0 ? true : false;
				r = atoi(acm::memory_read(apollo::var(apollo::ident(*$3))->memory_address).c_str()) > 0 ? true : false;
			}
			break;
		}
		break;
	case apollo::VarCall: /* bool or like (struct)var ... */
		switch (act_expr2) 
		{
		case apollo::NumCall:
		case apollo::ValueComparison:
			l = atoi(acm::memory_read(apollo::var(apollo::ident(*$1))->memory_address).c_str()) > 0 ? true : false;
			r = atoi($3->c_str()) > 0 ? true : false;
			break;
		case apollo::VarCall:
			l = atoi(acm::memory_read(apollo::var(apollo::ident(*$1))->memory_address).c_str()) > 0 ? true : false;
			r = atoi(acm::memory_read(apollo::var(apollo::ident(*$3))->memory_address).c_str()) > 0 ? true : false;
			break;
		}
		break;
	
	default:
		cout << "lop default1: " << to_string(act_expr1) << endl;
		cout << "lop default2: " << to_string(act_expr2) << endl;
		break;
	}

	apollo::ALL_OP op;
	if (*$2 == "&&") 
		$$ = new string(l && r ? "1" : "0");
	else
	if (*$2 == "||") 
		$$ = new string(l || r ? "1" : "0");

	apollo::pre_action(apollo::ValueComparison);
}| 
expr BOP expr 
{ 
	cout << "\texpr: " << *$1 << " " << *$2 << " " << *$3 << endl;
}| 
'(' expr ')' 
{ 
	$$ = $2; 
};

sinde: SINDE ident 
{
	cout << "\tself-increasing and decreasing: (" << *$1 << *$2 << ")" << endl;
}| 
ident SINDE	
{
	cout << "\tself-increasing and decreasing: (" << *$1 << *$2 << ")" << endl;
};

numeric: INTVAL 
{ 
	$$ = new string(to_string($1));
}| 
NUMBER 
{
	$$ = new string(to_string($1));
};

call_args: /* empty */ 
{
	$$ = nullptr;
	
	__list_params_callf.clear();
	apollo::map_actual_argument.clear();	
	index_actual_argument = 0;
}| 
constant_text_string 
{
	$$ = $1;
} |
call_args ',' constant_text_string 
{
	$1->append(" " + *$3);
} |
expr 
{ 
	$$ = $1;

			__list_params_callf.clear();
	apollo::map_actual_argument.clear();
	index_actual_argument = 0;
	
	apollo::possact action = apollo::pre_action(apollo::GET_PREACT).at(apollo::pact_size - 1);
	apollo::vartab * var_refer;

	switch (action) {
	case apollo::VarCall:
		apollo::pre_action(apollo::VarCall);
		var_refer = apollo::var(apollo::ident(*$1));
		if (var_refer) 
		{
			__list_params_callf += var_refer->type[0];
			apollo::map_actual_argument[index_actual_argument++] = acm::memory_read(var_refer->memory_address);
		}
		break;
	case apollo::ACT_INT_EXPR_OP_R:
	case apollo::NumCall:
		apollo::pre_action(apollo::NumCall);
		__list_params_callf += 'i';
		apollo::map_actual_argument[index_actual_argument++] = *$1;
		break;
	case apollo::FuncCall:
		apollo::vartab * expr_func_value = new apollo::vartab;
		expr_func_value->ident = apollo::put_ident(new string("expr_tmpl"));
		expr_func_value->ident->what = "variable";
		expr_func_value->ident->state = true;
		expr_func_value->type = calls_type_map[*$1];
		expr_func_value->memory_address = acm::memory_write(
			acm::__memory_alloc(acm::_M__ALLOC_FUNCTION_ACTUAL)
			,
			*$1
		);
		expr_func_value->next = apollo::varsym;
		apollo::varsym = expr_func_value;
		if (expr_func_value) 
		{
			apollo::pre_action(apollo::VarCall);
			__list_params_callf += expr_func_value->type[0];
			apollo::map_actual_argument[index_actual_argument++] = acm::memory_read(expr_func_value->memory_address);
		}
		break;
	}

	acm::memory_write("0xFFFFFFFF", apollo::map_actual_argument[index_actual_argument - 1], (fn + "\\temp-uss\\" + to_string(index_actual_argument - 1) + ".actual"));
	acm::memory_write("0xFFFFFFFF", to_string(index_actual_argument - 1), (fn + "\\temp-uss\\" + "index.actual"));
}| 
call_args ',' expr 
{
	apollo::possact action = apollo::pre_action(apollo::GET_PREACT).at(apollo::pact_size - 1);
	apollo::vartab * var_refer;
	
	switch (action) {
	case apollo::VarCall:
		apollo::pre_action(apollo::VarCall);
		var_refer = apollo::var(apollo::ident(*$3));
		if (var_refer) 
		{
			__list_params_callf += var_refer->type[0];
			apollo::map_actual_argument[index_actual_argument++] = acm::memory_read(var_refer->memory_address);
		}
		break;
	case apollo::ACT_INT_EXPR_OP_R:
	case apollo::NumCall:
		apollo::pre_action(apollo::NumCall);
		__list_params_callf += 'i';
		apollo::map_actual_argument[index_actual_argument++] = *$3;
		break;
	case apollo::FuncCall:
		apollo::vartab * expr_func_value = new apollo::vartab;
		expr_func_value->ident = apollo::put_ident(new string("expr_tmpr"));
		expr_func_value->ident->what = "variable";
		expr_func_value->ident->state = true;
		expr_func_value->type = calls_type_map[*$3];
		expr_func_value->memory_address = acm::memory_write(
			acm::__memory_alloc(acm::_M__ALLOC_FUNCTION_ACTUAL)
			,
			*$3
		);
		expr_func_value->next = apollo::varsym;
		apollo::varsym = expr_func_value;

		if (expr_func_value) 
		{
			apollo::pre_action(apollo::VarCall);
			__list_params_callf += expr_func_value->type[0];
			apollo::map_actual_argument[index_actual_argument++] = acm::memory_read(expr_func_value->memory_address);
		}
		break;
	}

	acm::memory_write("0xFFFFFFFF", apollo::map_actual_argument[index_actual_argument - 1], (fn + "\\temp-uss\\" + to_string(index_actual_argument - 1) + ".actual"));
	acm::memory_write("0xFFFFFFFF", to_string(index_actual_argument - 1), (fn + "\\temp-uss\\" + "index.actual"));
};


memory_address: HEX 
{
	$$ = $1;
}|
expr 
{
	apollo::possact action = apollo::pre_action(apollo::GET_PREACT).at(apollo::pact_size - 1);
	apollo::vartab * var;

	switch (action) 
	{
	case apollo::VarCall:
		var = apollo::var(apollo::ident(*$1));
		if ($1 != nullptr && var && var->type == "str" && acm::memory_read(var->memory_address).size() != 0) 
			$$ = new string(acm::memory_read(var->memory_address));
		else 
			$$ = nullptr;
		break;
	case apollo::FuncCall:
		if ($1 != nullptr && acm::__verify(acm::__correction(*$1)) && calls_type_map[*$1] == "str") 
			$$ = $1;
		else 
			$$ = nullptr;
		break;
	case apollo::TextCall:
		if ($1 != nullptr && acm::__verify(acm::__correction(*$1))) 
			$$ = $1;
		else 
			$$ = nullptr;
		break;
	case apollo::ACT_STRING_EXPR_OP_R:
		break;
	}
};

ident: NAME 
{ 
	$$ = $1;

	for (auto const &single : reference_codes) 
	{
		if (single.first == *$1) 
		{
			ident_is_refer_codes = true;
			break;
		}
	}

	if (__is_func) 
	{
		// cout << "\n\t\t\t\t\t%call " << __func_fn << " at " << yylineno << '\t' << $$ << '\t' << *$$ << endl;
		latest_ident = apollo::put_ident($$, 1);
	}
	else 
	{
		// cout << "\t\t\t\t\t" << fn << " at " << yylineno << '\t' << $$ << '\t' << *$$ << endl;
		latest_ident = apollo::put_ident($$);
	}
};

boolvals: BOOLVAL 
{ 
	$$ = $1; 
};

constant_text_string: CONST_T 
{
	$$ = $1;
};

text: text_fac			{ $$ = $1; }
	| text text_fac		{ $1->append(*$2); }
	;
text_fac: /* empty */		{ $$ = new string("");  }
		| ident			{ $$ = $1; }
		| numeric		{ $$ = new string(*$1); }
		| memory_address{ $$ = new string(*$1); }
		| OCHAR			{ $$ = new string(*$1); }
		| BLK			{ $$ = new string(" "); }
		| '+'			{ $$ = new string("+"); }
		| '-'			{ $$ = new string("-"); }
		| '*'			{ $$ = new string("*"); }
		| '/'			{ $$ = new string("/"); }
		| '!'			{ $$ = new string("!"); }
		| '^'			{ $$ = new string("^"); }
		| '#'			{ $$ = new string("#"); }
		| '$'			{ $$ = new string("$"); }
		| '%'			{ $$ = new string("%"); }
		| '&'			{ $$ = new string("&"); }
		| '('			{ $$ = new string("("); }
		| ')'			{ $$ = new string(")"); }
		| '['			{ $$ = new string("["); }
		| ']'			{ $$ = new string("]"); }
		| '{'			{ $$ = new string("{"); }
		| '}'			{ $$ = new string("}"); }
		| '_'			{ $$ = new string("_"); }
		| '='			{ $$ = new string("="); }
		| '|'			{ $$ = new string("|"); }
		| '/'			{ $$ = new string("/"); }
		| '?'			{ $$ = new string("?"); }
		| '<'			{ $$ = new string("<"); }
		| '>'			{ $$ = new string(">"); }
		| ';'			{ $$ = new string(";"); }
		| ':'			{ $$ = new string(":"); }
		| ','			{ $$ = new string(","); }
		| '.'			{ $$ = new string("."); }
		| '\\'			{ $$ = new string("\\"); }
		| '@'			{ $$ = new string("@"); }
		| '^'			{ $$ = new string("^"); }
		;

type: TYPE	{ $$ = $1; }
	| ident	{ $$ = $1; }
	;
access_modif: ACCESSM	{ $$ = $1; }
			;
blank: BLK				{ $$ = $1; }
	 ;

lot: /* empty */			{ $$ = nullptr; }
   | EOL			{ $$ = nullptr; }
   | '\t'			{ $$ = nullptr; }
   | EOL lot		{ $$ = nullptr; }
   | '\t' lot		{ $$ = nullptr; }
   | ' '			{ $$ = nullptr; }
   | ' ' lot		{ $$ = nullptr; }
   ;


appobject: /* empty */ 
{ 
	$$ = nullptr;
}| 
CODE ident PTR ao_language '{' memory_address '}' 
{
	apollo::log_debug("codes " + *$2 + " -> " + *$4 + "\t\t" + *$6);
	$$ = apollo::codes_definition(*$2, *$4, *$6);
};

ao_language: ident 	
{ 
	$$ = $1; 
};

if_cdtblock: f_block
{
	$$ = $1;
} |
nonblock_expr
{
	$$ = $1;
}
;

if: IF '(' expr ')' if_cdtblock
{
	$$ = nullptr;
	apollo::possact action = apollo::pre_action(apollo::GET_PREACT).at(apollo::pact_size - 1);
	bool expr_condition;

	switch (action) 
	{
	case apollo::NumCall:
	case apollo::ValueComparison:
		expr_condition = atoi($3->c_str()) > 0 ? true : false;
		if (expr_condition) 
		{
			apollo::log_debug("Process Control - IF - Condition is true");
			ACMC::OPPC oppc(pc_true_index, ACMC::OPPC::PLUS, _0x0007a120);
			string pcplus7a120 = oppc.volatile_set_get(nullptr);

			apollo::log_debug("__processcontrol " + pc_true_index + "\t__ 0x7A120");
			apollo::log_debug("pcplus7a120: " + pcplus7a120);

			if (pcplus7a120 != pc_true_index) 
			{
				ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".data"), ".data");
				ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".retval"), ".retval");
				ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".signature"), ".signature");
				ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".type"), ".type");
				ACMC::__acm_volatile_load_store(pcplus7a120, "1", (pc_true_index + fn));
			}

			string args_refer_codes = " --reference-codes [";
			int i = 0;
			bool b = false;
			for (auto const &single : apollo::map_codes) 
			{
				if (i + 1 == apollo::map_codes.size()) b = true;
				args_refer_codes += (single.first + "," + single.second + (b ? "" : ","));
				++i;
			}
			args_refer_codes += "]";

			system(string("apollo --func " + pc_true_index + " --memory-binding " + fn + args_refer_codes).c_str());

			switch (ACMC::rvi()) 
			{
				case 0:
					break;

				case 40:
					// cout << "Need exit" << endl;
					ACMC::rvs("0");
					exit(0);
					break;
				
				default:
					break;
			}

			pc_true_index.clear();
		} 
		$$ = new string(expr_condition ? "1" : "0");
		break;
	
	default:
		cout << "default-if-action: " << to_string(action) << endl;
		break;
	}
}| 
if ELSE if_cdtblock	
{
	$$ = nullptr;
	if ($1 != nullptr && atoi($1->c_str()) == 0) 
	{
		apollo::log_debug("Process Control - IF - Condition is false");
		ACMC::OPPC oppc(pc_false_index, ACMC::OPPC::PLUS, _0x0007a120);
		string pcplus7a120 = oppc.volatile_set_get(nullptr);

		apollo::log_debug("pcplus7a120: " + pcplus7a120);

		if (pcplus7a120 != pc_false_index) 
		{
			ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".data"), ".data");
			ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".retval"), ".retval");
			ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".signature"), ".signature");
			ACMC::__acm_volatile_load_store(pcplus7a120, ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".type"), ".type");
			ACMC::__acm_volatile_load_store(pcplus7a120, "1", (pc_false_index + fn));
		}

		string args_refer_codes = " --reference-codes [";
		int i = 0;
		bool b = false;
		for (auto const &single : apollo::map_codes) 
		{
			if (i + 1 == apollo::map_codes.size()) b = true;
			args_refer_codes += (single.first + "," + single.second + (b ? "" : ","));
			++i;
		}
		args_refer_codes += "]";

		system(string("apollo --func " + pc_false_index + " --memory-binding " + fn + args_refer_codes).c_str());

		switch (ACMC::rvi()) 
		{
			case 0:
				break;

			case 40:
				// cout << "Need exit" << endl;
				ACMC::rvs("0");
				exit(0);
				break;
			
			default:
				break;
		}

		pc_false_index.clear();
		$$ = new string("0");
	} 
	else 
	if ($1 != nullptr) {
		$$ = new string("1");
	}
}| 
if ELSE if
{
	/* Must admit to being a bit lazy in designing such an if structure due to logic, but I'll address the continuous if case. */
	/* if (true) { ... } else if ...... 
		This generative will never use the semantic 
		description, it just needs to do a little auxiliary role 
		(because $1, $3 are described by the first generative)
	*/
	elif_reject = false;
};

nonblock_expr: /* empty */ 
{
	/* ignore */
} |
expr
{
	/* ignore */ 
} | 
RET expr 
{
	/* ignore */
};


for: FOR '(' vardecl ';' expr ';' for_three_param ')' f_block 
										{
											cout << "\t%pc for 1" << endl;
										}
   | FOR '(' vardecl ';' expr ';' for_three_param ')' nonblock_expr ';' 
   										{
											cout << "\t%pc for 2" << endl;
										}
   ;

for_three_param: /* empty */					{ $$ = new string(to_string(0)); }
			   | sinde					{ $$ = $1; }
			   | expr					{ $$ = $1; }
			   ;

while: WHILE '(' expr ')' f_block		{ cout << "\t%pc while 1" << endl; }
	 | WHILE '(' expr ')' nonblock_expr ';'	
	 									{ cout << "\t%pc while 2" << endl; }
	 | DO f_block WHILE '(' expr ')' ';'
	 									{ cout << "\t%pc while 3" << endl; }
	 | DO f_block WHILE nonblock_expr ';'
	 									{ cout << "\t%pc while 4" << endl; }
	 ;

switch: SWITCH '(' expr ')' swt_block	{ cout << "\t%pc switch" << endl; }
	  ;

swt_block: '{' swt_cases '}'
		 ;

swt_cases: swt_case						{}
		 | swt_cases swt_case			{}
		 ;

swt_case: CASE swt_x_type ':' 								{}
		| CASE swt_x_type ':' swt_case_block BREAK ';'		{}
		;

swt_x_type: '\"' text '\"'			{ $$ = $2; }
		  | '\'' text '\''			{ $$ = $2; }
		  | type					{ $$ = $1; }
		  | ident					{ $$ = $1; }
		  | NUMBER					{ $$ = new string(to_string((int)$1)); }
		  | INTVAL					{ $$ = new string(to_string($1)); }
		  ;


outmsage: FOUT out_link_sym out_contents 
{ 
	cout << *$3 << '\n';
};

out_contents: out_content			{ $$ = $1; }
			| out_contents out_link_sym out_content	{ $1->append(*$3); }
			;

out_link_sym: ML	{ $$ = nullptr; }
			| ':'	{ $$ = nullptr; }
			;

out_content: constant_text_string 
{ 
	$$ = $1;
}
| expr
{
	apollo::possact action = apollo::pre_action(apollo::GET_PREACT).at(apollo::pact_size - 1);
	apollo::vartab * content;
	apollo::functab * temp_func;
	switch (action) 
	{
	case apollo::VarCall:
		content = apollo::var(apollo::ident(*$1));
		switch (apollo::statictype_checking(content->type)) {
		case apollo::_STRING:
			content->sval = acm::memory_read(content->memory_address);
			$$ = new string(content->sval);
			break;
		case apollo::_INT:
			content->ival = atoi(acm::memory_read(content->memory_address).c_str());
			$$ = new string(to_string(content->ival));
			break;
		case apollo::_DOUBLE:
			/* TODO */
			content->dval = (double)atof(acm::memory_read(content->memory_address).c_str());
			$$ = new string(to_string((double)content->dval));
			break;
		case apollo::_BOOL:
			content->bval = acm::memory_read(content->memory_address) == "1" ? true : false;
			$$ = new string(content->bval ? "true" : "false");
			break;
		}
		break;

	case apollo::NumCall:
		$$ = new string($1->c_str());
		break;

	case apollo::FuncCall:
		$$ = new string(ACMC::__acm_volatile_load_store(*$1, nullptr));
		break;

	case apollo::CodesCall:
		$$ = $1;
		break;
	}

}
		   ;

exitprog: FEXIT	
{
	cout << endl;
	for (apollo::vartab * p = apollo::varsym; p; p = p->next) {
		cout << "Variable Name: " << *p->ident->name << "\t\tIsvalid: " << apollo::is_it_valid(p->ident->name) << "\t\tscope: " << (p->scope.name->size() != 0 ? *p->scope.name : "<null>") << "\t\t%ADDR: " << p->memory_address << endl;
	}
	cout << endl;
	for (apollo::functab * p = apollo::funcsym; p; p = p->next) {
		cout << "Function Signature: " << *p->funcsign->name << "\t\tFunction Name: " << p->original_ident << "\t\tIsvalid: " << apollo::is_it_valid(new string(p->original_ident)) << "\t\t%ADDR: " << p->memory_address << endl;
	}
	cout << endl;

	YYACCEPT;
}
		;


built_in_function: address	{ $$ = $1; }
				 | mview	{ $$ = $1; }
				 | wait		{ $$ = $1; }
				 | pause	{ $$ = $1; }
				 ;

address: FADDRESS '(' ident ')' 
{
	$$ = nullptr;
	apollo::status_packets * tpack = new apollo::status_packets;

	tpack->_sym_v = apollo::var(apollo::ident(*$3));
	tpack->_sym_f = apollo::function(apollo::ident(*$3));

	if (tpack->_sym_v) 
	{
		cout << "(variable) <" << *$3 << ", " << tpack->_sym_v->memory_address << '>' << endl;
		$$ = new string(tpack->_sym_v->memory_address);
	}
	if (tpack->_sym_f) 
	{
		cout << "(function) <" << *$3 << ", " << tpack->_sym_f->memory_address << '>' << endl;
		$$ = new string(tpack->_sym_f->memory_address);
	}

	delete(tpack);
}
	   | FADDRESS ident 
{
	$$ = $2;
}
	   ;

mview: FMVIEW '(' memory_address ')' 
{
	string * data = new string(acm::memory_read(*$3));
	cout << *data << endl;
	$$ = data;
}|
FMVIEW '(' memory_address ',' expr ')' 
{
	cout << "okok" << endl;
};

wait: FWAIT '(' numeric ')' 
{
	Sleep(atoi($3->c_str()));
	$$ = $1;
}
	;

pause: FPAUSE 
{
	system("pause");
	$$ = $1;
}
	 ;

%%