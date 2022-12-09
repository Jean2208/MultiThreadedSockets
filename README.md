# Welcome!

This project is a Multithreaded program with interprocess communication using sockets.

It uses a version of the Shannon-Fano-Elias code algorithm generator. You can find more information about this algorithm at this website: https://en.wikipedia.org/wiki/Shannon%E2%80%93Fano%E2%80%93Elias_coding

## What does the code do?

Given an input file, such as the one provided in this repository, you can decode your string of symbols to a binary [prefix code](https://en.wikipedia.org/wiki/Prefix_code "Prefix code").

The input has a single line with the message (string or char array).

**_Example Input File:_** *AAAAAABBBBBCCCCDDDEE*

Given the previous input, the expected output is:  

*SHANNON-FANO-ELIAS Codes:*\

*Symbol A, Code: 001*\
*Symbol B, Code: 011*\
*Symbol C, Code: 1010*\
*Symbol D, Code: 1101*\
*Symbol E, Code: 11110*

## How do I use the code?
This program doesn't execute using simple (normal) compile command because this is a [multithreaded program](https://www.techtarget.com/whatis/definition/multithreading#:~:text=Multithreading%20is%20the%20ability%20of,requests%20from%20the%20same%20user.) using **pthread.h** library. We can use **g++** and **-lpthread** for compiling our C++ code (Linux OS preferred).

For the client file:

    g++ client.cpp -o client -lpthread

And for the server:

    g++ server.cpp -o server -lpthread

After having both our programs compiled lets give these two executable permissions.

For the client:

    chmod +x client

And for the server:

    chmod +x server

All set. We can now run our programs, make sure the server is running before executing the client:

    ./server <port_number>

Example:

    ./server 1234

And now the client:

    ./client <server_ip_address> <port_number> < <input_file_name>

Example:

     ./client 127.0.0.1 1234 < input.txt

## What is the Shannon-Fano-Elias code algorithm?

**Shannon–Fano–Elias coding** is a precursor to [arithmetic coding](https://en.wikipedia.org/wiki/Arithmetic_coding "Arithmetic coding"), in which probabilities are used to determine codewords. It produces a binary [prefix code](https://en.wikipedia.org/wiki/Prefix_code "Prefix code"), allowing for direct decoding. 

Given a [discrete random variable](https://en.wikipedia.org/wiki/Discrete_random_variable#Discrete_probability_distribution "Discrete random variable")  _X_ of [ordered](https://en.wikipedia.org/wiki/Total_order "Total order") values to be encoded, let p(x) be the probability for any x in X.

![enter image description here](https://i.ibb.co/zFqNj1R/Shanon.jpg) 

