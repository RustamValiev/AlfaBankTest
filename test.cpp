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
			// общие настройки
			size_t CharsInWord = 3 + rand() % 8;	//!< число символов в строке
			size_t WordsAmount = 3 + rand() % 1024;//!< число базовых строк
			size_t AddWordsAmount = 3 + rand() % 1024;//!< число подмешиваемых строк иной длины
			size_t LaceWordsAmount = 8 + rand() % 32;	//!< число базовых опорных вершин
			if (CharsInWord >= MAX_CHARS_IN_WORD)
				throw exception("Len of word is too much.");

			// сгенерировать исходные файлы: (1) файл задачи (исходная+целевая строки) и (2) словарь
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
@brief провести тесты согласно маски vTestLevel
@param[in] crGoalFileName - файл с целями
@param[in] crDictFileName - файл со словарм
@param[in] crTestDictFile - файл с тест - версией словаря для сравнения с исходником(см.generate_GoalAndDictFiles)
@param[in] rDict - проверочная версия(исходник), для сличения с той, которая будет прочтена
@param[in] vTestLevel - битовая маска тестов
*/
void test_cases(
	string const &	crGoalFileName,
	string const &	crDictFileName,
	string const &	crTestDictFile,
	StrVector  &	rDict,
	size_t			vTestLevel
) {
	cout << crGoalFileName << ": ";

	// тестировать чтение файлов
	if (vTestLevel & CHECK_FILES_READING)
		test_CheckReadingGoalAndDictFiles(crGoalFileName, crTestDictFile, 0, rDict);
	cout << " r";

	// тестировать построение графа
	CPlainPathBuilder::SimpleGraphData GraphData;
	if (vTestLevel & CHECK_GRAPH_BUILDING)
		test_GraphBuilding(crGoalFileName, crDictFileName, GraphData);
	cout << " g";

	// тестировать построение кратчайшего пути
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
@brief сгенерировать файл с целями и файл словаря для теста
@param [in] Path - путь для хранения файлов
@param [in] vNumSymbolsInWord - число символов в словах
@param [in] vWordsAmount - число слов в первоначальном (затравочном) наборе
@param [in] vAnotherLensAmount - число слов с длиною, отличающейся от vNumSymbolsInWord
@param [in] vLaceAmount - число опорных слов (из затравочного набора) через которые должен пройти путь
@param [out] rDict - формируемая тест-версия словаря
@param [out] rGoalFileName - имя файла с целями
@param [out] rDictFileName - имя файла со словарем
@param [out] rTestDictFileName - имя файла с тестовой версией словаря, точно соотвествующая rDict
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
	// файл с задачей (с исходной и целевой строками)
	ofstream GoalFile;
	string FileName = generate_string(7);
	rGoalFileName = Path + FileName + "-goal.txt";
	CAutoOpenFile<ofstream> AutoTF(GoalFile, rGoalFileName, ios::out);
	if (!GoalFile)
		throw exception("FAIL: can't create goal file.");

	// файл со словарем
	ofstream DictFile;
	rDictFileName = Path + FileName + "-dict.txt";
	CAutoOpenFile<ofstream> AutoDF(DictFile, rDictFileName, ios::out);
	if (!DictFile)
		throw exception("FAIL: can't create dictionary fail.");

	// файл с "упрощенным" словарем
	ofstream TestDictFile;
	rTestDictFileName = Path + FileName + "-dict-test.txt";
	CAutoOpenFile<ofstream> AutoTDF(TestDictFile, rTestDictFileName, ios::out);
	if (!TestDictFile)
		throw exception("FAIL: can't create test dictionary fail.");

	// формируется файл целей GoalFile и "базовый" словарь rDict
	// в "базовой" версии словаря есть определенное число уникальных слов, но
	// эти слова могут сильно отличаться друг от друга, поэтому почти гарантированно,
	// в "базовой" версии нет "путей" от исходного к целевому слову
	StrVector ChgDict;
	for (size_t i = 0; i < vWordsAmount + 2; ++i)
	{
		// генерируется новая строка
		string s = "";
		for (size_t j = 0; j < vNumSymbolsInWord; ++j)
			s += generate_char();
		// новая строка добавляется если среди ране добавленных не было такой же
		vector<string>::const_iterator it = find(rDict.begin(), rDict.end(), s);
		if (it == rDict.end())
			rDict.push_back(s);
		else
			--i;
	}
	GoalFile << rDict[0] << endl;
	GoalFile << rDict[1] << endl;
	ChgDict = rDict;

	// к "базовой версии" словаря добавить слова, которые создадут "путь" от
	// исходной строки к целевой, путь, который пройдет через vLaceAmount слов
	//
	// для этого в словаре (последовательно) берутся "опорные слова":
	// "исходное", "целевое", и vLaceAmount слов между ними
	// 
	// но эти слова отличаются друг от друга более чем на 1 символ,
	//
	// поэтому, чтобы создать "путь" - для всех пар слов "(i) и (i+1)" 
	// в словарь добавляются новые слова, которые "создают мини-путь" между (i) и (i+1)
	//
	// для всех пар слов "(i) и (i+1)" последовательно добавляются слова по следующему принципу:
	// 1) сначала добавляется слово, почти полностью похожее на слово (i), но в первой позиции
	// которого стоит первый символ слова (i+1),
	// 2) затем добавляется слово, также "почти похожее" на предыдущее, но у которого второй
	// символ заменен на второй символ из слова (i+1) 
	// ...) и т.д. - влоть до последнего символа.
	//
	// ясно, что за число итераций, равное (vNumSymbolsInWord-1), будет гарантировано создан
	// "мини-путь" от (i) к (i+1)
	//
	// всего в словарь будет добавлено (vNumSymbolsInWord-1)*(vLaceAmount+1) слов, благодаря
	// чему появится путь, не длиннее чем (vNumSymbolsInWord-1)*(vLaceAmount+1) слов
	if (vLaceAmount < vNumSymbolsInWord)	//!< число слов в пути между словами "в среднем" 
		vLaceAmount = vNumSymbolsInWord;	//!< должно быть не менее числа букв в слове
	size_t FirstIdx = 0;
	string EndStr = ChgDict[1];
	for (size_t i = 1; i < vLaceAmount; ++i)
	{
		size_t Sz = ChgDict.size() - 2;
		size_t NextIdx = 2 + (rand() % Sz); // слова выбираются случайно (кроме исходного и целевого)
		Lacing(ChgDict, ChgDict[FirstIdx], ChgDict[NextIdx]);
		FirstIdx = NextIdx;
	}
	Lacing(ChgDict, ChgDict[FirstIdx], EndStr);

	//в базовый словарь подмешиваются строки другой длины общим числом vAnotherLensAmount
	for (size_t i = 0; i < vAnotherLensAmount; ++i)
	{
		size_t AddLen = 1 + (rand() % vNumSymbolsInWord);
		size_t DstPos = rand() % (vWordsAmount + i);
		vector<string>::iterator it = ChgDict.begin();
		ChgDict.insert(it + DstPos, generate_string(vNumSymbolsInWord + AddLen));
	}

	//дополнительно перемешать массив (не трогая элементы 0 и 1)
	size_t Sz = ChgDict.size();
	for (size_t i = 2; i < Sz; ++i)
	{
		size_t NewPos = 2 + rand() % (Sz - 2);
		if (NewPos != i)
			swap(ChgDict[i], ChgDict[NewPos]);
	}

	// формируется файл словаря
	for (size_t i = 0; i < Sz; ++i)
		DictFile << ChgDict[i] << endl;

	// и файл словаря для теста
	for (size_t i = 2; i < vWordsAmount + 2; ++i)
		TestDictFile << rDict[i] << endl;
}

