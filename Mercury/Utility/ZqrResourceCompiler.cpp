#include "stdafx.h"
#include "ZqrResourceCompiler.h"
#include "..\String\StringDecoder.h"

BOOL m_bDefineAsString = FALSE;

ZqrResourceCompiler::ZqrResourceCompiler()
	{
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("PUSHBUTTON")),	((void*)&Parse_DlgItem_PUSHBUTTON));
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("EDITTEXT")),		((void*)&Parse_DlgItem_EDITTEXT)	);
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("COMBOBOX")),		((void*)&Parse_DlgItem_COMBOBOX)	);
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("LISTBOX")),		((void*)&Parse_DlgItem_LISTBOX)	);
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("GROUPBOX")),		((void*)&Parse_DlgItem_GROUPBOX)	);
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("LTEXT")),		((void*)&Parse_DlgItem_LTEXT)	);
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("SCROLLBAR")),	((void*)&Parse_DlgItem_SCROLLBAR));
	m_arrDlgItemType_Func.Add((void*)DecodeString(_T("CONTROL")),		((void*)&Parse_DlgItem_CONTROL)	);

	// Add defines from global struct. ##################
	int nCount	= sizeof(g_defines) / sizeof(DefineStruct);
	int nLoop	= 0;

	while( nLoop < nCount ){
		AddStyleDefine(g_defines[nLoop].sDefineName, g_defines[nLoop].nDefineValue);
		nLoop ++;
		}
	// ##################################################

	m_nDefineValue		= 1000;
	m_bDefineAsString	= FALSE;
	}

ZqrResourceCompiler::~ZqrResourceCompiler()
	{
	}

ZqrResourceCompiler*
ZqrResourceCompiler::GetCompiler()
	{
	static ZqrResourceCompiler compiler;
	return &compiler;
	}

INT_PTR CALLBACK AfxDlgProc_My(HWND hWnd, UINT message, WPARAM, LPARAM)
	{
 	if( message == WM_INITDIALOG )
 		{
 		// special case for WM_INITDIALOG
 		CDialog* pDlg = DYNAMIC_DOWNCAST(CDialog, CWnd::FromHandlePermanent(hWnd));
 		if( pDlg != NULL )
 			return pDlg->OnInitDialog();
 		else
 			return 1;
 		}
 	return 0;
	}

