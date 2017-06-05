#if !defined(PLATFORMLONGBINARY_H_INCLUDED)
#define      PLATFORMLONGBINARY_H_INCLUDED

#include "PlatformCompat.h"

class LongBinary
{
public:
	LongBinary();
	virtual ~LongBinary();

	operator HGLOBAL(){return m_hMemory;};

	void*	LockMemory	();
	void	UnlockMemory();

	BOOL	SetBinarySize(DWORD sizeNew, void* lpDataCopy = NULL);
	DWORD	GetBinarySize(){ return m_dwSize; };

protected:
	HGLOBAL		m_hMemory;
	DWORD		m_dwSize;
};

#endif // !defined(PLATFORMLONGBINARY_H_INCLUDED)

