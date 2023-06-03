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

# pragma once

# ifndef __ACM_H
# define __ACM_H

# include <apollo\Acm\amm.hpp>

# include <iostream>
# include <vector>
# include <string>
# include <Windows.h>
# include <io.h>
# include <cmath>

using namespace std;

/* ************************************************** */
/* Macros */

# define ACMC                   acm
# define HEXOP                  __hex_op
# define OPPC                   __hex_op_pc
# define ACMENV                 acm_env
# define MA                     MemoryArea
# define MEMORY_RELEASE         "memory-release.table.k"
# define MEMORY_RELEASE_BEFORE  "memory-release."
# define MEMORY_RELEASE_AFTER   ".v"
# define MEMORY_ALLOC_LIST      __memory_alloc_list

# define EMPTY_STR              ""
# define nullstr                EMPTY_STR

# define PUSH_ADDR(addr)        ( MEMORY_ALLOC_LIST.push_back(addr) )
# define SPTR(s)                ( new string(s) )

# define MALLOC                 memory_alloc
# define _MALLOC_DFT            true
# define _MALLOC_PROHIBIT       false
# define _MALLOC_ALLOW          _MALLOC_DFT
# define MALLOC_SET(b)          ( ACMC::MALLOC = (b) )

/* Pseudo-registers of ACM */

# define __rax  "rax"
# define __rbx  "rbx"
# define __rcx  "rcx"
# define __rdx  "rdx"
# define __rex  "rex"
# define __rfx  "rfx"
# define __rgx  "rgx"
# define __rhx  "rhx"
# define __rix  "rix"
# define __rjx  "rjx"
# define __rkx  "rkx"
# define __rlx  "rlx"
# define __rmx  "rmx"
# define __rnx  "rnx"
# define __rox  "rox"
# define __rpx  "rpx"
# define __rqx  "rqx"
# define __rrx  "rrx"
# define __rsx  "rsx"
# define __rtx  "rtx"
# define __rux  "rux"
// Contextual truncation
# define __rvx  "rvx"
# define __rwx  "rwx"
# define __rxx  "rxx"
# define __ryx  "ryx"
# define __rzx  "rzx"

/* ************************************************** */

using address = string;

/* Global declaration or definition */

/* --------------------------------------------------------------------------------------------------------------------------------------- */
/**
 * |--Vector-M--|
 * | 0x00000001 |
 * | 0x00000002 |
 * | 0xABCDEF12 |
 * | ...		|
 * |------------|
*/
/// @brief Memory allocation table: The memory addresses allocated to the program by the compiler(interpreter) are recorded in this table
vector<string> MEMORY_ALLOC_LIST;
/* --------------------------------------------------------------------------------------------------------------------------------------- */

/* end */

string __acm_root = string((const char *)getenv("APOLLO_MEMORY"));

namespace ACMC
{

    inline bool __alloc_level3_memory (const string &, const string &, const string &);
    inline string __alloc_level2_memory (const string &, const string &);

    inline int64_t __hex_to_dec (const string &);
    inline string __dec_to_hex (int64_t);

    bool MALLOC = _MALLOC_DFT;

    enum MA {
        _M__CODE    = 1,
            _M__ALLOC_FUNCTION                  = 101,
            _M__ALLOC_APPOBJ                    = 102,
            _M__ALLOC_PROCESS_CONTROL           = 103,
        _M__STATIC  = 2,
            _M__ALLOC_GLOBAL_VARIABLES          = 201,
            _M__ALLOC_STATIC_VARIABLES          = 202,
            _M__ALLOC_CONSTANTS                 = 203,
        _M__TEXT    = 3,
            _M__ALLOC_STRING_CONSTANTS          = 301,
        _M__HEAP    = 4,
            _M__ALLOC_DISCRETIONARY             = 401,
        _M__STACK   = 5,
            _M__ALLOC_FUNCTION_ACTUAL           = 501,
            _M__ALLOC_LOCAL_VARIABLES           = 502,
            _M__ALLOC_FUNCTION_RETURN_VALUE     = 503,
        _M__DATA    = 6,
            _M__ALLOC_COMPILE_TIME              = 601,
            _M__ALLOC_RUNTIME                   = 602,
            _M__ALLOC_LEVEL_1_DATA              = 603,
            _M__ALLOC_LEVEL_2_DATA              = 604,
        _M__EXTENS  = 7,
            _M__ALLOC_CACHE_A                   = 701,
            _M__ALLOC_CACHE_B                   = 702,
            _M__ALLOC_TEMPORARY                 = 703,
            _M__ALLOC_NOT_EASY_TO_DIVIDE        = 704
    };

