#ifndef _eval_h
#define _eval_h

#include <iostream>
#include <stack>
#include <unordered_map>
#include <sstream>
using namespace std;

stack<char> operators;
stack<int> nums;
unordered_map<char,int>cmp;

class ErrorException : public std::exception {
public:
	explicit ErrorException(string message) {
		this->message = message;
	}

	string getMessage() const{
		return message;
	}

private:
	std::string message;
};


inline int cal(char op,int n,int m){
	if(op=='+')return n+m;
	if(op=='-')return n-m;
	if(op=='*')return n*m;
	if(op=='/'){
		if(m==0)throw ErrorException("DIVIDE BY ZERO");
		return n / m;
	}
	if(op=='^')return n^m;
}
int eval(string exp){
	cmp.insert(make_pair('(',0));
	cmp.insert(make_pair(')',-1));
	cmp.insert(make_pair('*',1));
	cmp.insert(make_pair('/',1));
	cmp.insert(make_pair('+',2));
	cmp.insert(make_pair('-',2));
	cmp.insert(make_pair('^',3));

	exp +='\0';
	int len=0;
	char ch;
	int temp=0;
	while(len < exp.length()){
		ch = exp[len];
		len++;
		if('0' <= ch and ch <= '9'){
			temp = temp * 10 + (ch - '0');
		}
		else{
			if(ch!='\0'){
				if(operators.empty()){
					operators.push(ch);
					if(ch!='(')nums.push(temp);
					temp = 0;
				}
				else{
					if((ch!=')' and cmp[operators.top()]>cmp[ch]) or (operators.top() == '(' and ch!=')')){
						operators.push(ch);
						if(ch!='(')nums.push(temp);
						temp = 0;
					}
					else{
						while(!operators.empty() and (cmp[operators.top()]<=cmp[ch] or ch==')')){
							if(operators.top() == '(' and ch == ')'){
								operators.pop();
								break;
							}
							else{
								if(operators.top()=='(')break;
								temp = cal(operators.top(), nums.top(), temp);
								nums.pop();
								operators.pop();

							}
						}
						if(ch!=')')operators.push(ch),nums.push(temp),temp=0;
					}
				}
			}
			else{
				while(!operators.empty()){
					temp = cal(operators.top(),nums.top(),temp);
					operators.pop();
					nums.pop();
				}
				nums.push(temp);
			}
		}
	}

	return nums.top();
}


#endif