/**
@brief Добавить в массив rList строки, которые создают "мини-путь" от строки rcBeg до rcEnd
и для каждойдобавляемой строки добавить PUZZLE_ADD строк, отличающихся от нее на 1 символ
что создает "тупиковые, обманные" ответвления от этого пути
@param [out] rList - вектор, в который добавляются строки, создающий "мини-путь".
@param [in] сrcBeg - начальная строка для "мини-пути"
@param [in] сrcEnd - конечная строка для "мини-пути"
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
		// последовательно (на каждом шуаге) заменяем по одному символу
		for (size_t i = 0; i < (Len - 1); ++i)
		{
			// ... но сначала добавляем "запутывающие" слова - ведущие в тупик
			for (size_t j = 0; j < PUZZLE_ADD; ++j)
			{
				Beg[i] = generate_char();
				rList.push_back(Beg);
			}

			// ... и уже после этого добавляем "правильное" слово - ведущее в нужном направлении
			Beg[i] = End[i];
			rList.push_back(Beg);
		}
	}
}

/**
@brief прочесть файл с целями и со словарем, сличить с исходником-образцом
@param [in] crGoalFileName - файл с целями
@param [in] crDictFileName - файл со словарм
@param [in] rSrcCheck- проверочная версия (исходник), для сличения с той, которая будет прочтена
*/
void test_CheckReadingGoalAndDictFiles(
	string const &	crGoalFileName,
	string const &	crDictFileName,
	int				vInputMethod,
	StrVector &		rSrcCheck
) {
	// прочесть данные из источника данных, тип источника данных указан в vInputMethod
	StrVector		StrData;
	I_SimpleFacadeOfInput * mpInput = FACTORY_I_SimpleFacadeOfInput::Produce(vInputMethod);
	mpInput->ReadData(crGoalFileName.c_str(), crDictFileName.c_str(), StrData);

	// проверка правильности чтения данных из файлов
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
@brief Сгенерировать граф (на основании файла-с-целям и файла-со-словарем)
и проверить правильность построения

@details проверить, что  в вектор-векторе (в котором закодировано представление
графа) ...
1) для любого ребра любого ребра a->b существует b->a
2) слова, связанные связкой a->b отличаются ровно на одну букву
3) если слова из ВходногоНабора отличаются на одну букву,
то они занесены в граф (соответствующая «запись» есть в вектор-векторе)

