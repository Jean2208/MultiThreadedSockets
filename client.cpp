#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
using namespace std;


struct arguments
{
   char* hostName;      //passes the hostname to pthread function.
   char* portNumber;    //passes the port number to pthread function.
   int threadNumber ;   //passes the number of the thread to pthread function.
   double probability;  //passes each of the probabilities to pthread function.
   double fx;           //passes the result of fx to pthread function.
   string result;       //stores the finished code coming from the server.
};



char *removeDuplicate(char str[], int n) //function to remove the duplicates characters from the input string.
{
   //used as index in the modified string
   int index = 0;   
     
   //traverse through all characters
   for (int i=0; i<n; i++) 
   {
      //check if str[i] is present before it  
     int j;  
     for (j=0; j<i; j++) 
        if (str[i] == str[j])
           break;
       
     //if not present, then add it to result
     if (j == i)
        str[index++] = str[i];
   }
     
   return str;
}

vector<double> CalcProbabilities(string str) //this function first calculates the frequency of each symbol from the input string and then the probability of the symbol from its frequency.
{	
   int i;
   int freq[256] = {0};
   vector<int> frequencies;
    
   //calculating the frequency of each character.
   for(i = 0; str[i] != '\0'; i++)
   {
        freq[str[i]]++;
   }
    
   //pushing back the frequency of each character to frequencies vector.
   for(i = 0; i < 256; i++)
   {
      if(freq[i] != 0)
      {
         frequencies.push_back(freq[i]);
      }
   }

   vector<double> probabilities;
    
   //calculating the probabilities of each character using its frequency.
   for (i=0; i < frequencies.size(); i++) 
   {
    double probability = (double)frequencies[i]/str.size();
    probabilities.push_back(probability);
   }

   return(probabilities); //returning a vector with all the probabilities.
}

vector<double> CalcFx(vector<double> probabilities) //calculates fx using the probabilities. The formula comes from the Shannon–Fano–Elias coding algorithm.
{
   vector<double> fx;
   vector<double> cumulative_sum;
   int i;

   for (i=0; i < probabilities.size(); i++)
   {
    double x = probabilities[i] * 0.5;                                                   //multiply the probability by 0.5.
    double sum = x + std::accumulate(cumulative_sum.begin(), cumulative_sum.end(), 0.0); //you must get the sum of the previous probabilities from the one you'r calculating.
    cumulative_sum.push_back(probabilities[i]);                                          //push back the sum of the probabilities to vector.
    fx.push_back(sum);
   }

   return(fx); //returns the vector of fx results.
}




