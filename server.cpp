#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <pthread.h>
using namespace std;

void fireman(int) //the fireman process implemented to handle zombie process (when a process is created in UNIX using fork() system call, the parent process is cloned.)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);    
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman); 
    if (argc < 2)
    {
        cerr << "ERROR, no port provided\n";  //if no port number is provided when executing the server it will display an error message.
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //creating an empty socket with no port bound to it. We need to connect this empty socket to the port number we are going to use.
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);                         //atoi converts a character string to an integer value. Store the integer value inside a variable.
    serv_addr.sin_family = AF_INET;                 //used for populating the empty socket with AF_INET protocol.
    serv_addr.sin_addr.s_addr = INADDR_ANY;         //any address from the internet can connect to the server.
    serv_addr.sin_port = htons(portno);             //used to assign the port using htons function.
    if (bind(sockfd, (struct sockaddr *)&serv_addr, //connecting the empty socket with the port number given.
             sizeof(serv_addr)) < 0)
     {
        std::cerr << "ERROR on binding";
        exit(1);
    }
    listen(sockfd, 5);  //listen() is a function that tells the server the maximum number of connections that can handle, in this case 5. 
    clilen = sizeof(cli_addr);
    while (true)
    {
        /*at this point you accept the connection from the client and create a new socket that will send values to the client*/

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen); 
                                                                                        
        if (fork() == 0) //if the process is a child process then proceed with the rest of the code.
        {
            if (newsockfd < 0)
            {
                cerr << "ERROR on accept"; 
                exit(1);
            }
            double probability;
            n = read(newsockfd, &probability, sizeof(double)); //read the probability passed from the client.
            if (n < 0)
            {
                cerr << "ERROR reading from socket";
                exit(1);
            }

            double fx;
            n = read(newsockfd, &fx, sizeof(double)); //read the fx calculation passed from the client.
            if (n < 0)
            {
                cerr << "ERROR reading from socket";
                exit(1);
            }


            auto length = ceil(log2(1/probability)) + 1;           //chooses the length of the encoding of the code. The formula comes from the Shannon–Fano–Elias coding algorithm.

            auto integer = fx * 256;                               //convert fx to an integer for easier transformation to binary.

            string binary = std::bitset<8>(integer).to_string();   //converts to binary.
     
            string final_code = binary.substr(0, length);          //store our result in a string.


            char message[final_code.length() + 1];                 //convert the string to a *char[].
            strcpy(message, final_code.c_str());                   //copy the final code to the message array.

            int sMessage = strlen(message);                        //get the size of our message,
            n = write(newsockfd, &sMessage, sizeof(int));          //write the size of the message into the socket,
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, message, sMessage);              //write the message to the socket,
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
   
            close(newsockfd); //close socket.
            
            _exit(0); //exits the child process.
        }
    }
    close(sockfd); //close socket.
    return 0;
}