    inline string __acm_volatile_load_store (const string &, string *);
    inline string __acm_volatile_load_store (const string &, string *, const string &);
    inline string __acm_volatile_load_store (const string &, const string &);
    inline string __acm_volatile_load_store (const string &, const string &, const string &);

    inline string __memory_read (const string &);
    inline string __memory_read (const string &, const string &);
    inline string __memory_write (const string &, const string &);
    inline string __memory_write (const string &, const string &, const string &);

    /* Compatible with the previous writing style */
    inline string memory_read(const string &);
    inline string memory_read(const string &, const string &);
    inline string memory_write(const string &, const string &);
    inline string memory_write(const string &, const string &, const string &);

    /* don't use __forceinline */
    inline string __memory_alloc (MA);

    inline string __convert (const string &, bool);
    inline string __f_read (const string &);
    inline void __f_write (const string &, const string &);
    inline string __cd (const string &, const string &, int);
    inline string __correction (const string &);
    inline bool __verify (const string &);
    inline bool __memory_release (const string &);

    inline bool occupy (const string &);

    /* ************************************************** */
    /* Pseudo-registers of ACM */
    /* ************************************************** */
    
    /* Return Value */
    
    inline string rvs 
    (
        string *__volatile_swt
    );
    inline string rvs 
    (
        const string &__data
    );

    inline int rvi ();

    template<typename RT, typename T> 
    RT rax (T t) 
    {
        if (typeid(t) == typeid(int)) 
        {
            cout << "__rvx " << __acm_volatile_load_store(_0xffffffff, nullptr, __rvx) << endl;
        }
    }

    /* ************************************************** */

    /**
     * If the actual argument is ACM memory address, the memory is read and the data is returned.
     * If the actual argument is an absolute address, the __cd() function relocates it and then reads and returns the data.
     * @return (ACM) Memory Address
    */
    inline string codes_rv_addr (const string &);

    class HEXOP {
    public:
        string _ADDR;
        HEXOP operator+(const HEXOP& __R_OP_ADDR) {
            HEXOP ret;
            ret._ADDR = __dec_to_hex((__hex_to_dec(this->_ADDR) + __hex_to_dec(__R_OP_ADDR._ADDR)));
            return ret;
        }
        HEXOP operator-(const HEXOP& __R_OP_ADDR) {
            HEXOP ret;
            ret._ADDR = __dec_to_hex((__hex_to_dec(this->_ADDR) - __hex_to_dec(__R_OP_ADDR._ADDR)));
            return ret;
        }
        HEXOP operator*(const HEXOP& __R_OP_ADDR) {
            HEXOP ret;
            ret._ADDR = __dec_to_hex((__hex_to_dec(this->_ADDR) * __hex_to_dec(__R_OP_ADDR._ADDR)));
            return ret;
        }
        HEXOP operator/(const HEXOP& __R_OP_ADDR) {
            HEXOP ret;
            ret._ADDR = __dec_to_hex((__hex_to_dec(this->_ADDR) / __hex_to_dec(__R_OP_ADDR._ADDR)));
            return ret;
        }

