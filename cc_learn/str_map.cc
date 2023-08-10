/*
*/

#include <map>
#include <iostream>
#include <string>

using namespace std;

const char left_id = '(';
const char right_id = ')';

string get_var(istream in)
{
	string var;
	char c(0);
	for(in>>c; c!=right_id && c!=0; in>>c) {
		var += ch;
	}
	return var;
}

int main()
{
	string var;
	map<string,double> table;

	cin>>ch;
	while (ch!=left_id) {
		cin>>ch;
	}
	double d;
	cin >> d;
	table[var]
}
