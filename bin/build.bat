@REM /*
@REM  * This file is part of the [Apollo] project.
@REM  * Copyright [2023] [Rui Zhang] and other contributors.
@REM  * See the LICENSE file in the project root directory for the full license information.
@REM  * 
@REM  * The sharing rights for this project belong to all those who have contributed to the project.
@REM  * 
@REM  * Thanks to the following contributors for their dedication and support of this project:
@REM  *   - Rui Zhang 
@REM */

@echo off

Rem Codes
win_flex -o CodesAfter.cpp lexical\Codes.l
clang++ -w -I %APOLLO_INCLUDE% -Ofast -c CodesAfter.cpp && clang++ -w -I %APOLLO_INCLUDE% -Ofast -o codes.exe Codes.cpp CodesAfter.o


Rem Control Process
win_flex -o PCAfter.cpp lexical\PC.l
clang++ -w -I %APOLLO_INCLUDE% -Ofast -c PCAfter.cpp && clang++ -w -I %APOLLO_INCLUDE% -Ofast -o pc.exe PC.cpp PCAfter.o


Rem Func
win_flex -o FuncAfter.cpp lexical\Func.l
clang++ -w -I %APOLLO_INCLUDE% -Ofast -c FuncAfter.cpp && clang++ -w -I %APOLLO_INCLUDE% -Ofast -o code-func.exe Func.cpp FuncAfter.o


Rem Apollo
clang++ -w -I %APOLLO_INCLUDE% -Ofast -c Apollo.cpp
echo.
echo.
win_flex -o lexical.cpp lexical\Apollo.l
echo.
@REM echo.
win_bison --warnings=none -d -o syntax.cpp Apollo.y
@REM echo.
@REM echo.
clang++ -w -I %APOLLO_INCLUDE% -Ofast -c lexical.cpp
@REM echo.
@REM echo.
clang++ -w -I %APOLLO_INCLUDE% -Ofast -c syntax.cpp
@REM echo.
@REM echo.
clang++ -w -I %APOLLO_INCLUDE% -Ofast -o apollo.exe Apollo.o lexical.o syntax.o
echo.
@REM echo.
@REM echo. 
echo [Compilation progress]:    Compilation completed
echo.

powershell .\license.ps1

del *After.*
del lexical.*
del syntax.*
del Apollo.o
