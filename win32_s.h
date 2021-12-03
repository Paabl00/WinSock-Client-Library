#include <iostream>
#include <string>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

/*
Documentation

First off all.....

To possibly use macros and parameters for our THREAD Loops we need
to make two structs and then set it to the VOID pointers that you can set with 
setRecieveParameters(); and setSendParameters(); The recv struct prototype looks like this: (and must be constructed like this, same with send_params prot.)


struct RECV_PARAMS {
	DWORD Recv_BufferAccesibilityMode; THREAD_CONSOLE_PRINT_INFO / THREAD_STORE_INFO
	bool Recv_Raw; true/false
	DWORD Recv_Server_Type; SERVER_TYPE_JAVA / SERVER_TYPE_WINSOCK
	std::string* Recv_StoreAddress; address
	char* Recv_StoreAdress_Raw; true/false
};

And the SEND_PARAMS prototype:

struct SEND_PARAMS {
	DWORD Send_Mode; THREAD_KEYBOARD_SEND / THREAD_VARIABLE_SEND
	bool Send_Raw; true/false
	DWORD Send_Server_Type; SERVER_TYPE_JAVA / SERVER_TYPE_WINSOCK
	std::string* Send_StoredDataAddress; address
};

Both of them structs are pretty much self explanatory.

*/

struct Settings;

class Client {
private:

	//Recieving
	DWORD Recv_BufferAccesibilityMode = NULL;
	bool Recv_Raw = true;
	DWORD Recv_Server_Type = NULL;
	std::string* Recv_StoreAddress = nullptr;
	char* Recv_StoreAdress_Raw = nullptr;
	//Sending
	DWORD Send_Mode = NULL;
	bool Send_Raw = true;
	DWORD Send_Server_Type = NULL;
	std::string* Send_StoredDataAddress = nullptr;

	std::vector<std::string> packages;
	std::vector<void(*)()> functions;

public:
	SOCKET init(std::string ip, int port);
	WSAData WSA_init(int version_makeword_1, int version_makeword_2);
	std::string Auth(std::string key);
	void applyPackageReader(std::vector<std::string> packetlist); //check
	void applyPacketFunctions(std::vector<void(*)()> functionlist); //check
	std::string PackageManager(std::string buffer); //TODO //check
	int ClientSend(SOCKET dst, char buffer[]);
	std::string ClientRecieve(SOCKET src);
	std::string BufferToString(char buffer[]);
	std::string BufferToJavaString(char buffer[]);
	std::string JavaBufferToString(char buffer[]);
	int End(SOCKET con);

	//Thread settings/macros
	#define THREAD_CONSOLE_PRINT_INFO 1
    #define THREAD_STORE_INFO 2
	#define THREAD_KEYBOARD_SEND 6
	#define THREAD_VARIABLE_SEND 7

	//Server type
	#define SERVER_TYPE_JAVA 22
	#define SERVER_TYPE_WINSOCK 33
	
	//parameter functions
	void recieve_setBufferAccesibility(int value);
	void recieve_isBufferRaw(bool value);
	void recieve_setServerType(int value);
	void recieve_setStoreAddress(std::string* addr);
	void recieve_setStoreAddressRaw(char* addr);

	void send_setSendMode(int value);
	void send_isSendRaw(bool value);
	void send_setServerType(int value);
	void send_setStoreAddress(std::string* addr);

	//Thread - only used for forums - live message sending without specific package type - just clean message
	DWORD WINAPI Thread_ClientRecieve(SOCKET socket_src);
	DWORD WINAPI Thread_ClientSend(SOCKET dst);

	//Devlog methods
	void Devlog_parameters();

	bool threadPackageManager = false;

};

namespace ThreadHandler {
	DWORD WINAPI ThreadClientRecieve(LPVOID param);
	DWORD WINAPI ThreadClientSend(LPVOID param);

}

class ThreadParamStruct {
public:
	SOCKET sock;
	Client* Client;
};