        inline string plus  (const string &__op1, const string &__op2) 
        {
            HEXOP op1, op2;
                op1._ADDR = __op1;
                op2._ADDR = __op2;
            return (op1 + op2)._ADDR;
        }
        inline string sub   (const string &__op1, const string &__op2) 
        {
            HEXOP op1, op2;
                op1._ADDR = __op1;
                op2._ADDR = __op2;
            return (op1 - op2)._ADDR;
        }
        inline string mul   (const string &__op1, const string &__op2) 
        {
            HEXOP op1, op2;
                op1._ADDR = __op1;
                op2._ADDR = __op2;
            return (op1 * op2)._ADDR;
        }
        inline string div   (const string &__op1, const string &__op2) 
        {
            HEXOP op1, op2;
                op1._ADDR = __op1;
                op2._ADDR = __op2;
            return (op1 / op2)._ADDR;
        }

        inline string plus_addr (const string &__op1, const string &__op2) 
        {
            return ACMC::__correction(plus(__op1, __op2));
        }
        inline string sub_addr  (const string &__op1, const string &__op2) 
        {
            return ACMC::__correction(sub(__op1, __op2));
        }
        inline string mul_addr  (const string &__op1, const string &__op2) 
        {
            return ACMC::__correction(mul(__op1, __op2));
        }
        inline string div_addr  (const string &__op1, const string &__op2) 
        {
            return ACMC::__correction(div(__op1, __op2));
        }
    };

    class OPPC : HEXOP 
    {
    private:
        string _value = EMPTY_STR;
    public:
        enum OPPC_OP 
        {
            PLUS    = 0,
            SUB     = 1,
            MUL     = 2,
            DIV     = 3
        };
        using _OPPC_OP = enum OPPC_OP;

        OPPC (const string &__op1, OPPC::_OPPC_OP op, const string &__op2) 
        {
            if (__hex_to_dec(__op1) >= __hex_to_dec(AMM::_memory->code_area->_ProcessControl_HEAD)) 
            {
                switch (op) 
                {
                case this->PLUS:
                    this->volatile_set_get(new string(plus_addr(__op1, __op2)));
                    break;
                case this->SUB:
                    this->volatile_set_get(new string(sub_addr(__op1, __op2)));
                    break;
                case this->MUL:
                    this->volatile_set_get(new string(mul_addr(__op1, __op2)));
                    break;
                case this->DIV:
                    this->volatile_set_get(new string(div_addr(__op1, __op2)));
                    break;

                default:
                    // cerr << __FILE__ << " at " << __LINE__ << " <error-token>: " << op << endl;
                    break;
                }
            } else 
            {
                // cerr << __FILE__ << " at " << __LINE__ << " <error-address>: " << __op1 << endl;
                this->volatile_set_get(new string(__op1));
            }
        }
        ~OPPC () 
        {
            /* ignore */
        }

        inline string volatile_set_get (string *__volatile_switch) 
        {
            if (__volatile_switch != nullptr) 
                this->_value = *__volatile_switch;
            else 
                return this->_value;
            return *__volatile_switch;
        }
    };

};


/* ************************************************** */
/* Pseudo-registers of ACM */

/* Return Value */

inline string ACMC::rvs (string *__volatile_swt) 
{
    return __volatile_swt != nullptr 
        ? __acm_volatile_load_store(_0xffffffff, __volatile_swt, __rvx) 
        : __acm_volatile_load_store(_0xffffffff, nullptr, __rvx);
}

inline string ACMC::rvs (const string &__data) 
{
    return rvs(new string(__data));
}

inline int ACMC::rvi () 
{
    return atoi(rvs(nullptr).c_str());
}


/* ************************************************** */

inline bool ACMC::occupy (const string &__addr) 
{
    string t = MEMORY_RELEASE_BEFORE + __addr + MEMORY_RELEASE_AFTER;
    if ("1" == __acm_volatile_load_store(_0xffffffff, nullptr, t)) 
        return false;
    __acm_volatile_load_store(_0xffffffff, "1", t);
    return true;
}

