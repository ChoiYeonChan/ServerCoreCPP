#include "pch.h"

double func()
{
	double data = 4.1f;
	return data;
}

int main() 
{
	int a = 1;
	int b = 2;
	int c = 3;
	
	int d = 0;
	int e = 0;
	int f = 0;
	MemoryStream stream(30);
	StreamWriter writer(&stream);

	writer << a << b << c;

	StreamReader reader(&stream);
	
	reader >> e >> d >> f;
	
	std::cout << e << d << f << std::endl;

	stream.Display();

	return 0;
}
