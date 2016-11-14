#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include "Controller.h"
#include "BasicFactories.h"
#include "TextFileInput.h"
#include "PlainPathBuilder.h"

#include "Test.h"

const size_t AMOUNT_CHARS = 'z' - 'a' + 1;
const size_t MAX_CHARS_IN_WORD = 256;
const size_t PUZZLE_ADD = 4;

const size_t CHECK_FILES_READING = 1;
const size_t CHECK_GRAPH_BUILDING = 2;
const size_t CHECK_PATH_FINDING_QUICKLY = 4;
const size_t CHECK_PATH_FINDING_BRUTALLY = 8;
const size_t CHECK_ALL_METHODS = 0xF;

int main(int argc, char *argv[])
{
	srand(static_cast<size_t>(time(NULL)));
	test_All("c://!//files//", CHECK_ALL_METHODS, 100);
}

void test_All(
	string const	crPath,
	size_t			vTestLevel,
	size_t			vTestsAmount
) {
	for (size_t i = 0; i < vTestsAmount; ++i)
	{
		cout << i << " - ";
		string GoalFileName, DictFileName, TestDictFile;
		StrVector Dict;
		try
		{
			// ����� ���������
			size_t CharsInWord = 3 + rand() % 8;	//!< ����� �������� � ������
			size_t WordsAmount = 3 + rand() % 1024;//!< ����� ������� �����
			size_t AddWordsAmount = 3 + rand() % 1024;//!< ����� ������������� ����� ���� �����
			size_t LaceWordsAmount = 8 + rand() % 32;	//!< ����� ������� ������� ������
			if (CharsInWord >= MAX_CHARS_IN_WORD)
				throw exception("Len of word is too much.");

			// ������������� �������� �����: (1) ���� ������ (��������+������� ������) � (2) �������
			generate_GoalAndDictFiles(crPath + "test_", CharsInWord, WordsAmount, AddWordsAmount,
				LaceWordsAmount, Dict, GoalFileName, DictFileName, TestDictFile);

			test_cases(GoalFileName, DictFileName, TestDictFile, Dict, vTestLevel);
			cout << endl;
		}
		catch (exception e)
		{
			cout << e.what() << " : " << GoalFileName << endl;
		}
	}
}

/**
@brief �������� ����� �������� ����� vTestLevel
@param[in] crGoalFileName - ���� � ������
@param[in] crDictFileName - ���� �� �������
@param[in] crTestDictFile - ���� � ���� - ������� ������� ��� ��������� � ����������(��.generate_GoalAndDictFiles)
@param[in] rDict - ����������� ������(��������), ��� �������� � ���, ������� ����� ��������
@param[in] vTestLevel - ������� ����� ������
*/
void test_cases(
	string const &	crGoalFileName,
	string const &	crDictFileName,
	string const &	crTestDictFile,
	StrVector  &	rDict,
	size_t			vTestLevel
) {
	cout << crGoalFileName << ": ";

	// ����������� ������ ������
	if (vTestLevel & CHECK_FILES_READING)
		test_CheckReadingGoalAndDictFiles(crGoalFileName, crTestDictFile, 0, rDict);
	cout << " r";

	// ����������� ���������� �����
	CPlainPathBuilder::SimpleGraphData GraphData;
	if (vTestLevel & CHECK_GRAPH_BUILDING)
		test_GraphBuilding(crGoalFileName, crDictFileName, GraphData);
	cout << " g";

	// ����������� ���������� ����������� ����
	CPlainPathBuilder PlainBuilder;
	CPlainPathBuilder::IdxVector PathAsIntEdges;
	PlainBuilder.BuildPath(GraphData, PathAsIntEdges);

	size_t LenOfPath = PathAsIntEdges.size();
	if (LenOfPath)
	{
		if (vTestLevel & CHECK_PATH_FINDING_QUICKLY)
			test_CheckPathQuickly(GraphData, LenOfPath - 1);
		cout << " q";
		if (vTestLevel & CHECK_PATH_FINDING_BRUTALLY)
			test_CheckPathBrutally(GraphData, LenOfPath - 1);
		cout << " b";
	}
}

