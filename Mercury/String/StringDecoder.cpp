#include "StringDecoder.h"

UINT
DecodeString(LPCTSTR lpText){
	UINT	dwCode	= 0xFFFFFFFF;
	size_t	dwSize	= (size_t)_tcslen(lpText);
    
    BYTE* pSymbolCode = (BYTE*)lpText;
	UINT nCode = dwCode;
	UINT nEsi = 0;
	BYTE nIndex = 0;
    
	for(size_t i=0; i<dwSize; i++){
		nEsi	= (nCode & 0xFFFF);
		nCode >>= 8;
		nIndex = pSymbolCode[0];
		nIndex ^= ((BYTE)nEsi);
		nEsi = gDecoderData[nIndex];
		nEsi ^= nCode;
		nCode = nEsi;
        
		pSymbolCode ++;
    }
    
	dwCode = nCode;

/*
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
*/
	return dwCode;
	}