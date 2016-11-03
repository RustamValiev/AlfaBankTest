#include "stdafx.h"
#include "BasicFactories.h"
#include "TextFileInput.h"
#include "PlainPathBuilder.h"

I_SimpleFacadeOfInput * FACTORY_I_SimpleFacadeOfInput::Produce(
	size_t	vType
) {
	return new CTextFileInput<>();
}

I_SimpleFacadeOfPathBuilder * FACTORY_I_SimpleFacadeOfPathBuilder::Produce(
	size_t	vType
) {
	return new CPlainPathBuilder();
}