/**
@brief ������������� ���� � ������ � ���� ������� ��� �����
@param [in] Path - ���� ��� �������� ������
@param [in] vNumSymbolsInWord - ����� �������� � ������
@param [in] vWordsAmount - ����� ���� � �������������� (�����������) ������
@param [in] vAnotherLensAmount - ����� ���� � ������, ������������ �� vNumSymbolsInWord
@param [in] vLaceAmount - ����� ������� ���� (�� ������������ ������) ����� ������� ������ ������ ����
@param [out] rDict - ����������� ����-������ �������
@param [out] rGoalFileName - ��� ����� � ������
@param [out] rDictFileName - ��� ����� �� ��������
@param [out] rTestDictFileName - ��� ����� � �������� ������� �������, ����� �������������� rDict
*/
void generate_GoalAndDictFiles(
	string		Path,
	size_t		vNumSymbolsInWord,
	size_t		vWordsAmount,
	size_t		vAnotherLensAmount,
	size_t		vLaceAmount,
	StrVector & rDict,
	string &	rGoalFileName,
	string &	rDictFileName,
	string &	rTestDictFileName
) {
	// ���� � ������� (� �������� � ������� ��������)
	ofstream GoalFile;
	string FileName = generate_string(7);
	rGoalFileName = Path + FileName + "-goal.txt";
	CAutoOpenFile<ofstream> AutoTF(GoalFile, rGoalFileName, ios::out);
	if (!GoalFile)
		throw exception("FAIL: can't create goal file.");

	// ���� �� ��������
	ofstream DictFile;
	rDictFileName = Path + FileName + "-dict.txt";
	CAutoOpenFile<ofstream> AutoDF(DictFile, rDictFileName, ios::out);
	if (!DictFile)
		throw exception("FAIL: can't create dictionary fail.");

	// ���� � "����������" ��������
	ofstream TestDictFile;
	rTestDictFileName = Path + FileName + "-dict-test.txt";
	CAutoOpenFile<ofstream> AutoTDF(TestDictFile, rTestDictFileName, ios::out);
	if (!TestDictFile)
		throw exception("FAIL: can't create test dictionary fail.");

	// ����������� ���� ����� GoalFile � "�������" ������� rDict
	// � "�������" ������ ������� ���� ������������ ����� ���������� ����, ��
	// ��� ����� ����� ������ ���������� ���� �� �����, ������� ����� ��������������,
	// � "�������" ������ ��� "�����" �� ��������� � �������� �����
	StrVector ChgDict;
	for (size_t i = 0; i < vWordsAmount + 2; ++i)
	{
		// ������������ ����� ������
		string s = "";
		for (size_t j = 0; j < vNumSymbolsInWord; ++j)
			s += generate_char();
		// ����� ������ ����������� ���� ����� ���� ����������� �� ���� ����� ��
		vector<string>::const_iterator it = find(rDict.begin(), rDict.end(), s);
		if (it == rDict.end())
			rDict.push_back(s);
		else
			--i;
	}
	GoalFile << rDict[0] << endl;
	GoalFile << rDict[1] << endl;
	ChgDict = rDict;

	// � "������� ������" ������� �������� �����, ������� �������� "����" ��
	// �������� ������ � �������, ����, ������� ������� ����� vLaceAmount ����
	//
	// ��� ����� � ������� (���������������) ������� "������� �����":
	// "��������", "�������", � vLaceAmount ���� ����� ����
	// 
	// �� ��� ����� ���������� ���� �� ����� ����� ��� �� 1 ������,
	//
	// �������, ����� ������� "����" - ��� ���� ��� ���� "(i) � (i+1)" 
	// � ������� ����������� ����� �����, ������� "������� ����-����" ����� (i) � (i+1)
	//
	// ��� ���� ��� ���� "(i) � (i+1)" ��������������� ����������� ����� �� ���������� ��������:
	// 1) ������� ����������� �����, ����� ��������� ������� �� ����� (i), �� � ������ �������
	// �������� ����� ������ ������ ����� (i+1),
	// 2) ����� ����������� �����, ����� "����� �������" �� ����������, �� � �������� ������
	// ������ ������� �� ������ ������ �� ����� (i+1) 
	// ...) � �.�. - ����� �� ���������� �������.
	//
	// ����, ��� �� ����� ��������, ������ (vNumSymbolsInWord-1), ����� ������������� ������
	// "����-����" �� (i) � (i+1)
	//
	// ����� � ������� ����� ��������� (vNumSymbolsInWord-1)*(vLaceAmount+1) ����, ���������
	// ���� �������� ����, �� ������� ��� (vNumSymbolsInWord-1)*(vLaceAmount+1) ����
	if (vLaceAmount < vNumSymbolsInWord)	//!< ����� ���� � ���� ����� ������� "� �������" 
		vLaceAmount = vNumSymbolsInWord;	//!< ������ ���� �� ����� ����� ���� � �����
	size_t FirstIdx = 0;
	string EndStr = ChgDict[1];
	for (size_t i = 1; i < vLaceAmount; ++i)
	{
		size_t Sz = ChgDict.size() - 2;
		size_t NextIdx = 2 + (rand() % Sz); // ����� ���������� �������� (����� ��������� � ��������)
		Lacing(ChgDict, ChgDict[FirstIdx], ChgDict[NextIdx]);
		FirstIdx = NextIdx;
	}
	Lacing(ChgDict, ChgDict[FirstIdx], EndStr);

	//� ������� ������� ������������� ������ ������ ����� ����� ������ vAnotherLensAmount
	for (size_t i = 0; i < vAnotherLensAmount; ++i)
	{
		size_t AddLen = 1 + (rand() % vNumSymbolsInWord);
		size_t DstPos = rand() % (vWordsAmount + i);
		vector<string>::iterator it = ChgDict.begin();
		ChgDict.insert(it + DstPos, generate_string(vNumSymbolsInWord + AddLen));
	}

	//������������� ���������� ������ (�� ������ �������� 0 � 1)
	size_t Sz = ChgDict.size();
	for (size_t i = 2; i < Sz; ++i)
	{
		size_t NewPos = 2 + rand() % (Sz - 2);
		if (NewPos != i)
			swap(ChgDict[i], ChgDict[NewPos]);
	}

	// ����������� ���� �������
	for (size_t i = 0; i < Sz; ++i)
		DictFile << ChgDict[i] << endl;

	// � ���� ������� ��� �����
	for (size_t i = 2; i < vWordsAmount + 2; ++i)
		TestDictFile << rDict[i] << endl;
}

