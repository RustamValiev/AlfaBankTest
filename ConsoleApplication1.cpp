// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <iostream>
#include "Controller.h"

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "rus");
	try
	{
		StrVector 	Path;
		CSimpleController Controller;
		
		if (argc < 3)
			throw exception("FAIL: conditions is incomplete");
		Controller.Produce(argv[1], argv[2], 0, 0, Path);

		//Controller.Produce("c:\\!\\a.txt", "c:\\!\\b.txt", 0, 0, Path);
		if (!Path.size())
			cout << "Path absence." << endl;
		else
			for(int i = 0; i < Path.size(); ++i)
				cout << Path[i] << endl;
	}
	catch (exception e)			{ cout << e.what() << endl; }
	catch (...)					{ cout << "unexpected exception" << endl; }
}