BOOL 
ZqrResourceCompiler::ReadDefinesFromFile(CString strHdrFile, AutoSortedArray& arrDefines)
	{/*
	CString strContent;
	BOOL	bRet = FALSE;
	try
		{
		// open header file. =============================
		CFile file_res_hdr(strHdrFile, CFile::modeRead);
		UINT nFSize = (UINT)file_res_hdr.GetLength();
		if( nFSize <= 0 )
			{
			file_res_hdr.Close();
			m_strLastError = _T("[Error] empty resource header file !!!");
			return FALSE;
			}
		// ===============================================

		// read content. =================================
		strContent.GetBufferSetLength(nFSize);
		file_res_hdr.Read(strContent.GetBuffer(), nFSize);
		file_res_hdr.Close();
		// ===============================================

		bRet = ReadDefines(strContent, arrDefines);
		}
	catch(CFileException* e)
		{
		m_strLastError = _T("[Error] read header file !!!");
		return FALSE;
		}

	return bRet;*/
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::CompileDialogScriptFile(CString strDlgScriptFile, void*& lpBuffer, UINT& nBuffSize)
	{
	CString strDlgScript;
	BOOL	bRet = FALSE;
	try
		{
		// open header file. =============================
		CFile file_dlgscript(strDlgScriptFile, CFile::modeRead);
		UINT nFSize = file_dlgscript.GetLength();
		if( nFSize <= 0 )
			{
			file_dlgscript.Close();
			m_strLastError = _T("[Error] empty dialog script file !!!");
			return FALSE;
			}
		// ===============================================
		// read content. =================================
		strDlgScript.GetBufferSetLength(nFSize);
		file_dlgscript.Read(strDlgScript.GetBuffer(), nFSize);
		file_dlgscript.Close();
		// ===============================================

		bRet = CompileDialogScript(strDlgScript, lpBuffer, nBuffSize);
		}
	catch(CFileException* e)
		{
		e->Delete();
		m_strLastError = _T("[Error] read dialog script file !!!");
		return FALSE;
		}

	return bRet;
	}

BOOL 
ZqrResourceCompiler::CompileDialogScript(CString strDlgScript, void*& lpBuffer, UINT& nBuffSize)
	{
	LPTSTR		lpszContent	= strDlgScript.GetBuffer();
	DialogData	dlgData;
	CString		strWord;
	CDWordArray	arrBuffPtrs;
	CDWordArray	arrBuffSizes;

	if( ReadDialogScriptHeader(&dlgData, lpszContent) == FALSE )
		goto error;

	if( GetNextWord(lpszContent, strWord) == FALSE || strWord != _T("BEGIN") )
		goto error;

	// Compile each item of dialog template. ======================
	typedef BOOL (*Parse_DlgItem)	(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError);
	Parse_DlgItem pFunc = NULL;

	while( TRUE )
		{
		if( !GetNextWord(lpszContent, strWord) && strWord != _T("END") )
			goto error;

		if( strWord == _T("END") )
			break;

		void*	lpItemBuff		= NULL;
		UINT	nItemBuffSize	= 0L;
		UINT	nCode			= ::DecodeString(strWord.GetBuffer());
		int		nFind			= m_arrDlgItemType_Func.FindValue((void*)nCode);
		if( nFind == -1 )
			goto error;

		pFunc					= (Parse_DlgItem)m_arrDlgItemType_Func.GetData(nFind);
		if( pFunc )
			{
			if( (*pFunc)(this, lpszContent, lpItemBuff, nItemBuffSize, m_strLastError) == TRUE )
				{
				arrBuffPtrs	.Add((DWORD)lpItemBuff);
				arrBuffSizes.Add((DWORD)nItemBuffSize);
				dlgData.nItemCount ++;
				}
			else
				goto error;
			}
		else
			goto error;
		}
	// ==============================================================

	// build dialog template structure. =============================
	if( BuildDlgTemplate(&dlgData, lpBuffer, nBuffSize) == FALSE )
		goto error;
	else
		{
		arrBuffPtrs	.InsertAt(0, (DWORD)lpBuffer);
		arrBuffSizes.InsertAt(0, (DWORD)nBuffSize);
		}
	// ==============================================================

	// Calculate memory buffer size for Dialog temlate with each item.
	nBuffSize = 0L;
	for( int iLoop=0; iLoop<arrBuffSizes.GetSize(); iLoop++ )
		nBuffSize += arrBuffSizes[iLoop];
	// ===============================================================

	ASSERT( nBuffSize );
	BYTE* lpBuff	= new BYTE[nBuffSize];
	lpBuffer		= lpBuff;

	// Copy all templates by order. ==================================
	UINT nOffset = 0L;
	for( int iLoop=0; iLoop<arrBuffSizes.GetSize(); iLoop++ )
		{
		ASSERT( arrBuffPtrs[iLoop] && arrBuffSizes[iLoop] );
		memcpy(&lpBuff[nOffset], (void*)arrBuffPtrs[iLoop], arrBuffSizes[iLoop]);
		nOffset += arrBuffSizes[iLoop];
		}
	// ===============================================================

/*
	HWND hWnd = ::CreateDialogIndirect(AfxGetInstanceHandle(), (LPCDLGTEMPLATE)lpBuffer, GetDesktopWindow(), AfxDlgProc_My);
	if( IsWindow(hWnd) )
		::ShowWindow(hWnd, SW_SHOW);*/

	return TRUE;
error:

	for( int iLoop=0; iLoop<arrBuffPtrs.GetSize(); iLoop++ )
		{
		if( arrBuffPtrs[iLoop] )
			delete ((BYTE*)arrBuffPtrs[iLoop]);
		}

	return FALSE;
	}

BOOL		
ZqrResourceCompiler::ReadDialogScriptHeader(DialogData* pDlgData, LPTSTR& lpszScript)
	{
	CString strWord;
	int		nFind			= -1;
	LPTSTR	lpszScriptOld	= NULL;
	UINT	nDefValue		= 0L;
	long	lNumValue		= 0;
	CRect	rcValue;
	LOGFONT	lfValue;
	ZeroMemory(&lfValue, sizeof(LOGFONT));

	// Read dialog identifier. ==========================
	m_bDefineAsString = TRUE;
	if( GetDefineValue(lpszScript, nDefValue, m_arrDefineName_Value) )
		pDlgData->nID = nDefValue;
	else
		{
		m_strLastError = _T("[Error] Dialog Identifier not defined !!!");
		return FALSE;
		}
	m_bDefineAsString = FALSE;
	// ===================================================
	// Read dialog version. ==============================
	if( GetNextWord(lpszScript, strWord) && strWord == _T("DIALOGEX") )
		{
		pDlgData->nVer = 1L;
		}
	else
		{
		m_strLastError = _T("[Error] Dialog type not defined, it must be DIALOGEX !!!");
		return FALSE;
		}
	// ===================================================

	// Read dialog position. =============================
	if( GetRectValue(lpszScript, rcValue) )
		{
		pDlgData->ptDlg.x	= rcValue.left;
		pDlgData->ptDlg.y	= rcValue.top;
		pDlgData->szDlg.cx	= rcValue.right;
		pDlgData->szDlg.cy	= rcValue.bottom;
		}
	else
		return FALSE;
	// ===================================================

	// Read dialog style. ================================
	if( !GetNextWord(lpszScript, strWord) || strWord != _T("STYLE") )
		{
		m_strLastError = _T("[Error] STYLE keyword is missing !!!");
		return FALSE;
		}

	if( GetDefineValue(lpszScript, nDefValue, m_arrStyleName_Value) )
		pDlgData->nStyle = nDefValue;
	else
		return FALSE;
	// ===================================================

	// Read dialog extended style. =======================
	lpszScriptOld = lpszScript;
	if( GetNextWord(lpszScript, strWord) && strWord == _T("EXSTYLE") )
		{
		if( GetDefineValue(lpszScript, nDefValue, m_arrStyleName_Value) )
			pDlgData->nStyleEx = nDefValue;
		else
			return FALSE;
		}
	else
		{
		pDlgData->nStyleEx	= 0L;
		lpszScript			= lpszScriptOld;
		}
	// ===================================================

	// Read dialog caption text. =========================
	lpszScriptOld = lpszScript;
	if( GetNextWord(lpszScript, strWord) && strWord == _T("CAPTION") )
		{
		if( !GetStringValue(lpszScript, pDlgData->strCaption) )
			{
			m_strLastError = _T("[Error] Dialog caption text format !!!");
			return FALSE;
			}
		}
	else
		{
		pDlgData->strCaption	= _T("");
		lpszScript				= lpszScriptOld;
		}
	// ===================================================

	// Read dialog menu if exists. =======================
	lpszScriptOld = lpszScript;
	if( GetNextWord(lpszScript, strWord) && strWord == _T("MENU") )
		{
		if( GetDefineValue(lpszScript, nDefValue, m_arrDefineName_Value) )
			pDlgData->nMenuId = nDefValue;
		else
			return FALSE;
		}
	else
		{
		pDlgData->nMenuId	= 0L;
		lpszScript			= lpszScriptOld;
		}
	// ===================================================

	// Read dialog font. =================================
	if( !GetNextWord(lpszScript, strWord) || strWord != _T("FONT") )
		{
		m_strLastError = _T("[Error] FONT keyword isn't defined !!!");
		return FALSE;
		}
	// ===================================================

	if( GetLogFontValue(lpszScript, lfValue) )
		{
		pDlgData->bItalic		= lfValue.lfItalic;
		pDlgData->btCharset		= lfValue.lfCharSet;
		pDlgData->strFaceName	= lfValue.lfFaceName;
		pDlgData->nFontSize		= lfValue.lfHeight;
		pDlgData->nWeight		= lfValue.lfWeight;
		}
	else
		return FALSE;

	pDlgData->nItemCount = 0L;
	return TRUE;
	}

BOOL
ZqrResourceCompiler::GetLogFontValue(LPTSTR& lpszScript, LOGFONT& lfValue)
	{
	CString strValue;
	// Read font size. ===================================
	if( !GetNumericValue(lpszScript, lfValue.lfHeight) || lfValue.lfHeight <= 0 )
		{
		m_strLastError = _T("[Error] Font height is not valid !!!");
		return FALSE;
		}
	// ====================================================
	// Read font face name. ===============================
	if( GetNextWord(lpszScript, strValue) && strValue == _T(",") )
		{
		CString strFaceName;
		if( !GetStringValue(lpszScript, strFaceName) )
			{
			m_strLastError = _T("[Error] Font face name isn't valid !!!");
			return FALSE;
			}
		strcpy(lfValue.lfFaceName, strFaceName);
		}
	else
		{
		m_strLastError = _T("[Error] Script error "","" is missing !!!");
		return FALSE;
		}
	// ===================================================
	// Read font weight. =================================
	if( GetNextWord(lpszScript, strValue) && strValue == _T(",") )
		{
		if( !GetNumericValue(lpszScript, lfValue.lfWeight) )
			{
			m_strLastError = _T("[Error] Font weight isn't valid !!!");
			return FALSE;
			}
		}
	else
		{
		m_strLastError = _T("[Error] Script error "","" is missing !!!");
		return FALSE;
		}
	// ===================================================
	// Read font italic. =================================
	if( GetNextWord(lpszScript, strValue) && strValue == _T(",") )
		{
		if( !GetNumericValue(lpszScript, (long&)lfValue.lfItalic) )
			{
			m_strLastError = _T("[Error] Font italic isn't valid !!!");
			return FALSE;
			}
		}
	else
		{
		m_strLastError = _T("[Error] Script error "","" is missing !!!");
		return FALSE;
		}
	// ===================================================
	// Read font charset. ================================
	if( GetNextWord(lpszScript, strValue) && strValue == _T(",") )
		{
		if( !GetNumericValue(lpszScript, (long&)lfValue.lfCharSet) )
			{
			m_strLastError = _T("[Error] Font charset isn't valid !!!");
			return FALSE;
			}
		}
	else
		{
		m_strLastError = _T("[Error] Script error "","" is missing !!!");
		return FALSE;
		}
	// ===================================================
	return TRUE;
	}

BOOL
ZqrResourceCompiler::GetRectValue(LPTSTR& lpszScript, CRect& rcValue)
	{
	CString strValue;
	CPoint	ptLeftTop, ptRightBottom;
	if( GetPointValue(lpszScript, ptLeftTop)	&& 
		GetNextWord(lpszScript, strValue)		&& strValue == _T(",") &&
		GetPointValue(lpszScript, ptRightBottom) )
		{
		rcValue.SetRect(ptLeftTop.x, ptLeftTop.y, ptRightBottom.x, ptRightBottom.y);
		return TRUE;
		}

	if( strValue != _T(",") )
		m_strLastError = _T("[Error] , is missing !!!");
	return FALSE;
	}

BOOL		
ZqrResourceCompiler::GetPointValue(LPTSTR& lpszScript, CPoint& ptValue)
	{
	CString strValue;
	long	lNumValue = 0;
	if( GetNumericValue(lpszScript, lNumValue) )
		ptValue.x = lNumValue;
	else
		{
		m_strLastError = _T("[Error] x position is not defined !!!");
		return FALSE;
		}

	if( GetNextWord(lpszScript, strValue) && strValue == _T(",") &&
		GetNumericValue(lpszScript, lNumValue) )
		ptValue.y = lNumValue;
	else
		{
		m_strLastError = _T("[Error] y position is not defined !!!");
		return FALSE;
		}
	return TRUE;
	}

BOOL		
ZqrResourceCompiler::GetDefineValue(LPTSTR& lpszScript, UINT& nDefineValue, AutoSortedArray& arrDefines)
	{
	// Retrieve define's as string. ======
	CString strDefineString;
	if( !GetNextWord(lpszScript, strDefineString) )
		return FALSE;

	CString strWord;
	LPTSTR	lpszStart = lpszScript;

	while( GetNextWord(lpszScript, strWord) )
		{
		if( strWord == "|" )
			{
			strDefineString += strWord;
			if( !GetNextWord(lpszScript, strWord) )
				return FALSE;
			strDefineString += strWord;
			lpszStart		= lpszScript;
			}
		else
			{
			lpszScript = lpszStart;
			break;
			}
		}
	// ==================================

	return DefineStringToValue(strDefineString, nDefineValue, arrDefines);
	}

BOOL
ZqrResourceCompiler::GetNumericValue(LPTSTR& lpszScript, long& nNumericValue)
	{
	// retrieve next word. ================
	CString strWord;
	if( !GetNextWord(lpszScript, strWord) )
		return FALSE;
	// ====================================

	// Validate numeric string. ===========
	if( strWord.GetLength() <= 0 || !(strWord[0] >= '0' && strWord[0] <= '9') )
		return FALSE;
	// ====================================

	// Convert string to numeric value. ===
	if( strWord.Left(2) == _T("0x") )
		{
		LPTSTR lpszEnd	= NULL;
		nNumericValue	= (UINT)_strtoi64(strWord, &lpszEnd, 16);
		}
	else
		nNumericValue = atoi(strWord);
	// ====================================
	return TRUE;
	}

BOOL
ZqrResourceCompiler::GetStringValue(LPTSTR& lpszScript, CString& sStringValue)
	{
	if( !GetNextWord(lpszScript, sStringValue) || sStringValue.GetLength() < 2 )
		return FALSE;

	lpszScript = (LPTSTR)(((UINT)lpszScript) - sStringValue.GetLength());
	if( lpszScript[0] == '"' && lpszScript[sStringValue.GetLength()-1] != '"' )
		{
		int iLoop = 1;
		while( lpszScript[iLoop]  != '\0' )
			{
			if( lpszScript[iLoop] == '"' )
				{
				char chLast			= lpszScript[iLoop];
				lpszScript[iLoop]	= '\0';
				sStringValue		= &lpszScript[1];
				lpszScript[iLoop]	= chLast;
				lpszScript			= &lpszScript[iLoop+1];
				return TRUE;
				}
			iLoop ++;
			}
		return FALSE;
		}

	lpszScript	= &lpszScript[sStringValue.GetLength()];
	sStringValue.Remove('"');
	return TRUE;
	}

BOOL 
ZqrResourceCompiler::GetNextWord(LPTSTR& lpszScript, CString& strScriptWord)
	{
	// determine start position of word. ========================
	LPTSTR lpszWordStart = lpszScript;
	while( TRUE )
		{
		switch( lpszWordStart[0] )
			{
			case ' ':
				lpszWordStart ++;
				continue;
			case '\n':
				lpszWordStart ++;
				continue;
			case '\r':
				lpszWordStart ++;
				continue;/*
			case '\d':
				lpszWordStart ++;
				continue;*/
			case '\t':
				lpszWordStart ++;
				continue;
			case '\0':
				strScriptWord = _T("");
				return FALSE;
			};
		break;
		}
	// =========================================================

	// determine end position of word. =========================
	LPTSTR lpszWordEnd = lpszWordStart;
	if( lpszWordEnd[0] == ',' )
		{
		lpszWordEnd ++;
		}
	else
		while( TRUE )
			{
			if( lpszWordEnd[0] == ' ' )
				break;
			if( lpszWordEnd[0] == '\n' )
				break;
			if( lpszWordEnd[0] == '\r' )
				break;
			if( lpszWordEnd[0] == '\t' )
				break;
			if( lpszWordEnd[0] == ',' )
				break;
			if( lpszWordEnd[0] == '\0' ) 
				{
				strScriptWord = lpszWordStart;
				return FALSE;
				}
			lpszWordEnd ++;
			}
	// =========================================================

	char chCode		= lpszWordEnd[0];
	lpszWordEnd[0]	= '\0';
	strScriptWord	= lpszWordStart;
	lpszWordEnd[0]	= chCode;
	lpszScript		= lpszWordEnd;
	return TRUE;
	}

BOOL		
ZqrResourceCompiler::DefineStringToValue(CString& strDefineString, UINT& nDefineValue, AutoSortedArray& arrDefines)
	{
	// Convert define to value. =========
	strDefineString += "|";
	int		nFindOrSymbol	= strDefineString.Find('|');
	int		nFindOrSymbolOld= -1;
	nDefineValue			= 0L;
	do
		{
		// retrieve defines value. ======================
		CString strDefine = strDefineString.Mid(nFindOrSymbolOld + 1, nFindOrSymbol - nFindOrSymbolOld - 1);
		strDefine.Remove('\t');
		strDefine.Remove(' ');
		UINT nCode = DecodeString(strDefine.GetBuffer());
		ASSERT( nCode );
		// ==============================================

		if( strDefine[0] >= '0' && strDefine[0] <= '9' )
			{
			if( strDefine.GetLength() > 1 && strDefine[0] == '0' && strDefine[1] == 'x' )
				{
				LPTSTR lpszEnd	= NULL;
				nDefineValue	|= (UINT)_strtoi64(strDefine, &lpszEnd, 16);
				}
			else
				nDefineValue	|= atoi(strDefine);
			}
		else
			{
			int	 nFindValue = arrDefines.FindValue((void*)nCode);
			if( nFindValue != -1 )
				nDefineValue |= (int)arrDefines.GetData(nFindValue);
			else
				{
				if( !m_bDefineAsString )
					{
					TRACE(__FUNCTION__);
					TRACE1("%s undefined", strDefine);
					}
				else
					{
					arrDefines.AddAsLong(nCode, m_nDefineValue);
					nDefineValue = m_nDefineValue;
					m_nDefineValue ++;
					return TRUE;
					}
				}
			}

		nFindOrSymbolOld	= nFindOrSymbol;
		nFindOrSymbol		= strDefineString.Find('|', nFindOrSymbol + 1);
		}
	while( nFindOrSymbol != -1 );
	// ==================================
	return TRUE;
	}

void
ZqrResourceCompiler::AddStyleDefine(CString sDefName, UINT nValue)
	{
	sDefName.MakeUpper();
	// add define into array. ======================
	UINT nCode	= DecodeString((LPTSTR)sDefName.GetBuffer());
	if( m_arrStyleName_Value.FindValue((void*)nCode) != -1 )
		return;
	m_arrStyleName_Value.Add((void*)nCode, (void*)nValue);
	// ==============================================
	}

BOOL 
ZqrResourceCompiler::ReadDefines(CString& strHdrFileContent, AutoSortedArray& arrDefines)
	{
	// parse resource header file. ===================
	LPTSTR	lpszContent	= strHdrFileContent.GetBuffer();
	CString	strWord, strDefineName;

	BOOL	bDefineCmd	= FALSE;
	BOOL	bDefineName	= FALSE;
	BOOL	bDefineValue= FALSE;

	while( GetNextWord(lpszContent, strWord) )
		{
		if( !bDefineCmd )
			{
			if( strWord == _T("#define") )
				bDefineCmd = TRUE;
			}
		else
			{
			if( !bDefineName )
				{
				// save define value name. =====================
				bDefineName		= TRUE;
				strDefineName	= strWord;
				// =============================================
				}
			else 
			if( !bDefineValue )
				{
				// read define value. ==========================
				UINT nDefValue	= 0L;
				DefineStringToValue(strWord, nDefValue, arrDefines);
				// =============================================

				// add define into array. ======================
				UINT nCode	= DecodeString(strDefineName.GetBuffer());
			#ifdef _DEBUG
				if( arrDefines.FindValue((void*)nCode) != -1 ){
					TRACE("generated code by DecodeString already exists !!!");
					ASSERT( FALSE );
					}
			#endif
				arrDefines.AddAsLong(nCode, nDefValue);
				bDefineCmd = bDefineName = bDefineValue = FALSE;
				// ==============================================
				}
			}
		}
	// ===============================================
	return TRUE;
	}

BOOL
ZqrResourceCompiler::AddDefine(CString sDefineName, UINT nValue)
	{
	DWORD dwCode = DecodeString(sDefineName.GetBuffer());
	if( !dwCode || !nValue )
		return FALSE;
	m_arrDefineName_Value.AddAsLong(dwCode, nValue);
	return TRUE;
	}

BOOL 
ZqrResourceCompiler::BuildDlgTemplate(DialogData* pDlgData, void*& lpBuffer, UINT& nBuffSize)
	{
	nBuffSize		= sizeof(DLGTEMPLATEEX);
	BYTE* lpBuff	= new BYTE[nBuffSize];
	lpBuffer		= lpBuff;
	ZeroMemory(lpBuff, nBuffSize);

	((WORD*)&lpBuff[0])[0]	= (WORD)pDlgData->nVer; // dialog template version.
	lpBuff += sizeof(WORD);
	((WORD*)&lpBuff[0])[0]	= (WORD)-1; // for extended dialog template.
	lpBuff += sizeof(WORD);
	((DWORD*)&lpBuff[0])[0] = 0; // not supported.
	lpBuff += sizeof(DWORD);
	((DWORD*)&lpBuff[0])[0] = pDlgData->nStyleEx; // extended style.
	lpBuff += sizeof(DWORD);
	((DWORD*)&lpBuff[0])[0] = pDlgData->nStyle; // style.
	lpBuff += sizeof(DWORD);
	((WORD*)&lpBuff[0])[0]	= pDlgData->nItemCount;// item count.
	lpBuff += sizeof(WORD);
	((short*)&lpBuff[0])[0] = (short)pDlgData->ptDlg.x; // left position.
	lpBuff += sizeof(short);
	((short*)&lpBuff[0])[0] = (short)pDlgData->ptDlg.y; // top position.
	lpBuff += sizeof(short);
	((short*)&lpBuff[0])[0] = (short)pDlgData->szDlg.cx; // dialog width.
	lpBuff += sizeof(short);
	((short*)&lpBuff[0])[0] = (short)pDlgData->szDlg.cy; // dialog height.
	lpBuff += sizeof(short);
	((WORD*)&lpBuff[0])[0]	= pDlgData->nMenuId != 0 ? -1 : 0;// menu existance.
	lpBuff += sizeof(WORD);
	if( pDlgData->nMenuId != 0 )
		{
		((WORD*)&lpBuff[0])[0]	= pDlgData->nMenuId;// menu identifier.
		lpBuff += sizeof(WORD);
		}

	if( pDlgData->strWndClassName.IsEmpty() )
		{
		((WORD*)&lpBuff[0])[0]	= 0; // window class existance.
		lpBuff += sizeof(WORD);
		}
	else
		{
		((WORD*)&lpBuff[0])[0]	= -1; // window class existance.
		lpBuff += sizeof(WORD);
		MultiByteToWideChar(CP_ACP, 0L, pDlgData->strWndClassName.GetBuffer(), pDlgData->strWndClassName.GetLength(),
							((LPWSTR)&lpBuff[0]), pDlgData->strWndClassName.GetLength());
		lpBuff += (pDlgData->strWndClassName.GetLength())*sizeof(WORD);
		// set null point at the end. ===
		((WORD*)&lpBuff[0])[0]	= 0;
		lpBuff += sizeof(WORD);
		// ==============================
		}

	if( pDlgData->strCaption.IsEmpty() ) // window title.
		{
		((WORD*)&lpBuff[0])[0]	= 0;
		lpBuff += sizeof(WORD);
		}
	else
		{
		MultiByteToWideChar(CP_ACP, 0L, pDlgData->strCaption.GetBuffer(), pDlgData->strCaption.GetLength(),
							((LPWSTR)&lpBuff[0]), pDlgData->strCaption.GetLength());
		lpBuff += (pDlgData->strCaption.GetLength())*sizeof(WORD);
		// set null point at the end. ===
		((WORD*)&lpBuff[0])[0]	= 0;
		lpBuff += sizeof(WORD);
		// ==============================
		}

	((WORD*)&lpBuff[0])[0]	= (WORD)pDlgData->nFontSize;// font height.
	lpBuff += sizeof(WORD);
	((WORD*)&lpBuff[0])[0]	= (WORD)pDlgData->nWeight;// font weight.
	lpBuff += sizeof(WORD);
	((BYTE*)&lpBuff[0])[0]	= (BYTE)pDlgData->bItalic;// font italic.
	lpBuff += sizeof(BYTE);
	((BYTE*)&lpBuff[0])[0]	= (BYTE)pDlgData->btCharset;// font charset.
	lpBuff += sizeof(BYTE);

	if( pDlgData->strFaceName.IsEmpty() ) // font face name.
		{
		((WORD*)&lpBuff[0])[0]	= 0;
		lpBuff += sizeof(WORD);
		}
	else
		{
		MultiByteToWideChar(CP_ACP, 0L, pDlgData->strFaceName.GetBuffer(), pDlgData->strFaceName.GetLength(),
							((LPWSTR)&lpBuff[0]), pDlgData->strFaceName.GetLength());
		lpBuff += (pDlgData->strFaceName.GetLength())*sizeof(WORD);
		// set null point at the end. ===
		((WORD*)&lpBuff[0])[0]	= 0;
		lpBuff += sizeof(WORD);
		// ==============================
		}

	nBuffSize = lpBuff - ((BYTE*)lpBuffer);
	if( nBuffSize%sizeof(DWORD) )
		nBuffSize += nBuffSize%sizeof(DWORD);
	return TRUE;
	}

BOOL 
ZqrResourceCompiler::BuildItemTemplate(DlgItemData* pItemData, void*& lpBuffer, UINT& nBuffSize)
	{
	nBuffSize		= sizeof(DLGITEMTEMPLATEEX);
	BYTE* lpBuff	= new BYTE[nBuffSize];
	lpBuffer		= lpBuff;
	ZeroMemory(lpBuff, nBuffSize);

	((DWORD*)&lpBuff[0])[0]	= pItemData->helpID; // help Id. // not supported.
	lpBuff += sizeof(DWORD);
	((DWORD*)&lpBuff[0])[0]	= pItemData->nStyleEx; // Extended style.
	lpBuff += sizeof(DWORD);
	((DWORD*)&lpBuff[0])[0] = pItemData->nStyle; // Style.
	lpBuff += sizeof(DWORD);
	((short*)&lpBuff[0])[0] = (short)pItemData->ptItem.x; // left position.
	lpBuff += sizeof(short);
	((short*)&lpBuff[0])[0] = (short)pItemData->ptItem.y; // top position.
	lpBuff += sizeof(short);
	((short*)&lpBuff[0])[0] = (short)pItemData->szItem.cx; // width.
	lpBuff += sizeof(short);
	((short*)&lpBuff[0])[0] = (short)pItemData->szItem.cy; // height.
	lpBuff += sizeof(short);
	((DWORD*)&lpBuff[0])[0] = (DWORD)pItemData->nID; // Control Identifier.
	lpBuff += sizeof(DWORD);

	if( pItemData->strWndClass == _T("Button") )
		{
		((WORD*)&lpBuff[0])[0]	= -1; // window class existance.
		lpBuff += sizeof(WORD);
		((WORD*)&lpBuff[0])[0]	= 0x0080; // window class identifier.
		lpBuff += sizeof(WORD);
		}
	else
	if( pItemData->strWndClass == _T("Edit") )
		{
		((WORD*)&lpBuff[0])[0]	= -1; // window class existance.
		lpBuff += sizeof(WORD);
		((WORD*)&lpBuff[0])[0]	= 0x0081; // window class identifier.
		lpBuff += sizeof(WORD);
		}
	else
	if( pItemData->strWndClass == _T("Static") )
		{
		((WORD*)&lpBuff[0])[0]	= -1; // window class existance.
		lpBuff += sizeof(WORD);
		((WORD*)&lpBuff[0])[0]	= 0x0082; // window class identifier.
		lpBuff += sizeof(WORD);
		}
	else
	if( pItemData->strWndClass == _T("List box") )
		{
		((WORD*)&lpBuff[0])[0]	= -1; // window class existance.
		lpBuff += sizeof(WORD);
		((WORD*)&lpBuff[0])[0]	= 0x0083; // window class identifier.
		lpBuff += sizeof(WORD);
		}
	else
	if( pItemData->strWndClass == _T("Scroll bar") )
		{
		((WORD*)&lpBuff[0])[0]	= -1; // window class existance.
		lpBuff += sizeof(WORD);
		((WORD*)&lpBuff[0])[0]	= 0x0084; // window class identifier.
		lpBuff += sizeof(WORD);
		}
	else
	if( pItemData->strWndClass == _T("Combo box") )
		{
		((WORD*)&lpBuff[0])[0]	= -1; // window class existance.
		lpBuff += sizeof(WORD);
		((WORD*)&lpBuff[0])[0]	= 0x0085; // window class identifier.
		lpBuff += sizeof(WORD);
		}
	else
		{
		((WORD*)&lpBuff[0])[0]	= 0; // window class existance.
		MultiByteToWideChar(CP_ACP, 0L, pItemData->strWndClass.GetBuffer(), pItemData->strWndClass.GetLength(),
							((LPWSTR)&lpBuff[0]), pItemData->strWndClass.GetLength());
		lpBuff += (pItemData->strWndClass.GetLength())*sizeof(WORD);
		// set null point at the end. ===
		((WORD*)&lpBuff[0])[0]	= 0;
		lpBuff += sizeof(WORD);
		// ==============================
		}
	// Item Title. ================================
	if( !pItemData->strTitle.IsEmpty() )
		{
		MultiByteToWideChar(CP_ACP, 0L, pItemData->strTitle.GetBuffer(), pItemData->strTitle.GetLength(),
							((LPWSTR)&lpBuff[0]), pItemData->strTitle.GetLength());
		lpBuff += (pItemData->strTitle.GetLength())*sizeof(WORD);
		}
	// set null point at the end. ===
	((WORD*)&lpBuff[0])[0]	= 0;
	lpBuff += sizeof(WORD);
	// ==============================
	// =============================================

	((WORD*)&lpBuff[0])[0] = 0; // Extra byte count. // not supported.
	lpBuff += sizeof(WORD);
	
	nBuffSize = lpBuff - ((BYTE*)lpBuffer);
	if( nBuffSize%sizeof(DWORD) )
		nBuffSize += nBuffSize%sizeof(DWORD);
	return TRUE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_PUSHBUTTON(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	LPTSTR		lpszScriptOld;
	CString		strWord;
	DlgItemData itemData;
	CRect		rcValue;

	// Read item text. ===================================
	if( !pCompiler->GetStringValue(lpszScript, itemData.strTitle) )
		goto error;
	// ===================================================

	// Read item Identifier. =============================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		m_bDefineAsString = TRUE;
		if( !pCompiler->GetDefineValue(lpszScript, itemData.nID, pCompiler->m_arrDefineName_Value) )
			goto error;
		m_bDefineAsString = FALSE;
		}
	else
		goto error;
	// ===================================================
	// Read item position and size. ======================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		if( pCompiler->GetRectValue(lpszScript, rcValue) )
			{
			itemData.ptItem.x	= rcValue.left;
			itemData.ptItem.y	= rcValue.top;
			itemData.szItem.cx	= rcValue.right;
			itemData.szItem.cy	= rcValue.bottom;
			}
		else
			goto error;
		}
	else
		goto error;
	// ===================================================
	// Read style. =======================================
	lpszScriptOld		= lpszScript;
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		if( pCompiler->GetDefineValue(lpszScript, itemData.nStyle, pCompiler->m_arrStyleName_Value) )	
			{
			if( itemData.nStyle == 0L )
				{
				itemData.nStyle	= WS_VISIBLE|WS_CHILD|WS_TABSTOP;
				lpszScript		= lpszScriptOld;
				}
			else
				itemData.nStyle	|= WS_VISIBLE|WS_CHILD;
			}
		else
			{
			itemData.nStyle	= WS_VISIBLE|WS_CHILD;
			lpszScript		= lpszScriptOld;
			}
		}
	else
		{
		itemData.nStyle	= WS_VISIBLE|WS_CHILD;
		lpszScript		= lpszScriptOld;
		}
	// ===================================================
	// Read extended style. ==============================
	lpszScriptOld		= lpszScript;
	itemData.nStyleEx	= 0L;
	if( pCompiler->GetDefineValue(lpszScript, itemData.nStyleEx, pCompiler->m_arrStyleName_Value) )	
		{
		if( itemData.nStyleEx == 0L )
			lpszScript		= lpszScriptOld;
		}
	else
		lpszScript			= lpszScriptOld;
	// ===================================================*/

	itemData.strWndClass = _T("Button");
	itemData.nExtraCount = 0;
	itemData.helpID		 = 0;

	return ZqrResourceCompiler::BuildItemTemplate(&itemData, lpBuffer, nBuffSize);
