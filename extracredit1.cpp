//extra credit Jenna Busch


/*
	calculator08buggy.cpp | From: Programming -- Principles and Practice Using C++, by Bjarne Stroustrup
	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/


/*
1. Allow underscores in the calculator’s variable names.
2. Provide an assignment operator, =, so that you can change the value of a variable after you introduce it using let.
Discuss why that can be useful and how it can be a source of problems.
3. Provide named constants that you really can’t change the value of. Hint: You have to add a member to Variable that
distinguishes between constants and variables and check for it in set_value(). If you want to let the user define constants
(rather than just having pi and e defined as constants), you’ll have to add a notation to let the user express that, for
example, const pi = 3.14;.
*/



#include "std_lib_facilities.h"

//creating a token 
struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string val) : kind(ch),name(val) { }
};

//tokens stream
class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

//these are constants that the user can input to generate an action
const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char change = '=';  		//ex2
const char constant = 'c';
const char squareRoot = 's';
const char pi = 'pi';
const char e = 'e';
const char k = 'k';		//ex3


//gets chars from user
Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();
	double val;
	cin >> val;
	return Token(number, val);
	}
	default:
	{			
		if (isalpha(ch) || ch == '_') { // this allows for underscores
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch)) || ch == '_') 	//ex1
			{
				s += ch;
			}
			cin.unget();
			if (s == "L") return Token(let);
			if (s == "quit") return Token(quit);
			if(s == "=") return Token(change);
			if(s == "const") return Token(constant);
			if (s == "sqrt") return Token(squareRoot);
            if (s == "pi") return Token(pi);
			if (s == "e") return Token(e);
			if (s == "k") return Token(k);			//ex3


			return Token(name, s);
			
		}
		error("Bad token");
		return Token(' ');
	}
	}
}

//this ignores certain chars
void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

//creates variables for name and value
struct Variable {
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};

//vector of variables
vector<Variable> names;

//getter for value in vector
double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);

    return 0.0;
}

//setter for value
void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

//checks if a variable has already been used
bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

//calls expression 
double expression();

//this handles the cases entered by the user
double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("'(' expected");
	return d;
	}
	case '-':
		return -primary();
	case number:
		return t.value;
	case name:
		return get_value(t.name);
	case '+':
		return primary();
	case squareRoot:
    {
        t = ts.get();
        double x = expression();
        if (x < 0) error("negative square root cannot be calculated");
        t = ts.get();
        return sqrt(x);
    }
	case pi:
	{
		return 3.14; 
	}
	case e:
	{
		return 2.71;
	}
	case k:
	{
		return 1000; 
	}
	default:
		error("primary expected");
	}

    return 0.0;
}

//performs some of the operations
double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

//more operations
double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

//this checks how they are declaring variables and also declares them if correct
double declaration()
{
	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d));
	return d;
}

//generates the statement line
double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	case change:
		return declaration(); 
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

//generates and produces calculations
void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

//runs the program
int main()

try {
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
