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

		// два массива, 
		size_t CurrIdx = 0, NextIdx = 1;
		vector<vector<size_t> > LookingVertexes(2);
		vector<size_t> * pCurrVertexes = &LookingVertexes[CurrIdx];
		vector<size_t> * pNextVertexes = &LookingVertexes[NextIdx];
		
		pCurrVertexes->push_back(GOAL_VERTEX_IDX);
		size_t ViewedVertex = Elements;
		bool PathFounded = false;
		while (pCurrVertexes->size() && !PathFounded)
		{
			++CurrDistance;
			pNextVertexes->clear();
			for (size_t i = 0; i < pCurrVertexes->size()  && !PathFounded; ++i)
			{
				ViewedVertex = (*pCurrVertexes)[i];
				for (size_t j = 0; j < crGraphData[ViewedVertex].size() && !PathFounded; ++j)
				{
					size_t NextVertex = crGraphData[ViewedVertex][j];
					if (FIRST_VERTEX_IDX == NextVertex)
						PathFounded = true;
					else if (Distance[NextVertex] == Elements)
					{
						Previous[NextVertex] = ViewedVertex;
						Distance[NextVertex] = CurrDistance;
						pNextVertexes->push_back(NextVertex);
					}
				}
			}
			swap(CurrIdx, NextIdx);
			pCurrVertexes = &LookingVertexes[CurrIdx];
			pNextVertexes = &LookingVertexes[NextIdx];
		}

		rPath.clear();
		if (PathFounded)
		{
			rPath.push_back(FIRST_VERTEX_IDX);
			for (size_t i = 0; i < CurrDistance; ++i)
			{
				rPath.push_back(ViewedVertex);
				ViewedVertex = Previous[ViewedVertex];
			}
		}
	}
};
