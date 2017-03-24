#pragma once
#include <iostream>
#include <stack>
#include <string>
#include <sstream>

using namespace std;

#include<iostream>
using namespace std;

char operationStack[20];
int endOfStack; // end of stack is an index of last variable + 1;

char pop();
char look();
void push(char value);
int calculate(char* str);
int getPriority(char op);
int polCalculate(char* expression);
void check(char* infix, int res);


void inline check(char* infix, int exp)
{
	int result = calculate(infix);
	for (int i = 0; i < strlen(infix); i++)
		cout << infix[i] << " ";
	cout << "  result = " << result;
	exp == result ? cout << " OK" : cout << "FAIL";
	cout << endl;
}
int inline  calculate(char* str)
{
	endOfStack = 0;
	char polish[30];
	int ind = 0;

	for (int i = 0; i < strlen(str); i++)
	{
		if (isdigit(str[i]))
		{
			polish[ind] = str[i];
			ind++;
			continue;
		}
		if (str[i] == '(')
		{
			push(str[i]);
			continue;
		}
		if (str[i] == ')')
		{
			char c = pop();
			while (c != '(')
			{
				polish[ind] = c;
				ind++;
				c = pop();
			}
			continue;
		}

		if (endOfStack != 0)
		{
			char c = pop();
			while (c != '(' && getPriority(c) >= getPriority(str[i]) && c != 0)
			{
				polish[ind] = c;
				ind++;
				c = pop();
			}
			push(c);
			push(str[i]);
		}
		else
			push(str[i]);
	}

	while (endOfStack > 0)
	{
		polish[ind] = pop();
		ind++;
	}
	polish[ind] = '\0';
	int result = polCalculate(polish);
	return result;
}
int inline polCalculate(char* expression)
{
	endOfStack = 0;
	int len = strlen(expression);
	for (int i = 0; i < len; i++)
	{
		if (isdigit(expression[i]))
		{
			push(expression[i]);
			continue;
		}

		int operandTwo = pop() - '0';
		int operandOne = pop() - '0';
		switch (expression[i])
		{
		case '+': { push(operandOne + operandTwo + '0'); break; }
		case '-': { push(operandOne - operandTwo + '0'); break; }
		case '*': { push(operandOne * operandTwo + '0'); break; }
		case '/': { push(operandOne / operandTwo + '0'); break; }
		}
	}
	return pop() - '0';
}
void push(char value)
{
	operationStack[endOfStack] = value;
	endOfStack++;
}
char pop()
{
	if (endOfStack > 0)
		endOfStack--;
	else
		return 0;
	return operationStack[endOfStack];
}
int getPriority(char op)
{
	if (op == '+' || op == '-')
		return 0;
	return 1;
}
char look()
{
	return operationStack[endOfStack - 1];
}