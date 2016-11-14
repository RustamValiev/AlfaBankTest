#pragma once

void test_All(
	string const	crPath,
	size_t			vTestLevel,
	size_t			vTestsAmount
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/**
@brief сгенерировать строку случайных символов длиною vNumSymbolsInWord
*/
string generate_string(
	int		vNumSymbolsInWord
);

inline char generate_char();