/**
* @file    Client.cpp
* @author  Mrutyunjaya Pattanshetti
* @date    1-Oct-2018
* @brief   Libraray Client application that perform fallowing things
*			1] Send data to Libraray server over NamedPipe
*			2] Get Async notification aboout the book added at the server
*			3] Get Book details with Sync method 
*			
*/

/* Includes ------------------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>
#include <stdint.h>
#include <winsock.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <typeinfo>
#include <winerror.h>
#include <thread>
#include<limits>
#include <future>
/*----------------------------------------------------------------------------*/
using namespace std;

/*SignleTon Class that open existing Pipe and closes Pipe */
class StreamLabClient {

private:
	HANDLE pHandle;  //NamedPipe handler
	static StreamLabClient *s_instance;

	/*Constrcutor which creates existing NamedPipe*/
	StreamLabClient()
	{
		pHandle = CreateFile(TEXT("\\\\.\\pipe\\StreamLab"),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
			NULL);
	}
public:
	/* close pipe when StreamLabClientClient expires */
	~StreamLabClient() {
		CloseHandle(pHandle);
	}
	
	/*Return NamedPipe*/
	HANDLE GetPipe() {
		return pHandle;
	}

	/*Return SingleTon Object*/
	static StreamLabClient *instance()
	{
		if (!s_instance)
			s_instance = new StreamLabClient();
		return s_instance;
	}
};

/*Init Obj to zero*/
StreamLabClient *StreamLabClient::s_instance = 0;

/*Structure that forms the packet*/
typedef struct {
	int OpId;     //Operation Id {1: Add 2:Get}
	struct Data{
		char name[1024]; 
		int Id;   //Id to Get Book 
		char Author[1024];
	};
	Data data; //Reference to Data in Header
}Packet;

/*
@Param:Handle
@Return:void
@ This function is used to get book updated notification
*/
void Read_Pipe_Data(HANDLE Pipe) {
	char p[1024];
	/* Read data from Pipe */
	if(ReadFile(Pipe, &p, sizeof(p), NULL, NULL));
	printf("%s\n", p);
	
}

int main(void)
{
	
	
	
	int choice;
	Packet pdata;
	
	
	while (StreamLabClient::instance()->GetPipe() != INVALID_HANDLE_VALUE){
		
			cout << "Wel come StreamLab Library" << endl;
			cout << "=====================================\n";
			cout << "Please use the below options \n";
			cout << "1:Add books to DB\n";
			cout << "2:Get book from DB \n";
			cout << "3:Exit\n";
			cin >> choice;
			switch (choice)
			{
			case 1:
				system("cls");
				cout << "Add Menu\n";
				cout << "=====================================\n";
				try
				{
					
					cout << "Enter Book Name\n";
					cin >> pdata.data.name;
					cout << "Enter Author Name\n";
					cin >> pdata.data.Author;
					pdata.OpId = choice;
					//Send data to Pipe Async response will be recived later
					WriteFile(StreamLabClient::instance()->GetPipe(), &pdata, sizeof(pdata), NULL, NULL);
				}
				catch (std::exception& e)
				{
					cout << "You have entered invalid data please enter int \n";
					system("pause");
				}
				break;
			case 2:
				system("cls");
				cout << "Get Menu\n";
				cout << "=====================================\n";
				cout << "Enter Book Id";
				try
				{
					cin >> pdata.data.Id;
					pdata.OpId = choice;
					//Send data to Pipe sync response will be recived
					WriteFile(StreamLabClient::instance()->GetPipe(), &pdata, sizeof(pdata), NULL, NULL);
					/*Clear the Pipe*/
					FlushFileBuffers(
						StreamLabClient::instance()->GetPipe()
						);
					//Sync Response
					ReadFile(StreamLabClient::instance()->GetPipe(), &pdata, sizeof(pdata), NULL, NULL);

					cout <<"ID="<<pdata.data.Id<<"\tName="<<pdata.data.name <<"\tAuthor="<<pdata.data.Author<<endl;
					system("pause");
					
				}
				catch (std::exception& e)
				{
					cout << "You have entered invalid data please enter int \n";
					system("pause");
				}
				break;
				
				
				
			case 3:
				return 0;
			default:
				cout << "Inavlid choice" << endl;
				system("pause");
				system("cls");
			}
			pdata = { 0 };
			/*Async recevier for AddBook to server*/
			if (choice == 1)
			std::async(Read_Pipe_Data, StreamLabClient::instance()->GetPipe());

		}
		
	cout << "System borken Server connection is not availble" << endl;
	system("pause");
	return 0;
}