# LibraryApp_With_NamedPipe

***************************************************
* This project is demo for NamedPipe using which the small libarary application is implemented
* There are 3 Application 
   1] Server
   2] Client

* 1] Server has got implementaion of
* Creating  NamedPipe
* Creating  Custome Class to which allows user to add and get details
* List is used to maintin the objects of the class
* 2] Client has got implementaion of
* Connecting to existing NamedPipe
* Request to create the objects on servre
* Add details to object
* Perfoms Async Read request to get the updated notifications(Assumptions is made that client may get notfiied about addded
      books at any time);
* Perfoms Sync Read to get the book details
    
* The Packet is used to communicate bteween client and server, Fllowing is the structre of packet.

   OpId| Name of the Book|  ID of the Book|Author of the Book

       
 * OpId in packet carries the kind of operation to be done at the server side
    Eg: OpId=1 Indicates Add and Create object 
 
 
 # Files
    *Client.cpp
    *Server.cpp
    *Binaries
    *Video of the running application.
 To satrt the both application simply run bat file.
 
# Note
  The applications is built on windows 64 bit
  Due to async call for read data the print may come in btw the char, But at the application or UI level this can be taken care.
