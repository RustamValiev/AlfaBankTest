#pragma once

#include "BasicPrimtives.h"

/**
@class I_SimplePathProduceController
@brief Интерфейс, который должен реализовываться классом, инкапсулирующим обработку данных.

@details Класс, реализующий интерфейс I_SimplePathProduceController
должен инкапсулировать следующие действия:
- чтение (получение) данных из источников данных
- обработку и подготовку данных
- поиск пути от "исходной" до "целевой" строки
*/
class I_SimplePathProduceController
{
public:
	/**
	@brief Прочитать задание из источника данных, обработать, вычислить путь.
	@param [in] cpGoals − спецификация получения "исходной" и "целевой" строк
	@param [in] cpDict − спецификация получения словаря
	@param [in] vInputMethod − метод чтения данных (конкретизирует стратегию в части чтения данных)
	@param [in] vBuildMethod − метод получения результата (конкретизирует стратегию в обработки данных)
	@param [out] rPath − список слов, через которые проходит путь от "исходной" и "целевой" строки
	*/
	virtual void Produce(
		char const *	cpGoals,
		char const *	cpDict,
		size_t			vInputMethod,
		size_t			vBuildMethod,
		StrVector &		rPath
	) = 0;
};

/**
@class СTaskDefinition
@brief Входные данные

@details Этот вектор строк является структурой, в которой хранятся данные,
передаваемые от класса-читальщика к классу-обработчику.

В 0-м элементе хранится "исходная" строка (ОТ которой нужно построить путь)
В 1-м элементе хранится "целевая" строка (ДО которой нужно построить путь)
В последующих элементах хранятся строки словаря.
*/
typedef  StrVector  CTaskDefinition;

/**
@class I_SimpleFacadeOfInput
@brief Интерфейс (фасад) источника данных.

@details Класс, реализующий интерфейс I_SimpleFacadeOfInput должен реализовать:
 - чтение исходных данных из источников  ("файл с целями" и "файл со словарем")
 - подготовку данных
 - формирование "задачи" - объекта TaskDefinition (вектора строк) 
*/
class I_SimpleFacadeOfInput
{
public:
	/**
	@brief Прочитать задание из источников данных, сформировать задачу (объект типа TaskDefinition).
	@param [in] cpGoals − спецификация получения "исходной" и "целевой" строк
	@param [in] cpDict − спецификация получения словаря
	@param [out] rTaskData − сформированная задача - объект TaskDefinition
	*/
	virtual void ReadData(
		char const *		cpGoals,
		char const *		cpDict,
		CTaskDefinition &	rTaskData
	) = 0;
};

/**
@class I_SimplePathBuilder
@brief Интерфейс (фасад) построителя пути

@details Класс, реализующий интерфейс I_SimplePathBuilder должен найти путь
от "исходного" до "целевого слова" через "словарь"

Исходные данные (исходное и целеовое слова и словарь) передаются в объекте TaskDefinition
*/
class I_SimpleFacadeOfPathBuilder
{
public:
	/**
	@brief Построить путь "исходного" до "целевого слова" через "словарь"
	Прочитать задание из источников данных, сформировать задачу (объект типа TaskDefinition).
	@param [in] сrTaskData − объект TaskDefinition со спецификацией задачи
	@param [out] rPath − путь в виде последовательности (вектора) строк, через которые проходит путь,
	если путь построить невозможно, вектор пустой.
	*/
	virtual void Build(
		CTaskDefinition const &	crTaskData,
		StrVector &				rPath
	) = 0;
};