#pragma once
#include "../_platformCompat/PlatformCompat.h"

class _Rect;
class _Point;
class _Size;
class LongBinary;

#pragma pack(1)

class GrowableMemory
{
public:
	GrowableMemory(UINT nInitialSize = 0x400, UINT nGrowSize = 0x400, bool bUseHeap = true);
	virtual ~GrowableMemory();

public:
	bool	AddBytes				(BYTE* lpData, int nSize); 
	bool	AddString				(_string& sValue);
	int		SetBufferSize			(UINT nSize);
	void	SetReadonlyBuffer		(BYTE* lpData, int nSize);
	void	ZeroUsedMemory			(){m_nUsedSize = m_nOffset = 0; };
	void	SetUsedBufferSize		(int nSizeUsed)
		{
		if( nSizeUsed < 0 ) return;
		if( nSizeUsed > m_nBufferSize )
			nSizeUsed = m_nBufferSize;
		m_nUsedSize = nSizeUsed;
		};

	BYTE*	EmptyBufferGetPtr		();

	bool	ReadInt8				(char* cVal);
	bool	ReadInt16				(short* shVal);
	bool	ReadInt32				(int* nVal);
	bool	ReadInt64				(__int64* bigVal);
	bool	ReadFloat				(float* fVal);
	bool	ReadDouble				(double* dVal);
	bool	ReadString				(_string* sVal);
	bool	ReadBinary				(LongBinary* pBinary);
	bool	ReadtagRect				(_Rect* pRect);
	bool	ReadtagPoint			(_Point* pPoint);
	bool	ReadtagSize				(_Size* pSize);
	bool	ReadLogFont				(LOGFONT* pLogFont);

	BYTE*	GetCurrentOffsetPtr		();
	DWORD	GetCurrentOffset		();
	bool	SetCurrentOffset		(DWORD dwOffset);
	BYTE*	GetOffsetPtr			(DWORD dwOffset);
	BOOL	ReadSkipBytes			(int nBytesSkip);

	BYTE*	GetBufferPtr			(){return m_pBuffer;};
	int		GetBufferSize			(){return m_nBufferSize;};
	int		GetUsedBufferSize		(){return m_nUsedSize;};
	int		GetOffset				(){return m_nOffset;};
	void	ResetBufferPointerPosition(){m_nOffset = 0;};

protected:
	UINT		m_nInitialSize;
	UINT		m_nGrowSize;
	bool		m_bUseHeap;
	bool		m_bReadOnly;
	HANDLE		m_hHeap;
	UINT		m_nBufferSize;
	UINT		m_nUsedSize;
	UINT		m_nOffset;
	BYTE*		m_pBuffer;
    //void*       m_pTemp;
};