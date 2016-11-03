#pragma once
#include "BasicFactories.h"
#include "BasicInterfaces.h"

/**
@class CController
@brief Класс, инкапсулирующий обработку данных.

@details CController инкапсулирует следующие действия:
 - чтение (получение) данных из источников данных
 - обработку и подготовку данных
 - поиск пути от "исходной" до "целевой" строки
*/

class CSimpleController : public I_SimplePathProduceController
{
public:
	/**
	 @brief Прочитать задание из файлов, обработать, вычислить путь.
	 @param [in] cpGoals − путь до файла с "исходной" и "целевой" строками
	 @param [in] cpDict − путь до файла со словарем
	 @param [in] vInputMethod − метод чтения файла (конкретизирует стратегию в части чтения данных)
	 @param [in] vInputMethod − метод обработки графа (конкретизирует стратегию в части поиска пути)
	 @param [out] rPath − список слов, через которые проходит путь от "исходной" до "целевой" строки
	 */
	virtual void Produce(
		char const *	cpGoals,
		char const *	cpDict,
		size_t			vInputMethod,
		size_t			vBuildMethod,
		StrVector &		rPath
	) {
		StrVector		StrData;
		try
		{
			// прочесть данные из источника данных, тип источника данных указан в vInputMethod
			I_SimpleFacadeOfInput * mpInput = FACTORY_I_SimpleFacadeOfInput::Produce(vInputMethod);
			mpInput->ReadData(cpGoals, cpDict, StrData);

			// получить (вычислить) путь, используя выбранный (указанный в vBuildMethod) алгоритм
			I_SimpleFacadeOfPathBuilder * mpProcessor = FACTORY_I_SimpleFacadeOfPathBuilder::Produce(vBuildMethod);
			mpProcessor->Build(StrData, rPath);
		}
		catch (good_exception e)
		{
			// условия возникновение данного искючения, если исходная строка и целевая строки ...
			//    * или совпадают
			//    * или связаны напрямую
			// (т.е. rPath сформирован), поэтому просто копируем данные из StrData 
			rPath = StrData;
		}
		catch( ... )
		{
			// остальные исключения пробрасывам дальше
			throw; 
		}
	}
};