/**
@brief �������� � ������ rList ������, ������� ������� "����-����" �� ������ rcBeg �� rcEnd
� ��� ����������������� ������ �������� PUZZLE_ADD �����, ������������ �� ��� �� 1 ������
��� ������� "���������, ��������" ����������� �� ����� ����
@param [out] rList - ������, � ������� ����������� ������, ��������� "����-����".
@param [in] �rcBeg - ��������� ������ ��� "����-����"
@param [in] �rcEnd - �������� ������ ��� "����-����"
*/
void Lacing(
	StrVector &		rList,
	string const &	crBeg,
	string const &	crEnd
) {
	char Beg[MAX_CHARS_IN_WORD] = {};
	char End[MAX_CHARS_IN_WORD] = {};

	strcpy(Beg, crBeg.c_str());
	strcpy(End, crEnd.c_str());

	size_t Len = crBeg.size();

	if (Len)
	{
		// ��������������� (�� ������ �����) �������� �� ������ �������
		for (size_t i = 0; i < (Len - 1); ++i)
		{
			// ... �� ������� ��������� "������������" ����� - ������� � �����
			for (size_t j = 0; j < PUZZLE_ADD; ++j)
			{
				Beg[i] = generate_char();
				rList.push_back(Beg);
			}

			// ... � ��� ����� ����� ��������� "����������" ����� - ������� � ������ �����������
			Beg[i] = End[i];
			rList.push_back(Beg);
		}
	}
}

