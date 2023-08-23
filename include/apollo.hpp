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
using namespace std;

# include <string>
# include <vector>
# include <map>
# include <unordered_set>

# include <stdarg.h>
# include <string.h>
# include <math.h>
# include <Windows.h>
# include <io.h>

# include <apollo\Acm\acm.hpp>
# define CODES_EXECUTE
# include <apollo\Codes\codes.hpp>

# define LATEST_FUNCTION_CALLS	"/fcalls.latest"
# define MEMORY_ENV_PATH 		memory_env_loc
# define APOLLO_ENV_PATH 		apo_env_loc
# define global_set				(fn + "/global/")
# define symbol_set				(fn + "/Symbol-Set/")
# define func_set				(fn + "/Function-Set/")

# define _ACCESS_ _access


// @{ /* apollo.l */
	extern bool					__DEFINE_FUNCTION_SWITCH;
// @}

// @{ /* apollo.cpp */
	extern int					pci;
	extern string				APOLLO_ENV_PATH, MEMORY_ENV_PATH, fn, __func_fn;
	extern vector<string>		cache_k;
	extern vector<string>		cache_v;
	extern bool					no_args;
	extern bool					__is_func;
	extern bool					rec_text;
	extern map<string, string> 	calls_type_map;
	extern map<string, string>	reference_codes;
	extern const char * combstr	(const string&, const string&);

	extern int Main;
	extern int Function;
	extern int ProcessControl;

	extern int cplt_t;
// @}

	vector<string> temp_uss_tag_list;
	vector<string> delete_list;


namespace apollo 
{
	const char * f_read(const char *);
	using cpl_msgi_basic_type	= string;
	using operation				= string;

	/* Access modifiers */
	enum access_modifiers {
		_PUBLIC 	= 1,	_PRIVATE = 2,
		_PROTECTED	= 3
	};
	/* End of Access modifiers */

	/* Class modifiers */
	enum class_modifiers {
		_SINGLE = 1
	};
	/* End of Class modifiers */


	/* Static type */
	enum atypes {
		NOSUPPORTTYPE = -1,	TYPEINIVAL = 0,
		_STRING		  =  1,	_INT   	   = 2,
		_DOUBLE   	  =  3,	_BOOL      = 4
	};
	
	/**
	 * @brief Static type checking
	 * 
	 * @return atypes 
	 */
	atypes statictype_checking (string);
	/* End of Static type */


	/**
	 * @brief The set of all ident symbols.....
	 * 
	 * The symbol added in does not necessarily have a practical role, 
	 * you need to redefine the symbol according to the actual situati
	 * on affecting the state value, for example, to define a function
	 * , and first put an ident, but because the symbols of the functi
	 * on are function signatures, so this symbol is invalid, you need
	 * to wait for the completion of the function definition to delete
	 * this symbol, and redefine this symbol.
	 */
	struct ident_symset {
		/* Symbol Name */
		string * name = new string("");

		/* Symbol Status */
		bool 	 state;

		string	what = "";

		// struct ident_symset * scope;

		/* Next ident symbols */
		struct ident_symset * next;
	};
	using identset = struct ident_symset;
	/* temp */
	identset * identsym;
	
	/**
	 * @brief Put a symbol, but it is temporarily an invalid symbol.
	 * 
	 * @return identset* 
	 */
	identset * 	put_ident	 	(string *);
	identset * 	put_ident	 	(string *, int);
	/**
	 * @brief default is 0
	 * @a Default			:	0
	 * @a Match-Layer-MM	:	1
	*/
	int			put_ident_mode = 0;
	/**
	 * @brief Get the ident sym object
	 * 
	 * @return identset* 
	 */
	identset * 	ident		 	(string);
	/**
	 * @brief ident symbol security verification to prove whether this
	 * ident symbol is valid or not.
	 * 
	 * @return true 
	 * @return false 
	 */
	inline bool		is_it_valid		(string *);
	/* End of symbol_set */


	/**
	 * @brief Variable symbol table.....
	 * 
	 * The variables declared or defined successfully are stored in th
	 * e variable symbol table, including the variable values.
	 */
	struct var_symtab {
		/* Basic info */
		/* string * ident;*/
		identset * 	ident;
		string	   	type = "";
		identset  	scope;
		string		memory_address;
		/* Variable Value */
		string	   	sval = "";
		int	       	ival = 0;
		double     	dval = 0;
		bool		bval = false;

		bool		refer = false;

		/* next symbol */
		struct var_symtab * next;
	};
	typedef struct var_symtab vartab;
	/* temp */
	vartab * varsym;
	
	/**
	 * @brief Put in a variable symbol, and full information about the variable.
	 * 
	 * @return vartab* 
	 */
	vartab *	definition_variable		(identset *, string, string);
	vartab * 	definition_variable 	(identset *, string, int);
	vartab * 	definition_variable 	(identset *, string, double);
	vartab * 	definition_variable 	(identset *, string, bool);
	/**
	 * @brief Put in a variable symbol that is only declared and not defined.
	 * 
	 * @return vartab* 
	 */
	vartab * 	declaration_variable 	(identset *, string);
	/**
	 * @brief Get a pointer to the variable table of the 'ident' symbol.
	 * 
	 * @return vartab* 
	 */
	vartab *	var				(identset *);
	bool		put_var_risk_guarantee = false;
	/* End of var_symtab */


	/* function */
	struct func_symtab {
		/* Function Signature */
		identset 				* funcsign;
		string					original_ident;
		vector<string> 			params;
		string 					return_type;
		access_modifiers		access;
		identset				parent_scope;
		vector<identset *>		members;

		string					return_value_address;
		string					memory_address;
		
		/* Next function */
		struct func_symtab * next;
	};
	using functab = struct func_symtab;
	/* temp */
	struct func_symtab * funcsym;
	vector<string> 		tmp_func_params;
	vector<identset *> 	tmp_func_members;
	bool				ftis_init_mark = false;
	/**
	 * @brief Complete function.....
	 * Generate a complete function (unwanted parameters can be represented by NULL)
	 * 
	 * @return functab* 
	 */
	functab *	function		(identset *, vector<string>, string, access_modifiers, vector<identset *>);
	/**
	 * @brief Basic function definition (simplified writing method)
	 * 
	 * @return functab* 
	 */
	functab *	function		(identset *, vector<string>, vector<identset *>);
	/**
	 * @brief Get the function symbol table for the ident symbol
	 * 
	 * @return functab* 
	 */
	functab *	function		(identset *);
	functab *	function		(string);
	bool		func_risk_guarantee = false;
	/**
	 * @brief Generating function signatures
	 * 
	 * @return identset* 
	 */
	inline identset * gen_func_signature 
	(
		identset *, 
		vector<string>
	);
	/**
	 * @brief Put a new argument to the function argument list
	 * 
	 * @return vector<string>
	 */
	inline vector<string> put_func_param 
	(
		string
	);
	/**
	 * @brief Putting a new member into the function member list
	 * 
	 * @return vector<identset*> 
	 */
	inline vector<identset*> put_func_member 
	(
		identset *
	);
	/**
	 * @brief Function temporary information table initialization
	 * 
	 */
	inline void ftis_init 
	(
		identset *
	);
	/* End of function */


	/* Predictive Action */
	enum possible_actions {
		PRE_ACTION_INIT = -1,	GET_PREACT = -2,
		/* Calling a function */
		FuncCall = 0,
		/* Calling a variable */
		VarCall = 1,
		/* Call one or more numbers, int or double type */
		NumCall = 2,
		/* */
		TextCall = 3,

		/* Just like `a == b` */
		ValueComparison = 4,

		/* if */
		IfCall = 5,

		CodesCall = 6,

		ACT_STRING_EXPR_OP_R	= 1001,	ACT_INT_EXPR_OP_R = 1002,	ACT_DOUBLE_EXPR_OP_R = 1003
	};
	using possact = enum possible_actions;
	using preacts = vector<possact>;
	preacts temp_pacts;
	int pact_size = 0;
	/**
	 * @brief Pre Action
	 * 
	 * @return preacts 
	 */
	preacts pre_action (possible_actions);
	preacts pre_action_pop ();
	/* End of Predictive Action */

	/* Status */
	identset	crt_scope;
	identset	null_scope;
	identset	backup_scope;
	/**
	 * @brief Set the scope object
	 * 
	 * @return void
	 */
	void set_scope(identset);
	/**
	 * @brief get scope
	 * 
	 * @return identset* 
	 */
	identset scope();
	/* End of Status */

	class Stack {
		public:
			Stack() {}
			~Stack() {}

			union stack_elements {
				int count_ptr;
				string * msg;
			};

			using stack_e = union stack_elements;
			vector<stack_e> stack_basic;

			void	init_stack	();
			void	push		(int);
			void	push		(string*);
			stack_e	pop			();
			stack_e	top			();
			bool	empty		();
			void	clear		();
	};

	/* Fuck, this is my wrong */
	/// @brief Soon to be obsolete, follow up with template refactoring HashTable
	class HashTable {
		public:
			HashTable() {
				// cout << "Hello HahsTable is Run!" << endl;
			}
			~HashTable() {
				// cout << "Bye~ HashTable is Release!" << endl;
			}

			// Range [0, ＋∞)
			int		size = 0;

			bool 	put			(string, string);
			bool 	puts		(string, string, string);
			bool 	update		(string, string);
			string 	get			(string);
			string	get_nohint	(string);

			/// @brief 
			/// @param _ key
			/// @return The Description
			string	get_desc	(string);

			/// @brief v2 = αβ(k, v1)
			/// @param __function_signature k
			/// @param __n v1
			/// @return v2 - Refer to 'Writing in actual parameters Amend. β(b)'
			string	alpha_beta	(string, string);

			struct keys {
				string key = "";
				struct keys * next;
			};
			struct keys * keys_t;

			struct vals {
				string key = "";
				string value = "";
				struct vals * next;
			};
			struct vals * vals_t;

			struct desc {
				string key = "";
				string descript = "";
				struct desc * next;
			};
			struct desc * desc_t;
	};
	HashTable expr_var;
	/**/

	class FuncStack : Stack {
	public:
		FuncStack () {}
		~FuncStack () {}
		HashTable table;
	private:
		int i;
	};
	FuncStack func_stack;

	/* something...... */
	// 0 - int, 1 - double
	int numeric_type = -1;
	/* End of ... */

	cpl_msgi_basic_type * expr_assign_ident	(operation *, operation *);



	enum ops 
	{
		/* Quadratic Operations */

		/* + */
		OP_ADD = 101,
		
		/* - */
		OP_SUB = 102,
		
		/* × */
		OP_MUL = 103,
		
		/* / */
		OP_DIV = 104,

		/* ^ */
		OP_POW = 105,

		/* Comparison */

		/* > */
		OP_GT	= 201,
		/* < */
		OP_LT	= 202,
		/* != */
		OP_NE	= 203,
		/* == */
		OP_EE	= 204,
		/* >= */
		OP_GTE	= 205,
		/* <= */
		OP_LTE	= 206,

