#include "stdafx.h"
#include "MenuHelper.h"
#include "..\Path\Path.h"
#include "..\XML\Markup.h"
#include "..\Image\ImageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MenuHelper::MenuHelper(){
	}

MenuHelper::~MenuHelper(){
	}

bool
MenuHelper::WalkThroughSubMenuItems_Rec(CMenu* pSubMenu, CString& sXmlMenu){
	if( !pSubMenu ) return false;
	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(MENUITEMINFO));
	char szItemText[255];

	int nItemCt = pSubMenu->GetMenuItemCount();
	for(int i=0; i<nItemCt; i++){
		mii.cbSize		= sizeof(MENUITEMINFO);
		mii.fMask		= MIIM_ID|MIIM_STRING|MIIM_SUBMENU;
		mii.hSubMenu	= 0;
		mii.wID			= 0;
		mii.dwTypeData	= (LPSTR)szItemText;
		mii.cch			= 254;
		szItemText[0]	= '\0';
		if( pSubMenu->GetMenuItemInfo(i, &mii, 1) ){
			CString sMenuItem;
			if( mii.hSubMenu ){
				sMenuItem.Format(_T("<submenu title=\x22%s\x22>\r\n"), mii.dwTypeData ? (LPTSTR)mii.dwTypeData : _T(""));
				sXmlMenu += sMenuItem;
				WalkThroughSubMenuItems_Rec(CMenu::FromHandle(mii.hSubMenu), sXmlMenu);
				sXmlMenu += _T("</submenu>\r\n");
				}
			else{
				if( mii.dwTypeData )
					sMenuItem.Format(_T("<item id=\x22%d\x22>%s</item>\r\n"), (int)mii.wID, (LPTSTR)mii.dwTypeData);
				else
					sMenuItem.Format(_T("<item id=\x22%d\x22></item>\r\n"), (int)mii.wID);
				sXmlMenu += sMenuItem;
				}
			}
		}
	return true;
	}

bool
MenuHelper::SaveMenuAsXml(CMenu* pMenu, const CString sMenuXmlFile, bool bReplaceExistsing){
	if( !pMenu ) return false;
	MENUITEMINFO mii;
	memset(&mii, 0, sizeof(MENUITEMINFO));
	char szItemText[255];

	CString sMenuXml	= _T("<menu>\r\n");
	int		nLoop		= 0;
	CMenu*	pSubMenu1	= pMenu->GetSubMenu(nLoop);
	while( pSubMenu1 ){
		mii.cbSize		= sizeof(MENUITEMINFO);
		mii.fMask		= MIIM_STRING;
		mii.hSubMenu	= 0;
		mii.wID			= 0;
		mii.dwTypeData	= (LPSTR)szItemText;
		mii.cch			= 254;
		szItemText[0]	= '\0';
		if( pMenu->GetMenuItemInfo(nLoop, &mii, 1) ){
			CString sMenuItem;
			sMenuItem.Format(_T("<submenu title=\x22%s\x22>\r\n"), mii.dwTypeData);
			sMenuXml += sMenuItem;
			}
		else
			sMenuXml += _T("<submenu title=\x22\x22>\r\n");

		WalkThroughSubMenuItems_Rec(pSubMenu1, sMenuXml);
		sMenuXml	+= _T("</submenu>\r\n");
		nLoop		++;
		pSubMenu1	= pMenu->GetSubMenu(nLoop);
		}
	sMenuXml += _T("</menu>\r\n");

	bool bRet = Path::SaveAsFile(sMenuXml.GetBuffer(), sMenuXml.GetLength(), sMenuXmlFile);
	return bRet;
	}

HMENU 
MenuHelper::CreatePopupMenuFromXml(CMarkup* pXml, StringToStringArray* pItemData){
	wchar_t wszItemText[100];
	CString sTagName, sId, sData;
	HMENU	hPopup		= NULL;
	int		nIndex		= 0;
	while( pXml->FindElem() ){
		sTagName = pXml->GetTagName();
		if( !sTagName.CompareNoCase(_T("submenu")) ){
			sData	= pXml->GetAttrib(_T("title"));
			pXml->IntoElem();
			HMENU hPopupRec = CreatePopupMenuFromXml(pXml, pItemData);
			if( hPopupRec ){
				if( !hPopup )
					hPopup = ::CreatePopupMenu();
				ImageHelper::UTF8ToUnicode(&sData, wszItemText, 100);
				::AppendMenuW(hPopup, MF_STRING|MF_POPUP, (UINT_PTR)hPopupRec, wszItemText);
				}
			pXml->OutOfElem();
			continue;
			}

		if( !sTagName.CompareNoCase(_T("item")) ){
			sId		= pXml->GetAttrib(_T("id"));
			sData	= pXml->GetData();

			if( !hPopup )
				hPopup = ::CreatePopupMenu();

			ImageHelper::UTF8ToUnicode(&sData, wszItemText, 100);
			UINT_PTR nIDNew = (UINT_PTR)atoi(sId.GetBuffer());
			if( nIDNew > 0 )
				::AppendMenuW(hPopup, MF_STRING, nIDNew, wszItemText);
			else // Separator.
				::AppendMenuW(hPopup, MF_SEPARATOR, 0L, NULL);


			if( pItemData ){
				sData = pXml->GetAttrib(_T("data"));
				if( !sData.IsEmpty() )
					pItemData->Add(sId, sData);
				}

			nIndex ++;
			continue;
			}
		}
	return hPopup;
	}