/**
@brief �������� ���� � ������ � �� ��������, ������� � ����������-��������
@param [in] crGoalFileName - ���� � ������
@param [in] crDictFileName - ���� �� �������
@param [in] rSrcCheck- ����������� ������ (��������), ��� �������� � ���, ������� ����� ��������
*/
void test_CheckReadingGoalAndDictFiles(
	string const &	crGoalFileName,
	string const &	crDictFileName,
	int				vInputMethod,
	StrVector &		rSrcCheck
) {
	// �������� ������ �� ��������� ������, ��� ��������� ������ ������ � vInputMethod
	StrVector		StrData;
	I_SimpleFacadeOfInput * mpInput = FACTORY_I_SimpleFacadeOfInput::Produce(vInputMethod);
	mpInput->ReadData(crGoalFileName.c_str(), crDictFileName.c_str(), StrData);

	// �������� ������������ ������ ������ �� ������
	size_t vWordsAmount = rSrcCheck.size();
	if (StrData.size() != vWordsAmount)
	{
		string s = "ERROR: unexpected length of file in check " + crGoalFileName;
		throw exception(s.c_str());
	}
	sort(rSrcCheck.begin(), rSrcCheck.begin() + rSrcCheck.size());
	sort(StrData.begin(), StrData.begin() + StrData.size());
	for (size_t i = 0; i < vWordsAmount; ++i)
		if (rSrcCheck[i] != StrData[i])
		{
			string s = "ERROR: unexpected word in file in check " + crDictFileName;
			throw exception(s.c_str());
		}
}

/**
@brief ������������� ���� (�� ��������� �����-�-����� � �����-��-��������)
� ��������� ������������ ����������

@details ���������, ���  � ������-������� (� ������� ������������ �������������
�����) ...
1) ��� ������ ����� ������ ����� a->b ���������� b->a
2) �����, ��������� ������� a->b ���������� ����� �� ���� �����
3) ���� ����� �� �������������� ���������� �� ���� �����,
�� ��� �������� � ���� (��������������� �������� ���� � ������-�������)

� ������ ��������� ��������� ������� (1), (2) � (3) ������������ ����������

@param [in] crGoalFileName - ������������ ����� � ������
@param [in] crDictFileName - ������������ ����� �� ��������
@param [out] rGraphData - ����������� ����.
*/
void test_GraphBuilding(
	string const &	crGoalFileName,
	string const &	crDictFileName,
	CPlainPathBuilder::SimpleGraphData & rGraphData
) {
	CTaskDefinition		StrData;
	I_SimpleFacadeOfInput * mpInput = FACTORY_I_SimpleFacadeOfInput::Produce(0);
	mpInput->ReadData(crGoalFileName.c_str(), crDictFileName.c_str(), StrData);

	CPlainPathBuilder PlainBuilder;
	PlainBuilder.BuildGraph(StrData, rGraphData);

	// ���������, ��� � ������-������� (� ������� ������������ �������������
	// �����) ��� ������ ����� ������ ����� a->b ���������� b->a
	//
	// � ��, ��� �����, ��������� ������� a->b ���������� ����� �� ���� �����
	for (size_t i = 0; i < rGraphData.size(); ++i)
	{
		// ��� ���� ��� a->b ���������, ��� ���� b->a
		for (size_t j = 0; j < rGraphData[i].size(); ++j)
		{
			size_t idx = rGraphData[i][j]; // �� ������� i ���� � ������� j ����� � ������� idx
			vector<size_t>::const_iterator it = find(rGraphData[idx].begin(), rGraphData[idx].end(), i);
			bool Found = (it != rGraphData[idx].end());
			if (!Found)
				throw exception("ERROR: pair not founded");
			// ���������, ��� ���� a->b, �� ��������������� ����� ����������� ����� �� ���� �����
			if (1 != DiffCount(StrData[i].c_str(), StrData[idx].c_str()))
				throw exception("ERROR: words are not complicicated");
		}
	}

	// ���������, ��� ���� ����� �� �������������� ���������� �� ���� �����,
	// �� ��� �������� � ���� (��������������� �������� ���� � ������-�������).
	//
	// ��� ���� �� ����� "�����������" �� ��������� (������� �� �������� � ����,
	// ������� � ���������� ��� ��������� ��� (������ ������)
	for (size_t i = 0; i < StrData.size(); ++i)
	{
		for (size_t j = 0; j < StrData.size(); ++j)
		{
			if (1 == DiffCount(StrData[i].c_str(), StrData[j].c_str()))
			{
				vector<size_t>::const_iterator it = find(rGraphData[i].begin(), rGraphData[i].end(), j);
				if (it == rGraphData[i].end())
					if (rGraphData[i].size() && rGraphData[j].size()) // �������� ���������, � ������� ��� ���
						throw exception("ERROR: not all 'corresponding (pair) words' are processed");
			}
		}
	}
}

