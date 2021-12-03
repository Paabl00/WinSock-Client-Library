#include "win32_s.h"

struct Settings {
	bool DEVLOG = false; //fix sometime functions of devlog cuz looks nasty and horrible af
}settings;

WSAData Client::WSA_init(int version_makeword_1, int version_makeword_2) { //Initialize on the start of your winsock program
	WSADATA WSAdata;
	WSAStartup(MAKEWORD(version_makeword_1, version_makeword_2), &WSAdata);
	return WSAdata;
}

std::string Client::Auth(std::string key) {
	char first_num_c = key.at(5);
	char first_num_second_c = key.at(6);

	int first_num = first_num_c - '0';
	int first_num_second = first_num_second_c - '0';

	std::string first_num_final = std::to_string(first_num) + std::to_string(first_num_second);


	char second_num_c = key.at(11);
	char second_num_second_c = key.at(12);

	int second_num = second_num_c - '0';
	int second_num_second = second_num_second_c - '0';

	std::string second_num_final = std::to_string(second_num) + std::to_string(second_num_second);

	int final_first_int;
	int final_second_int;

	std::stringstream ss;
	ss << first_num_final;
	ss >> final_first_int;

	ss.clear();

	ss << second_num_final;
	ss >> final_second_int;

	int hashcode = final_first_int + final_second_int + 3;

	std::string new_key = "";
	int start = key.find("[");
	char temp = key.at(start);
	new_key = new_key + temp;

	for (int i = 1; i < 17; i++) {
		if (i == 5 || i == 6 || i == 11 || i == 12) {
			temp = key.at(start + i);
			new_key = new_key + temp;
		}
		else {
			temp = key.at(start + i);
			new_key = new_key + (char)(temp + hashcode);
		}
	}

	new_key = new_key + "]";

	return new_key;

}