inline string ACMC::codes_rv_addr (const string &__volatile_arg) 
{
    if (__volatile_arg.size() == 0) 
    {
        // ERROR
        return EMPTY_STR;
    } else 
    {
        if (__volatile_arg[1] == 'x') 
        {
            /* (ACM) Memory Address */
            // Codes aka msp/appobj [0x7A121, 0xF4240]
            if (__hex_to_dec(__volatile_arg) >= __hex_to_dec(AMM::_memory->code_area->_APPOBJ_HEAD) 
             && __hex_to_dec(__volatile_arg) <= __hex_to_dec(AMM::_memory->code_area->_APPOBJ_TAIL))
            {
                // Is Codes
                // Read and return it
                return __acm_volatile_load_store(__volatile_arg, nullptr, ".rvaddr");
            } else 
            {
                return EMPTY_STR;
            }
        } else 
        {
            /* DIR */
            string temp_path = __cd(__volatile_arg, ".rvaddr", 0);
            if (_access(temp_path.c_str(), 0) == 0) 
            {
                // Read and return it
                return __f_read(temp_path);
            } else 
            {
                return EMPTY_STR;
            }
        }
    }
    return EMPTY_STR;
}

inline string ACMC::__acm_volatile_load_store (const string &__addr, string *__data) 
{
    if (__data == nullptr) 
        return __memory_read(__addr);
    __memory_write(__addr, *__data);
    return *__data;
}
inline string ACMC::__acm_volatile_load_store (const string &__addr, const string &__data) 
{
    return __acm_volatile_load_store(__addr, new string(__data));
}
inline string ACMC::__acm_volatile_load_store (const string &__addr, string *__data, const string &__target) 
{
    if (__data == nullptr) 
        return __memory_read(__addr, __target);
    __memory_write(__addr, *__data, __target);
    return *__data;
}
inline string ACMC::__acm_volatile_load_store (const string &__addr, const string &__data, const string &__target) 
{
    return __acm_volatile_load_store(__addr, new string(__data), __target);
}

inline string ACMC::__memory_read (const string &__addr) 
{
    return AMM::_amm__memory_read(__addr);
}
inline string ACMC::__memory_read (const string &__addr, const string &__target) 
{
    string target = __cd(__convert(__addr, false), __target, 0);
    if (_access(target.c_str(), 0) == -1) 
        return __addr;
    return __f_read(target);
}
inline string ACMC::memory_read (const string &__addr) 
{
    return __memory_read(__addr);
}
inline string ACMC::memory_read(const string &__addr, const string &__target) 
{
    return __memory_read(__addr, __target);
}

inline string ACMC::__memory_write (const string &__addr, const string &__data) 
{
    return AMM::_amm__memory_write(__addr, __data);
}
inline string ACMC::__memory_write (const string &__addr, const string &__data, const string &__target) 
{
    string target = __cd(__convert(__addr, true), __target, 0);
    __f_write(target, __data);
    return __addr;
}
inline string ACMC::memory_write (const string &__addr, const string &__data) 
{
    return __memory_write(__addr, __data);
}
inline string ACMC::memory_write (const string &__addr, const string &__data, const string &__target) 
{
    return __memory_write(__addr, __data, __target);
}

inline string ACMC::__convert (const string &__addr, bool __create) 
{
    return AMM::_amm__convert(__addr, __create);
}

inline string ACMC::__f_read (const string &__path)
{
    return AMM::_amm__f_read(__path);
}

inline void ACMC::__f_write (const string &__file, const string &__data)
{
    AMM::_amm__f_write(__file, __data);
}

inline string ACMC::__cd (const string &__sa, const string &__sb, int w) 
{
	int n = 0, n1 = 0;
	string r;

	for (auto const &c : __sa) 
		if (c == '\\' || c == '/') ++n;

	for (auto const &c : __sa) {
		r = r + c;
		if (c == '\\' || c == '/') 
			++n1;
		if (n1 == (n - w)) 
			break;
	}
    r += __sb;

    return r;
}