void *ToServer(void *x_void_ptr) //this function converts all our probabilities into binary code by using sockets (called by pthread). 
{
    
   struct arguments *pos_ptr = (arguments *) x_void_ptr; //declare our struct pointer to store its values inside the function.

   double prob = pos_ptr->probability;       //get the probability from the struct.
   char* portnumber = pos_ptr->portNumber;   //get the port number from the struct.
   char* hostname = pos_ptr->hostName;       //get the host name from the struct.
   double fx = pos_ptr->fx;                  //get the fx result from the struct.


   int sockfd, portno, n;
   struct sockaddr_in serv_addr;             //this struct is going to get populated to pass information about the server.
   struct hostent *server;                   //we cannot assume that we will always receive an ip address, we could instead receive a name such as 'localhost'.


   portno = atoi(portnumber);                   //atoi converts a character string to an integer value. Store the integer value inside a variable.
   sockfd = socket(AF_INET, SOCK_STREAM, 0);    //create an empty socket to start the communication.
   if (sockfd < 0)                              
   {
      cerr << "ERROR opening socket";           //if the creation of the socket was unsuccessful display an error message.
   }
   
   server = gethostbyname(hostname);            //get the information about the server (like the ip address).
   if (server == NULL)                          
   {
     std::cerr << "ERROR, no such host\n";      //if the host is invalid display an error message.
     exit(0);
   }                                                          
    
   /*for this next part we write/read values to/from the socket.*/

   bzero((char *)&serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr,
      (char *)&serv_addr.sin_addr.s_addr,
      server->h_length);                                                          
   serv_addr.sin_port = htons(portno);
   if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)   //connect to the socket.
   {
     std::cerr << "ERROR connecting"; //if the connection to the socket was unsuccessful display an error message.
     exit(1);
   } 


   n = write(sockfd, &prob, sizeof(double)); //write the probability to the socket.
   n = write(sockfd, &fx, sizeof(double));   //write fx to the socket.
   if (n < 0)
   {
     std::cerr << "ERROR writing to socket"; //if writing to the socket was unsuccessful display an error message.
     exit(1);
   }


   int size;
   n = read(sockfd, &size, sizeof(int)); //reads the size of the message from the socket to use it later as a buffer.
   if (n < 0)
   {
      std::cerr << "ERROR reading from socket"; //if reading from the socket was unsuccessful display an error message.
      exit(1);
   }


   char *buffer = new char[size + 1]; //store the message coming from the server as a buffer type *char[]
   bzero(buffer, size + 1);           //the bzero() function erases the data in the n bytes of the memory starting at the location pointed to by s, by writing zeros.
   n = read(sockfd, buffer, size);    //read the buffer from the socket (the binary code calculated on the server).


   string final_result(buffer);     //our final binary code will be stored in the variable string final_result.
   pos_ptr->result = final_result;  //pass the results to the struct for the main thread to access them.
   delete [] buffer;                //delete buffer to deallocate memory from heap.
   close(sockfd);                   //close the socket.
   
   
   return NULL;

}
    

int main(int argc, char *argv[])
{

   string str;             
   getline(cin, str); //receives the input line of symbols.
	 
   sort(str.begin(), str.end()); //sorting the string
    
   /*making the string a char[]*/
	char arr[str.length() + 1]; 
	strcpy(arr, str.c_str());

	/*deleting duplicates from sorted char/string*/
	int n = sizeof(arr) / sizeof(arr[0]);
   string alphabet = removeDuplicate(arr, n); 

   /*storing the symbols inside array*/
   char symbols[alphabet.length() + 1]; 
   strcpy(symbols, alphabet.c_str());

   
   vector<double> probabilities = CalcProbabilities(str); //calculate the probability of each character.
   vector<double> fx = CalcFx(probabilities);             //calculate fx using the Shannon–Fano–Elias coding algorithm formula. 


   int NUMBER_OF_THREADS = alphabet.size(); //the number of threads will be equal to the amount of non-duplicated symbols.
   pthread_t tid[NUMBER_OF_THREADS];        //pthread initializer with number of threads as size.   
   arguments arg[NUMBER_OF_THREADS];        //call struct with indexes equal to the number of threads.


   for (int i=0; i < NUMBER_OF_THREADS; i++)
   {
      arg[i].hostName = argv[1];               //passing hostname to the struct.
      arg[i].portNumber = argv[2];             //passing the port number to the struct.
      arg[i].threadNumber = i;                 //passing the number of the operation/thread to the struct.
      arg[i].fx = fx[i];                       //passing the calculated fx values to the struct.
      arg[i].probability = probabilities[i];   //passing the probabilities to the struct.

      pthread_create(&tid[i], nullptr, ToServer, &arg[i]); //create the pthread and call the ToServer function with the corresponding parameters. 
   }

   wait(nullptr);

    
   for (int i=0; i < NUMBER_OF_THREADS; i++)
   {
      pthread_join(tid[i], nullptr); //pthread_join() function provides a simple mechanism allowing an application to wait for a thread to terminate.
   }


   /*print our code*/

   cout << "SHANNON-FANO-ELIAS Codes:" << "\n" "\n";

   for (int i=0; i < NUMBER_OF_THREADS; i++) 
   {
      cout << "Symbol " << symbols[i] << ", Code:" << arg[i].result << endl;
   }


   return 0;

}