SOCKET Client::init(std::string ip, int port) { //Initialize the whole winsock for work

	SOCKET server;
	SOCKADDR_IN sock_addr;

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		std::cout << "Failed on socket creation. Please report to a moderator error nr init#01" << std::endl;
		return -1;
	}

	sock_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &sock_addr.sin_addr);
	sock_addr.sin_port = htons(port);

	if (connect(server, (const sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		std::cout << "Failed on connecting to server." << std::endl;
		std::cout << "Check your internet connection or information from passione's" << std::endl;
		return -1;
	}

	return server;

}

int Client::ClientSend(SOCKET dst, char buffer[]) {
	
	std::string buffer_string = BufferToJavaString(buffer);

	if (send(dst, buffer_string.c_str(), buffer_string.size(), 0) == SOCKET_ERROR) {
		std::cout << "Sending information to server failed. Please report error send#01" << std::endl;
		return - 1;
	}

	return 0;

}

std::string Client::ClientRecieve(SOCKET src) {

	char buffer[1024];
	size_t recvbytes;

	ZeroMemory(buffer, 1024);

	recvbytes = recv(src, buffer, 1024, 0);
	if (recvbytes < 0) {
		std::cout << "Failed to recieve information from server. Please report error recv#01" << std::endl;
		return NULL;
	}

	std::string translated_buffer = JavaBufferToString(buffer);

	return translated_buffer;

}

std::string Client::BufferToString(char buffer[]) {
	std::string buffer_string = buffer;
	return buffer_string;
}

std::string Client::BufferToJavaString(char buffer[]) {
	std::string buffer_java_string = buffer;
	buffer_java_string.append("\n");

	return buffer_java_string;
}

std::string Client::JavaBufferToString(char buffer[]) {
	std::string java_buffer_string = buffer;
	//java_buffer_string.erase(std::remove(java_buffer_string.begin(), java_buffer_string.end(), '\n'), java_buffer_string.end());

	if (java_buffer_string.size() > 0) { //nasty af fix soemtime
		java_buffer_string.erase(java_buffer_string.size() - 1);
		java_buffer_string.erase(java_buffer_string.size() - 1);
	}

	java_buffer_string.append("\0");
	return java_buffer_string;
}

int Client::End(SOCKET con) { //Call always after closing a socket/ending program.
	closesocket(con);
	WSACleanup();
	return 0;
}

void Client::recieve_setBufferAccesibility(int value) {
	Recv_BufferAccesibilityMode = value;
}

void Client::recieve_isBufferRaw(bool value) {
	Recv_Raw = value;
}

void Client::recieve_setServerType(int value) {
	Recv_Server_Type = value;
}

void Client::recieve_setStoreAddress(std::string* addr) {
	Recv_StoreAddress = addr;
}

void Client::recieve_setStoreAddressRaw(char* addr) {
	Recv_StoreAdress_Raw = addr;
}

void Client::send_setSendMode(int value) {
	Send_Mode = value;
}

void Client::send_isSendRaw(bool value) {
	Send_Raw = value;
}

void Client::send_setServerType(int value) {
	Send_Server_Type = value;
}

void Client::send_setStoreAddress(std::string* addr) {
	Send_StoredDataAddress = addr;
}
 
DWORD WINAPI Client::Thread_ClientRecieve(SOCKET socket_src) { //TODO: Packet management, Exceptions

	const DWORD MAX_BYTES = 1024;
	char buffer[MAX_BYTES];
	size_t recvbytes;
	SOCKET server = socket_src;

	if (settings.DEVLOG) {
		std::cout << "Thread_ClientRecieve SOCKET - " + server << std::endl;
	}

	if (settings.DEVLOG) {
		std::cout << "Thread_ClientRecieve params: " << std::endl;
		Devlog_parameters();
	}

	while (true) {

		ZeroMemory(buffer, MAX_BYTES);
		
		recvbytes = recv(server, buffer, MAX_BYTES, 0);

		if (recvbytes <= 0) {
			std::cout << "Recieving info from server failed. Cancellig thread..." << std::endl;
			break;
		}

		if (Recv_BufferAccesibilityMode == THREAD_CONSOLE_PRINT_INFO) {

			std::string console_text;

			if (Recv_Raw) {
				std::cout << buffer << std::endl;
			}
			if (Recv_Server_Type == SERVER_TYPE_JAVA && !Recv_Raw) {
				console_text = JavaBufferToString(buffer);
				if (threadPackageManager) {
					console_text = PackageManager(console_text);
				}
				std::cout << console_text << std::endl;
			}
			if (Recv_Server_Type == SERVER_TYPE_WINSOCK && !Recv_Raw) {
				console_text = BufferToString(buffer);
				if (threadPackageManager) {
					console_text = PackageManager(console_text);
				}
				std::cout << console_text << std::endl;
			}
		}

		if (Recv_BufferAccesibilityMode == THREAD_STORE_INFO) {

			std::string buffer_s;

			if (Recv_Raw) {
				Recv_StoreAdress_Raw = buffer;
			}
			if (Recv_Server_Type == SERVER_TYPE_JAVA && !Recv_Raw) {
				buffer_s = JavaBufferToString(buffer);
				if (threadPackageManager) {
					buffer_s = PackageManager(buffer_s);
				}
				*Recv_StoreAddress = buffer_s;
			}
			if (Recv_Server_Type == SERVER_TYPE_WINSOCK && !Recv_Raw) {
				buffer_s = BufferToString(buffer);
				if (threadPackageManager) {
					buffer_s = PackageManager(buffer_s);
				}
				*Recv_StoreAddress = buffer_s;
			}
		}

	}

	return 0;

} //TODO: Before and after function user-override possible

DWORD WINAPI Client::Thread_ClientSend(SOCKET dst) {

	const DWORD MAX_BYTES = 1024;
	char buffer[1024];
	SOCKET server = dst;

	if (settings.DEVLOG) {
		std::cout << "Thread_ClientSend SOCKET - " + server << std::endl;
	}

	if (settings.DEVLOG) {
		std::cout << "Thread_ClientSend params: " << std::endl;
		Devlog_parameters();
	}

	while (true) {

		if (Send_Mode == THREAD_KEYBOARD_SEND) {
			std::cin.getline(buffer, 1024);
			std::string data;

			if (Send_Raw) {
				if (send(server, buffer, MAX_BYTES, 0) == SOCKET_ERROR) {
					std::cout << "Failed while sending to the server. Closing thread...." << std::endl;
					return -1;
				}
			}

			if (Send_Server_Type == SERVER_TYPE_JAVA) {
				data = BufferToJavaString(buffer);
				if (send(server, data.c_str(), data.size(), 0) == SOCKET_ERROR) {
					std::cout << "Failed while sending to the server. Closing thread...." << std::endl;
					return -1;
				}
			}

			if (Send_Server_Type == SERVER_TYPE_WINSOCK) {
				data = BufferToString(buffer);
				if (send(server, data.c_str(), data.size(), 0) == SOCKET_ERROR) {
					std::cout << "Failed while sending to the server. Closing thread...." << std::endl;
					return -1;
				}
			}

		}

		if (Send_Mode == THREAD_VARIABLE_SEND) {

			if (Send_StoredDataAddress == NULL) {
				std::cout << "Please enter a address to use the thread variable send function. Cancelling thread..." << std::endl;
				return -1;
			}

			//memset(thread_settings.Send_StoredDataAddress, 0x90, sizeof(thread_settings.Send_StoredDataAddress));
			std::string StoredData = *Send_StoredDataAddress;

			if (Send_Server_Type == SERVER_TYPE_JAVA) {
				StoredData.append("\n");

				if (send(server, StoredData.c_str(), StoredData.size(), 0) == SOCKET_ERROR) {
					std::cout << "Failed while sending to the server. Closing thread...." << std::endl;
					return -1;
				}
			}
			else {
				if (send(server, StoredData.c_str(), StoredData.size(), 0) == SOCKET_ERROR) {
					std::cout << "Failed while sending to the server. Closing thread...." << std::endl;
					return -1;
				}
			}
		}
	}

}//TODO: Before and after function user-override possible

DWORD WINAPI ThreadHandler::ThreadClientRecieve(LPVOID param) {

	ThreadParamStruct* params = (ThreadParamStruct*)param;

	if (settings.DEVLOG) {
		std::cout << "ThreadClientRecieve - params->Client* - " << params->Client << std::endl;
		std::cout << "ThreadClientRecieve - params->sock - " << params->sock << std::endl;
	}

	(params->Client)->Thread_ClientRecieve(params->sock);
	return 1;

}

DWORD WINAPI ThreadHandler::ThreadClientSend(LPVOID param) {

	ThreadParamStruct* params = (ThreadParamStruct*)param;

	if (settings.DEVLOG) {
		std::cout << "ThreadClientSend - params->Client* - " << params->Client << std::endl;
		std::cout << "ThreadClientSend - params->sock - " << params->sock << std::endl;
	}

	(params->Client)->Thread_ClientSend(params->sock);
	return 1;

}

void Client::applyPackageReader(std::vector<std::string> packetlist) {
	packages = packetlist;
	return;
}

void Client::applyPacketFunctions(std::vector<void(*)()> functionlist) {
	functions = functionlist;
	return;
}

std::string Client::PackageManager(std::string buffer) {

	//TODO: 
	//Check if the list of packages isnt empty
	if (packages.empty()) return NULL;

	//try to find the packages in recieved buffer (if not raw)

	for (int i = 0; i < packages.size(); i++) {
		std::string package = packages[i];
		//check together with buffer
		
		size_t pos = buffer.find(package);
		if (pos != std::string::npos) {
			buffer.erase(pos, package.length());
			if (functions[i] != nullptr) {
				void (*f)() = functions[i];
				f();
			}
		}
		else {
			continue;
		}

		// if found delete packet from buffer and launch packetr function (packet pointer??)
		// else continue comparing packet with buffer.
	}

	//remember : apply devlog if necesarry

	return buffer;
}

void Client::Devlog_parameters() {
	std::cout << "---------------------------------------------------------------" << std::endl;
	std::cout << "Recv_bufferAccesibilityMode: " << Recv_BufferAccesibilityMode << std::endl;
	std::cout << "Recv_raw: " << Recv_Raw << std::endl;
	std::cout << "Recv_Server_Type: " << Recv_Server_Type << std::endl;
	std::cout << "Recv_StoreAddress: " << &Recv_StoreAddress << std::endl;
	std::cout << "Recv_StoreAddressRaw: " << &Recv_StoreAdress_Raw << std::endl;
	std::cout << "Send_Mode: " << Send_Mode << std::endl;
	std::cout << "Send_Raw: " << Send_Raw << std::endl;
	std::cout << "Send_Server_Type: " << Send_Server_Type << std::endl;
	std::cout << "Send_StoredDataAddress: " << &Send_StoredDataAddress << std::endl;
	std::cout << "---------------------------------------------------------------" << std::endl;

	return;
}