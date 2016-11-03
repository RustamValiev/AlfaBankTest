#pragma once
#include "stdafx.h"
#include <iostream>
#include "BasicPrimtives.h"
#include "BasicInterfaces.h"

/**
@class CPlainPathBuilder
@brief Построитель пути от "исходного" до "целевого слова" через "словарь",
с использованием алгоритма, описанного в "Пояснительной записке"

Исходные данные (исходное и целеовое слова и словарь) передаются в объекте TaskDefinition
*/
class CPlainPathBuilder : public I_SimpleFacadeOfPathBuilder
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
	) {
		// построить граф "связей между словами"
		SimpleGraphData GraphData;
		//TaskDefinition TaskData = crTaskData;
		BuildGraph(crTaskData, GraphData);

		// найти кратчайший путь в графе
		IdxVector PathAsIntEdges;
		BuildPath(GraphData, PathAsIntEdges);
		
		// по индексам вершин (через которые проходит путь) построить
		// список слов, составляющих путь от кота к слону
		for (size_t i = 0; i < PathAsIntEdges.size(); ++i)
			rPath.push_back(crTaskData[PathAsIntEdges[i]]);
	}

private:
	typedef vector<vector<size_t> >	SimpleGraphData;
	typedef vector<size_t>			IdxVector;

	void BuildGraph(
		CTaskDefinition		vTaskData,
		SimpleGraphData &	rGraphData
	) {
		size_t WordsCnt = vTaskData.size();
		rGraphData.resize(WordsCnt);

		for (size_t i = 0; i < WordsCnt; ++i)
		{
			// для взятой (i-й) строки: 
			//   * проверить, что строка не совпадает с одной из уже просмотренных,
			//   * добавить к графу "обратные" грани - ведущие из взятой вершины к предыдущим.
			for (size_t j = 0; j < i; ++j)
			{
				if ("" != vTaskData[j])
				{
					// выяснить число различающихся символов
					size_t lDiffCount = DiffCount(vTaskData[i].c_str(), vTaskData[j].c_str());
					if (!lDiffCount)
					{
						// если новое слово совпадает с одним из просмотренных ранее,
						// то: сбросить полученные данные, саму строку стереть
						rGraphData[i].clear();
						vTaskData[i] = "";
						break;
					}
					if (1 == lDiffCount)
						rGraphData[i].push_back(j);
				}
			}

			// ... построить "прямые" грани - от предыдущих вершин к данной (i-й) вершине
			for (size_t j = 0; j < rGraphData[i].size(); ++j)
				rGraphData[rGraphData[i][j]].push_back(i);
		}
	}

	void BuildPath(
		SimpleGraphData const &	crGraphData,
		IdxVector &				rPath
	) {
		// индексы исходной и целевой вершин
		const int FIRST_VERTEX_IDX = 0;
		const int GOAL_VERTEX_IDX = 1;

		// заполнить массивы Distance и Previous служебным значением,
		// (которое не может "появиться" в них в процессе вычислений)
		size_t Elements = crGraphData.size();
		IdxVector Distance(Elements);
		IdxVector Previous(Elements);
		for (size_t i = 0; i < Elements; ++i)
		{
			Distance[i] = Elements;
			Previous[i] = Elements;
		}

		// идем "в обратном порядке" - начинаем с "целевой" вершины
		// и "идем" в сторону первой вершины
		size_t CurrDistance = 0;
		Distance[GOAL_VERTEX_IDX] = CurrDistance;

		// два массива, Src и Dst, используемые в процессе обработки
		//   * Src это pSrcVertexes - список "к рассмотрению"
		//   * Dst это pDstVertexes - подготавливаемый список
		size_t CurrIdx = 0, NextIdx = 1;
		vector<vector<size_t> > LookingVertexes(2);
		vector<size_t> * pSrcVertexes = &LookingVertexes[CurrIdx];
		vector<size_t> * pDstVertexes = &LookingVertexes[NextIdx];
		
		// начинаем с "целевой вершины"
		pSrcVertexes->push_back(GOAL_VERTEX_IDX);

		// поиск пути, признак того, что путь найден: PathFounded=true
		bool PathFounded = false;
		while (pSrcVertexes->size() && !PathFounded)
		{
			// следующие вершины, которые берутся в рассмотренипе
			// имеют расстояние до "конечной вершины" на единицу больше
			++CurrDistance;

			// готовимся к следующему проходу, очищаем вектор "следующих рассматриваемых вершин"
			pDstVertexes->clear();
			for (size_t i = 0; i < pSrcVertexes->size()  && !PathFounded; ++i)
			{
				// берем вершину из списка "к рассмотрению", перебираем исходящие дуги
				// точнее - все вершины, связаные с данной (которая "к рассмотрению") 
				size_t ViewedVertex = (*pSrcVertexes)[i];
				for (size_t j = 0; j < crGraphData[ViewedVertex].size() && !PathFounded; ++j)
				{
					// для каждой "связанной вершины"....
					size_t NextVertex = crGraphData[ViewedVertex][j];
					
					// проверяем - была ли "новая вершина" уже посчитана ранее - в этом случае для нее
					// вычислена дистанция до "целевой вершины" и занесена в  Distance (изначально в
					// ячейках находится значение, которое не может быть получено в процессе вычислений 
					// (равноеElements), что является признаком того, что вершина еще не была просмотрена)
					if (Distance[NextVertex] == Elements)
					{
						// если вершина еще не была просмотрена, то для нее заполняем значения в векторах
						//   Distance - расстояние до "целевой вершины"
						//   Previous - индекс вершины, через которую идет кратчайший путь к "целевой"
						// далее, заносим вершину в "список к последующему просмотру"
						Previous[NextVertex] = ViewedVertex;
						Distance[NextVertex] = CurrDistance;
						pDstVertexes->push_back(NextVertex);
					}

					// проверяем не является ли она "исходной" (т.е. не нашли ли искомый путь)
					// в таком случае устаналиваем признак того, что найдено искомое (PathFounded = true)
					if (FIRST_VERTEX_IDX == NextVertex)
						PathFounded = true;
				}
			}

			// чтобы не тратить время на копирование - просто делаем swap массивов
			swap(CurrIdx, NextIdx);
			pSrcVertexes = &LookingVertexes[CurrIdx];
			pDstVertexes = &LookingVertexes[NextIdx];
		}

		// строим путь от "исходной" до "целевой" вершины - раскрутка делается в порядке
		// обратном тому, которому следовали на предыдущем шаге - раскрутка производится
		// с использованием массива Previous - в таком случае первой идет "исходная вершина",
		// затем - все вершины пути вплоть до "целевой" вершины
		rPath.clear();
		if (PathFounded)
		{
			size_t VertexIdx = FIRST_VERTEX_IDX;
			for (size_t i = 0; i < CurrDistance+1; ++i)
			{
				rPath.push_back(VertexIdx);
				VertexIdx = Previous[VertexIdx];
			}
		}
	}
};
