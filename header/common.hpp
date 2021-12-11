#ifndef COMMON_HPP
#define COMMON_HPP

#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string>
#include<string.h>
#include<map>
#include<vector>
#include<fstream>
#include<algorithm>

using namespace std;
string userInput();
int openFile(ifstream&, string);

class Record{
    private:
        int _accNum;
        float _balance;
    public:
        //constructor
        Record();
        //getters and setters
        void setAccountNumber(int const &);
        int getAccountNumber();
        void setBalance(float const &);
        float getBalance();
        //destructor
        ~Record();
};

#endif