bool 
MenuHelper::LoadMenuFromXml(const CString sMenuXmlFile, CMenu& menu, StringToStringArray* pItemData){
	BYTE*	lpXmlContent	= NULL;
	int		nSize			= 0;
	if( !Path::ReadFileContent(sMenuXmlFile, lpXmlContent, nSize) )
		return false;

	int nOffset = 0;
	// Is UTF8 signature.
	if( lpXmlContent[0] == 0xEF && lpXmlContent[1] == 0xBB && lpXmlContent[2] == 0xBF )
		nOffset = 3;

	BYTE* lpXmlDoc = new BYTE[nSize - nOffset + 1];
	lpXmlDoc[nSize - nOffset] = '\0';
	memcpy(lpXmlDoc, &lpXmlContent[nOffset], nSize - nOffset);
	delete [] lpXmlContent;

	CMarkup xml;
	if( !xml.SetDoc((LPCTSTR)lpXmlDoc) ){
		delete [] lpXmlDoc;
		return false;
		}

	delete [] lpXmlDoc;
	if( !xml.FindElem(_T("menu")) || !xml.IntoElem() )
		return false;

	if( !menu.CreateMenu() )
		return false;

	// Submenu loop.
	wchar_t wszItemText[100];
	while( xml.FindElem(_T("submenu")) ){
		CString sSubMenuTitle = xml.GetAttrib(_T("title"));
		if( !xml.IntoElem() ) break;
		HMENU hSubMenu = CreatePopupMenuFromXml(&xml, pItemData);
		if( hSubMenu ){
			ImageHelper::UTF8ToUnicode(&sSubMenuTitle, wszItemText, 100);
			::AppendMenuW(menu.m_hMenu, MF_STRING|MF_POPUP, (UINT_PTR)hSubMenu, wszItemText);
			}

		xml.OutOfElem();
		}
	return true;
	}

bool
MenuHelper::CopyItemStrings(HMENU hMenuFrom, HMENU hMenuTo){
	if( !hMenuFrom || !hMenuTo )
		return false;

	wchar_t wszText[100];
	MENUITEMINFOW miinfo;
	memset(&miinfo, 0, sizeof(MENUITEMINFOW));
	miinfo.cbSize		= sizeof(MENUITEMINFOW);
	miinfo.fMask		= MIIM_SUBMENU|MIIM_STRING|MIIM_ID;
	miinfo.cch			= 99;
	miinfo.dwTypeData	= wszText;

	MENUITEMINFOW miinfoTo;
	memset(&miinfoTo, 0, sizeof(MENUITEMINFOW));
	miinfoTo.cbSize		= sizeof(MENUITEMINFOW);
	miinfoTo.fMask		= MIIM_STRING;
	miinfoTo.cch		= 99;
	miinfoTo.dwTypeData	= wszText;

	int nItemCt = ::GetMenuItemCount(hMenuFrom);
	if( nItemCt != ::GetMenuItemCount(hMenuTo) )
		return false;

	for(int i=0; i<nItemCt; i++){
		miinfo.cch				= 99;
		miinfo.dwTypeData		= wszText;
		wszText[0]				= 0;
		wszText[1]				= 0;
		if( !::GetMenuItemInfoW(hMenuFrom, i, TRUE, &miinfo) )
			continue;

		if( miinfo.hSubMenu ){
			miinfoTo.fMask = MIIM_SUBMENU;
			::GetMenuItemInfoW(hMenuTo, i, TRUE, &miinfoTo);
			miinfoTo.fMask = MIIM_STRING;

			if( miinfoTo.hSubMenu )
				CopyItemStrings(miinfo.hSubMenu, miinfoTo.hSubMenu);
			}
		else{
			if( !miinfo.wID )
				continue; // Separator
			}

		miinfoTo.cch			= miinfo.cch;
		miinfoTo.dwTypeData		= wszText;
		::SetMenuItemInfoW(hMenuTo, i, TRUE, &miinfoTo);
		}
	return true;
	}