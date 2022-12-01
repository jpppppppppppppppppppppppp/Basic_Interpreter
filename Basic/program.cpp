/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
	lines.clear();
	all_statements.clear();
    // Replace this stub with your own code
    //todo
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
	if(lines.empty())all_statements.insert(std::make_pair(lineNumber,line)),lines.insert(lines.begin(),lineNumber);
	else{
		auto iter = std::lower_bound(lines.begin(), lines.end(), lineNumber);
		if(all_statements.find(lineNumber)!=all_statements.end())all_statements[lineNumber] = line;
		else all_statements.insert(std::make_pair(lineNumber, line)), lines.insert(iter, lineNumber);
	}
	// Replace this stub with your own code
    //todo
}

void Program::removeSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
}

std::string Program::getSourceLine(int lineNumber) {
    // Replace this stub with your own code

    //todo
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
    //todo
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
   // Replace this stub with your own code
   //todo
}

int Program::getFirstLineNumber() {
    // Replace this stub with your own code
    //todo
}

int Program::getNextLineNumber(int lineNumber) {
    // Replace this stub with your own code
    //todo
}

//more func to add
//todo