const size_t ELEMENT_IS_EMPTY = static_cast<size_t>(-1);

/**
@brief ���������, ��� �� ���������� ���� ������, ��� ��������� (������������ � �������)
"������� �������"
� ������ ���� ���-���� ���� ������ - ������� ���������� ����������

@param [in] crGraphData - ������-������ � ������� �������� ���������� � �����
@param [in] vPathLength - ����������� ����������� ����� �������� ����
*/
void test_CheckPathQuickly(
	CPlainPathBuilder::SimpleGraphData const &	crGraphData,
	size_t										vPathLength
) {
	CPlainPathBuilder::IdxVector  PathesLengths;
	PathesLengths.resize(crGraphData.size(), ELEMENT_IS_EMPTY);
	if (test_findShortestPathQuickly(crGraphData, 0, 0, 1, PathesLengths, vPathLength - 1))
		throw exception("ERROR: Algorithm calculated wrong result. Shortest path exists. (quick check)");
}

/**
@brief ����������� ������� ������ ����, ������ �� ����� vMaxLength ����,
"����������" ������� ("��������� � ���")

@details
���� ������ ���� ������ �� ����� ��� vMaxLength ���� - ������� �������� true
���� ���� ������ �� ����� ��� vMaxLength ���� �� ������ - ������� �������� false

1) ����� ������������� "���������� ������", ��� ����� ������������ ������ rPathesLengths,
� ������� �������� ��� ����������� ��������, ����� �� ��������� ��� ����������� ������

2) � ������� rPathesLengths ����������� ���������� �� �������� �������, �������, ����
� �������� �������� ��������� �������, ��� ������� ��� ��������� ��� ����������, �� ���
��������, ���
* ���� ��� ��� ����������� � �������� �������� ������������ ���������
* ���� ��� ���� ����������� ������ ����� - � ��� ��� ���� ����������� ��� ����������� ����� ����,
� ��������� ����� ���������, ��� ���� �� �������� � ������� ������ �� ��������� ����� �����

@param [in] crGraphData - ������-������ � ������� �������� ���������� � �����
@param [in] vFromIdx - ������ ������� �� ������� ������ ����
@param [in] vGoalIdx - ������ ������� � ������� ������ ����
@param [in] vMaxLength - �����������: ������������ ����� ����
@param [out] rPathesLengths - ������ ��� ����������� ���� �����
*/
bool test_findShortestPathQuickly(
	CPlainPathBuilder::SimpleGraphData const &	crGraphData,
	size_t										vPrevIdx,
	size_t										vCheckedIdx,
	size_t										vGoalIdx,
	CPlainPathBuilder::IdxVector				rPathesLengths,
	size_t										vPathLength
) {
	for (size_t i = 0; i < crGraphData[vCheckedIdx].size(); ++i)
	{
		size_t Idx = crGraphData[vCheckedIdx][i];
		if (Idx == vGoalIdx)
			return true;
		if (ELEMENT_IS_EMPTY == rPathesLengths[Idx])
			rPathesLengths[Idx] = vPathLength;
	}

	for (size_t i = 0; i < crGraphData[vCheckedIdx].size(); ++i)
	{
		size_t Idx = crGraphData[vCheckedIdx][i];
		if ((vPrevIdx != Idx) && (vPathLength > 1) && (rPathesLengths[Idx] == vPathLength))
		{
			if (test_findShortestPathQuickly(crGraphData, vCheckedIdx, Idx, vGoalIdx, rPathesLengths, vPathLength - 1))
				return true;
		}
		else
			printf("");
	}
	return false;
}