inline string ACMC::__dec_to_hex (int64_t __dec) 
{
    string hex = EMPTY_STR;
    string arr[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };  

    while(true) {
        int remainder = __dec % 16;
        hex = arr[remainder] + hex;

        __dec = __dec / 16;

        if (__dec <= 0) 
            break;
    }

    return hex;
}

inline int64_t ACMC::__hex_to_dec(const string &__hex) 
{
    int64_t decNum = 0;
    int j = 0;
    for (int i = __hex.length() - 1; i >= (__hex[1] == 'x' ? 2 : 0); i--) 
	{
        char c = __hex[i];
        int n = 0;

        if (c >= '0' && c <= '9') {
            n = c - '0';
        } else if (c >= 'a' && c <= 'f') {  
            n = 10 + c - 'a';
        } else if (c >= 'A' && c <= 'F') {  
            n = 10 + c - 'A';  
        } else{

            return -1;
        }  

       	decNum += n * pow(16, j);

       	j++;  
    }  

    return decNum;
} 

inline string ACMC::__correction (const string &__hex) 
{
    int size = 10;
    string s = "0x";

    if (__hex.size() > 8) 
        return __hex;
    else if (__hex.size() <= 8) 
    {
        for (; s.size() + __hex.size() < size; ) 
            s += '0';
    }

    s += __hex;
    return s;
}

inline bool ACMC::__alloc_level3_memory (const string &__addr, const string &__segh, const string &__segt) 
{
    int tmp_dec = __hex_to_dec(__addr);

    if ((tmp_dec >= __hex_to_dec(__segh) + 1) && (tmp_dec <= __hex_to_dec(__segt))) 
    {
        if (__memory_read(_0xffffffff, (MEMORY_RELEASE_BEFORE + __addr + MEMORY_RELEASE_AFTER)) == "0") 
        {
            __memory_write(_0xffffffff, "1", (MEMORY_RELEASE_BEFORE + __addr + MEMORY_RELEASE_AFTER));
            MEMORY_ALLOC_LIST.push_back(__addr);
            return true;
        }
    }
    return false;
}

inline string ACMC::__alloc_level2_memory (const string &__segh, const string &__segt) 
{
    HEXOP hexop_t;

    string ret = __memory_read(__segh);
    string tmp = hexop_t.plus_addr(ret, "0x1");

    if (__hex_to_dec(tmp) <= __hex_to_dec(__segt)) 
    {
        __acm_volatile_load_store(__segh, tmp);
        return ret;
    }
    else 
        cout << "The memory address (" << tmp << ") is greater than the maximum range of the specified data segment." << endl;
    return EMPTY_STR;
}