error:
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_EDITTEXT(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	LPTSTR		lpszScriptOld;
	CString		strWord;
	DlgItemData itemData;
	CRect		rcValue;

	// Read item Identifier. =============================
	m_bDefineAsString = TRUE;
	if( !pCompiler->GetDefineValue(lpszScript, itemData.nID, pCompiler->m_arrDefineName_Value) )
		goto error;
	m_bDefineAsString = FALSE;
	// ===================================================
	// Read item position and size. ======================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		if( pCompiler->GetRectValue(lpszScript, rcValue) )
			{
			itemData.ptItem.x	= rcValue.left;
			itemData.ptItem.y	= rcValue.top;
			itemData.szItem.cx	= rcValue.right;
			itemData.szItem.cy	= rcValue.bottom;
			}
		else
			goto error;
		}
	else
		goto error;
	// ===================================================
	// Read style. =======================================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		lpszScriptOld = lpszScript;
		if( pCompiler->GetDefineValue(lpszScript, itemData.nStyle, pCompiler->m_arrStyleName_Value) )	
			{
			if( itemData.nStyle == 0L )
				{
				itemData.nStyle	= WS_VISIBLE|WS_CHILD;
				lpszScript		= lpszScriptOld;
				}
			else
				itemData.nStyle	|= WS_VISIBLE|WS_CHILD;
			}
		else
			{
			itemData.nStyle	= WS_VISIBLE|WS_CHILD;
			lpszScript		= lpszScriptOld;
			}
		}
	else
		goto error;
	// ===================================================
	// Read extended style. ==============================
	lpszScriptOld			= lpszScript;
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		lpszScriptOld		= lpszScript;
		itemData.nStyleEx	= 0L;
		if( pCompiler->GetDefineValue(lpszScript, itemData.nStyleEx, pCompiler->m_arrStyleName_Value) )	
			{
			if( itemData.nStyleEx == 0L )
				lpszScript		= lpszScriptOld;
			}
		else
			lpszScript			= lpszScriptOld;
		}
	else
		{
		lpszScript			= lpszScriptOld;
		itemData.nStyleEx	= 0L;
		}
