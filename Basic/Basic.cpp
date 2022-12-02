#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Utils/tokenScanner.hpp"
#include "eval.hpp"
using namespace std;




class Program {
public:
	std::vector<int>lines;
	std::unordered_map<int, std::string> all_statements;
	Program() = default;
	~Program() = default;
	void addSourceLine(int lineNumber, const std::string &line) {
		if(lines.empty())all_statements.insert(std::make_pair(lineNumber,line)),lines.insert(lines.begin(),lineNumber);
		else{
			auto iter = std::lower_bound(lines.begin(), lines.end(), lineNumber);
			if(all_statements.find(lineNumber)!=all_statements.end())all_statements[lineNumber] = line;
			else all_statements.insert(std::make_pair(lineNumber, line)), lines.insert(iter, lineNumber);
		}
	}
	void clear(){
		all_statements.clear();
		lines.clear();
	}
};


void processLine(std::string line, Program &program,unordered_map<string,int>&vars);


int main() {
    Program program;
	unordered_map<string,int>vars;
    //cout << "Stub implementation of BASIC" << endl;

    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
	            continue;
            processLine(input, program, vars);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

int getExpVal(TokenScanner &scanner,unordered_map<string,int>&vars){
	string token,exp;
	while(scanner.hasMoreTokens()){
		token = scanner.nextToken();
		token = scanner.getStringValue(token);
		if(scanner.getTokenType(token)==NUMBER){
			for(auto i:token) if(i=='.')throw ErrorException("SYNTAX ERROR");
			while(token.length()>1 and token[0]=='0')token.erase(0,1);
			exp += token;
		}
		if(scanner.getTokenType(token)==OPERATOR){
			exp += token;
		}
		if(scanner.getTokenType(token)==WORD){
			if(vars.count(token)){
				int temp = vars[token];
				string a;
				if(temp==0)exp +="0";
				else if(temp>0){
					while(temp>0){
						a = static_cast<char>(temp%10+'0') + a;
						temp/=10;
					}
					exp += a;
				}
				else{
					temp = -temp;
					while(temp>0){
						a = static_cast<char>(temp%10+'0') + a;
						temp/=10;
					}
					if(!exp.empty() and exp[exp.length()-1]=='+')exp.erase(exp.length()-1),exp += "-"+a;
					if(!exp.empty() and exp[exp.length()-1]=='-')exp[exp.length()-1]='+',exp+=a;
					else exp+="-"+a;
				}
			}
			else throw ErrorException("VARIABLE NOT DEFINED");
		}
	}
	return eval(exp);
}
inline bool c(int lhs,int rhs,const string &o){
	if(o=="=")return lhs==rhs;
	if(o=="<")return lhs<rhs;
	if(o==">")return lhs>rhs;
}
void run(vector<int>::iterator &le, Program &program,unordered_map<string,int>&vars,char mod,bool &f){
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(program.all_statements[*le]);
	string token;
	int len=0;
	while(scanner.hasMoreTokens()){
		token = scanner.nextToken();
		len++;
		if(len==1){
			if(token=="GOTO"){
				if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				else{
					token = scanner.nextToken();
					if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					else{
						if(mod=='r'){
							int l = 0;
							for(auto j: token)l = l * 10 + j - '0';
							if(program.all_statements.count(l))
								le= std::find(program.lines.begin(), program.lines.end(),l),run(le, program,  vars, 'r', f);
							else throw ErrorException("LINE NUMBER ERROR");
						}
					}
				}
			}
			else if(token == "END"){
				if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				if(mod == 'r')f = false;
			}
			else if(token == "IF"){
				if(mod=='r'){
					string lhs, rhs, o;
					while (scanner.hasMoreTokens()){
						token = scanner.nextToken();
						if(token != "=" and token != "<" and token != ">")lhs += token;
						else{
							o = token;
							break;
						}
					}
					while (scanner.hasMoreTokens()){
						token = scanner.nextToken();
						if(token != "THEN")rhs += token;
						else break;
					}
					TokenScanner scanner_1, scanner_2;
					scanner_1.ignoreWhitespace(), scanner_1.scanNumbers(), scanner_1.setInput(lhs);
					scanner_2.ignoreWhitespace(), scanner_2.scanNumbers(), scanner_2.setInput(rhs);
					if(c(getExpVal(scanner_1, vars), getExpVal(scanner_2, vars), o)){
						token = scanner.nextToken();
						int l = 0;
						for(auto j: token)l = l * 10 + j - '0';
						if(program.all_statements.count(l))
							le = std::find(program.lines.begin(), program.lines.end(), l), run(le, program, vars,
							                                                                   'r', f);
						else throw ErrorException("LINE NUMBER ERROR");
					}
					else{
						le = std::upper_bound(program.lines.begin(), program.lines.end(), *le);
					}
				}
			}
			else{
				if(token == "REM"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				}
				else if(token == "PRINT"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					if(mod == 'r'){

						int ans = getExpVal(scanner, vars);
						cout << ans << endl;

					}
				}
				else if(token == "LET"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					std::string var = scanner.nextToken();
					if(var == "LET" || var == "REM" || var == "PRINT" || var == "INPUT" || var == "END" ||
					   var == "GOTO" || var == "IF" || var == "THEN" || var == "RUN" || var == "LIST" ||
					   var == "CLEAR" || var == "QUIT" || var == "HELP")
						throw ErrorException("SYNTAX ERROR");

					if(scanner.getTokenType(var) != WORD)throw ErrorException("SYNTAX ERROR");

					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					std::string op = scanner.nextToken();
					if(op != "=")throw ErrorException("SYNTAX ERROR");
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					if(mod == 'r'){
						int val = getExpVal(scanner, vars);
						if(vars.count(var))vars[var] = val;
						else vars.insert(make_pair(var, val));
					}
				}
				else if(token == "INPUT"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					int varnum = 0;
					vector<string> varss;
					while (scanner.hasMoreTokens()){
						varnum++;
						token = scanner.nextToken();
						varss.push_back(token);
						if(scanner.getTokenType(token) != WORD)throw ErrorException("SYNTAX ERROR");
					}
					if(mod == 'r'){
						for(int i = 0; i < varnum; i++){
							cout << " ? ";
							string tline;
							getline(std::cin, tline);
							int val;
							if(tline.empty()){
								i--;
								continue;
							}
							else{
								bool flag = false;
								for(int j = 0; j < tline.length(); j++){
									if(!((tline[0] >= '0' and tline[0] <= '9') or tline[0] == '-'))flag = true;
									if(j > 0)
										if(tline[j] < '0' or tline[j] > '9'){
											flag = true;
										}
								}
								if(flag){
									i--;
									cout << "INVALID NUMBER" << endl;
									continue;
								}
								TokenScanner scanner_t;
								scanner_t.ignoreWhitespace();
								scanner_t.scanNumbers();
								scanner_t.setInput(tline);
								try {
									val = getExpVal(scanner_t, vars);
								} catch (...) {
									i--;
									cout << "INVALID NUMBER" << endl;
									continue;
								}

							}
							if(vars.count(varss[i]))vars[varss[i]] = val;
							else vars.insert(make_pair(varss[i], val));
						}
					}
				}
				le = std::upper_bound(program.lines.begin(), program.lines.end(),*le);
			}
		}
	}
}
void run(const string &command, Program &program, unordered_map<string,int>&vars,char mod,bool &f){
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(command);
	string token;
	int len=0;
	while(scanner.hasMoreTokens()){
		token = scanner.nextToken();
		len++;
		if(len==1){
			if(token=="GOTO"){
				if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				else{
					token = scanner.nextToken();
					if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					if(scanner.getTokenType(token)!=NUMBER)throw ErrorException("SYNTAX ERROR");
				}
			}
			else if(token == "END"){
				if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
			}
			else{
				if(token == "REM"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				}
				else if(token == "PRINT"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				}
				else if(token == "LET"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					std::string var = scanner.nextToken();
					if(var == "LET" || var == "REM" || var == "PRINT" || var == "INPUT" || var == "END" ||
					   var == "GOTO" || var == "IF" || var == "THEN" || var == "RUN" || var == "LIST" ||
					   var == "CLEAR" || var == "QUIT" || var == "HELP")
						throw ErrorException("SYNTAX ERROR");

					if(scanner.getTokenType(var) != WORD)throw ErrorException("SYNTAX ERROR");

					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					std::string op = scanner.nextToken();
					if(op != "=")throw ErrorException("SYNTAX ERROR");
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
				}
				else if(token == "INPUT"){
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					int varnum = 0;
					vector<string> varss;
					while (scanner.hasMoreTokens()){
						varnum++;
						token = scanner.nextToken();
						varss.push_back(token);
						if(scanner.getTokenType(token) != WORD)throw ErrorException("SYNTAX ERROR");
					}
				}
				else if(token == "IF"){
					string lhs, rhs, o;
					while (scanner.hasMoreTokens()){
						token = scanner.nextToken();
						if(token != "=" and token != "<" and token != ">")lhs += token;
						else{
							o = token;
							break;
						}
					}
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					while (scanner.hasMoreTokens()){
						token = scanner.nextToken();
						if(token != "THEN")rhs += token;
						else break;
					}
					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");

						token = scanner.nextToken();
						if(scanner.getTokenType(token)!=NUMBER)throw ErrorException("SYNTAX ERROR");


				}
				else throw ErrorException("SYNTAX ERROR");
			}
		}
	}
}

void processLine(std::string line, Program &program, unordered_map<string,int>&vars) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

		int len = 0;
		while (scanner.hasMoreTokens()){
			len++;
			std::string token = scanner.nextToken();

			if(len == 1){
				if(token == "QUIT"){
					if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					else exit(0);
				}
				if(token == "INPUT"){

					if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					int varnum = 0;
					vector<string> varss;
					while (scanner.hasMoreTokens()){
						varnum++;
						token = scanner.nextToken();
						varss.push_back(token);
						if(scanner.getTokenType(token) != WORD)throw ErrorException("SYNTAX ERROR");
					}
					for(int i = 0; i < varnum; i++){
						cout << " ? ";
						string tline;
						getline(std::cin, tline);
						int val;
						if(tline.empty()){
							i--;
							continue;
						}
						else{
							bool flag = false;
							for(int j = 0; j < tline.length(); j++){
								if(!((tline[0] >= '0' and tline[0] <= '9') or tline[0] == '-'))flag = true;
								if(j > 0)
									if(tline[j] < '0' or tline[j] > '9'){
										flag = true;
									}
							}
							if(flag){
								i--;
								cout << "INVALID NUMBER" << endl;
								continue;
							}
							TokenScanner scanner_t;
							scanner_t.ignoreWhitespace();
							scanner_t.scanNumbers();
							scanner_t.setInput(tline);
							try {
								val = getExpVal(scanner_t, vars);
							} catch (...) {
								i--;
								cout << "INVALID NUMBER" << endl;
								continue;
							}

						}
						if(vars.count(varss[i]))vars[varss[i]] = val;
						else vars.insert(make_pair(varss[i], val));
					}

				}
				if(scanner.getTokenType(token) == NUMBER and scanner.hasMoreTokens()){
					int number = 0;
					for(auto i: token)number = number * 10 + i - '0';
					string command = line.erase(0,token.length()+1);
					bool temp = true;
					run(command,program,vars,'c', temp);
					program.addSourceLine(number, command);
					return;
				}
				if(token == "LIST"){
					if(scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
					else{
						if(!program.lines.empty())
							for(auto i: program.lines)
								std::cout << i<<" "<<program.all_statements[i] << std::endl;
					}
				}
				if(token == "LET"){
					try {
						if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
						std::string var = scanner.nextToken();
						if(var == "LET" || var == "REM" || var == "PRINT" || var == "INPUT" || var == "END" ||
						   var == "GOTO" || var == "IF" || var == "THEN" || var == "RUN" || var == "LIST" ||
						   var == "CLEAR" || var == "QUIT" || var == "HELP")
							throw ErrorException("SYNTAX ERROR");

						if(scanner.getTokenType(var)!=WORD)throw ErrorException("SYNTAX ERROR");

						if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
						std::string op = scanner.nextToken();
						if(op != "=")throw ErrorException("SYNTAX ERROR");
						if(!scanner.hasMoreTokens())throw ErrorException("SYNTAX ERROR");
						int val = getExpVal(scanner,vars);
						if(vars.count(var))vars[var]=val;
						else vars.insert(make_pair(var,val));
					} catch (ErrorException& ex) {
						throw ex;
					}
				}
				if(scanner.getTokenType(token) == NUMBER and !scanner.hasMoreTokens()){
					int number = 0;
					for(auto i: token)number = number * 10 + i - '0';
					if(program.all_statements.count(number)){
						program.all_statements.erase(number);
						program.lines.erase(std::find(program.lines.begin(), program.lines.end(), number));
					}
				}
				if(token == "PRINT"){
					try {
						int ans = getExpVal(scanner,vars);
						cout << ans << endl;
					}catch (ErrorException& ex) {
						throw ex;
					}
				}
				if(token == "CLEAR"){
					program.clear();
					vars.clear();
				}
				if(token == "HELP"){
					cout << "brain fucking" << endl;
				}
				if(token == "RUN"){
					if(program.lines.empty())continue;
					else{
						bool flag = true;
						auto l = program.lines.begin();
						while(l!=program.lines.end() and flag)run(l,program,vars,'r',flag);
					}
				}
			}
		}

    //todo
}