		/* Logical Operators */
		OP_AND	= 301,
		OP_OR	= 302

		/* Binocular Operations */

	};
	using ALL_OP = enum ops;

	string * expr_operations (ALL_OP, operation *, operation *);

	cpl_msgi_basic_type * var_def_assign_expr (string, operation *, operation *);


	HashTable code_func;

	/**
	 * @brief The combination produces, for example, a unique identifier for a variable or a function in the current compilation scope, often referred to as SymSet.
	 * @param __scope Is used to get the parent scope, and the parent of the parent scope, up to the top-level scope.
	 * @param __ident Indicates what the symbol being processed is a thing.
	*/
	inline string symbol_set_name 
	(
		const identset &__scope, 
		identset *__ident
	);

	bool __ALLOC_FUNCTION_REAL_PARAMETERS = false;
	bool __SWITCH_;

	void test();
	inline string __memory_address_convert (string, bool);

	struct __status_packets 
	{
		// The function returns Boolean discrete values representing whether or not to use the values in the copy of the structure, respectively.
		bool			effective = false;
		identset	*	_sym_s; /* Basic Symbol		*/
		vartab		*	_sym_v; /* Variable Symbol	*/
		functab		*	_sym_f; /* Function Symbol	*/

		string			_val_s; /* The value of the string type */
		int				_val_i; /* The value of the int type	*/
		double			_val_d;
		bool			_val_b;

		atypes			_type_t;
	};
	/**
	 * Since the function is required to return a definite @a discrete-value, it is also required to return @a other-data, such as a value of type int. 
	 * ↑
	 * To solve thie problem, so there is this structure.
	*/
	using status_packets = struct __status_packets;

	/// @brief Function @a calls
	/// @param __ident Function @a Name
	/// @param __list Function @a formal-parameter type list
	/// @return Function @a Return-Value
	status_packets * function_calls (string&, string &, functab *);

	status_packets * ident_refer (string&);

	status_packets * temp_uss_aggregation (acm::MemoryArea, const string &, const string &, const string &, const string &);
	bool temp_uss_aggregation_jump_once = false;

// @{ log and the debug
	status_packets * log();
	status_packets * log_append();
	status_packets * log_debug(string);
// @} End of `log & debug`

// -------------------------------------------------------
	// f : max(lp) = size() - 1
	// β(a) = (size() - 1)
	// β(b) = vec_formal[β(a)]
	/* formal parameters */
	vector<string>					vec_formal;
// -------------------------------------------------------
	/* <function-signature, <index, formal-parameter>> */
	map<string, map<int, string>>	ABT;
// -------------------------------------------------------
	/* <index, actual-parameter> */
	map<int, string>				map_actual_argument;
// -------------------------------------------------------
	/* <ident, summary-symset> */
	map<identset *, string>			map_symset;

	/* Trigger Response Mechanism (TRM) */
	enum E_TRM 
	{
		TRM_DEFINE_VAR	= 10,
		TRM_DECLARE_VAR	= 11,
		TRM_DEFINE_FUNC	= 20,
	};
	status_packets * trm (E_TRM);
	/// @brief 
	/// @param __n ident
	/// @return status_packets * R
	status_packets * trm_var(const string &__n);

	ACMC::HEXOP hexop;

	// Function Formal Argument Set
	map<string, vector<string>> ffa;
	/* f{<string>} -> ffa ∪ {<vector>} */
	// Function Formal Argument Set
	map<string, string> fft;
	vector<string> __fft;


	/// @brief Volatile load or store the memory address pointed to by the latest function called
	/// @return (ACM) Memory Address
	inline string latest_fcalls (string *); 
	inline string latest_fcalls (const string &);

	// Scope backup before calling the funtion
	identset scope_before_function_call;

	struct external_code : identset 
	{
		string		language;
		identset	parent_scope;
		string		memory_address;
		string		code_return;
		struct external_code * next;
	};
	using codes = struct external_code;
	codes * codes_sym;

	map<string, string> map_codes;

	// <const ident &, <language, memory_address>*>
	inline codes * codes_volatile_get_new (const string &, pair<string, string> *);
	// <const ident &, const <language, memory_address> &>
	inline codes * codes_volatile_get_new (const string &, const pair<string, string> &);

	inline string * codes_definition (const string &, const string &, const string &);

	/// @param __expr The value of the expression
	/// @return __expr -> ACM
	inline string * __return__ 
	(
		string *__expr
	);

}

void apollo::test () 
{
	map<string, map<int, string>> a;
	map<int, string> b;

	a["hello"].insert(map<int, string>::value_type(1, "koko"));

	cout << "huhu: " << a["hello"][1] << endl;

	codes_volatile_get_new("hi", pair<string, string>("x", "y"));

}

inline string * apollo::__return__ (string * __expr) 
{
	possact action = pre_action(GET_PREACT).at(pact_size - 1);

	ACMC::OPPC oppc(__func_fn, ACMC::OPPC::PLUS, _0x0007a120);
	string	pcplus7a120 = oppc.volatile_set_get(nullptr),
			frv_addr,
			frv_type,
			what;
	log_debug("pcplus7a120: " + pcplus7a120);

	if (atoi(ACMC::__acm_volatile_load_store(pcplus7a120, nullptr, (__func_fn + fn)).c_str()) > 0) 
	{
		frv_addr = ACMC::__acm_volatile_load_store(pcplus7a120, nullptr, ".retval");
		frv_type = ACMC::__acm_volatile_load_store(pcplus7a120, nullptr, ".type");
		// cout << "1: " << frv_addr << " >> " << frv_type << endl;
	} 
	else 
	{
		frv_addr = ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".retval");
		frv_type = ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".type");
		// cout << "2: " << frv_addr << " >> " << frv_type << endl;
	}


	if (__is_func) 
	{
		// cout << "\n\t\t\t\t\t%call " << __func_fn << " at " << yylineno << '\t' << $$ << endl;
	}
	else 
	{
		// cout << "\t\t\t\t\t" << fn << " at " << yylineno << '\t' << $$ << endl;
	}
	log_debug("ret [" + __func_fn + ", " + frv_addr + "], (" + frv_type + ")" + *__expr);

	vartab * v;

	if (action == CodesCall) 
	{
		// $$ = ACMC::__acm_volatile_load_store();
		cout << "$2 " << *__expr << endl;
		return 0;
	}

	switch (statictype_checking(frv_type)) 
	{
	case _INT: 
		switch (action) 
		{
		case FuncCall:
			if (__expr != nullptr) 
			{
				if (statictype_checking(calls_type_map[*__expr]) == statictype_checking(frv_type)) 
				{
					string addr  = ACMC::__acm_volatile_load_store(latest_fcalls(nullptr), nullptr, ".retval");
					cout << "back call func: " << latest_fcalls(nullptr) << "\tval addr: " << addr << endl;
					ACMC::__acm_volatile_load_store(frv_addr, ACMC::__acm_volatile_load_store(addr, nullptr));
					log_debug("FuncCall OK");
				}
				else 
				{
					log_debug("FuncCall No, type: " + calls_type_map[*__expr]);
				}
			}
			break;

		case VarCall:
			// TODO if (v = var(ident(*__expr)))
			v = var(ident(*__expr));
			if (v) 
				ACMC::__acm_volatile_load_store(frv_addr, ACMC::__acm_volatile_load_store(v->memory_address, nullptr));
			else 
				log_debug("Failure to reference a variable as the return value of a function");
			break;

		case TextCall:
			acm::memory_write(frv_addr, *__expr);
			cout << "TextCall Ok" << endl;
			break;

		case ACT_INT_EXPR_OP_R:
		case NumCall:
			ACMC::__acm_volatile_load_store(frv_addr, __expr);
			log_debug("frv." + frv_addr + " = " + acm::memory_read(frv_addr));
			// ACMC::__acm_volatile_load_store(_0xffffffff, "40", "rvx");
			ACMC::rvs("40");
			exit(0);
			break;
		
		default:
			log_debug("default-action : " + to_string(action));
			break;
		}

		break;

	case _STRING: 
		// TODO
		break;

	default:
		cout << "default---" << endl;
		break;
	}
	return __expr;
}

inline string _w_codes (int t) 
{
	// TODO w ∈ Reserved Portion
	ACMC::HEXOP op;
	switch (t) 
	{
		case 0: /* Reference Function */
			return op.plus_addr(op.mul_addr(_0x0007a120, "0x2"), _0x0007a120);
		case 1: /* Reference Codes */
			return op.plus_addr(_w_codes(0)/* Base Address */, _0x0007a120);
		case 2: /* Reference PC */
			return op.plus_addr(_w_codes(1)/* Base Address */, _0x0007a120);
	}
	return EMPTY_STR;
}

inline apollo::codes * apollo::codes_volatile_get_new (const string &__ident, pair<string, string> *__codes_info) 
{
	if (__codes_info != nullptr) 
	{
		codes * code = new codes;
		
		*code->name				= __ident;
		code->state				= true;
		code->what				= "codes";
		code->language			= __codes_info->first;
		code->memory_address 	= __codes_info->second;

		ACMC::__acm_volatile_load_store(code->memory_address, code->language, ".lid");

		code->parent_scope		= scope();

		code->next 				= codes_sym;
		codes_sym 				= code;

		map_codes[__ident]		= __codes_info->second;

		map_symset[code]		= symbol_set_name(code->parent_scope, code);

		string t1 = (MEMORY_ENV_PATH + __0xffffffff + fn + "\\Symbol-Set\\");

		if (no_args) 
		{
			if (_ACCESS_(string(t1 + map_symset[code] + "." + to_string(pci)).c_str(), 0) == 0) 
				return nullptr;
			ACMC::__acm_volatile_load_store(_0xffffffff, __codes_info->second, (symbol_set + map_symset[code] + "." + to_string(pci)));
			delete_list.push_back(MEMORY_ENV_PATH + __0xffffffff + (symbol_set + map_symset[code] + "." + to_string(pci)));
		} else 
		{
			if (_ACCESS_(string(t1 + map_symset[code]).c_str(), 0) == 0) 
				return nullptr;
			ACMC::__acm_volatile_load_store(_0xffffffff, __codes_info->second, (symbol_set + map_symset[code]));
			delete_list.push_back(MEMORY_ENV_PATH + __0xffffffff + (symbol_set + map_symset[code]));
		}

// @{
		string w = cplt_t == 0 ? fn : cplt_t == 1 ? __func_fn : cplt_t == 2 ? __func_fn : "0";
		ACMC::__acm_volatile_load_store(_0xffffffff, __codes_info->second, (global_set + __ident + w + "codes"));
		delete_list.push_back(MEMORY_ENV_PATH + __0xffffffff + global_set + __ident + w + "codes");
FILE_WRITE_SET(_FILE_WRITE_AP);
		ACMC::__acm_volatile_load_store(_0xffffffff, "codes-", (global_set + __ident + w));
		delete_list.push_back(MEMORY_ENV_PATH + __0xffffffff + global_set + __ident + w);
FILE_WRITE_SET(_FILE_WRITE_DFT);
// @}

		return code;
	} else 
	{
		for (codes * code = codes_sym; code; code = code->next) 
		{
			if (*code->name == __ident) 
				return code;
		}
	}
	return nullptr;
}
inline apollo::codes * apollo::codes_volatile_get_new (const string &__ident, const pair<string, string> &__codes_info) 
{
	return codes_volatile_get_new(__ident, new pair<string, string>(__codes_info));
}
inline string * apollo::codes_definition (const string &__ident, const string &__language, const string &__memory) 
{
	codes * code = codes_volatile_get_new(__ident, pair<string, string>(__language, __memory));
	if (code) 
	{
		code->code_return = ACMC::__memory_alloc(ACMC::_M__ALLOC_FUNCTION_RETURN_VALUE);
		ACMC::__acm_volatile_load_store(code->memory_address, code->code_return, ".rvaddr");
		return code->name;
	} else 
	{
		log_debug("failed codes definition...");
	}
	return nullptr;
}