inline string ACMC::__memory_alloc (ACMC::MA __ma) 
{
    /*  2023-1-25 21-8  Match Layer MM (function) cancel alloc memory. Use C++ Macro */
    if (!MALLOC) return nullstr;

    bool stop = false;
    string level3_memory = __acm_volatile_load_store(_0xffffffff, nullptr, MEMORY_RELEASE);

    if (level3_memory.size() <= 2) stop = true;

    string addr, ret;

    // level3-memory
    for (auto const &c : level3_memory) 
    {
        if (stop)       break;
        if (c == '[')   addr.clear();
        else
        if (c == ']')
        {
            bool success = false;

            switch (__ma) 
            {
            /* Code */
            case _M__ALLOC_FUNCTION:
                success = __alloc_level3_memory(addr, AMM::_memory->code_area->_FUNCTION_HEAD, AMM::_memory->code_area->_FUNCTION_TAIL);
                break;
            case _M__ALLOC_APPOBJ:
                success = __alloc_level3_memory(addr, AMM::_memory->code_area->_APPOBJ_HEAD, AMM::_memory->code_area->_APPOBJ_TAIL);
                break;

            case _M__ALLOC_PROCESS_CONTROL:
# ifdef AMM_2023
                success = __alloc_level3_memory(addr, AMM::_memory->code_area->_ProcessControl_HEAD, AMM::_memory->code_area->_ProcessControl_TAIL);
# elif defined(AMM_2022)
                success = __alloc_level3_memory(addr, AMM::_memory->code_area->_FUNCTION_HEAD, AMM::_memory->code_area->_FUNCTION_TAIL);
# endif /* AMM */
                break;

            /* Static */
            case _M__ALLOC_GLOBAL_VARIABLES:
                success = __alloc_level3_memory(addr, AMM::_memory->static_area->_GLOBALVARS_HEAD, AMM::_memory->static_area->_GLOBALVARS_TAIL);
                break;
            case _M__ALLOC_STATIC_VARIABLES:
                success = __alloc_level3_memory(addr, AMM::_memory->static_area->_STATICVARS_HEAD, AMM::_memory->static_area->_STATICVARS_TAIL);
                break;
            case _M__ALLOC_CONSTANTS:
                success = __alloc_level3_memory(addr, AMM::_memory->static_area->_CONSTANT_HEAD, AMM::_memory->static_area->_CONSTANT_TAIL);
                break;

            /* Text */
# ifdef AMM_2023
            case _M__ALLOC_STRING_CONSTANTS:
            case _M__TEXT:
                success = __alloc_level3_memory(addr, AMM::_memory->text_area->_StringConstants_HEAD, AMM::_memory->text_area->_StringConstants_TAIL);
# elif defined(AMM_2022)
            case _M__ALLOC_STRING_CONSTANTS:
            case _M__TEXT:
                success = __alloc_level3_memory(addr, AMM::_memory->text_area->_MEM_AREA_TEXT_HEAD, AMM::_memory->text_area->_MEM_AREA_TEXT_TAIL);
# endif /* AMM */
                break;

            /* Heap */
# ifdef AMM_2023
            case _M__ALLOC_DISCRETIONARY:
            case _M__HEAP:
                success = __alloc_level3_memory(addr, AMM::_memory->heap_area->_Discretionary_HEAD, AMM::_memory->heap_area->_Discretionary_TAIL);
# elif defined(AMM_2022)
            case _M__ALLOC_DISCRETIONARY:
            case _M__HEAP:
                success = __alloc_level3_memory(addr, AMM::_memory->heap_area->_MEM_AREA_HEAP_HEAD, AMM::_memory->heap_area->_MEM_AREA_HEAP_TAIL);
# endif /* AMM */
                break;
            
            /* Stack */
            case _M__ALLOC_FUNCTION_ACTUAL:
                success = __alloc_level3_memory(addr, AMM::_memory->stack_area->_FunctionActual_HEAD, AMM::_memory->stack_area->_FunctionActual_TAIL);
                break;
            case _M__ALLOC_LOCAL_VARIABLES:
                success = __alloc_level3_memory(addr, AMM::_memory->stack_area->_LOCALVARS_HEAD, AMM::_memory->stack_area->_LOCALVARS_TAIL);
                break;
            case _M__ALLOC_FUNCTION_RETURN_VALUE:
                success = __alloc_level3_memory(addr, AMM::_memory->stack_area->_FRV_HEAD, AMM::_memory->stack_area->_FRV_TAIL);
                break;
            
            /* Data */
# ifdef AMM_2023
            case _M__ALLOC_COMPILE_TIME:
                success = __alloc_level3_memory(addr, AMM::_memory->data_area->_CompileTime_HEAD, AMM::_memory->data_area->_CompileTime_TAIL);
                break;
            case _M__ALLOC_RUNTIME:
                success = __alloc_level3_memory(addr, AMM::_memory->data_area->_Runtime_HEAD, AMM::_memory->data_area->_Runtime_TAIL);
                break;
            case _M__ALLOC_LEVEL_1_DATA:
                success = __alloc_level3_memory(addr, AMM::_memory->data_area->_Level1data_HEAD, AMM::_memory->data_area->_Level1data_TAIL);
                break;
            case _M__ALLOC_LEVEL_2_DATA:
                success = __alloc_level3_memory(addr, AMM::_memory->data_area->_Level2data_HEAD, AMM::_memory->data_area->_Level2data_TAIL);
                break;

            /* Extension */
            case _M__ALLOC_CACHE_A:
                success = __alloc_level3_memory(addr, AMM::_memory->extension_area->_CacheA_HEAD, AMM::_memory->extension_area->_CacheA_TAIL);
                break;
            case _M__ALLOC_CACHE_B:
                success = __alloc_level3_memory(addr, AMM::_memory->extension_area->_CacheB_HEAD, AMM::_memory->extension_area->_CacheB_TAIL);
                break;
            case _M__ALLOC_TEMPORARY:
                success = __alloc_level3_memory(addr, AMM::_memory->extension_area->_Temporary_HEAD, AMM::_memory->extension_area->_Temporary_TAIL);
                break;
            case _M__ALLOC_NOT_EASY_TO_DIVIDE:
                success = __alloc_level3_memory(addr, AMM::_memory->extension_area->_NotEasyToDivide_HEAD, AMM::_memory->extension_area->_NotEasyToDivide_TAIL);
                break;
# endif /* AMM 2023 */
            }

            if (success) 
            {
                return addr;
            } 
            else /* level3 memory, state 1. */
                continue;
        } else addr += c;
    }

    // level2-memory
    switch (__ma) 
    {
    /* Code */
    case _M__ALLOC_FUNCTION:
        ret = __alloc_level2_memory(AMM::_memory->code_area->_FUNCTION_HEAD, AMM::_memory->code_area->_FUNCTION_TAIL);
        break;
    case _M__ALLOC_APPOBJ:
        ret = __alloc_level2_memory(AMM::_memory->code_area->_APPOBJ_HEAD, AMM::_memory->code_area->_APPOBJ_TAIL);
        break;

    case _M__ALLOC_PROCESS_CONTROL:
# ifdef AMM_2023
        ret = __alloc_level2_memory(AMM::_memory->code_area->_ProcessControl_HEAD, AMM::_memory->code_area->_ProcessControl_TAIL);
# elif defined(AMM_2022)
        ret = __alloc_level2_memory(AMM::_memory->code_area->_FUNCTION_HEAD, AMM::_memory->code_area->_FUNCTION_TAIL);
# endif /* AMM */
        break;

    /* Static */
    case _M__ALLOC_GLOBAL_VARIABLES:
        ret = __alloc_level2_memory(AMM::_memory->static_area->_GLOBALVARS_HEAD, AMM::_memory->static_area->_GLOBALVARS_TAIL);
        break;
    case _M__ALLOC_STATIC_VARIABLES:
        ret = __alloc_level2_memory(AMM::_memory->static_area->_STATICVARS_HEAD, AMM::_memory->static_area->_STATICVARS_TAIL);
        break;
    case _M__ALLOC_CONSTANTS:
        ret = __alloc_level2_memory(AMM::_memory->static_area->_CONSTANT_HEAD, AMM::_memory->static_area->_CONSTANT_TAIL);
        break;

    /* Text */
# ifdef AMM_2023
    case _M__ALLOC_STRING_CONSTANTS:
    case _M__TEXT:
        ret = __alloc_level2_memory(AMM::_memory->text_area->_StringConstants_HEAD, AMM::_memory->text_area->_StringConstants_TAIL);
# elif defined(AMM_2022)
    case _M__ALLOC_STRING_CONSTANTS:
    case _M__TEXT:
        ret = __alloc_level2_memory(AMM::_memory->text_area->_MEM_AREA_TEXT_HEAD, AMM::_memory->text_area->_MEM_AREA_TEXT_TAIL);
# endif /* AMM */
        break;

    /* Heap */
# ifdef AMM_2023
    case _M__ALLOC_DISCRETIONARY:
    case _M__HEAP:
        ret = __alloc_level2_memory(AMM::_memory->heap_area->_Discretionary_HEAD, AMM::_memory->heap_area->_Discretionary_TAIL);
# elif defined(AMM_2022)
    case _M__ALLOC_DISCRETIONARY:
    case _M__HEAP:
        ret = __alloc_level2_memory(AMM::_memory->heap_area->_MEM_AREA_HEAP_HEAD, AMM::_memory->heap_area->_MEM_AREA_HEAP_TAIL);
# endif /* AMM */
        break;
    
    /* Stack */
    case _M__ALLOC_FUNCTION_ACTUAL:
        ret = __alloc_level2_memory(AMM::_memory->stack_area->_FunctionActual_HEAD, AMM::_memory->stack_area->_FunctionActual_TAIL);
        break;
    case _M__ALLOC_LOCAL_VARIABLES:
        ret = __alloc_level2_memory(AMM::_memory->stack_area->_LOCALVARS_HEAD, AMM::_memory->stack_area->_LOCALVARS_TAIL);
        break;
    case _M__ALLOC_FUNCTION_RETURN_VALUE:
        ret = __alloc_level2_memory(AMM::_memory->stack_area->_FRV_HEAD, AMM::_memory->stack_area->_FRV_TAIL);
        break;
    
    /* Data */
# ifdef AMM_2023
    case _M__ALLOC_COMPILE_TIME:
        ret = __alloc_level2_memory(AMM::_memory->data_area->_CompileTime_HEAD, AMM::_memory->data_area->_CompileTime_TAIL);
        break;
    case _M__ALLOC_RUNTIME:
        ret = __alloc_level2_memory(AMM::_memory->data_area->_Runtime_HEAD, AMM::_memory->data_area->_Runtime_TAIL);
        break;
    case _M__ALLOC_LEVEL_1_DATA:
        ret = __alloc_level2_memory(AMM::_memory->data_area->_Level1data_HEAD, AMM::_memory->data_area->_Level1data_TAIL);
        break;
    case _M__ALLOC_LEVEL_2_DATA:
        ret = __alloc_level2_memory(AMM::_memory->data_area->_Level2data_HEAD, AMM::_memory->data_area->_Level2data_TAIL);
        break;

    /* Extension */
    case _M__ALLOC_CACHE_A:
        ret = __alloc_level2_memory(AMM::_memory->extension_area->_CacheA_HEAD, AMM::_memory->extension_area->_CacheA_TAIL);
        break;
    case _M__ALLOC_CACHE_B:
        ret = __alloc_level2_memory(AMM::_memory->extension_area->_CacheB_HEAD, AMM::_memory->extension_area->_CacheB_TAIL);
        break;
    case _M__ALLOC_TEMPORARY:
        ret = __alloc_level2_memory(AMM::_memory->extension_area->_Temporary_HEAD, AMM::_memory->extension_area->_Temporary_TAIL);
        break;
    case _M__ALLOC_NOT_EASY_TO_DIVIDE:
        ret = __alloc_level2_memory(AMM::_memory->extension_area->_NotEasyToDivide_HEAD, AMM::_memory->extension_area->_NotEasyToDivide_TAIL);
        break;
# endif /* AMM 2023 */
    }

    // Put the allocated memory address into the memory allocation table.
    if (__verify(ret) && ret.size() != 0) {
        MEMORY_ALLOC_LIST.push_back(ret);
    }

    return ret;
}

inline bool ACMC::__verify (const string &__addr) 
{
    if (__addr.size() == 10 && __addr[1] == 'x') 
    {
        for (auto const &c : __addr) 
        {
            if (0 <= c <= 9 || c == 'x' || 'F' >= c >= 'A') 
                return true;
        }
    } 
    return false;
}

inline bool ACMC::__memory_release (const string &__addr) 
{
FILE_WRITE_SET(_FILE_WRITE_AP);
    __acm_volatile_load_store(_0xffffffff, ("[" + __addr + "]"), MEMORY_RELEASE);
FILE_WRITE_SET(_FILE_WRITE_DFT);
    __acm_volatile_load_store(_0xffffffff, "0", (MEMORY_RELEASE_BEFORE + __addr + MEMORY_RELEASE_AFTER));
    return true;
}


# endif // __ACM_H