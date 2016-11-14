#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <exception>

#include <cstdio>

using namespace std;

typedef vector<string>			StrVector;

class good_exception : public std::exception
{
public:
	good_exception() :exception() {}
	good_exception(const char * const &rcpcMessage) : exception(rcpcMessage) {}
	good_exception(const char * const &rcpcMessage, int vCode) : exception(rcpcMessage, vCode) {}
	good_exception(const exception &crRight) : exception(crRight) {}
	//exception& operator=(const exception &right) {}

	good_exception(std::string const &crMessage) : exception(crMessage.c_str()) {}
};

template <typename TFileType = ifstream>
class CAutoOpenFile
{
public:
	TFileType & mrThisFile;

	CAutoOpenFile(
		TFileType &			rThisFile,
		string const &		crFileName,
		ios_base::openmode	vMode
	) : mrThisFile(rThisFile) {
		mrThisFile.open(crFileName, vMode);
	};

	~CAutoOpenFile() { mrThisFile.close(); };
};