inline string apollo::latest_fcalls (string *__volat) 
{
	if (__volat == nullptr) 
		return ACMC::__acm_volatile_load_store(_0xffffffff, nullptr, (fn + LATEST_FUNCTION_CALLS));
	return ACMC::__acm_volatile_load_store(_0xffffffff, *__volat, (fn + LATEST_FUNCTION_CALLS));
}
inline string apollo::latest_fcalls (const string &__acm_memory_address) 
{
	return latest_fcalls(new string(__acm_memory_address));
}

apollo::status_packets * apollo::trm(apollo::E_TRM e) 
{
	status_packets * R = new status_packets;

	switch (e) 
	{
	case TRM_DEFINE_VAR:
	case TRM_DECLARE_VAR:
		break;
	default:
		log_debug("TRM -> default: " + to_string(e));
		break;
	}

	return R;
}

apollo::status_packets * apollo::trm_var (const string &__n) 
{
	status_packets * R = new status_packets;
	if (is_it_valid(new string(__n))) 
	{
		R->effective = true;
		string	t = memory_env_loc + __0xffffffff + "\\" + fn + "\\temp-uss\\" + __n + ".",
				c;
		c = t + fn;
		temp_uss_tag_list.push_back(c);
		c = t + "type";
		temp_uss_tag_list.push_back(c);
		c = t + "what";
		temp_uss_tag_list.push_back(c);
	} 
	else 
		log_debug("trm_var -> invalid: " + __n);
	return R;
}

// @{ log and the debug
apollo::status_packets * apollo::log_debug (string __msg) 
{
	// cout << "\t\t\t\t\t";
	// cout << __msg << '\n';
	return nullptr;
}
// @} End of `log & debug`

apollo::status_packets * apollo::temp_uss_aggregation (acm::MemoryArea __m, const string &__n, const string &__t, const string &__s, const string &__w)
{
	status_packets * sp = new status_packets;

	if (temp_uss_aggregation_jump_once) 
	{
		sp->effective					=
		temp_uss_aggregation_jump_once	= false;
		return sp;
	};

	switch (__m) 
	{
	case acm::_M__ALLOC_FUNCTION:
	case acm::_M__ALLOC_GLOBAL_VARIABLES:
		acm::memory_write("0xFFFFFFFF", __s,
			fn + "\\temp-uss\\" + __n + "." + fn
		);
		if (__t.size() != 0) 
		{
			acm::memory_write("0xFFFFFFFF", __t,
				fn + "\\temp-uss\\" + __n + ".type"
			);
		}
		acm::memory_write("0xFFFFFFFF", __w, 
			fn + "\\temp-uss\\" + __n + ".what"
		);
		break;
	default:
		sp->effective = false;
		break;
	}

	return sp;
}

constexpr int __ident_refer_default = 0;
constexpr int __ident_refer_f	 	= 1;
constexpr int __ident_refer_v 		= 2;

apollo::status_packets * apollo::ident_refer (string &__ident) 
{
	ACMC::OPPC oppc(__func_fn, ACMC::OPPC::PLUS, _0x0007a120);
	string pcplus7a120 = oppc.volatile_set_get(nullptr);

	bool pc = atoi(ACMC::__acm_volatile_load_store(pcplus7a120, nullptr, (__func_fn + fn)).c_str()) > 0 ? true : false;

	log_debug("> ident refer: " + __ident);
	log_debug("pcplus7a120: " + pcplus7a120);

	log_debug("ident_refer: " + __ident);
	status_packets * ret_s_p = new status_packets;
	string	__target_loc = memory_env_loc + __0xffffffff 
		+ "\\" 
		+ fn 
		+ "\\SFR\\" 
		+ (pc ? ACMC::__acm_volatile_load_store(pcplus7a120, nullptr, ".signature") : ACMC::__acm_volatile_load_store(__func_fn, nullptr, ".signature"))
		+ "."
		+ __ident 
		+ "." 
		+ fn;
	log_debug("--> " + __target_loc);

	string	__temp_uss_loc = memory_env_loc + __0xffffffff 
		+ "\\" 
		+ fn 
		+ "\\temp-uss\\" 
		+ __ident 
		+ "." 
		+ fn;
	int what = 0;

	// Error of `R`, It in here.
	if (!no_args && _ACCESS_(__target_loc.c_str(), 0) == 0) 
	{
		log_debug("ident_refer -> target : " + acm::memory_read(__func_fn, ".signature") + "." + __ident);
		string symset = f_read(__target_loc.c_str());
		what = (
			symset[1] == 'f' ? __ident_refer_f : symset[1] == 'v' ? __ident_refer_v 
			: __ident_refer_default
		);

		string target_symbol_loc = memory_env_loc + __0xffffffff 
			+ "\\" 
			+ fn 
			+ "\\Symbol-Set\\" 
			+ symset;

		string m_addr = f_read(target_symbol_loc.c_str());

		log_debug("ident_refer -> ADDR: " + m_addr);

		string type_loc = memory_env_loc + __0xffffffff + "\\" + fn + "\\SFR\\" + acm::memory_read(__func_fn, ".signature") + "." + __ident + ".type";
		string type = (string)f_read(type_loc.c_str());

		put_var_risk_guarantee			= 
		temp_uss_aggregation_jump_once	= true;


		string tmp;;

MALLOC_SET(_MALLOC_PROHIBIT);
		switch (statictype_checking(type)) 
		{
			case _INT:
				tmp = *scope().name;
				if (tmp.size() != 0) *scope().name = function(tmp)->original_ident;
				ret_s_p->_sym_v = definition_variable(put_ident(new string(__ident)), type, atoi(acm::memory_read(m_addr).c_str()));
				ret_s_p->_sym_v->memory_address = m_addr;
				ret_s_p->effective = ret_s_p->_sym_v->ident->state = true;
				if (tmp.size() != 0)
					*scope().name = tmp;
                break;
			case _STRING:
				ret_s_p->_sym_v = definition_variable(put_ident(new string(__ident)), type, acm::memory_read(m_addr));
				ret_s_p->_sym_v->memory_address = m_addr;
				ret_s_p->effective = ret_s_p->_sym_v->ident->state = true;
				break;
			case _DOUBLE:
				ret_s_p->_sym_v = definition_variable(put_ident(new string(__ident)), type, atof(acm::memory_read(m_addr).c_str()));
				ret_s_p->_sym_v->memory_address = m_addr;
				ret_s_p->effective = ret_s_p->_sym_v->ident->state = true;
				break;
			case _BOOL:
				ret_s_p->_sym_v = definition_variable(put_ident(new string(__ident)), type, acm::memory_read(m_addr) == "1" ? true : false);
				ret_s_p->_sym_v->memory_address = m_addr;
				ret_s_p->effective = ret_s_p->_sym_v->ident->state = true;
				break;
			default:
				cout << "type-default" << endl;
				break;
		}
ACMC::__acm_volatile_load_store(_0xffffffff, m_addr, (fn + "\\Symbol-Set\\" + symset));
MALLOC_SET(_MALLOC_ALLOW);
	} else {
		ret_s_p->effective = false;
		log_debug("ident_refer -> else");
		system("pause");
	}

	log_debug("------------------------------\n\n");
	return ret_s_p;
}

