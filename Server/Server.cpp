/**
* @file    Client.cpp
* @author  Mrutyunjaya Pattanshetti
* @date    1-Oct-2018
* @brief   Libraray Server application that perform fallowing things
*			1] Send data to Libraray server over NamedPipe
*			2] Get Async notification aboout the book added at the server
*			3] Get Book details with Sync method
*
*/

/* Includes ------------------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <winsock.h>
#include <string>
#include <iostream>
#include <list>
using namespace std;
/*----------------------------------------------------------------------------*/
/*Class that creates Pipe and manages*/
class StreamLabServer {
private:
	HANDLE pHandle;
public:
	
	/*Constrcutor which creates NamedPipe*/
	StreamLabServer() {
		pHandle = CreateNamedPipe(TEXT("\\\\.\\pipe\\StreamLab"),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1,
			1024 * 16,
			1024 * 16,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);
	}
	/* Close Pipe */
	~StreamLabServer() {
		CloseHandle(pHandle);
	}
	/*Return Pipe*/
	HANDLE GetPipe() {
		return pHandle;
	}
	/*Return connected state*/
	bool Connected() {
		return ConnectNamedPipe(pHandle, NULL);
	}
};
/*Custom class for Libaray*/
class LibraryClass {
private:
	char Name[1024];
	int Id;
	char Author[1024];
public:
	LibraryClass()
	{

	};
	/*Update Name of Book*/
	void UpdateName(char Name[1024]) {
		strcpy_s(this->Name,Name);
		
	}
	/*Return Name of the Book*/
	char  *GetName() {
		return Name;
	}
	/*Update Id of the Book*/
	void UpdateId(int Id) {
		this->Id = Id;
	}
	/*Return Id of the Book*/
	int GetId() {
		return Id;
	}
	/*Return Author of the Book*/
	char *GetAuthor() {
		return Author;
	}
	/*Update Name of the Book*/
	void UpdateAuthor(char Author[1024]) {
		strcpy_s(this->Author, Author);
	}
};
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

/*List STL to hold the LBooks or LibrarayClass objects*/
std::list<LibraryClass*> libraraylist;
/*ID for Books*/
static int ID = 0;
int main() {
	
	/* buffer for sending update to pipe */
	char Buffer[1024];
	
	/*Packet to capture the data packet from the client */
	Packet pData;
	int choices,found; //Var to check user choice and flag to check item found
	StreamLabServer Server;
	
	while (Server.GetPipe() != INVALID_HANDLE_VALUE){
		/*Loop till connected to Pipe*/
		while (Server.Connected() != FALSE)
		{
			/*Read NamedPipe*/
			while ((ReadFile(Server.GetPipe(), &pData, sizeof(pData), NULL, NULL) != FALSE) && (GetLastError() != ERROR_BROKEN_PIPE))
			{
				LibraryClass *obj = new LibraryClass(); //Create Object if data recived 
				
				switch (pData.OpId)
				{
				case 1:
					/*Add details of book*/
					cout << "Add\n";
					obj->UpdateName(pData.data.name);
					obj->UpdateAuthor(pData.data.Author);
					obj->UpdateId(++ID);
					/*Add Object to lsit*/
					libraraylist.push_back(obj);
					/*Send update*/
					strcpy_s(Buffer, "Update done!");
					cout << Buffer << endl;
					WriteFile(Server.GetPipe(), &Buffer, sizeof(Buffer), NULL, NULL);
					break;
				case 2:
					cout << "Get\n";
				    found= 0;
					/*Iterate through objects to get the book details*/
                   for (auto const& i : libraraylist) {
					   
						if (i->GetId() == pData.data.Id)
						{
							
							
							found = 1;
							pData.data.Id = i->GetId();
							strcpy_s(pData.data.name, i->GetName());
							strcpy_s(pData.data.Author, i->GetAuthor());
							cout << pData.data.name<<endl;
							cout << pData.data.Author<<endl;
							/*Send the required Book details*/
							WriteFile(Server.GetPipe(), &pData, sizeof(pData), NULL, NULL);
							
							
						}
						
						
					}
				   /*Send Empty Data Set as data not exist*/
				   if (found == 0)
				   {
					   strcpy_s(pData.data.name,"NaN");
					   strcpy_s(pData.data.Author, "NaN");
					   pData.data.Id = 0;
					   cout << pData.data.name << endl;
					   cout << pData.data.Author << endl;
					   WriteFile(Server.GetPipe(), &pData, sizeof(pData), NULL, NULL);
				   }
				   break;
				

				}

				
				
				
			}
				
		}
		SetLastError(0);
		/* close pipe as client has been dropped */
		DisconnectNamedPipe(Server.GetPipe());
		
		
	 } 
	
	return 0;
}