// Включение заголовочных файлов
#define WIN32_LEAN_AND_MEAN

#include <iostream> // Включение заголовочного файла для ввода-вывода
#include <Windows.h> // Включение заголовочного файла для Windows API
#include <WinSock2.h> // Включение заголовочного файла для Windows сокетов
#include <WS2tcpip.h> // Включение заголовочного файла для Windows сокетов TCP/IP

using namespace std;

// Основная функция программы
int main()
{
	// Установка локали
	std::locale::global(std::locale(""));

	// Инициализация структуры данных для WinSock
	WSADATA wsaData;
	ADDRINFO hints;
	ADDRINFO* addrResult;
	SOCKET ClientSocket = INVALID_SOCKET;

	// Строка для отправки данных
	const char* sendBuffer = " Hello from Client";
	const char* sendBuffer2 = "мама я хочу спать";
	char recvBuffer[512];

	// Инициализация WinSock
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		cout << "WSAStartup failed" << endl; // Вывод сообщения об ошибке и завершение программы
		return 1;
	}

	// Заполнение структуры hints
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Получение информации об адресе
	result = getaddrinfo("localhost", "788", &hints, &addrResult);
	if (result != 0) {
		cout << "getaddrinfo failed" << endl; // Вывод сообщения об ошибке и завершение программы
		return 1;
	}

	// Создание сокета
	ClientSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (ClientSocket == INVALID_SOCKET) {
		cout << "socket creation with error" << endl; // Вывод сообщения об ошибке и завершение программы
		freeaddrinfo(addrResult);
		return 1;
	}

	// Подключение к серверу
	result = connect(ClientSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		cout << "Unable to connect server" << endl; // Вывод сообщения об ошибке и завершение программы
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	// Отправка данных
	result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), MSG_DONTROUTE);
	if (result == SOCKET_ERROR) {
		cout << "send Failed" << endl; // Вывод сообщения об ошибке и завершение программы
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	
	cout << "Sent " << result << " bytes"; // Вывод количества отправленных байт

	// Закрытие соединения для отправки данных
	result = shutdown(ClientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		cout << "shutdown error" << endl; // Вывод сообщения об ошибке и завершение программы
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	// Прием данных от сервера
	do {
		ZeroMemory(recvBuffer, 512);
		result = recv(ClientSocket, recvBuffer, 512, 0);
		if (result > 0) {
			cout << "Получено " << result << "байт" << endl; // Вывод количества принятых байт
			cout << "Получено " << recvBuffer << endl; // Вывод принятых данных
		}
		else if (result == 0) {
			cout << "Connection closed" << endl; // Вывод сообщения о закрытии соединения
		}
		else {
			cout << "Сообщение не получено" << endl; // Вывод сообщения об ошибке при приеме данных
		}
	} while (result > 0);

	// Закрытие сокета и очистка ресурсов WinSock
	closesocket(ClientSocket);
	freeaddrinfo(addrResult);
	WSACleanup();
}