apollo::status_packets * apollo::function_calls (string &__ident, string &__list, apollo::functab * _f) 
{
	log_debug("");
	log_debug("[call] Function " + __ident + " call start");
	log_debug("\t> Arguments: " + __list);

	string FUNCTION_SIGNATURE = "_F__" + __ident + "_" + __list;

	// Function Return Value
	status_packets * V = new status_packets;

	if (apollo::code_func.get_nohint(FUNCTION_SIGNATURE).size() != 0) 
	{
// @{ Actual parameters written to memory
		if (__list.size() != 0) 
		{
			for (int ap_i = 0, ap_j = 0, threshold = 0; ap_i <= map_actual_argument.size() + 1; /* ignore */) {
				// cout << "ap_i: " << ap_i << endl;
				for (int ab = 0; ab <= ABT.size(); ++ab) 
				{
					// cout << "ab: " << ab << endl;
					string t = ABT[FUNCTION_SIGNATURE][ap_i];
					if (t.size() != 0) 
					{
						log_debug("Formal Parameter: " + t);
						/* Got the identifier of the formal parameters */
						vartab * formal;
						if (ident(t) != nullptr) 
						{
							// cout << "!= nullptr" << endl;
							formal = var(ident(t));
						} else 
						{
							cout << "== nullptr" << endl;
							string tmp = acm::memory_read(__func_fn, ".signature");
							acm::memory_write(__func_fn, FUNCTION_SIGNATURE, ".signature");
							status_packets * sp = ident_refer(t);
							if (sp && sp->effective) 
							{
								formal = var(ident(*sp->_sym_v->ident->name));
								*formal->scope.name = __ident;
							}
							else 
							{
								cout << "PASS ......." << endl;
							}
							acm::memory_write(__func_fn, tmp, ".signature");
						}
						// 这个地方可以纠正 what 错误
						if (formal) {
							formal->ident->what = "variable";
							if (*formal->scope.name == __ident /* Writing in Actual parameter */) {
								switch (statictype_checking(formal->type)) 
								{
								case _INT:
									formal->ival = atoi(map_actual_argument[ap_j].c_str());
								case _STRING:
									formal->sval = map_actual_argument[ap_j];
								case _DOUBLE:
									formal->dval = atof(map_actual_argument[ap_j].c_str());
								case _BOOL:
									formal->bval = atoi(map_actual_argument[ap_j].c_str()) == 1 ? true : false;
									break;
								}
								log_debug(t + "(" + formal->memory_address + ") : " + map_actual_argument[ap_j] + "\n");
								acm::memory_write(formal->memory_address, map_actual_argument[ap_j]);
								ap_j++;

								log_debug("> Summary of the Function References");
								log_debug("\t" + FUNCTION_SIGNATURE + "." + fn);
								log_debug("\t|\t|\t|");
								log_debug("\t" + map_symset[formal->ident] + "\n");

								// Conditions
								// acm::memory_write("0xFFFFFFFF", map_symset[formal->ident], 
								// 	fn + "\\SFR\\" + FUNCTION_SIGNATURE + "." + t + "." + fn
								// );

								log_debug("_f: " + (_f ? _f->memory_address : "false") + "sig: " + FUNCTION_SIGNATURE);

								log_debug("> Summary of the Function References");
								log_debug("\t" + FUNCTION_SIGNATURE + ".type");
								log_debug("\t|\t|\t|");
								log_debug("\t" + formal->type + "\n");

								temp_uss_tag_list.push_back((memory_env_loc + __0xffffffff + "\\" + fn + "\\SFR\\" + (_f ? (_f->memory_address + ".") : (FUNCTION_SIGNATURE + ".")) + t + "." + fn));
								// acm::memory_write("0xFFFFFFFF", formal->type,
								// 	fn + "\\SFR\\" + FUNCTION_SIGNATURE + "." + t + ".type"
								// );

								temp_uss_tag_list.push_back((memory_env_loc + __0xffffffff + "\\" + fn + "\\SFR\\" + (_f ? (_f->memory_address + ".") : (FUNCTION_SIGNATURE + ".")) + t + ".type"));
							} else 
							{
								cout << "scope-name: " << *formal->scope.name << endl;
								cout << "__ident: " << __ident << endl;
							}
						} 
						else cout << " (err)Attention: Function calls - Formal parameters do not exist" << endl;
						++ap_i;
					}
					if (ab == ABT.size()) ++ap_i;
				}
			}
		}
// @}

// @{ Execute the function code block
		functab * f = function(FUNCTION_SIGNATURE);

		if (_f) 
			f = _f;
		else 
			f = function(FUNCTION_SIGNATURE);

		if (f) {
			string args_refer_codes = " --reference-codes [";
			int i = 0;
			bool b = false;
			for (auto const &single : map_codes) 
			{
				if (i + 1 == map_codes.size()) b = true;
				args_refer_codes += (single.first + "," + single.second + (b ? EMPTY_STR : ","));
				++i;
			}
			args_refer_codes += "]";

			latest_fcalls(f->memory_address);

			string t1 = ("apollo --func " + f->memory_address + " --memory-binding " + fn + args_refer_codes);
			// cout << t1 << endl;
			scope_before_function_call = scope();
			system(t1.c_str());
set_scope(scope_before_function_call);
			// Function Return Value
			V->effective	= true;
			V->_sym_f		= f;
			V->_val_s		= acm::memory_read(f->return_value_address);
		} else {
			V->effective	= false;
			log_debug("(err)Attention: Function calls - Function do not exist: " + FUNCTION_SIGNATURE);
		}
// @}
	} else 
	{
		log_debug("Mapping function: " + FUNCTION_SIGNATURE);
		log_debug("------------------------------");
		log_debug("Match Layer MM (function-calls) -> Start");

		string	type	= acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + FUNCTION_SIGNATURE + ".type")),
				symset	= acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + FUNCTION_SIGNATURE + "." + fn)),
				what	= acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + FUNCTION_SIGNATURE + ".what"));
		status_packets	* tpack = new status_packets, 
						* mpack = new status_packets;
		functab 		* func;

		if (what == "function" && symset != _0xffffffff) 
		{
			V->effective					=
			tpack->effective				= 
			mpack->effective				=
			tpack->_val_b					=
			func_risk_guarantee				=
			temp_uss_aggregation_jump_once	= true;

			// Function Return Type
			if (type != _0xffffffff) tpack->_val_s = type;

			// Memory Address
			mpack->_val_s = acm::memory_read(_0xffffffff, (fn + "\\Symbol-Set\\" + symset));
			if (mpack->_val_s == _0xffffffff) 
			{
				cout << "0xFFFFFFFF -> " << (fn + "\\Symbol-Set\\" + symset) << endl;
			}
		} else
			tpack->effective = mpack->effective = V->effective = false;
		
		if (tpack->effective) 
		{
			log_debug("Match Layer MM (function-calls) -> effective : yes : function");
			// load args
			vector<string> args;

			// load mbrs
			vector<identset *> mbrs;

MALLOC_SET(_MALLOC_PROHIBIT);
			// construction of the function
			set_scope(null_scope);
			func = function(ident(__ident), args, mbrs);
			func->funcsign->what = "function";
MALLOC_SET(_MALLOC_ALLOW);
			log_debug("Match Layer MM (function-calls) -> Before ADDR: " + func->memory_address);
		}

		if (tpack->_val_b && func) 
		{
			func->return_type			= type != _0xffffffff ? type : func->return_type;
			func->return_value_address	= acm::memory_read(func->memory_address, ".retval");
			func->memory_address		= mpack->_val_s;
			log_debug("Match Layer MM (function-calls) -> After ADDR: " + func->memory_address);
			func->parent_scope			= null_scope;
			*func->funcsign->name		= FUNCTION_SIGNATURE;
			// cout << "new signature: " << FUNCTION_SIGNATURE << endl;
			func->funcsign->state		= true;
			acm::memory_write(_0xffffffff, func->memory_address, (fn + "\\Symbol-Set\\" + symset));
			acm::memory_write(_0xffffffff, func->memory_address, (fn + "\\Function-Set\\" + *func->funcsign->name));
			log_debug("Match Layer MM (function-calls) -> done");
			set_scope(*func->funcsign);
		}

		delete(tpack);
		delete(mpack);

		log_debug("Match Layer MM (function-calls) -> End");
		log_debug("------------------------------");

		// load ABT and load map_actual_argument
		int index_ABT = atoi(acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + *func->funcsign->name + ".abt")).c_str());
		int index_arg = atoi(acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + "index.actual")).c_str());
		
		for (int iabt = 1; iabt <= index_ABT; ++iabt) 
			ABT[*func->funcsign->name].insert(map<int, string>::value_type(iabt, acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + *func->funcsign->name + "-" + to_string(iabt) + ".abt"))));

		for (int iarg = 0; iarg <= index_arg; ++iarg) 
			map_actual_argument[iarg] = acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + to_string(iarg) + ".actual"));

		code_func.put(FUNCTION_SIGNATURE, acm::memory_read(func->memory_address));

		log_debug("Re-call - completion of function reference construction");
		log_debug("\t|\t|\t|");
		// Try agent
		V = function_calls(__ident, __list, func);

		V->_val_s = ACMC::__acm_volatile_load_store(V->_sym_f->memory_address, nullptr, ".retval");
	}

	// Make the mapping mechanism trigger conditions ready for the next function call
	code_func.update(FUNCTION_SIGNATURE, EMPTY_STR);

	log_debug("[call] End of function " + __ident + " call");
	pre_action(FuncCall);
	return V;
}

inline string apollo::__memory_address_convert (string __memory_address, bool create) 
{
    string _t = memory_env_loc, __m2;

    int i = 0; for (auto const &c : __memory_address) {
        if (i++ < 2 && __memory_address[1] == 'x') 
            continue;
        __m2 += c;
        __m2 += '\\';
        /* if (i < 10) __m2 += '\\'; */
    }

    _t = (string)combstr(_t, __m2);

    if (create && _ACCESS_(_t.c_str(), 0) == -1) {
        string command = (string)combstr("mkdir ", _t);
        system(command.c_str());
    }

    _t = (string)combstr(_t, ".data");

    return _t;
}

inline string apollo::symbol_set_name (const apollo::identset &__scope, apollo::identset *__ident) 
{
	string r("#");

	r += __ident->what.size() == 0 ? "error-what" : __ident->what;

	r += "_$(";

	identset t = __scope;
	vartab * v = var(__ident);

	bool loop = true;

	for ( ; ; ) 
	{
		if (t.what.size() == 0) 
		{
			r.append("default");
			break;
		}
		r.append(t.what);
		r += '.';
		r.append(*t.name);
		if (t.what == "class") 
		{
			// TODO CLASS
			identset __t;
			t = __t;
		} else
		if (t.what == "function") 
		{
			functab * __func = function(*t.name);
			if (__func == nullptr) {
				// cout << "\n\t\tsymbol_set_name: at t.what is function" << endl;
				break;
			}
			t = __func->parent_scope;
			if (t.what.size() == 0) break;
			r += '.';
			continue;
		} else
		if (t.what == "variable") 
		{
			vartab * __var = var(ident(*t.name));
			if (__var == nullptr) {
				// cout << "\n\t\tsymbol_set_name: at t.what is variable" << endl;
				break;
			}
			t = __var->scope;
			if (t.what.size() == 0) break;
			r += '.';
			continue;
		} else
		if (t.what == "codes") 
		{
			codes * __code = codes_volatile_get_new(*t.name, nullptr);
			if (__code == nullptr) 
				break;

			t = __code->parent_scope;

			if (t.what.size() == 0) 
				break;

			r += '.';
			continue;
		} else
		{
			cout << "else: " << t.what << endl;
			break;
		}
	}
	r += (")_@" + *__ident->name);
	return r;
}
/**/

/**
 * @brief The following areas are defined
 */


/* *** Stack *** */
void apollo::Stack::push (int ival) {
	stack_e e;
	e.count_ptr = ival;
	stack_basic.push_back(e);
}

void apollo::Stack::push (string * msg) {
	stack_e e;
	e.msg = msg;
	stack_basic.push_back(e);
}

apollo::Stack::stack_e apollo::Stack::pop () {
	if (stack_basic.size() == 0) {
		stack_e e;
		return e;
	}
	stack_e e = top();
	stack_basic.pop_back();
	return e;
}

apollo::Stack::stack_e apollo::Stack::top () {
	if (stack_basic.size() == 0) {
		stack_e e;
		return e;
	}
	return stack_basic.at(stack_basic.size() - 1);
}

bool apollo::Stack::empty () {
	return stack_basic.empty();
}

void apollo::Stack::clear () {
	stack_basic.clear();
}
/* *** Stack *** */


