#include "PlatformFont_MacOSX.h"
#include "PlatformDeviceContext.h"
#include "../Utility/StringHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_Font::_Font() : font_(NULL) {
    _attributes = nil;
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
    
	/*font_       = font;
    _attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                            font_, NSFontAttributeName,
                           [NSColor whiteColor], NSForegroundColorAttributeName,
                           nil, NSBackgroundColorAttributeName,
                           nil];*/
	return TRUE;
	}

FONTDef
_Font::Detach(){
	if( IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}
	FONTDef ret = font_;
	font_       = NULL;
    _attributes = nil;
	return ret;
	}

BOOL
_Font::DeleteObject(){
	if( !IsNull() ){
        font_ = nil;
        _attributes = nil;
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
		return FALSE;/*
    font_ = [NSFont fontWithName:[NSString stringWithCString:(const char*)pszFontName encoding:NSUnicodeStringEncoding] size:(CGFloat)nHeight];
    _attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                   font_, NSFontAttributeName,
                   [NSColor whiteColor], NSForegroundColorAttributeName,
                   nil, NSBackgroundColorAttributeName,
                   nil];*/
    return FALSE;
	}

int
_Font::GetLogFont(LOGFONT* pLogFont){
	if( IsNull() || !pLogFont )
		return 0;
    memset(pLogFont, 0, sizeof(LOGFONT));
    
    //strcpy(pLogFont->lfFaceName, [font_.fontName cStringUsingEncoding:NSASCIIStringEncoding]);
    _tcscpy_(pLogFont->lfFaceName, [font_.fontName cStringUsingEncoding:NSASCIIStringEncoding]);
    pLogFont->lfHeight = (int)font_.pointSize;
    return sizeof(LOGFONT);
	}

BOOL
_Font::CreateFontIndirect(LOGFONT* pLogFont){
	if( !IsNull() || !pLogFont )
		return FALSE;
    /*
    font_       = [NSFont fontWithName:[NSString stringWithUTF8String:pLogFont->lfFaceName] size:(CGFloat)pLogFont->lfHeight];
    _attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                   font_, NSFontAttributeName,
                   [NSColor whiteColor], NSForegroundColorAttributeName,
                   nil, NSBackgroundColorAttributeName,
                   nil];*/
	return !IsNull();
	}

BOOL
_Font::GetTextSize(_string* pStr, _Size& szText){
	//if( IsNull() )
	//	return FALSE;
    
    //NSString* str = [NSString stringWithCharacters:(const unichar*)pStr->c_str() length:pStr->length()];
    //NSSize sz = [str sizeWithAttributes:_attributes];
    
    //szText.cx = sz.width;
    //szText.cy = sz.height;
	return TRUE;
	}

int
_Font::GetSymbolIndexByXOffset(_string* pStr, int nXTextStart, int nXSymbolAt){
	if( IsNull() || !pStr || !pStr->length() )
		return -1;
    /*
    NSString*           str = [NSString stringWithCharacters:(const unichar*)(pStr->c_str()) length:pStr->length()];
	_Size				szText(0, 0);
	int					nRet = -1;
    int                 nLen = (int)str.length;
    NSRange range;

	for(int i=0; i<nLen; i++){
        range.location = i;
        
        NSSize sz = [[str substringWithRange:range] sizeWithAttributes:_attributes];
        szText.cx = sz.width;
        szText.cy = sz.height;
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
		nRet = (int)(nLen - 1);
	return nRet;*/
    return 0;
	}

int
_Font::GetSymbolWidthArray(_string* pStr, CDWordArray& arrSymbolWidth){
	if( IsNull() || !pStr || !pStr->length() )
		return 0;
/*
	NSString*           str = [NSString stringWithCharacters:(const unichar*)(pStr->c_str()) length:pStr->length()];
	_Size				szText(0, 0);
	int					nRet = -1;
    int                 nLen = (int)str.length;
    NSRange range;

	for(int i=0; i<nLen; i++){
		NSSize sz = [[str substringWithRange:range] sizeWithAttributes:_attributes];
        szText.cx = sz.width;
        szText.cy = sz.height;
        
		arrSymbolWidth.push_back(szText.cx);
		nRet ++;
		}
	return nRet;*/ return 0;
	}

BOOL
_Font::StretchedTextFont(_Font* pFontOut, float fStretchCX, float fStretchCY, bool bUnderLine, bool bBold){
	if( IsNull() || !pFontOut )
		return FALSE;

	LOGFONT lf;
	if( !GetLogFont(&lf) )
		return FALSE;
     
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
	return false;
	}

NSDictionary*
_Font::GetAttributesDictionary(){
    return nil;
    //return _attributes;
    }