//	else
//		goto error;
	// ===================================================*/

	itemData.strWndClass = _T("Edit");
	itemData.nExtraCount = 0;
	itemData.helpID		 = 0;

	return ZqrResourceCompiler::BuildItemTemplate(&itemData, lpBuffer, nBuffSize);
error:
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_COMBOBOX(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_LISTBOX(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_GROUPBOX(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_LTEXT(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	LPTSTR		lpszScriptOld;
	CString		strWord;
	DlgItemData itemData;
	CRect		rcValue;

	// Read item text. ===================================
	if( !pCompiler->GetStringValue(lpszScript, itemData.strTitle) )
		goto error;
	// ===================================================

	// Read item Identifier. =============================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		m_bDefineAsString = TRUE;
		if( !pCompiler->GetDefineValue(lpszScript, itemData.nID, pCompiler->m_arrDefineName_Value) )
			goto error;
		m_bDefineAsString = FALSE;
		}
	else
		goto error;
	// ===================================================
	// Read item position and size. ======================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		if( pCompiler->GetRectValue(lpszScript, rcValue) )
			{
			itemData.ptItem.x	= rcValue.left;
			itemData.ptItem.y	= rcValue.top;
			itemData.szItem.cx	= rcValue.right;
			itemData.szItem.cy	= rcValue.bottom;
			}
		else
			goto error;
		}
	else
		goto error;
	// ===================================================
	// Read style. =======================================
	lpszScriptOld = lpszScript;
	if( pCompiler->GetDefineValue(lpszScript, itemData.nStyle, pCompiler->m_arrStyleName_Value) )	
		{
		if( itemData.nStyle == 0L )
			{
			itemData.nStyle	= WS_VISIBLE|WS_CHILD|WS_TABSTOP;
			lpszScript		= lpszScriptOld;
			}
		else
			itemData.nStyle	|= WS_VISIBLE|WS_CHILD;
		}
	else
		{
		itemData.nStyle	= WS_VISIBLE|WS_CHILD;
		lpszScript		= lpszScriptOld;
		}
	// ===================================================
	// Read extended style. ==============================
	lpszScriptOld		= lpszScript;
	itemData.nStyleEx	= 0L;
	if( pCompiler->GetDefineValue(lpszScript, itemData.nStyleEx, pCompiler->m_arrStyleName_Value) )	
		{
		if( itemData.nStyleEx == 0L )
			lpszScript		= lpszScriptOld;
		}
	else
		lpszScript			= lpszScriptOld;
	// ===================================================*/

	itemData.strWndClass = _T("Static");
	itemData.nExtraCount = 0;
	itemData.helpID		 = 0;

	return ZqrResourceCompiler::BuildItemTemplate(&itemData, lpBuffer, nBuffSize);
error:
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_SCROLLBAR(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	return FALSE;
	}

BOOL 
ZqrResourceCompiler::Parse_DlgItem_CONTROL(ZqrResourceCompiler* pCompiler, LPTSTR& lpszScript, void*& lpBuffer, UINT& nBuffSize, CString& strError)
	{
	LPTSTR		lpszScriptOld;
	CString		strWord;
	DlgItemData itemData;
	CRect		rcValue;

	// Read item text. ===================================
	if( !pCompiler->GetStringValue(lpszScript, itemData.strTitle) )
		goto error;
	// ===================================================

	// Read item Identifier. =============================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		m_bDefineAsString = TRUE;
		if( !pCompiler->GetDefineValue(lpszScript, itemData.nID, pCompiler->m_arrDefineName_Value) )
			goto error;
		m_bDefineAsString = FALSE;
		}
	else
		goto error;
	// ===================================================

	// Read class name. ==================================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		if( !pCompiler->GetStringValue(lpszScript, itemData.strWndClass) )
			goto error;
		}
	else
		goto error;
	// ===================================================

	// Read style. =======================================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		lpszScriptOld = lpszScript;
		if( pCompiler->GetDefineValue(lpszScript, itemData.nStyle, pCompiler->m_arrStyleName_Value) )	
			{
			if( itemData.nStyle == 0L )
				{
				itemData.nStyle	= WS_VISIBLE|WS_CHILD|WS_TABSTOP;
				lpszScript		= lpszScriptOld;
				}
			else
				itemData.nStyle	|= WS_VISIBLE|WS_CHILD;
			}
		else
			{
			itemData.nStyle	= WS_VISIBLE|WS_CHILD;
			lpszScript		= lpszScriptOld;
			}
		}
	else
		goto error;
	// ===================================================

	// Read item position and size. ======================
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		if( pCompiler->GetRectValue(lpszScript, rcValue) )
			{
			itemData.ptItem.x	= rcValue.left;
			itemData.ptItem.y	= rcValue.top;
			itemData.szItem.cx	= rcValue.right;
			itemData.szItem.cy	= rcValue.bottom;
			}
		else
			goto error;
		}
	else
		goto error;
	// ===================================================

	// Read extended style. ==============================
	lpszScriptOld		= lpszScript;
	if( pCompiler->GetNextWord(lpszScript, strWord) && strWord == _T(",") )
		{
		lpszScriptOld		= lpszScript;
		itemData.nStyleEx	= 0L;
		if( pCompiler->GetDefineValue(lpszScript, itemData.nStyleEx, pCompiler->m_arrStyleName_Value) )	
			{
			if( itemData.nStyleEx == 0L )
				lpszScript		= lpszScriptOld;
			}
		else
			lpszScript			= lpszScriptOld;
		}
	else
		{
		lpszScript		= lpszScriptOld;
		itemData.nStyleEx = 0L;
		}
