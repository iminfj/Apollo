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


# ifndef AMM_H
# define AMM_H 1
# define AMM_2023

# include <iostream>
using namespace std;
# include <io.h>
# include <windows.h>
# include <stdlib.h>
# include <fstream>
# include <string>
# include <vector>

/* ************************************************** */
// Macros

/**
 * @a Apollo Memory Model -- Standardization
*/
# define AMM                amm

# define FILE_WRITE_MODE    file_write_mode
# define _FILE_WRITE_W      0
# define _FILE_WRITE_WB     1
# define _FILE_WRITE_AP     2
# define _FILE_WRITE_DFT    _FILE_WRITE_W

# define _0xffffffff        "0xFFFFFFFF"
# define SPECIAL_MA         _0xffffffff
# define _0x0007a120        "0x0007A120"
# define _0x7a120           _0x0007a120
# define ACM_PROC           ".pc"
# define ACM_FREAD_BUFFER   2048

/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
# define FILE_WRITE_SET(mode) ( AMM::FILE_WRITE_MODE = (mode) )
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */

/* ************************************************** */

string acm_root = string((const char *)getenv("APOLLO_MEMORY"));

namespace AMM 
{
    int FILE_WRITE_MODE = _FILE_WRITE_DFT;

    inline string _amm__memory_read (const string &);
    inline string _amm__memory_write (const string &, const string &);
    inline string _amm__f_read (const string &);
    inline void _amm__f_write (const string &, const string &);
    inline string _amm__convert (const string &, bool);

/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __code {
        string  _MEM_AREA_CODE_HEAD     = string("0x00000000");
        string  _MEM_AREA_CODE_TAIL     = string("0x002DC6C0");
        // function segment
        string  _FUNCTION_HEAD          = _amm__memory_read("0x00000000") == "0x00000000" ? _amm__memory_write("0x00000000", "0x00000001") : string("0x00000000");
        string  _FUNCTION_TAIL          = string("0x0007A120");
        // appobj segment
        string  _APPOBJ_HEAD            = _amm__memory_read("0x0007A121") == "0x0007A121" ? _amm__memory_write("0x0007A121", "0x0007A122") : string("0x0007A121");
        string  _APPOBJ_TAIL            = string("0x000F4240");
        // Process Control
        string  _ProcessControl_HEAD    = _amm__memory_read("0x000F4241") == "0x000F4241" ? _amm__memory_write("0x000F4241", "0x000F4242") : string("0x000F4241");
        string  _ProcessControl_TAIL    = string("0x0016E360");
        // Reserved portion && pc+0x7a120 >= 0x16e362
        string  _ReservedPortion_HEAD   = _amm__memory_read("0x0016E361") == "0x0016E361" ? _amm__memory_write("0x0016E361", "0x0016E362") : string("0x0016E361");
        string  _ReservedPortion_TAIL   = string("0x002DC6C0");
    };
    using _code = struct __code;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __static {
        string  _MEM_AREA_STATIC_HEAD   = string("0x002DC6C1");
        string  _MEM_AREA_STATIC_TAIL   = string("0x017D7840");
        // global variables segment
        string  _GLOBALVARS_HEAD        = _amm__memory_read("0x002DC6C1") == "0x002DC6C1" ? _amm__memory_write("0x002DC6C1", "0x002DC6C2") : string("0x002DC6C1");
        string  _GLOBALVARS_TAIL        = string("0x005B8D80");
        // static variables segment
        string  _STATICVARS_HEAD        = _amm__memory_read("0x005B8D81") == "0x005B8D81" ? _amm__memory_write("0x005B8D81", "0x005B8D82") : string("0x005B8D81");
        string  _STATICVARS_TAIL        = string("0x00895440");
        // constant segment
        string  _CONSTANT_HEAD          = _amm__memory_read("0x00895441") == "0x00895441" ? _amm__memory_write("0x00895441", "0x00895442") : string("0x00895441");
        string  _CONSTANT_TAIL          = string("0x00B71B00");
        // Reserved portion
        string  _ReservedPortion_HEAD   = _amm__memory_read("0x00B71B01") == "0x00B71B01" ? _amm__memory_write("0x00B71B01", "0x00B71B02") : string("0x00B71B01");
        string  _ReservedPortion_TAIL   = string("0x017D7840");
    };
    using _static = struct __static;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __text {
        string  _MEM_AREA_TEXT_HEAD     = string("0x017D7841");
        string  _MEM_AREA_TEXT_TAIL     = string("0x019BFCC0");
        // String constants
        string  _StringConstants_HEAD   = _amm__memory_read("0x017D7841") == "0x017D7841" ? _amm__memory_write("0x017D7841", "0x017D7842") : string("0x017D7841");
        string  _StringConstants_TAIL   = string("0x018CBA80");
        // Reserved portion
        string  _ReservedPortion_HEAD   = _amm__memory_read("0x018CBA81") == "0x018CBA81" ? _amm__memory_write("0x018CBA81", "0x018CBA82") : string("0x018CBA81");
        string  _ReservedPortion_TAIL   = string("0x019BFCC0");        
    };
    using _text = struct __text;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __heap {
        string  _MEM_AREA_HEAP_HEAD     = string("0x019BFCC1");
        string  _MEM_AREA_HEAP_TAIL     = string("0x02FAF080");
        // Discretionary
        string  _Discretionary_HEAD     = _amm__memory_read("0x019BFCC1") == "0x019BFCC1" ? _amm__memory_write("0x019BFCC1", "0x019BFCC2") : string("0x019BFCC1");
        string  _Discretionary_TAIL     = string("0x0280DE80");
        // Reserved portion
        string  _ReservedPortion_HEAD   = _amm__memory_read("0x0280DE81") == "0x0280DE81" ? _amm__memory_write("0x0280DE81", "0x0280DE82") : string("0x0280DE81");
        string  _ReservedPortion_TAIL   = string("0x02FAF080");
    };
    using _heap = struct __heap;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __stack {
        string  _MEM_AREA_STACK_HEAD    = string("0x02FAF081");
        string  _MEM_AREA_STACK_TAIL    = string("0x03B20B80");
        // Function Actual Argument
        string  _FunctionActual_HEAD    = _amm__memory_read("0x02FAF081") == "0x02FAF081" ? _amm__memory_write("0x02FAF081", "0x02FAF082") : string("0x02FAF081");
        string  _FunctionActual_TAIL    = string("0x0328B740");

        // local variables segment
        string  _LOCALVARS_HEAD         = _amm__memory_read("0x0328B741") == "0x0328B741" ? _amm__memory_write("0x0328B741", "0x0328B742") : string("0x0328B741");
        string  _LOCALVARS_TAIL         = string("0x035E1F20");
        // Function Return Value && 2023-2-15 Codes Return
        string  _FRV_HEAD               = _amm__memory_read("0x035E1F21") == "0x035E1F21" ? _amm__memory_write("0x035E1F21", "0x035E1F22") : string("0x035E1F21");
        string  _FRV_TAIL               = string("0x0365C040");
        // Reserved portion
        string  _ReservedPortion_HEAD   = _amm__memory_read("0x0365C041") == "0x0365C041" ? _amm__memory_write("0x0365C041", "0x0365C042") : string("0x0365C041");
        string  _ReservedPortion_TAIL   = string("0x03B20B80");
    };
    using _stack = struct __stack;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __data 
    {
        string  _MEM_AREA_DATA_HEAD     = string("0x03B20B81");
        string  _MEM_AREA_DATA_TAIL     = string("0x043B5FC0");
        // Compile time
        string  _CompileTime_HEAD       = _amm__memory_read("0x03B20B81") == "0x03B20B81" ? _amm__memory_write("0x03B20B81", "0x03B20B82") : string("0x03B20B81");
        string  _CompileTime_TAIL       = string("0x03C14DC0");
        // Runtime
        string  _Runtime_HEAD           = _amm__memory_read("0x03C14DC1") == "0x03C14DC1" ? _amm__memory_write("0x03C14DC1", "0x03C14DC2") : string("0x03C14DC1");
        string  _Runtime_TAIL           = string("0x03D09000");
        // Level 1 data
        string  _Level1data_HEAD        = _amm__memory_read("0x03D09001") == "0x03D09001" ? _amm__memory_write("0x03D09001", "0x03D09002") : string("0x03D09001");
        string  _Level1data_TAIL        = string("0x03DFD240");
        // Level 2 data
        string  _Level2data_HEAD        = _amm__memory_read("0x03DFD241") == "0x03DFD241" ? _amm__memory_write("0x03DFD241", "0x03DFD242") : string("0x03DFD241");
        string  _Level2data_TAIL        = string("0x03EF1480");
        // Reserved portion
        string  _ReservedPortion_HEAD   = _amm__memory_read("0x03EF1481") == "0x03EF1481" ? _amm__memory_write("0x03EF1481", "0x03EF1482") : string("0x03EF1481");
        string  _ReservedPortion_TAIL   = string("0x043B5FC0");
    };
    using _data = struct __data;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __extension 
    {
        string  _MEM_AREA_EXTENS_HEAD   = string("0x043B5FC1");
        string  _MEM_AREA_EXTENS_TAIL   = string("0x04C4B400");
        // Cache A
        string  _CacheA_HEAD            = _amm__memory_read("0x043B5FC1") == "0x043B5FC1" ? _amm__memory_write("0x043B5FC1", "0x043B5FC2") : string("0x043B5FC1");
        string  _CacheA_TAIL            = string("0x044AA200");
        // Cache B
        string  _CacheB_HEAD            = _amm__memory_read("0x044AA201") == "0x044AA201" ? _amm__memory_write("0x044AA201", "0x044AA202") : string("0x044AA201");
        string  _CacheB_TAIL            = string("0x0459E440");
        // Temporary
        string  _Temporary_HEAD         = _amm__memory_read("0x0459E441") == "0x0459E441" ? _amm__memory_write("0x0459E441", "0x0459E442") : string("0x0459E441");
        string  _Temporary_TAIL         = string("0x04692680");
        // Not easy to divide
        string  _NotEasyToDivide_HEAD   = _amm__memory_read("0x04692681") == "0x04692681" ? _amm__memory_write("0x04692681", "0x04692682") : string("0x04692681");
        string  _NotEasyToDivide_TAIL   = string("0x047868C0");
        // Reserved portion
        string  _ReservedPortion_HEAD   = _amm__memory_read("0x047868C1") == "0x047868C1" ? _amm__memory_write("0x047868C1", "0x047868C2") : string("0x047868C1");
        string  _ReservedPortion_TAIL   = string("0x04C4B400");

        // ffffffff
        string _____f = _amm__memory_read(_0xffffffff) == _0xffffffff ? _amm__memory_write(_0xffffffff, "1") : _0xffffffff;
    };
    using _extension = struct __extension;
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */
    struct __level2_memory 
    {
        // Code Area
        _code       * code_area         = new _code;
        // Static Area
        _static     * static_area       = new _static;
        // Text Constant Area
        _text       * text_area         = new _text;
        // Heap Area
        _heap       * heap_area         = new _heap;
        // Stack Area
        _stack      * stack_area        = new _stack;
        // Data Area
        _data       * data_area         = new _data;
        // Extension Area
        _extension  * extension_area    = new _extension;
    };
    using _level2_memory = struct __level2_memory;
    _level2_memory * _memory = new _level2_memory;
}
/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */

inline string AMM::_amm__memory_read (const string &__addr) 
{
    string target = _amm__convert(__addr, false);
    if (_access(target.c_str(), 0) == -1) 
        return __addr;
    return (string)_amm__f_read(target);
}

inline string AMM::_amm__convert (const string &__addr, bool __create) 
{
    string t, t1 = __addr,  m2;
    int i = 0; for (auto const &c : t1) {
        if (i++ < 2 && t1[1] == 'x') 
            continue;
        m2 += c;
        m2 += '\\';
    }
    t = acm_root + m2;

    if (__create && _access(t.c_str(), 0) == -1) 
        system(string("mkdir " + t).c_str());

    t += ".data";
    return t;
}

// inline string AMM::_amm__f_read (const string &__path)
// {
//     char buffer[ACM_FREAD_BUFFER]; memset(buffer, 0, sizeof(buffer));

//     if (_access(__path.c_str(), 0) == 0) 
//     {
//         FILE * fp = fopen(__path.c_str(), "r");
//             fread(buffer, sizeof(char), ACM_FREAD_BUFFER, fp);
//         fclose(fp);
//     } else return "non-exist";
//     // cout << __path << " | read < " << buffer << endl;
//     return string((const char *)buffer);
// }

inline std::string AMM::_amm__f_read(const std::string &__path)
{
    std::ifstream file(__path);
    if (!file.is_open())
    {
        return "file-not-found";
    }

    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

    return std::string(buffer.begin(), buffer.end());
}


inline string AMM::_amm__memory_write (const string &__addr, const string &__data) 
{
    // string target = _amm__convert(__addr, true);
    _amm__f_write(_amm__convert(__addr, true), __data);
    return __addr;
}

inline void AMM::_amm__f_write (const string &__file, const string &__data)
{
    FILE * fp;
    switch (FILE_WRITE_MODE) 
    {
    case _FILE_WRITE_W:
        fp = fopen(__file.c_str(), "w");
        break;
    case _FILE_WRITE_AP:
        fp = fopen(__file.c_str(), "a+");
        break;
    default:
        fp = fopen(__file.c_str(), "w");
        break;
    }

    fprintf(fp, "%s", __data.c_str());
    fclose(fp);
}

# endif /* AMM_H */

/* ------------------------------------------------------------------------------------------------------------------------------------------------------ */