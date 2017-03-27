#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int main()
{
	std::ifstream in("C:\\Users\\andre\\OneDrive\\Документы\\visual studio 2015\\Projects\\SEM2LAB7\\Parser\\exports.txt");
	ofstream out("out.def");
	string data;
	while(!in.eof())
	{
		getline(in, data);
		istringstream iss(data);
		string a;
		do
		{
			iss >> a;
		} while (isdigit(a[0]));
		out << "_"  << a << endl;
	}
	out.close();
	in.close();
	return 0;
}