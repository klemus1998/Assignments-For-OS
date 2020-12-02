/* Karla Lemus

	Assignment 2
	Due April 1

	This assignment is part of a simple client/server exchange. In this case
	the client will connect with the server and send a request for a specific city.

	Sources used:
	https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
	https://piazza.com/class_profile/get_resource/k4bjn3aw8qc2ov/k728tmpytds3ch
	https://piazza.com/class_profile/get_resource/k4bjn3aw8qc2ov/k728q0r46xo4mg
	https://piazza.com/class_profile/get_resource/k4bjn3aw8qc2ov/k728oddb9jh2j4
	geeksforgeeks.org/vector-in-cpp-stl/
*/

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <string.h>
#include <sstream>
#define MAX 80


using namespace std;

void error(char *msg) //error message if something goes wrong
{
	perror(msg);
	exit(0);
}

void func(int sockfd) //function taken from geeksforgeeks t
{						//used to have the client talk with the server
	char buff[256]; //used to take in message from client
	int n = 0;
	for (;;)
	{


		bzero(buff, 257);	//clear buffer
		cout << "Enter a city name: ";

		if (n == 0)
		{
			cin.ignore(INT8_MAX, '\n'); //make sure that it doesn't keep repeating no data
		}
		cin.getline(buff, 256);			//get the city name

		stringstream ss;
		ss.str(buff);
		string sBuffer = ss.str();


		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));

		cout << buff;							//display the message from server

		if (sBuffer == "exit") {
			printf("Server Exit...\n");
			break;
		}

		n++;
	}
}

int main()
{
	int sockfd, connfd, portno;		
	struct sockaddr_in servaddr, cli;
	struct hostent *hp;
	char myname[MAX+1];

	cout << "Enter host name: ";
	cin >> myname;

	memset(&servaddr, 0, sizeof(struct sockaddr_in));

	gethostname(myname, MAX);
	hp = gethostbyname(myname);
	if (hp == NULL)
		return (-1);
	cout << "Enter port number: ";
	cin >> portno;

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign port number
	servaddr.sin_family = hp->h_addrtype;
	servaddr.sin_port = htons(portno);

	// connect the client socket to server socket 
	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat 
	func(sockfd);

	// close the socket 
	close(sockfd);

	return 0;
}

