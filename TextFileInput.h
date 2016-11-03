#pragma once
#include "stdafx.h"
#include "BasicPrimtives.h"
#include "BasicInterfaces.h"
#include <algorithm>

#include <cstdio>

using namespace std;

// калибровочный класс для "читателя из файла", действие: приведение к нижнему регистру
class CCalibrateToLower
{
public:
	static void execute(string & rStr) { transform(rStr.begin(), rStr.end(), rStr.begin(), ::tolower); }
};

// калибровочный класс для "читателя из файла", действие: заглушка, не делать ничего
class CCalibrateNoAdjust
{
public:
	static void execute(string & rStr) {};
};

/**
@class CTextFileInput
@brief  "читатель из файла": загружает данные из текстовых файлов,

@details Класс, реализует интерфейс I_SimpleFacadeOfInput и реализует:
- чтение исходных данных из источников  ("файл с целями" и "файл со словарем")
- подготовку данных
- формирование "задачи" - объекта TaskDefinition (вектора строк)

Предусмотрена "калибровка" класса для добавления возможностей пред- и пост- обработки
строк, читаемых из файла. Для этого предназначен параметр шаблона Calibrate.
Пример "калибровки" (пред-пост-обработки строк) - приведение строк к нижнему регистру.
*/
template< typename Calibrate = CCalibrateToLower >
class CTextFileInput : public I_SimpleFacadeOfInput
{
public:
	CTextFileInput() {}

	virtual ~CTextFileInput() {}

	/**
	@brief Прочитать данные из файлов, сформировать задачу (объект типа TaskDefinition).
	@param [in] cpGoalsFileName − имя (с путем) файла, содержащего "исходную" и "целевую" строки
	@param [in] cpDictFileName − имя (с путем) файла, содержащего строки словаря
	@param [out] rTaskData − сформированная задача - объект TaskDefinition
	*/
	virtual void ReadData(
		char const *		cpGoalsFileName,
		char const *		cpDictFileName,
		CTaskDefinition &	rTaskData
	) {
		ReadGoal(cpGoalsFileName, rTaskData);
		ReadDict(cpDictFileName, rTaskData);
	}

private:
	void ReadGoal(
		char const *		cpGoalsFileName,
		CTaskDefinition &	rTaskData
	) const;

	void ReadDict(
		char const *		cpDictFileName,
		CTaskDefinition &	rTaskData
	) const;
};

/**
@brief Найти число различающихся символов в двух строках. Различием считается если в одной строк
в позииции i есть символ, а во второй - символа нет (т.е. вторая строка короче первой). 
@param [in] cpStr1 − первая строка
@param [in] cpStr2 − вторая строка
@return [out] - число различающихся символов
*/
inline size_t DiffCount(
	const char *	cpStr1,
	const char *	cpStr2
) {
	size_t Cnt = 0;
	size_t i = 0;
	for ( ; cpStr1[i] && cpStr2[i]; ++i)
		if (cpStr1[i] != cpStr2[i])
			++Cnt;
	for (; cpStr1[i] || cpStr2[i]; ++i)
		++Cnt;
	return Cnt;
}

template< typename Calibrate >
void CTextFileInput< Calibrate >::ReadGoal(
	char const *		cpGoalsFileName, 
	CTaskDefinition &	rTaskData
) const {
	// открыть файл с задачей (исходной и целевой строками)
	ifstream GoalFile;
	CAutoOpenFile AutoTF(GoalFile, cpGoalsFileName, ios::in);
	if (!GoalFile)
		throw exception("FAIL: No goal file.");

	// прочесть исходную и целевую строки
	string BegStr = "", EndStr = "";
	GoalFile >> BegStr;
	GoalFile >> EndStr;

	// CHECK: строки непустые и одинаковой длины
	size_t LenOfStr = BegStr.length();
	if ((!LenOfStr) || (EndStr.length() != LenOfStr))
		throw exception("FAIL: goal is unavailable for processing");

	// "подрегулировать" строки (например, привести к одному регистру)
	Calibrate::execute(BegStr);
	Calibrate::execute(EndStr);

	// положить в строки в общий список 
	rTaskData.push_back(BegStr);
	rTaskData.push_back(EndStr);

	// CHECK: начальное и конечное состояния:
	//     * либо совпадают;
	//     * либо "связаны напрямую".
	// Если это так - генерируется исключенин спец.типа good_exception
	//
	// Конечно, решение сообщать об успехе генерацией исключения - "не айс",
	// но в некоторых ситуациях (иногда) я считаю, оправданным использование
	// такой техники. Одна из таких ситуций - получение результата на стадии
	// загрузки (развертывания) данных - еще до запуска основного алгоритма.
	//
	// Я оправдываю это тем, что (в данном контексте) это в некотором смысле
	// нештатная ситуация, или как минимум - ситуация требующая нестандартной 
	// (для данной стадии) обработки, поскольку развертывание данных еще не
	// выполнено до конца, и вытаскивание наверх результата "опережающего" вычисления 
	// (до завершения инициализации) фактически "ломает" исполнения принципа RAII. 
	if (BegStr == EndStr)
		throw good_exception(BegStr + "\r\n" + BegStr);
	if (1 == DiffCount(BegStr.c_str(), EndStr.c_str()))
		throw good_exception(BegStr + "\r\n" + EndStr);
}

template< typename Calibrate >
void CTextFileInput< Calibrate >::ReadDict(
	char const *		cpDictFileName, 
	CTaskDefinition &	rTaskData
) const {
	// открыть файл со словарем
	ifstream DictFile;
	CAutoOpenFile AutoDF(DictFile, cpDictFileName, ios::in);
	if (!DictFile)
		throw exception("FAIL: no gictionary fail.");

	// прочесть словарь в общий список - берутся только строки нужной длины
	size_t LenOfStr = rTaskData[0].length();
	while (!DictFile.eof())
	{
		string IncommingStr = "";
		DictFile >> IncommingStr;
		Calibrate::execute(IncommingStr);

		if (IncommingStr.length() == LenOfStr)
			rTaskData.push_back(IncommingStr);
//		cout << IncommingStr << ":(" << IncommingStr.length() << ") " << endl;
	}
}