apollo::cpl_msgi_basic_type * apollo::expr_operations (apollo::ALL_OP op, apollo::operation * op_l, apollo::operation * op_r) {
	possact act_l = pre_action(GET_PREACT).at(pact_size - 2);
	possact act_r = pre_action(GET_PREACT).at(pact_size - 1);

	switch(act_l) {
	case VarCall:
		{
			vartab * var_l = var(ident(*op_l));
			switch (act_r) {
			case ACT_INT_EXPR_OP_R:
			case NumCall:	/* var + num */
				{
					switch (statictype_checking(var_l->type)) {
					case _INT:
					
						var_l->ival = atoi(acm::memory_read(var_l->memory_address).c_str());

						switch (op) {
						case OP_ADD:
							pre_action(act_l);
							pre_action(ACT_INT_EXPR_OP_R);
							return new string(to_string(var_l->ival + atoi(op_r->c_str())));
						case OP_SUB:
							pre_action(act_l);
							pre_action(ACT_INT_EXPR_OP_R);
							return new string(to_string(var_l->ival - atoi(op_r->c_str())));
						case OP_MUL:
							pre_action(act_l);
							pre_action(ACT_INT_EXPR_OP_R);
							return new string(to_string(var_l->ival * atoi(op_r->c_str())));
						case OP_DIV:
							pre_action(act_l);
							pre_action(ACT_INT_EXPR_OP_R);
							return new string(to_string(var_l->ival / atoi(op_r->c_str())));
						case OP_POW:
							pre_action(act_l);
							pre_action(ACT_INT_EXPR_OP_R);
							return new string(to_string((int)pow(var_l->ival, atoi(op_r->c_str()))));
						
						case OP_GT: 
							pre_action(ValueComparison);
							return new string(to_string(var_l->ival > atoi(op_r->c_str()) ? 1 : 0));
						case OP_LT:
							pre_action(ValueComparison);
							return new string(to_string(var_l->ival < atoi(op_r->c_str()) ? 1 : 0));
						case OP_NE:
							pre_action(ValueComparison);
							return new string(to_string(var_l->ival != atoi(op_r->c_str()) ? 1 : 0));
						case OP_EE:
							pre_action(ValueComparison);
							return new string(to_string(var_l->ival == atoi(op_r->c_str()) ? 1 : 0));
						case OP_GTE:
							pre_action(ValueComparison);
							return new string(to_string(var_l->ival >= atoi(op_r->c_str()) ? 1 : 0));
						case OP_LTE:
							pre_action(ValueComparison);
							return new string(to_string(var_l->ival <= atoi(op_r->c_str()) ? 1 : 0));
						}
						
						break;
					default:
						cout << "\n\tYou can try the type conversion so that the code is correct." << endl;
						return new string("type_error");
						break;
					}
				}
				break;

			case VarCall:	/* var + var */
				{
					vartab * var_r = var(ident(*op_r));
					if (var_l->type == var_r->type) {
						switch (statictype_checking(var_l->type)) {
						case _STRING:
							{
								pre_action(ACT_STRING_EXPR_OP_R);

								var_l->sval = acm::memory_read(var_l->memory_address);
								var_r->sval = acm::memory_read(var_r->memory_address);

								string * n = new string();
								n->append(var_l->sval);	n->append(var_r->sval);
								return n;
							}
						case _INT:
							{
								var_l->ival = atoi(acm::memory_read(var_l->memory_address).c_str());
								var_r->ival = atoi(acm::memory_read(var_r->memory_address).c_str());

								switch (op) {
								case OP_ADD:
									pre_action(act_l);
									pre_action(ACT_INT_EXPR_OP_R);
									return new string(to_string(var_l->ival + var_r->ival));
								case OP_SUB:
									pre_action(act_l);
									pre_action(ACT_INT_EXPR_OP_R);
									return new string(to_string(var_l->ival - var_r->ival));
								case OP_MUL:
									pre_action(act_l);
									pre_action(ACT_INT_EXPR_OP_R);
									return new string(to_string(var_l->ival * var_r->ival));
								case OP_DIV:
									pre_action(act_l);
									pre_action(ACT_INT_EXPR_OP_R);
									return new string(to_string(var_l->ival / var_r->ival));
								case OP_POW:
									pre_action(act_l);
									pre_action(ACT_INT_EXPR_OP_R);
									return new string(to_string((int)pow(var_l->ival, var_r->ival)));
								
								case OP_GT:
									pre_action(ValueComparison);
									return new string(to_string(var_l->ival > var_r->ival ? 1 : 0));
								case OP_LT:
									pre_action(ValueComparison);
									return new string(to_string(var_l->ival < var_r->ival ? 1 : 0));
								case OP_NE:
									pre_action(ValueComparison);
									return new string(to_string(var_l->ival != var_r->ival ? 1 : 0));
								case OP_EE:
									pre_action(ValueComparison);
									return new string(to_string(var_l->ival == var_r->ival ? 1 : 0));
								case OP_GTE:
									pre_action(ValueComparison);
									return new string(to_string(var_l->ival >= var_r->ival ? 1 : 0));
								case OP_LTE:
									pre_action(ValueComparison);
									return new string(to_string(var_l->ival <= var_r->ival ? 1 : 0));
								}

							}
							break;
						case _DOUBLE:
							{
								pre_action(ACT_DOUBLE_EXPR_OP_R);
								break;
							}
						}
					}
				}
			break;
			}
		}
		break;

	case ACT_INT_EXPR_OP_R:
	case NumCall:
		switch (act_r) {
			case ACT_INT_EXPR_OP_R:
			case NumCall: /* state: num(α) & num(β) */

				switch (op) {
				case OP_ADD:
					pre_action(act_l);
					pre_action(NumCall);
					return new string(to_string(atoi(op_l->c_str()) + atoi(op_r->c_str())));
				case OP_SUB:
					pre_action(act_l);
					pre_action(NumCall);
					return new string(to_string(atoi(op_l->c_str()) - atoi(op_r->c_str())));
				case OP_MUL:
					pre_action(act_l);
					pre_action(NumCall);
					return new string(to_string(atoi(op_l->c_str()) * atoi(op_r->c_str())));
				case OP_DIV:
					pre_action(act_l);
					pre_action(NumCall);
					return new string(to_string(atoi(op_l->c_str()) / atoi(op_r->c_str())));
				case OP_POW:
					pre_action(act_l);
					pre_action(NumCall);
					return new string(to_string((int)pow(atoi(op_l->c_str()), atoi(op_r->c_str()))));

				case OP_GT:
					pre_action(ValueComparison);
					return new string(to_string(atoi(op_l->c_str()) > atoi(op_r->c_str()) ? 1 : 0));
				case OP_LT:
					pre_action(ValueComparison);
					return new string(to_string(atoi(op_l->c_str()) < atoi(op_r->c_str()) ? 1 : 0));
				case OP_NE:
					pre_action(ValueComparison);
					return new string(to_string(atoi(op_l->c_str()) != atoi(op_r->c_str()) ? 1 : 0));
				case OP_EE:
					pre_action(ValueComparison);
					return new string(to_string(atoi(op_l->c_str()) == atoi(op_r->c_str()) ? 1 : 0));
				case OP_GTE:
					pre_action(ValueComparison);
					return new string(to_string(atoi(op_l->c_str()) >= atoi(op_r->c_str()) ? 1 : 0));
				case OP_LTE:
					pre_action(ValueComparison);
					return new string(to_string(atoi(op_l->c_str()) <= atoi(op_r->c_str()) ? 1 : 0));
				}

				break;
			case VarCall: /* state: num(α) & var(β) */
				vartab * var_r = var(ident(*op_r));
				if (statictype_checking(var_r->type) == apollo::_INT) {

					pre_action(NumCall);
					var_r->ival = atoi(acm::memory_read(var_r->memory_address).c_str());
					
					switch (op) {
					case OP_ADD:
						pre_action(act_l);
						pre_action(NumCall);
						return new string(to_string(atoi(op_l->c_str()) + var_r->ival));
					case OP_SUB:
						pre_action(act_l);
						pre_action(NumCall);
						return new string(to_string(atoi(op_l->c_str()) - var_r->ival));
					case OP_MUL:
						pre_action(act_l);
						pre_action(NumCall);
						return new string(to_string(atoi(op_l->c_str()) * var_r->ival));
					case OP_DIV:
						pre_action(act_l);
						pre_action(NumCall);
						return new string(to_string(atoi(op_l->c_str()) / var_r->ival));
					case OP_POW:
						pre_action(act_l);
						pre_action(NumCall);
						return new string(to_string((int)pow(atoi(op_l->c_str()), var_r->ival)));
					
					case OP_GT:
						pre_action(ValueComparison);
						return new string(to_string(atoi(op_l->c_str()) > var_r->ival ? 1 : 0));
					case OP_LT:	
						pre_action(ValueComparison);
						return new string(to_string(atoi(op_l->c_str()) < var_r->ival ? 1 : 0));
					case OP_NE:
						pre_action(ValueComparison);
						return new string(to_string(atoi(op_l->c_str()) != var_r->ival ? 1 : 0));
					case OP_EE:
						pre_action(ValueComparison);
						return new string(to_string(atoi(op_l->c_str()) == var_r->ival ? 1 : 0));
					case OP_GTE:
						pre_action(ValueComparison);
						return new string(to_string(atoi(op_l->c_str()) >= var_r->ival ? 1 : 0));
					case OP_LTE:
						pre_action(ValueComparison);
						return new string(to_string(atoi(op_l->c_str()) <= var_r->ival ? 1 : 0));
					}
				}
				break;
		}
		break;

	case TextCall:
		break;
	
	default:
		cout << "act_l: " << act_l << "\nact_r: " << act_r << endl;
		break;
	}

	return new string("tail-error");
}

apollo::cpl_msgi_basic_type * apollo::var_def_assign_expr (string type, apollo::operation * id, apollo::operation * expr) {
	vartab * gen_var;

	possact action = pre_action(GET_PREACT).at(pact_size - 1);

	switch (action) {
	case VarCall:
		{
			vartab * var_call = var(ident(*expr));

			if (var_call->type == type) {
				switch (statictype_checking(var_call->type)) {
				case _STRING: 
					var_call->sval = acm::memory_read(var_call->memory_address);
					gen_var = definition_variable(ident(*id), type, var_call->sval);
					break;
				case _INT: 
					var_call->ival = atoi(acm::memory_read(var_call->memory_address).c_str());
					gen_var = definition_variable(ident(*id), type, var_call->ival);
					break;
				case _DOUBLE: 
					var_call->dval = atof(acm::memory_read(var_call->memory_address).c_str());
					gen_var = definition_variable(ident(*id), type, var_call->dval);
					break;
				case _BOOL: 
					var_call->bval = acm::memory_read(var_call->memory_address) == "1" ? true : false;
					gen_var = definition_variable(ident(*id), type, var_call->bval);
					break;
				}
			} else {
				/* (var_call->type) is not equal (type) */
				cout << "\n\tYou can try the type conversion so that the code is correct." << endl;
				return new cpl_msgi_basic_type("type_error");
			}
		} 
		break;

	case FuncCall:
		if (expr != nullptr) 
		{
			if (calls_type_map[*expr] == type) 
			{
				string frv_addr = acm::memory_read(latest_fcalls(nullptr), ".retval");
				string drv_data = acm::memory_read(frv_addr);

				switch (statictype_checking(type)) 
				{
				case _INT: 
					gen_var = definition_variable(ident(*id), type, atoi(drv_data.c_str()));
					break;
				case _STRING:
					gen_var = definition_variable(ident(*id), type, drv_data);
					break;
				case _DOUBLE:
					gen_var = definition_variable(ident(*id), type, atof(drv_data.c_str()));
					break;
				case _BOOL:
					gen_var = definition_variable(ident(*id), type, atoi(drv_data.c_str()) == 1 ? true : false);
					break;
				default:
					cout << "default-type: " << type << endl;
					break;
				}
			} 
			else 
			{
				if (calls_type_map[*expr].size() == 0) 
				{
					cout << "Don't!" << endl;
				} else cout << "yeah: " << calls_type_map[*expr] << endl;
				log_debug("doesn't match up with the horse's head");
				return new string("type-error");
			}
		}
		break;
	
	case ACT_STRING_EXPR_OP_R:
		gen_var = definition_variable(ident(*id), type, *expr);
		break;

	case NumCall:
		gen_var = definition_variable(ident(*id), type, atoi(expr->c_str()));
		break;
	case ACT_INT_EXPR_OP_R:
		gen_var = definition_variable(ident(*id), type, atoi(expr->c_str()));
		break;
	
	case ACT_DOUBLE_EXPR_OP_R:
		gen_var = definition_variable(ident(*id), type, atof(expr->c_str()));
		break;

	
	default:
		cout << "\n\tinvalid ACT: " << action << endl;
		gen_var = nullptr;
		break;
	}

	return gen_var ? gen_var->ident->name : nullptr;
}

