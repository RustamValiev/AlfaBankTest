#pragma once

void test_All(
	string const	crPath,
	size_t			vTestLevel,
	size_t			vTestsAmount
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/**
@brief ������������� ������ ��������� �������� ������ vNumSymbolsInWord
*/
string generate_string(
	int		vNumSymbolsInWord
);

inline char generate_char();