//---------------------------------------------------------------------------

#ifndef sysVCLH
#define sysVCLH
//---------------------------------------------------------------------------

namespace sysVCL {

typedef bool (*TreeNodeCompareFunction)(TTreeNode *node, void *compareData);

/*
	find and return item index in comboBox or -1
    attention combo box object must be String
*/
int FindComboBoxItemByObject(TComboBox *comboBox, String object);

/*
	find specify by findControlName under parent control
	return TWinControl* or NULL
*/
TControl * FindControl(TWinControl *parent, const String &findControlName);

/*
	return current selected object as int, convert StrToInt( (TSensor *) (TObject *) )
	if no Combo Box value is selected, return defaulValue [0 - default]
*/
int GetComboBoxCurrentObjectAsInt(TComboBox *comboBox, int defaultValue);

/*
	@param tree
	@param compareData any data passed to compare function
	@param compare pointer to compare function
	@param [parentNode = NULL] paren node
	find and return TTreeNode * returned by compare function or NULL
*/
TTreeNode *FindTreeNodeByCompareFunction(
	TTreeView *tree,
	void *compareData,
	TreeNodeCompareFunction compare,
	TTreeNode *parentNode = NULL);

/*
	@param tree
	@param nodeName - text of node
	@param [parentNode = NULL] parent node
	find and return TTreeNode * by node name or NULL
*/
TTreeNode * FindTreeNodeByNodeName(TTreeView *tree, String nodeName, TTreeNode *parentNode = NULL);

/*
	@param tree
    @param nodeName
    @param parentNode
    find direct child node of parent Node
*/
TTreeNode * FindChildTreeNodeByNodeName(TTreeView *tree, String nodeName, TTreeNode *parentNode);

/*
	@param stringGrid
	@param iColumn
	@param value
	find row in string grid where cells[iRow][iCol] == value
	return -1 if not find
*/
int FindStringGridRowByCellValue(TStringGrid *stringGrid, int iColumn, String sValue);

void DeleteStringGridRow(TStringGrid *stringGrid, int iRowToDelete);

void ClearStringGrid(TStringGrid *stringGrid);

/*
	return ptr component or NULL
*/
TComponent * StringToComponent(const String &stringComponent);

}
#endif
