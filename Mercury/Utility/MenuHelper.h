#pragma once
#include "..\Array\StringToStringArray.h"

class CMarkup;
class MenuHelper
{
public:
	MenuHelper();
	virtual ~MenuHelper();

	static bool			SaveMenuAsXml				(CMenu* pMenu, const CString sMenuXmlFile, bool bReplaceExistsing);
	static bool			LoadMenuFromXml				(const CString sMenuXmlFile, CMenu& menu, StringToStringArray* pItemData = NULL);
	static bool			CopyItemStrings				(HMENU hMenuFrom, HMENU hMenuTo);

protected:
	static bool			WalkThroughSubMenuItems_Rec	(CMenu* pSubMenu, CString& sXmlMenu);
	static HMENU		CreatePopupMenuFromXml		(CMarkup* pXml, StringToStringArray* pItemData);
};