#include "pch.h"

double func()
{
	double data = 4.1f;
	return data;
}

int main() 
{
	MemoryStream stream(30);
	double a = 0.0f;
	double b = 0.0f;
	double c = 0.0f;
	int myarray[3] = { 1, 2, 3 };
	StreamWriter writer(&stream);

	double* ptr = writer.Reserve<double>(2);
	stream.Display();
	std::cout << "\n";

	writer << (double)4.5f;
	stream.Display();
	std::cout << "\n";

	*ptr = 4.1;
	stream.Display();
	*(ptr + 1) = 4.2;
	stream.Display();
	std::cout << "\n";

	StreamReader reader(&stream);
	reader >> a;
	reader >> b;
	reader >> c;

	stream.Display();
	std::cout << a << " " << b << " " << c << std::endl;

	stream.CleanUp();
	stream.Display();

	MemoryStream stream2((char*)(myarray), 12, 30, true);
	StreamReader reader2(&stream2);
	stream2.Display();
	int a2, b2, c2 = 0;
	reader2 >> a2;
	reader2 >> b2;
	reader2 >> c2;
	stream2.Display();
	std::cout << a2 << " " << b2 << " " << c2 << std::endl;

	stream.SetBuffer((char*)(myarray), 12, 30);
	stream.Display();
	int a3, b3, c3 = 0;
	reader >> a3;
	reader >> b3;
	reader >> c3;
	stream.Display();
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << a3 << " " << b3 << " " << c3 << std::endl;
	return 0;
}