apollo::cpl_msgi_basic_type * apollo::expr_assign_ident (apollo::operation * id, apollo::operation * expr) 
{
	log_debug("id: " + *id + "\texpr: " + *expr);

	possact action = pre_action(GET_PREACT).at(pact_size - 1);
	vartab * lop;
	switch (action) {
	case PRE_ACTION_INIT:
		cout << "\n\tini-pre-action" << endl;
		break;
	case GET_PREACT:
		cout << "\n\tget-pre-action" << endl;
		break;
	case FuncCall:
		cout << "\n\taction-function call" << endl;
		lop = var(ident(*id));
		/* left op */
		if (expr != nullptr && lop) 
		{
			if (calls_type_map[*expr] == lop->type) 
			{
				switch (statictype_checking(lop->type)) 
				{
				case _INT:
					lop->ival = atoi(expr->c_str());
				case _STRING:
					lop->sval = *expr;
				case _DOUBLE:
					lop->dval = atof(expr->c_str());
				case _BOOL:
					lop->bval = atoi(expr->c_str()) == 1 ? true : false;
					acm::memory_write(lop->memory_address, *expr);
					break;
				}
			}
		}
		break;
	case VarCall:
		{
			if (is_it_valid(id)) {
				vartab * var_id = var(ident(*id));
				vartab * var_expr = var(ident(*expr));
				if (var_id->type == var_expr->type) {
					switch (apollo::statictype_checking(var_id->type)) {
						case apollo::_STRING:
							var_expr->sval = acm::memory_read(var_expr->memory_address);
							var_id->sval = var_expr->sval;
							acm::memory_write(var_id->memory_address, var_id->sval);
							break;
						case apollo::_INT:
							var_expr->ival = atoi(acm::memory_read(var_expr->memory_address).c_str());
							var_id->ival = var_expr->ival;
							acm::memory_write(var_id->memory_address, to_string(var_id->ival));
							break;
						case apollo::_DOUBLE:
							var_expr->dval = atof(acm::memory_read(var_expr->memory_address).c_str());
							var_id->dval = var_expr->dval;
							acm::memory_write(var_id->memory_address, to_string(var_id->dval));
							break;
						case apollo::_BOOL:
							var_expr->bval = acm::memory_read(var_expr->memory_address) == "1" ? true : false;
							var_id->bval = var_expr->bval;
							acm::memory_write(var_id->memory_address, to_string(var_id->bval));
							break;
					}
				} else {
					cout << "\n\tYou can't pass a value of type " << var_expr->type << " to a variable of type " << var_id->type << "." << endl;
					// return new string("type_error_&var_eae");
				}
			} else {
				cout << "\t'" << *id << "' is an invalid identifier." << endl;
				// return id;
			}
		}
		break;

	case NumCall:
		{
			if (is_it_valid(id)) {
				vartab * var_id = var(ident(*id));
				switch (statictype_checking(var_id->type)) {
				case apollo::_INT:
					var_id->ival = atoi(expr->c_str());
					acm::memory_write(var_id->memory_address, to_string(var_id->ival));
					break;
				case apollo::_DOUBLE:
					var_id->dval = atof(expr->c_str());
					acm::memory_write(var_id->memory_address, to_string(var_id->dval));
					break;
				default:
					cout << "\n\tYou can't pass a value of type " << (numeric_type ? "double" : "int") << " to a variable of type " << var_id->type << "." << endl;
					break;
				}
			} else {
				cout << "\t'" << *id << "' is an invalid identifier." << endl;
			}
		}
		break;
	
	case ACT_STRING_EXPR_OP_R:
		break;
	case ACT_INT_EXPR_OP_R:
		if (is_it_valid(id)) {
			if (!var(ident(*id))) return id;
			vartab * var_id = var(ident(*id));
			var_id->ival = atoi(expr->c_str());
			acm::memory_write(var_id->memory_address, to_string(var_id->ival));
		} else {
			cout << "\t'" << *id << "' is an invalid identifier." << endl;
		}
		break;
	}
	return id;
}


/* HashTable */

string apollo::HashTable::get_desc (string key) {
	for (struct keys * k = keys_t; k; k = k->next) {
		if (k->key == key) {
			for (struct desc * d = desc_t; d; d = d->next) {
				if (d->key == k->key) return d->descript;
			}
		}
	}
	return "";
}

bool apollo::HashTable::update (string __key, string __val) 
{
	for (struct keys * k = keys_t; k; k = k->next) 
	{
		if (k->key == __key) 
		{
			for (struct vals * v = vals_t; v; v = v->next) 
			{
				if (v->key == __key) 
				{
					v->value = __val;
					return true;
				}
			}
		}
	}
	return false;
}

bool apollo::HashTable::put (string key, string value) {
	/* The presence of k directly overrides v->value.s */
	if (get_nohint(key) != "") {
		struct vals * v = new struct vals;
		/* Here, because I have enough confidence in my logic, I just 
		   skip the judgment. 
		*/
		v->value = value;
		return true;
	}

	struct keys * k = new struct keys;
	struct vals * v = new struct vals;

	k->key		= key;
	v->key 		= key;
	v->value	= value;

	k->next		= keys_t;
	keys_t		= k;
	v->next		= vals_t;
	vals_t		= v;

	++size;

	return true;
}

bool apollo::HashTable::puts (string key, string value, string desc) {
	/* The presence of k directly overrides v->value.s */

	if (get_nohint(key) != "") {
		struct vals * v = new struct vals;
		/* Here, because I have enough confidence in my logic, I just 
		   skip the judgment. 
		*/
		// v->value = value;
		return true;
	}

	struct keys * k = new struct keys;
	struct vals * v = new struct vals;
	struct desc * d = new struct desc;

	k->key		= key;
	v->key 		= key;
	v->value	= value;
	d->key		= key;
	d->descript	= desc;

	k->next		= keys_t;
	keys_t		= k;
	v->next		= vals_t;
	vals_t		= v;
	d->next		= desc_t;
	desc_t		= d;

	++size;

	return true;
}

string apollo::HashTable::get (string key) {
	for (struct keys * k = keys_t; k; k = k->next) {
		if (k->key == key) {
			for (struct vals * v = vals_t; v; v = v->next) {
				if (v->key == k->key) return v->value;
			}
			cout << "\n\tSorry, no value bound to key(" << k->key << ") found." << endl;
			return "";
		}
	}
	// cout << "\n\tSorry, please check if you have put this key(" << key << ") to the HashTable." << endl;
	return "";
}

string apollo::HashTable::get_nohint (string key) {
	for (struct keys * k = keys_t; k; k = k->next) {
		if (k->key == key) {
			for (struct vals * v = vals_t; v; v = v->next) {
				if (v->key == k->key) return v->value;
			}
		}
	}
	return "";
}

string apollo::HashTable::alpha_beta (string __function_signature, string __n) {
	for (struct keys * k = keys_t; k; k = k->next) {
		for (struct vals * v1 = vals_t; v1; v1 = v1->next) {
			if (k->key == __function_signature && (v1->value == __n && v1->key == k->key)) {
				for (struct desc * v2 = desc_t; v2; v2 = v2->next)
					if (v2->key == k->key) {
						cout << "α(a) = " << k->key << endl;
						cout << "β(a) = " << v1->value << endl;
						cout << "β(b) = '" << v2->descript << "'" << endl;
						return v2->descript;
					}
			}
		}
	}
	return "";
}
/* End of HashTable */


/* function */
void apollo::ftis_init (apollo::identset * sym) {
	tmp_func_params.clear();
	tmp_func_members.clear();
	// set_scope(sym);
}

vector<string> apollo::put_func_param (string params_type) {
	tmp_func_params.push_back(params_type);
	return tmp_func_params;
}

vector<apollo::identset *> apollo::put_func_member (apollo::identset * member) {
	tmp_func_members.push_back(member);
	return tmp_func_members;
}

apollo::functab * apollo::function (apollo::identset * func_sym, vector<string> params, string ret_type, apollo::access_modifiers access, vector<apollo::identset *> members) {
	functab * func = new functab;

	func->funcsign 			= gen_func_signature(func_sym, params);
	func->params			= params;
	func->return_type		= ret_type;
	func->access			= access;
	func->funcsign->state 	= true;
	func->parent_scope		= backup_scope;
	func->members			= members;

	func->next				= funcsym;
	funcsym					= func;

	return func;
}

