/*
p146.12
*/

#include <string>
#include <iostream>

using namespace std;

int count_pair(const char *str, const char pair[2])
{
	cout << "(char*)";
	if (str==0 || *str=='\0')
		return 0;
	int count(0);
	while (*(str+1) != '\0') {
		if (*str==pair[0] && *(str+1)==pair[1])
			++count;
		++str;
	}
	return count;
}

int count_pair(const string& str, const char pair[2])
{
	cout << "(string)";
	if (str.empty())
		return 0;
	int count(0);
	for (string::const_iterator i=str.begin(); (i+1)!=str.end(); ++i) {
		if (*i == pair[0] && *(i+1) == pair[1])
			++count;
	}
	return count;
}

int main()
{
	string s;
	const char pair[2] = { 'A','B' };

	cin >> s;
	cout << "char* version: " << count_pair(s.c_str(), pair) << '\n';
	cout << "string version: " << count_pair(s, pair) << '\n';
}