//		goto error;
	// ===================================================

	itemData.nExtraCount = 0;
	itemData.helpID		 = 0;

	return ZqrResourceCompiler::BuildItemTemplate(&itemData, lpBuffer, nBuffSize);
error:
	return FALSE;
	}
/*
UINT 
DecodeString(LPTSTR lpText)
	{
	DWORD dwCode = (DWORD)-1;
	DWORD dwSize = strlen(lpText);
	
	_asm
	{
		push  ebx;
		push  ecx;
		push  esi;
		push  edx;
		push  edi;
		mov   eax,dword ptr dwCode;
		lea   edi, dword ptr gDecoderData;
		mov   edx,dword ptr dwSize;
		mov   ecx,lpText;
m1:
		xor   ebx,ebx;
		inc   ecx;
		movzx esi,ax;
		shr   eax,8;
		mov   bl,byte ptr [ecx-1];
		xor   ebx,esi;
		and   ebx,0FFh;
		mov   esi,dword ptr [ebx*4+edi];
		xor   esi,eax;
		mov   eax,esi;
		mov   esi,edx;
		dec   edx;
		test  esi,esi;
		jne   m1;
		
		mov   dwCode,eax;
		pop   edx;
		pop   esi;
		pop   ecx;
		pop   ebx;
		pop   edi;
	};

	return dwCode;
	}*/