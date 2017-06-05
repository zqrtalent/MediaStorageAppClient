#pragma once

class ThisCallHelper;
class BasePacket;

typedef void (__thiscall ThisCallHelper::*FileModifiedMethod)(HANDLE hFile, DWORD dwSizeNew);
typedef void (__thiscall ThisCallHelper::*PacketListenerMethod)(BasePacket* p, void* pParam);
typedef bool (__thiscall ThisCallHelper::*Handle_Packet)(BYTE* lpData, int nSize);

class ThisCallHelper{
public:
	ThisCallHelper();
	~ThisCallHelper();

	void Call_FileModifiedMethod(FileModifiedMethod m, HANDLE hFile, DWORD dwSizeNew){
		(this->*m)(hFile, dwSizeNew);
		}

	void Call_PacketListenerMethod(PacketListenerMethod m, BasePacket* p, void* pParam){
		(this->*m)(p, pParam);
		}

	bool Call_HandlePacketMethod(Handle_Packet m, BYTE* lpData, int nSize){
		return (this->*m)(lpData, nSize);
		}
	};