в случае выявления нарушения условий (1), (2) и (3) генерируются исключения

@param [in] crGoalFileName - наименование файла с целями
@param [in] crDictFileName - наименование файла со словарем
@param [out] rGraphData - построенный граф.
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

	// проверить, что в вектор-векторе (в котором закодировано представление
	// графа) для любого ребра любого ребра a->b существует b->a
	//
	// и то, что слова, связанные связкой a->b отличаются ровно на одну букву
	for (size_t i = 0; i < rGraphData.size(); ++i)
	{
		// для всех дуг a->b убедиться, что есть b->a
		for (size_t j = 0; j < rGraphData[i].size(); ++j)
		{
			size_t idx = rGraphData[i][j]; // из вершины i дуга с номером j ведет к вершине idx
			vector<size_t>::const_iterator it = find(rGraphData[idx].begin(), rGraphData[idx].end(), i);
			bool Found = (it != rGraphData[idx].end());
			if (!Found)
				throw exception("ERROR: pair not founded");
			// проверить, что если a->b, то соответствующие слова различаются ровно на одну букву
			if (1 != DiffCount(StrData[i].c_str(), StrData[idx].c_str()))
				throw exception("ERROR: words are not complicicated");
		}
	}

	// проверить, что если слова из ВходногоНабора отличаются на одну букву,
	// то они занесены в граф (соответствующая «запись» есть в вектор-векторе).
	//
	// при этом не нужно "реагировать" на дубликаты (которые не включены в граф,
	// поэтому у дубликатов нет исходящих дуг (вектор пустой)
	for (size_t i = 0; i < StrData.size(); ++i)
	{
		for (size_t j = 0; j < StrData.size(); ++j)
		{
			if (1 == DiffCount(StrData[i].c_str(), StrData[j].c_str()))
			{
				vector<size_t>::const_iterator it = find(rGraphData[i].begin(), rGraphData[i].end(), j);
				if (it == rGraphData[i].end())
					if (rGraphData[i].size() && rGraphData[j].size()) // исключая дубликаты, у которых нет дуг
						throw exception("ERROR: not all 'corresponding (pair) words' are processed");
			}
		}
	}
}

