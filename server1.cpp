/*	Karla Lemus
	Assignment 2
	Due: April 1


	This assignment is part of a simple client/server exchange. In this case
	the server will wait for connection requests from your client and exchange one-line
	text messages with it.

	Sources used:
	https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
	https://piazza.com/class_profile/get_resource/k4bjn3aw8qc2ov/k728tmpytds3ch
	https://piazza.com/class_profile/get_resource/k4bjn3aw8qc2ov/k728q0r46xo4mg
	https://piazza.com/class_profile/get_resource/k4bjn3aw8qc2ov/k728oddb9jh2j4
	geeksforgeeks.org/vector-in-cpp-stl/
*/

#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#define MAX 80

using namespace std;

class city //class named city to store the cityname, temperature, and forecast
{
private:
	string cityName;
	int degree;
	string cityForecast;

public:
	city(string name)
	{
		cityName = name;
		cityForecast = "none";
	}
	city(string name, int temp, string forecast)
	{
		cityName = name;
		degree = temp;
		cityForecast = forecast;
	}
	string getCity()
	{
		return cityName;
	}

	int getDeg()
	{
		return degree;
	}

	string getFore()
	{
		return cityForecast;
	}

	void print()
	{
			cout << "City Name: " << cityName << endl;
			cout << "Temperature: " << degree << endl;
			cout << "Forecast: " << cityForecast << endl;
	}


	bool operator== (const city &rhs)
	{
		if (this->getFore() == "none")
			return false; //find out if it's a temp city
		return cityName == rhs.cityName;
	}
};

void error(char *msg)
{
	perror(msg);
	exit(1);
}

void fill(vector<city> &tempVector, ifstream &tempStream)
{
	string line = "";
	string tempCityName = "";
	string strTempDeg = "";
	string tempForecast = "";
	int intTempDeg;
	while (getline(tempStream, line))
	{
		if (line.empty())
			break;
		istringstream ss(line);

		getline(ss, tempCityName, ',');
		getline(ss, strTempDeg, ',');
		intTempDeg = stoi(strTempDeg);
		getline(ss, tempForecast, ',');

		tempVector.push_back(city(tempCityName, intTempDeg, tempForecast));
	}
	tempStream.close();
}

void printAll1(vector<city> tempVector)
{
	for (int i = 0; i < tempVector.size(); i++)
	{
		cout << "City Name: " << tempVector[i].getCity() << endl;
		cout << "Temperature: " << tempVector[i].getDeg() << endl;
		cout << "Forecast: " << tempVector[i].getFore() << endl;
	}
}

void func(int sockfd, vector<city> tempVector)
{
	int n;
	char buff[256];
	string message = "";

	// infinite loop for chat 
	for (;;) {
		bzero(buff, 256);

		// read the message from client and copy it in buffer 
		read(sockfd, buff, sizeof(buff));

		stringstream ss;
		ss.str(buff);
		string sBuffer = ss.str();

		//tempVector.push_back(sBuffer);
		vector<city>::iterator it;
		int element = 0;

		it = find(tempVector.begin(), tempVector.end(), sBuffer);
		if (it != tempVector.end())
		{
			element = distance(tempVector.begin(), it);
				cout << "Weather report for " << tempVector[element].getCity() << endl;
				message = "Tomorrow's maximum temperature is " + to_string(tempVector[element].getDeg()) + "F\n"
					+ "Tomorrow's sky condition is " + tempVector[element].getFore() + "\n\n";
				cout << message;
		}
		else
		{
			cout << "Weather report for " << sBuffer << endl;
			message = "No data \n\n";
			cout << message;
		}

		bzero(buff, 256);
		for (int i = 0; i < message.length(); i++)
			buff[i] = message[i];

		// and send that buffer to client 
		write(sockfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended. 
		if (sBuffer == "exit") {
			printf("Server Exit...\n");
			break;
		}
	}
}



int main()
{

	vector<city> City;

	ifstream inFile;
	inFile.open("weather20.txt");

	while (!inFile.eof())
	{
		fill(City, inFile);
	}
	
	int portno, sockfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cli;
	struct hostent *hp;
	char myname[MAX + 1];

	cout << "Enter the server host name: ";
	cin >> myname;

	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	gethostname(myname, MAX);
	hp = gethostbyname(myname);
	if (hp == NULL)
		return (-1);

	printf("Enter server port number: ");
	scanf("%d", &portno);
	if ((portno < 2000) || (portno > 65535))
	{
		fprintf(stderr, "ERROR, invalid port number provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1)
	{
		cout << "socket creation failed...\n";
		exit(0);
	}
	else
		cout << "Socket successfully created...\n";
	bzero((char*)&servaddr, sizeof(servaddr));

	servaddr.sin_family = hp->h_addrtype;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(portno);

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
	if (connfd < 0) {
		cout << "server acccept failed...\n" << endl;
		return 0;
	}
	else
		cout << "server acccept the client...\n";


	// Function for chatting between client and server 
	func(connfd, City);

	// After chatting close the socket 
	close(connfd);
	return 0;

}