/**
@brief ���������, ��� �� ���������� ���� ������, ��� ��������� (������������ � �������)
"���������� �������" (� ���)
� ������ ���� ���-���� ���� ������ - ������� ���������� ����������

@param [in] crGraphData - ������-������ � ������� �������� ���������� � �����
@param [in] vPathLength - ����������� ����������� ����� �������� ����
*/
void test_CheckPathBrutally(
	CPlainPathBuilder::SimpleGraphData const &	crGraphData,
	size_t										vPathLength
) {
	CPlainPathBuilder::IdxVector  PathesLengths;
	PathesLengths.resize(crGraphData.size(), ELEMENT_IS_EMPTY);
	if (test_findShortestPathBrutally(crGraphData, 0, 0, 1, vPathLength - 1, PathesLengths))
		throw exception("ERROR: Algorithm calculated wrong result. Shortest path exists. (brutall check)");
}

/**
@brief ����������� ������� ������ ����, ������ �� ����� vMaxLength ����,
"����������" ������� ("��������� � ���")

@details
���� ������ ���� ������ �� ����� ��� vMaxLength ���� - ������� �������� true
���� ���� ������ �� ����� ��� vMaxLength ���� �� ������ - ������� �������� false

1) ����� ������������� "���������� ������", ��� ����� ������������ ������ rPathesLengths,
� ������� �������� ��� ����������� ��������, ����� �� ��������� ��� ����������� ������

2) � ������� rPathesLengths ����������� ���������� �� �������� �������, �������, ����
� �������� �������� ��������� �������, ��� ������� ��� ��������� ��� ����������, �� ���
��������, ���
* ���� ��� ��� ����������� � �������� �������� ������������ ���������
* ���� ��� ���� ����������� ������ ����� - � ��� ��� ���� ����������� ��� ����������� ����� ����,
� ��������� ����� ���������, ��� ���� �� �������� � ������� ������ �� ��������� ����� �����

@param [in] crGraphData - ������-������ � ������� �������� ���������� � �����
@param [in] vFromIdx - ������ ������� �� ������� ������ ����
@param [in] vGoalIdx - ������ ������� � ������� ������ ����
@param [in] vMaxLength - �����������: ������������ ����� ����
@param [out] rPathesLengths - ������ ��� ����������� ���� �����
*/
bool test_findShortestPathBrutally(
	CPlainPathBuilder::SimpleGraphData const & crGraphData,
	size_t	vPrevIdx,
	size_t	vFromIdx,
	size_t  vGoalIdx,
	size_t  vMaxLength,
	CPlainPathBuilder::IdxVector	rPathesLengths
) {
	for (size_t i = 0; i < crGraphData[vFromIdx].size(); ++i)
	{
		size_t NextIdx = crGraphData[vFromIdx][i];
		if ((NextIdx != vPrevIdx) && (rPathesLengths[NextIdx] != ELEMENT_IS_EMPTY))
		{
			rPathesLengths[NextIdx] = vMaxLength;
			if (NextIdx == vGoalIdx)
				return true;
			if ((vMaxLength > 1) && test_findShortestPathBrutally(crGraphData, vFromIdx, NextIdx, vGoalIdx, vMaxLength - 1, rPathesLengths))
				return true;
		}
	}
	return false;
}

/**
@brief ������������� ������ ��������� �������� ������ vNumSymbolsInWord
*/
string generate_string(
	int		vNumSymbolsInWord
) {
	string s = "";
	for (int i = 0; i < vNumSymbolsInWord; ++i)
		s += generate_char();
	return s;
}

inline char generate_char() { return ('a' + rand() % AMOUNT_CHARS); }
