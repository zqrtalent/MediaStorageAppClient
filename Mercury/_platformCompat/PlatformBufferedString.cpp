#include "PlatformBufferedString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BufferedString::BufferedString() : _string() {
	m_pBuffer		= NULL;
	m_nBufferSize	= 0;
	}

BufferedString::~BufferedString() {
	if( m_pBuffer )
		delete [] m_pBuffer;
	m_pBuffer		= NULL;
	m_nBufferSize	= 0;
	}

void
BufferedString::operator=(_string& str){
	((_string*)this)->operator=(str);
	}

void 
BufferedString::operator=(LPCTSTR lpsz){
	((_string*)this)->operator=(lpsz);
	}

TCHAR*
BufferedString::GetBuffer(UINT minBufferLen){
	if( m_nBufferSize < minBufferLen ){
		TCHAR* pBufferTemp = new TCHAR[minBufferLen + 1];
		if( m_pBuffer ){
			memcpy(pBufferTemp, m_pBuffer, m_nBufferSize);
			delete [] m_pBuffer;
			}
		m_nBufferSize	= sizeof(TCHAR)*(minBufferLen+1);
		m_pBuffer		= pBufferTemp;
		}
	return m_pBuffer;
	}

void
BufferedString::ReleaseBuffer(int nNewLen /*= -1*/){
	if( m_pBuffer && m_nBufferSize > 0 ){
		int nBufferLen = m_nBufferSize/sizeof(TCHAR);
		if( nNewLen < 0 )
			nNewLen = (int)_tcslen(m_pBuffer);
		if( nNewLen > nBufferLen )
			nNewLen = nBufferLen - 1;

		if( nNewLen > 0 ){
			m_pBuffer[nNewLen] = '\0';
			resize(nNewLen);
			memcpy((void*)c_str(), m_pBuffer, nNewLen);
			}
		else{
			empty();
			}

		delete [] m_pBuffer;
		m_pBuffer		= NULL;
		m_nBufferSize	= 0;
		}
	}