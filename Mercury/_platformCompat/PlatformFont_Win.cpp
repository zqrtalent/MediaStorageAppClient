#include "PlatformFont_Win.h"
#include "PlatformDeviceContext.h"
#include "../Utility/StringHelper.h"
//#include "../EmpbedeedFont/EmbedeedFontMan.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_Font::_Font() : font_(NULL) {
	}

_Font::~_Font() {
	DeleteObject();
	}

BOOL
_Font::Attach(FONTDef font){
	if( !IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}
	font_ = font;
	return TRUE;
	}

FONTDef
_Font::Detach(){
	if( IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}
	FONTDef ret = font_;
	font_ = NULL;
	return ret;
	}

BOOL
_Font::DeleteObject(){
	if( !IsNull() ){
		::DeleteObject(font_);
		font_ = NULL;
		}
	return TRUE;
	}

BOOL
_Font::CopyFont(_Font* pCopyInto){
	if( IsNull() || !pCopyInto )
		return FALSE;
	LOGFONT lf;
	GetLogFont(&lf);
	return pCopyInto->CreateFontIndirect(&lf);
	}

BOOL
_Font::CreateFont(const TCHAR* pszFontName, int nHeight){
	if( !IsNull() )
		return FALSE;

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight			= nHeight;
	lf.lfWeight			= FW_REGULAR;
	lf.lfItalic			= 0;
	lf.lfUnderline		= 0;
	lf.lfStrikeOut		= 0;
	lf.lfCharSet		= ANSI_CHARSET;
	lf.lfQuality		= PROOF_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy_s(lf.lfFaceName, 32, pszFontName);
	return CreateFontIndirect(&lf);
	}

int
_Font::GetLogFont(LOGFONT* pLogFont){
	if( IsNull() || !pLogFont )
		return 0;
	return GetObject(font_, sizeof(LOGFONT), pLogFont);
	}

BOOL
_Font::CreateFontIndirect(LOGFONT* pLogFont){
	if( !IsNull() || !pLogFont )
		return FALSE;
	font_ = ::CreateFontIndirect(pLogFont);
	return !IsNull();
	}

BOOL
_Font::GetTextSize(std::string* pStr, _Size& szText){
	if( IsNull() )
		return FALSE;

	_DC memDC;
	memDC.CreateCompatibleDC(NULL);
	memDC.SelectObject		(font_);

	LOGFONT lf;
	GetLogFont(&lf);

	wchar_t	wszTemp[255];
	int		nLen = StringHelper::UTF8ToUnicode(pStr->c_str(), wszTemp, 254);
	memDC.GetTextExtentPoint32W(wszTemp, nLen, szText);
	/*
	EmbedeedFontMan* pEmbedeedFontMan = EmbedeedFontMan::GetInstance();
	int nFontId = pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	if( nFontId == -1 )
		memDC.GetTextExtentPoint32W(wszTemp, nLen, szText);
	else
		pEmbedeedFontMan->GetTextSize(&memDC, wszTemp, nLen, nFontId, szText);
		*/
	memDC.DeleteDC();
	return TRUE;
	}

int
_Font::GetSymbolIndexByXOffset(std::string* pStr, int nXTextStart, int nXSymbolAt){
	if( IsNull() || !pStr || !pStr->length() )
		return -1;
	return 0;
	/*
	_DC memDC;
	memDC.CreateCompatibleDC(NULL);
	memDC.SelectObject(*this);

	LOGFONT lf;
	GetLogFont(&lf);

	wchar_t	wszTemp[255];
	int		nLen = StringHelper::UTF8ToUnicode(pStr, wszTemp, 254);

	EmbedeedFontMan*	pEmbedeedFontMan = EmbedeedFontMan::GetInstance();
	int					nFontId = pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	Size				szText(0, 0);
	wchar_t				wszSymbol[2];
	int					nRet = -1;

	for(int i=0; i<nLen; i++){
		wszSymbol[0] = wszTemp[i];
		wszSymbol[1] = 0;

		if( nFontId == -1 )
			memDC.GetTextExtentPoint32W(wszSymbol, 1, szText);
		else{
			if( pEmbedeedFontMan->GetTextSize(&memDC, wszSymbol, 1, nFontId, szText) )
				szText.cx += EmbedeedFontMan::GetDistanceBetweenSymbols();
			}
		nXTextStart += szText.cx;

		if( nXSymbolAt <= nXTextStart ){
			if( nXSymbolAt <= (nXTextStart - szText.cx/2) )
				nRet = i;
			else
				nRet = i + 1;
			break;
			}
		}

	if( nXSymbolAt > nXTextStart )
		nRet = (nLen - 1);

	memDC.DeleteDC();
	return nRet;*/
	}

int
_Font::GetSymbolWidthArray(std::string* pStr, CDWordArray& arrSymbolWidth){
	if( IsNull() || !pStr || !pStr->length() )
		return 0;
	/*
	_DC memDC;
	memDC.CreateCompatibleDC(NULL);
	memDC.SelectObject		(*this);

	LOGFONT lf;
	GetLogFont(&lf);

	wchar_t	wszTemp[255];
	int		nLen = StringHelper::UTF8ToUnicode(pStr, wszTemp, 254);

	EmbedeedFontMan*	pEmbedeedFontMan	= EmbedeedFontMan::GetInstance();
	int					nFontId				= pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	Size				szText(0, 0);
	wchar_t				wszSymbol[2];
	int					nRet = 0;

	for(int i=0; i<nLen; i++){
		wszSymbol[0] = wszTemp[i];
		wszSymbol[1] = 0;
		if( nFontId == -1 )
			memDC.GetTextExtentPoint32W(wszSymbol, 1, szText);
		else{
			if( pEmbedeedFontMan->GetTextSize(&memDC, wszSymbol, 1, nFontId, szText) )
				szText.cx += EmbedeedFontMan::GetDistanceBetweenSymbols();
			else
				continue;
			}
		arrSymbolWidth.push_back(szText.cx);
		nRet ++;
		}
	memDC.DeleteDC();
	return nRet;*/
	return 0;
	}

BOOL
_Font::StretchedTextFont(_Font* pFontOut, float fStretchCX, float fStretchCY, bool bUnderLine, bool bBold){
	if( IsNull() || !pFontOut )
		return FALSE;

	LOGFONT lf;
	if( !GetLogFont(&lf) )
		return FALSE;
/*
	if( fStretchCX < 0.8f )
		fStretchCX = 0.08f;
*/
	float fMin = 0.5f, fMax = 1.8f;
	if( fStretchCY < fMin )
		fStretchCY = fMin;

	if( fStretchCY > fMax )
		fStretchCY = fMax;

	if( fStretchCX > fMax )
		fStretchCX = fMax;

	lf.lfWidth	= (lf.lfWidth * fStretchCX);
	lf.lfHeight = (lf.lfHeight * fStretchCY);

	if( bUnderLine )
		lf.lfUnderline = 1;
//	else
//		lf.lfUnderline = 0;

	if( bBold )
		lf.lfWeight = FW_BOLD;
	return pFontOut->CreateFontIndirect(&lf);
	}

bool
_Font::IsEmbedeedFontAvailable(){
	if( IsNull() )
		return false;
	/*
	LOGFONT lf;
	GetLogFont(&lf);
	EmbedeedFontMan* pEmbedeedFontMan = EmbedeedFontMan::GetInstance();
	int nFontId = pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	return (nFontId != -1);*/
	return false;
	}