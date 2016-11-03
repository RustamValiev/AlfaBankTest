#pragma once
#include "BasicInterfaces.h"

/**
@class FACTORY_I_SimpleFacadeOfInput
@brief Фабрика класса для объектов, реализующих (поддерживающих) интерфейс I_SimpleFacadeOfInput
(интерфес читателя данных)
*/
class FACTORY_I_SimpleFacadeOfInput
{
public:
	/**
	@brief Получить объект, реализующий интерфейс I_SimpleFacadeOfInput.
	@param [in] vType − идентификатор типа
	@return − объект типа vType, реализующий интерфейс I_SimpleFacadeOfInput.
	*/
	static I_SimpleFacadeOfInput * Produce(
		size_t	vType
	);
};

/**
@class FACTORY_I_SimpleFacadeOfPathBuilder
@brief Фабрика класса для объектов, реализующих (поддерживающих) интерфейс I_SimpleFacadeOfPathBuilder
(интерфес построителя пути)
*/
class FACTORY_I_SimpleFacadeOfPathBuilder
{
public:
	/**
	@brief Получить объект, реализующий интерфейс I_SimpleFacadeOfPathBuilder.
	@param [in] vType − идентификатор типа
	@return − объект типа vType, реализующий интерфейс I_SimpleFacadeOfPathBuilder.
	*/
	static I_SimpleFacadeOfPathBuilder * Produce(
		size_t	vType
	);
};