apollo::functab * apollo::function (apollo::identset * func_sym, vector<string> params, vector<identset *> members)
{
	for (int i = cache_k.size() - 1; i < cache_k.size(); --i) 
		code_func.put(cache_k[i], cache_v[i]);

	functab * func = new functab;

	func->funcsign 			= gen_func_signature(func_sym, params);
	func->funcsign->what	= "function";
	func->funcsign->state	= true;
	
	func->parent_scope		= backup_scope;
	
	string symset_summary	= symbol_set_name(func->parent_scope, func->funcsign);
	map_symset[func->funcsign] = symset_summary;
	string __symbol_set_loc	= (string)combstr(combstr(combstr(memory_env_loc, "F\\F\\F\\F\\F\\F\\F\\F\\"), fn), "\\Symbol-Set\\");
	string __function_set_loc 
							= (string)combstr(combstr(combstr(memory_env_loc, "F\\F\\F\\F\\F\\F\\F\\F\\"), fn), "\\Function-Set\\");
	// Summary of formal parameter references
	string __SFR_loc		= (string)combstr(combstr(combstr(memory_env_loc, "F\\F\\F\\F\\F\\F\\F\\F\\"), fn), "\\SFR\\");
	string __temp_uss_loc	= (string)combstr(combstr(combstr(memory_env_loc, "F\\F\\F\\F\\F\\F\\F\\F\\"), fn), "\\temp-uss\\");

	if (no_args) 
	{
		if (_ACCESS_( combstr(__symbol_set_loc, 
					combstr(symset_summary,
					combstr(".", to_string(pci)))), 0) == 0) 
		{
			if (!func_risk_guarantee) 
			{
				log_debug("%return nullptr - func - pci");
				return nullptr;
			}
			func_risk_guarantee = false;
		}
	}
	else
	{
		if (_ACCESS_(combstr(__symbol_set_loc, symset_summary), 0) == 0) 
		{
			if (!func_risk_guarantee) 
			{
				log_debug("%return nullptr - func - un-pci - " + __symbol_set_loc + symset_summary);
				return nullptr;
			}
		}
		func_risk_guarantee = false;
	}

	func->original_ident	= *func_sym->name;
	func->params			= params;
	func->members			= members;

	func->memory_address	= acm::memory_write(
		acm::__memory_alloc(acm::_M__ALLOC_FUNCTION)
		,
		code_func.get(*func->funcsign->name)
	);
	acm::memory_write(func->memory_address, *func->funcsign->name, ".signature");
	temp_uss_aggregation(acm::_M__ALLOC_FUNCTION, *func->funcsign->name, "", symset_summary, "function");

	if (_ACCESS_(__symbol_set_loc.c_str(), 0) == -1)		system(combstr("mkdir ", __symbol_set_loc));
	if (_ACCESS_(__function_set_loc.c_str(), 0) == -1)	system(combstr("mkdir ", __function_set_loc));
	if (_ACCESS_(__SFR_loc.c_str(), 0) == -1) 			system(combstr("mkdir ", __SFR_loc));
	if (_ACCESS_(__temp_uss_loc.c_str(), 0) == -1) 		system(combstr("mkdir ", __temp_uss_loc));

	if (no_args) 
	{
		if (!func_risk_guarantee) 
		{
			string loc_symbol	= (fn + "\\Symbol-set\\" + symset_summary + "." + to_string(pci));
			string loc_function	= (fn + "\\Function-Set\\" + *func->funcsign->name + "." + to_string(pci));

			ACMC::__acm_volatile_load_store(_0xffffffff, func->memory_address, loc_symbol);
			ACMC::__acm_volatile_load_store(_0xffffffff, func->memory_address, loc_function);

			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc_symbol);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc_function);
		} 
		func_risk_guarantee = false;
	}
	else 
	{
		if (!func_risk_guarantee) 
		{
			string loc_symbol	= (fn + "\\Symbol-Set\\" + symset_summary);
			string loc_function	= (fn + "\\Function-Set\\" + *func->funcsign->name);

			ACMC::__acm_volatile_load_store(_0xffffffff, func->memory_address, loc_symbol);
			ACMC::__acm_volatile_load_store(_0xffffffff, func->memory_address, loc_function);

			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc_symbol);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc_function);
		} 
		func_risk_guarantee = false;
	}

	func->next				= funcsym;
	funcsym					= func;

	return func;
}

apollo::functab * apollo::function (apollo::identset * ident) {
	for (functab * p = funcsym; p; p = p->next) {
		if (*p->funcsign->name == *ident->name) 
			return p;
	}
	// cout << "\n\tSorry, the function symbol you want to return is not found, maybe you can check for spelling errors." << endl;
	return nullptr;
}
apollo::functab * apollo::function (string ident_str) {
	for (functab * p = funcsym; p; p = p->next) {
		if (*p->funcsign->name == ident_str) 
			return p;
	}
	// cout << "\n\tSorry, the function symbol you want to return is not found, maybe you can check for spelling errors." << endl;
	return nullptr;
}

/* function signatrue */
apollo::identset * apollo::gen_func_signature ( apollo::identset * funcsym, vector<string> funcparamslist) {
	identset * signa = new identset;

	string * s = new string("_F__");

	s->append(*funcsym->name);
	s->append("_");

	for (auto const& t : funcparamslist) {
		switch (statictype_checking(t)) {
			case _STRING: 
				s->append("s");
				break;
			case _INT:
				s->append("i");
				break;
			case _DOUBLE:
				s->append("d");
				break;
			case _BOOL:
				s->append("b");
				break;
			
			default:
				s->append("u0");
				break;
		}
	}

	signa->name  = s;

	// REMOVE_SCOPE
	// signa->scope = funcsym->scope;
	signa->state = funcsym->state;
	signa->next  = funcsym->next;

	return signa;
}

/* End of function */


/* Status */
void apollo::set_scope (identset symbol) {
	backup_scope	= scope();
	crt_scope		= symbol;
}

apollo::identset apollo::scope () {
	return crt_scope;
}
/* End of Status */

apollo::identset * apollo::put_ident (string * ident_ptr, int mode) 
{
	switch (mode) 
	{
	case 0:	/* Default */
		put_ident_mode = 0;
		break;
	case 1: /* Match Layer MM */
		put_ident_mode = 1;
		break;
	default:
		put_ident_mode = 0;
		break;
	}
	return put_ident(ident_ptr);
}

/* ident_symset */
apollo::identset * apollo::put_ident (string * ident_ptr) 
{
// @{ Match Layer Mapping Mechanism.
//    Refer Manuscript: Match Layer Mapping Mechanism. 2023-1-2
	if (__is_func && put_ident_mode == 1) 
	{
		log_debug("------------------------------");
		log_debug("Match Layer MM -> Start");
		// set default
		put_ident_mode = 0;

		string	type	= acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + *ident_ptr + ".type")),
				symset	= acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + *ident_ptr + "." + fn)),
				what	= acm::memory_read(_0xffffffff, (fn + "\\temp-uss\\" + *ident_ptr + ".what"));
		status_packets	* tpack = new status_packets, 
						* mpack = new status_packets;
		vartab			* var;

		if (type != _0xffffffff && symset != _0xffffffff) 
		{
			tpack->effective				= 
			mpack->effective				=
			tpack->_val_b					=
			put_var_risk_guarantee			=
			temp_uss_aggregation_jump_once	= true;
			
			tpack->_val_s = type;
			
			mpack->_val_s = acm::memory_read(_0xffffffff, (fn + "\\Symbol-Set\\" + symset));
		} else 
			tpack->effective = mpack->effective = false;

		if (tpack->effective) 
		{	/* Start building an instance of the mapped object. */
			log_debug("Match Layer MM -> effective: yes : variable");

MALLOC_SET(_MALLOC_PROHIBIT);
			switch (statictype_checking(tpack->_val_s)) 
			{
			case _INT: 
				var = definition_variable(put_ident(ident_ptr), tpack->_val_s, atoi(acm::memory_read(mpack->_val_s).c_str()));
				log_debug("Match Layer MM -> Before ADDR: " + var->memory_address);
				log_debug("Match Layer MM -> " + var->memory_address + " = " + acm::memory_read(var->memory_address));
				break;
			case _STRING: 
				var	= definition_variable(put_ident(ident_ptr), tpack->_val_s, acm::memory_read(mpack->_val_s));
				break;
			case _DOUBLE: 
				var	= definition_variable(put_ident(ident_ptr), tpack->_val_s, atof(acm::memory_read(mpack->_val_s).c_str()));
				break;
			case _BOOL: 
				var	= definition_variable(put_ident(ident_ptr), tpack->_val_s, acm::memory_read(mpack->_val_s) == "1" ? true : false);
				break;
			default:
				tpack->_val_b = false;
				log_debug("error-type: " + tpack->_val_s);
				break;
			}
MALLOC_SET(_MALLOC_ALLOW);

			if (tpack->_val_b) 
			{
				if (var) 
				{
					var->memory_address = mpack->_val_s;
					log_debug("Match Layer MM -> After ADDR: " + var->memory_address);
					log_debug("Match Layer MM -> " + var->memory_address + " = " + acm::memory_read(var->memory_address));
					var->scope			= null_scope;
					var->ident->state	= true;
					acm::memory_write("0xFFFFFFFF", var->memory_address,
						fn + "\\Symbol-Set\\" + symset
					);
					log_debug("Match Layer MM -> done");
				}
			}
		} 
		else log_debug("Match Layer MM -> don't know what : " + what);

		delete(tpack);
		delete(mpack);

		log_debug("Match Layer MM -> End");
		log_debug("------------------------------");
	}
// @} End of `Match Layer MM`.


	identset * ident_t = new identset;

	ident_t->name = ident_ptr;
	
	if (ident(*ident_ptr) != nullptr && is_it_valid(ident_ptr)) 
		ident_t->state = true;
	else 
		ident_t->state = false;

	ident_t->next = identsym;
	identsym = ident_t;

	if (__DEFINE_FUNCTION_SWITCH) {
		ident_t->what = "function";
		set_scope(*ident_t);
		__DEFINE_FUNCTION_SWITCH = false;

		__ALLOC_FUNCTION_REAL_PARAMETERS = true;
	}

	return ident_t;
}

apollo::identset * apollo::ident (string ident_name) {
	for (identset * p = identsym; p; p = p->next) {
		if (*p->name == ident_name) {
			return p;
		}
	}

	return nullptr;
}

bool apollo::is_it_valid (string * ident_ptr) {
	identset * ident_t = ident(*ident_ptr);
	return ident_t ? ident_t->state ? true : false : false;
}
/* End of ident_symset */


/* var_symtab */
apollo::vartab * apollo::definition_variable (apollo::identset * ident_ptr, string type, string sval) {
	vartab * var = new vartab;

	var->ident 			= ident_ptr;
	var->ident->what	= "variable";
	var->ident->state 	= true;
	var->scope			= scope();

	var->next  			= varsym;
	varsym	   			= var;

	string symset_summary 
						= symbol_set_name(var->scope, var->ident);
	map_symset[ident_ptr] = symset_summary;
	string __symbol_set_loc	
						= (string)combstr(combstr(combstr(memory_env_loc.c_str(), "F\\F\\F\\F\\F\\F\\F\\F\\"), fn.c_str()), "\\Symbol-Set\\");	

	if (no_args) 
	{
		if (_ACCESS_(	combstr(__symbol_set_loc, 
					combstr(symset_summary, 
					combstr(".", to_string(pci)))), 0) == 0) 
		{
			if (!put_var_risk_guarantee) 
			{
				log_debug("%return nullptr - sval - pci");
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	} 
	else
	{
		if (_ACCESS_(combstr(__symbol_set_loc, symset_summary), 0) == 0) {
			if (!put_var_risk_guarantee) 
			{
				log_debug("%return nullptr - sval - un-pci");
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	}

	var->type  			= type;

	acm::MemoryArea alloc_ma = __ALLOC_FUNCTION_REAL_PARAMETERS ? acm::_M__ALLOC_FUNCTION_ACTUAL : acm::_M__ALLOC_GLOBAL_VARIABLES;
	var->memory_address	= acm::__memory_alloc(alloc_ma);
	
	acm::memory_write(var->memory_address, sval);
	temp_uss_aggregation(alloc_ma, *var->ident->name, var->type, map_symset[var->ident], "variable");

	if (_ACCESS_(__symbol_set_loc.c_str(), 0) == -1) {
		string cmd = "mkdir ";
		system(combstr(cmd.c_str(), __symbol_set_loc.c_str()));
	}

	if (no_args) 
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary + "." + to_string(pci));
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}
	else
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary);
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}

	trm_var(*var->ident->name);

	return var;
}