const size_t ELEMENT_IS_EMPTY = static_cast<size_t>(-1);

/**
@brief првоерить, что не существует пути короче, чем указанный (передаваемый в функцию)
"быстрым методом"
в случае если все-таки путь найден - функция генерирует исключение

@param [in] crGraphData - вектор-вектор в котором хранится информация о графе
@param [in] vPathLength - максимально разрешенная длина искомого пути
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
@brief рекурсивная функция поиска пути, длиною не более vMaxLength слов,
"брутальным" методом ("перебором в лоб")

@details
если НАЙДЕН путь длиною не более чем vMaxLength слов - функция сообщает true
если путь длиною не более чем vMaxLength слов НЕ найден - функция сообщает false

1) метод оптимизирован "отсечением хвоста", для этого используется массив rPathesLengths,
в котором хранятся уже вычисленные значения, чтобы не повторять уже вычисленные поиски

2) в массиве rPathesLengths сохраняется расстояние до исходной вершины, поэтому, если
в процессе перебора встречаем вершину, для которой уже вычислено это расстояние, то это
означает, что
* либо она уже просмотрена в процессе текущего рекурсивного просмотра
* либо она была просмотрена совсем ранее - и для нее были просмотрены все возможности найти пути,
а поскольку поиск продолжен, эти пути не приводят к целевой строке за указанное число хопов

@param [in] crGraphData - вектор-вектор в котором хранится информация о графе
@param [in] vFromIdx - индекс вершины от которой искать путь
@param [in] vGoalIdx - индекс вершины к которой искать путь
@param [in] vMaxLength - ограничение: максимальная длина пути
@param [out] rPathesLengths - массив уже заполненных длин путей
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
@brief првоерить, что не существует пути короче, чем указанный (передаваемый в функцию)
"брутальным методом" (в лоб)
в случае если все-таки путь найден - функция генерирует исключение

@param [in] crGraphData - вектор-вектор в котором хранится информация о графе
@param [in] vPathLength - максимально разрешенная длина искомого пути
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
@brief рекурсивная функция поиска пути, длиною не более vMaxLength слов,
"брутальным" методом ("перебором в лоб")

@details
если НАЙДЕН путь длиною не более чем vMaxLength слов - функция сообщает true
если путь длиною не более чем vMaxLength слов НЕ найден - функция сообщает false

1) метод оптимизирован "отсечением хвоста", для этого используется массив rPathesLengths,
в котором хранятся уже вычисленные значения, чтобы не повторять уже вычисленные поиски

2) в массиве rPathesLengths сохраняется расстояние до исходной вершины, поэтому, если
в процессе перебора встречаем вершину, для которой уже вычислено это расстояние, то это
означает, что
* либо она уже просмотрена в процессе текущего рекурсивного просмотра
* либо она была просмотрена совсем ранее - и для нее были просмотрены все возможности найти пути,
а поскольку поиск продолжен, эти пути не приводят к целевой строке за указанное число хопов

@param [in] crGraphData - вектор-вектор в котором хранится информация о графе
@param [in] vFromIdx - индекс вершины от которой искать путь
@param [in] vGoalIdx - индекс вершины к которой искать путь
@param [in] vMaxLength - ограничение: максимальная длина пути
@param [out] rPathesLengths - массив уже заполненных длин путей
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
@brief сгенерировать строку случайных символов длиною vNumSymbolsInWord
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
