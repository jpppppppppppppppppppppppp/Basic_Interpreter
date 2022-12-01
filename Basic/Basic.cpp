/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
	            continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
	int len = 0;
	while(scanner.hasMoreTokens()){
		len++;
		std::string token = scanner.nextToken();

		if(len==1){
			if(token == "QUIT"){
				if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				else exit(0);
			}
			if(scanner.getTokenType(token)==NUMBER and scanner.hasMoreTokens()){
				int number=0;
				for(auto i:token)number = number*10+i-'0';
				program.addSourceLine(number,line);
				return;
			}
			if(token == "LIST"){
				if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				else{
					if(!program.lines.empty())
						for(auto i:program.lines)
							std::cout << program.all_statements[i] << std::endl;
				}
			}
			if(token == "LET"){
				try{
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					std::string var = scanner.nextToken();
					if(
						var == "LET"||var == "REM"||var == "PRINT"||var == "INPUT"||var == "END"||var == "GOTO"||
						var == "IF"||var == "THEN"||var == "RUN"||var== "LIST"||var == "CLEAR"||var == "QUIT"||var == "HELP"
						)throw ErrorException("SYNTAX ERROR");
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					std::string op = scanner.nextToken();
					if(op!="=")throw ErrorException("SYNTAX ERROR");
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					std::string val = scanner.nextToken();
				}catch (ErrorException &ex) {
					throw ex;
				}
			}
			if(scanner.getTokenType(token)==NUMBER and !scanner.hasMoreTokens()){
				int number=0;
				for(auto i:token)number = number*10+i-'0';
				if(program.all_statements.count(number)){
					program.all_statements.erase(number);
					program.lines.erase(std::find(program.lines.begin(), program.lines.end(),number));
				}
			}
			if(token == "CLEAR"){
				program.clear();
			}
		}
	}

    //todo
}

