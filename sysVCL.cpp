#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysVCL {

#ifdef __BORLANDC__

int FindComboBoxItemByObject(TComboBox *comboBox, String object) {
	for (int itItem = 0; itItem < comboBox->Items->Count; ++itItem) {
		String itemObject = *(String *)comboBox->Items->Objects[itItem];
		if (itemObject == object) return itItem;
	}

	return -1;
}

int GetComboBoxCurrentObjectAsInt(TComboBox *comboBox, int defaultValue) {
	if (comboBox->ItemIndex == -1) return defaultValue;
	return StrToInt(*(String *)(comboBox->Items->Objects[comboBox->ItemIndex]));
}

TControl * FindControl(TWinControl *parent, const String &findControlName) {
	TControl *control;
	for (int ctr = 0; ctr < parent->ControlCount; ctr++) {
		control = (TControl*)parent->Controls[ctr];
		if (control->Name == findControlName)
			return control;
	}

	return NULL;
}

TTreeNode * FindTreeNodeByCompareFunction(
	TTreeView *tree,
	void *compareData,
	TreeNodeCompareFunction compare,
	TTreeNode *parentNode) {

	if (parentNode == NULL) {
		for (int itNode = 0, len = tree->Items->Count;  itNode < len; ++itNode) {
			if (compare(tree->Items->Item[itNode], compareData)) {
				return tree->Items->Item[itNode];
			}
        }
    } else {
    	TTreeNode *node = parentNode;
		while ((node = node->GetNext()) != NULL) {
			if (compare(node, compareData)) {
				return node;
			}
        }
    }
	return NULL;
}

TTreeNode * FindTreeNodeByNodeName(TTreeView *tree, String nodeName, TTreeNode *parentNode){
	if (parentNode == NULL) {
        for (int itNode = 0, len = tree->Items->Count;  itNode < len; ++itNode) {
            if (tree->Items->Item[itNode]->Text == nodeName) {
                return tree->Items->Item[itNode];
            }
        }
    } else {
    	TTreeNode *node = parentNode;
    	while ((node = node->GetNext()) != NULL) {
			if (node->Text == nodeName) {
				return node;
            }
        }
    }
	return NULL;
}

TTreeNode * FindChildTreeNodeByNodeName(TTreeView *tree, String nodeName, TTreeNode *parentNode){
	for (TTreeNode *child = parentNode->getFirstChild(); child != NULL; child = child->getNextSibling()) {
		if (child->Text == nodeName) {
			return child;
        }
    }

    return NULL;
}

int FindStringGridRowByCellValue(TStringGrid *stringGrid, int iColumn, String sValue) {
	for (int itRow = 0; itRow < stringGrid->RowCount; ++itRow) {
	    if (stringGrid->Cells[iColumn][itRow] == sValue) {
    		return itRow;
        }
    }

    return -1;
}

void DeleteStringGridRow(TStringGrid *stringGrid, int iRowToDelete) {
	if (stringGrid->RowCount == 1) {
		for (int itCol = 0; itCol < stringGrid->ColCount; ++itCol) {
			stringGrid->Cells[itCol][0] = "";
		}
	}

	for (int itRow = iRowToDelete; itRow < stringGrid->RowCount - 1; ++itRow) {
		for (int itCol = 0; itCol < stringGrid->ColCount; ++itCol) {
			stringGrid->Cells[itCol][itRow] = stringGrid->Cells[itCol][itRow + 1];
		}
	}
	stringGrid->RowCount -= 1;
}

void ClearStringGrid(TStringGrid *stringGrid) {
	stringGrid->RowCount = stringGrid->FixedRows + 1;

	for (int itCol = 0; itCol < stringGrid->ColCount; ++itCol) {
		stringGrid->Cells[itCol][stringGrid->RowCount - 1] = "";
	}
}

//register all necessary classes for working with ObjectTextToBinary function
static void __RegisterClasses() {
	RegisterClass( __classid(TForm));
	RegisterClass( __classid(TEdit));
	RegisterClass( __classid(TImage));
	RegisterClass( __classid(TButton));
	RegisterClass( __classid(TScrollBox));
	RegisterClass( __classid(TShape));
	RegisterClass( __classid(TLabel));
}

TComponent * StringToComponent(const String &stringComponent) {
	__RegisterClasses();

	TMemoryStream *memoryStream = new TMemoryStream();
	TStringStream *stringStream = new TStringStream(stringComponent);
	TComponent *component = NULL;

	try {
		ObjectTextToBinary(stringStream, memoryStream);
		memoryStream->Seek(0, soFromBeginning);
		component = memoryStream->ReadComponent(NULL);
	} __finally {
		if (memoryStream) {
			delete memoryStream;
		}
		if (stringStream) {
			delete stringStream;
		}
	}

	return component;
}

#endif
}