apollo::vartab * apollo::definition_variable (apollo::identset * ident_ptr, string type, int ival) {
	vartab * var = new vartab;

	var->ident 			= ident_ptr;
	var->ident->what	= "variable";
	var->ident->state 	= true;
	var->scope			= scope();

	var->next			= varsym;
	varsym	   			= var;

	string symset_summary 
						= symbol_set_name(var->scope, var->ident);
	map_symset[ident_ptr] = symset_summary;
	string __symbol_set_loc	
						= (string)combstr(combstr(combstr(memory_env_loc.c_str(), "F\\F\\F\\F\\F\\F\\F\\F\\"), fn.c_str()), "\\Symbol-Set\\");

	if (no_args) 
	{
		if (_ACCESS_(	combstr(__symbol_set_loc, 
					combstr(symset_summary, 
					combstr(".", to_string(pci)))), 0) == 0) 
		{
			if (!put_var_risk_guarantee) 
			{
				cout << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	} 
	else
	{
		if (_ACCESS_(combstr(__symbol_set_loc, symset_summary), 0) == 0) {
			if (!put_var_risk_guarantee) 
			{
				cout << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	}

	var->type  			= type;

	acm::MemoryArea alloc_ma = __ALLOC_FUNCTION_REAL_PARAMETERS ? acm::_M__ALLOC_FUNCTION_ACTUAL : acm::_M__ALLOC_GLOBAL_VARIABLES;
	var->memory_address	= acm::__memory_alloc(alloc_ma);
	acm::memory_write(var->memory_address, to_string(ival));
	temp_uss_aggregation(alloc_ma, *var->ident->name, var->type, map_symset[var->ident], "variable");

	if (_ACCESS_(__symbol_set_loc.c_str(), 0) == -1) {
		string cmd = "mkdir ";
		system(combstr(cmd.c_str(), __symbol_set_loc.c_str()));
	}

	if (no_args) 
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary + "." + to_string(pci));
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}
	else
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary);
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}

	trm_var(*var->ident->name);

	return var;
}

apollo::vartab * apollo::definition_variable (apollo::identset * ident_ptr, string type, double dval) {
	vartab * var = new vartab;

	var->ident 			= ident_ptr;
	var->ident->what	= "variable";
	var->ident->state 	= true;
	var->scope			= scope();

	var->next			= varsym;
	varsym	   			= var;

	string symset_summary 
						= symbol_set_name(var->scope, var->ident);
	map_symset[ident_ptr] = symset_summary;
	string __symbol_set_loc	
						= (string)combstr(combstr(combstr(memory_env_loc.c_str(), "F\\F\\F\\F\\F\\F\\F\\F\\"), fn.c_str()), "\\Symbol-Set\\");

	if (no_args) 
	{
		if (_ACCESS_(	combstr(__symbol_set_loc, 
					combstr(symset_summary, 
					combstr(".", to_string(pci)))), 0) == 0) 
		{
			if (!put_var_risk_guarantee) 
			{
				cerr << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	} 
	else
	{
		if (_ACCESS_(combstr(__symbol_set_loc, symset_summary), 0) == 0) {
			if (!put_var_risk_guarantee) 
			{
				cout << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	}

	var->type  			= type;

	acm::MemoryArea alloc_ma = __ALLOC_FUNCTION_REAL_PARAMETERS ? acm::_M__ALLOC_FUNCTION_ACTUAL : acm::_M__ALLOC_GLOBAL_VARIABLES;
	var->memory_address	= acm::__memory_alloc(alloc_ma);

	acm::memory_write(var->memory_address, to_string(dval));
	temp_uss_aggregation(alloc_ma, *var->ident->name, var->type, map_symset[var->ident], "variable");

	if (_ACCESS_(__symbol_set_loc.c_str(), 0) == -1) {
		string cmd = "mkdir ";
		system(combstr(cmd.c_str(), __symbol_set_loc.c_str()));
	}

	if (no_args) 
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary + "." + to_string(pci));
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}
	else
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary);
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}

	trm_var(*var->ident->name);

	return var;
}

apollo::vartab * apollo::definition_variable (apollo::identset * ident_ptr, string type, bool bval) {
	vartab * var = new vartab;

	var->ident 			= ident_ptr;
	var->ident->what	= "variable";
	var->ident->state 	= true;
	var->scope			= scope();

	var->next			= varsym;
	varsym	   			= var;

	string symset_summary 
						= symbol_set_name(var->scope, var->ident);
	map_symset[ident_ptr] = symset_summary;
	string __symbol_set_loc	
						= (string)combstr(combstr(combstr(memory_env_loc.c_str(), "F\\F\\F\\F\\F\\F\\F\\F\\"), fn.c_str()), "\\Symbol-Set\\");

	if (no_args) 
	{
		if (_ACCESS_(	combstr(__symbol_set_loc, 
					combstr(symset_summary, 
					combstr(".", to_string(pci)))), 0) == 0) 
		{
			if (!put_var_risk_guarantee) 
			{
				cerr << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	} 
	else
	{
		if (_ACCESS_(combstr(__symbol_set_loc, symset_summary), 0) == 0) {
			if (!put_var_risk_guarantee) 
			{
				cout << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	}

	var->type  			= type;

	acm::MemoryArea alloc_ma = __ALLOC_FUNCTION_REAL_PARAMETERS ? acm::_M__ALLOC_FUNCTION_ACTUAL : acm::_M__ALLOC_GLOBAL_VARIABLES;
	var->memory_address	= acm::__memory_alloc(alloc_ma);

	acm::memory_write(var->memory_address, to_string(bval));
	temp_uss_aggregation(alloc_ma, *var->ident->name, var->type, map_symset[var->ident], "variable");

	if (_ACCESS_(__symbol_set_loc.c_str(), 0) == -1) {
		system(combstr("mkdir ", __symbol_set_loc));
	}

	if (no_args) 
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary + "." + to_string(pci));
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}
	else
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary);
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}

	trm_var(*var->ident->name);

	return var;
}

apollo::vartab * apollo::declaration_variable (apollo::identset * ident_ptr, string type) {
	vartab * var = new vartab;

	var->ident 			= ident_ptr;
	var->ident->what	= "variable";
	var->ident->state	= true;
	var->scope			= scope();

	var->next			= varsym;
	varsym	   			= var;

	string symset_summary 
						= symbol_set_name(var->scope, var->ident);
	map_symset[ident_ptr] = symset_summary;
	string __symbol_set_loc	
						= (string)combstr(combstr(combstr(memory_env_loc.c_str(), "F\\F\\F\\F\\F\\F\\F\\F\\"), fn.c_str()), "\\Symbol-Set\\");

	if (no_args) 
	{
		if (_ACCESS_(	combstr(__symbol_set_loc, 
					combstr(symset_summary, 
					combstr(".", to_string(pci)))), 0) == 0) 
		{
			if (!put_var_risk_guarantee) 
			{
				cerr << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	} 
	else
	{
		if (_ACCESS_(combstr(__symbol_set_loc, symset_summary), 0) == 0) {
			if (!put_var_risk_guarantee) 
			{
				cout << "\t%return nullptr" << endl;
				return nullptr;
			}
			put_var_risk_guarantee = false;
		}
	}

	var->type  			= type;

	acm::MemoryArea alloc_ma = __ALLOC_FUNCTION_REAL_PARAMETERS ? acm::_M__ALLOC_FUNCTION_ACTUAL : acm::_M__ALLOC_GLOBAL_VARIABLES;

	var->memory_address	= acm::__memory_alloc(alloc_ma);

	acm::memory_write(var->memory_address, "");
	temp_uss_aggregation(alloc_ma, *var->ident->name, var->type, map_symset[var->ident], "variable");

	if (_ACCESS_(__symbol_set_loc.c_str(), 0) == -1) {
		system(combstr("mkdir ", __symbol_set_loc));
	}

	if (no_args) 
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary + "." + to_string(pci));
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}
	else
	{
		if (!put_var_risk_guarantee) 
		{
			string loc = (fn + "\\Symbol-Set\\" + symset_summary);
			ACMC::__acm_volatile_load_store(_0xffffffff, var->memory_address, loc);
			delete_list.push_back(memory_env_loc + __0xffffffff + "\\" + loc);
		}
		put_var_risk_guarantee = false;
	}

	trm_var(*var->ident->name);

	return var;
}

apollo::vartab * apollo::var (apollo::identset * ident_ptr) {
	for (vartab * p = varsym; p; p = p->next) {
		if (*p->ident->name == *ident_ptr->name) 
			return p;
	}
	return nullptr;
}
/* End of var_symtab */


/* Static type checking */
apollo::atypes apollo::statictype_checking (string type_str) {
	atypes types;

	if 		(type_str == "str") 
		types =  _STRING;
	else if (type_str == "int") 
		types =  _INT;
	else if (type_str == "double")
		types =  _DOUBLE;
	else if (type_str == "bool")
		types =  _BOOL;
	else 
		types = NOSUPPORTTYPE;

	return types;
}
/* End of Static type checking */


/* Predictive Action */
apollo::preacts apollo::pre_action (apollo::possible_actions Action) 
{
	if (Action == PRE_ACTION_INIT) {
		pact_size = 0;
		temp_pacts.clear();
		return temp_pacts;
	} else if (Action == GET_PREACT) 
		return temp_pacts;
	
	temp_pacts.push_back(Action);
	pact_size++;
	return temp_pacts;
}
apollo::preacts apollo::pre_action_pop () 
{
	if (temp_pacts.size() != 0) 
		temp_pacts.pop_back();
	return temp_pacts;
}

const char * apollo::f_read (const char * _path) 
{
	FILE * fp = fopen(_path, "rb");
	char _bf[4096];
	memset(_bf, 0, sizeof(_bf));
	fread(_bf, sizeof(char), 4096, fp);
	fclose(fp);
	return _strdup(string((const char *)_bf).c_str());
}
/* End of Predictive Action */


/* End of definition */



extern int yylineno;
extern string scantext;
void yyerror(const char *, ...);

extern char * yytext;

void yyerror(const char * s, ...)
{
	string t(yytext);
	cerr << fn << "-" << yylineno << ": " << "Is not the expected symbol `" << t << "` and, of course, this could be a problem with the compiler itself." << endl;
	if (cplt_t != 0) 
	{
		cerr << "Address " << __func_fn << endl;
		cerr << ACMC::__acm_volatile_load_store(__func_fn, nullptr) << endl;
		exit(0);
	}
}

/* symbol table */
/* hash a symbol */
unsigned symhash(string sym)
{
	unsigned int hash = 0;
	unsigned c;

	for (auto &c : sym) {
		hash = hash * 9^c;
	}

	return hash;
}
