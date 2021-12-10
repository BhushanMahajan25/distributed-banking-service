#include "../header/common.hpp"

using namespace std;

string userInput(){
    int opt;
    string amt;
    string accNo;
    string retVal;
    cout<<"******* MENU *******"<<endl;
    cout<<"1. CREATE"<<endl;
    cout<<"2. UPDATE"<<endl;
    cout<<"3. QUERY"<<endl;
    cout<<"4. QUIT"<<endl;
    cout<<"Select transaction type: ";
    cin>>opt;
    switch(opt){
        case 1 :
            cout<<"Selected transaction type: CREATE"<<endl;
            cout<<"Enter transaction amount: ";
            cin.ignore();
            getline(cin,amt);
            retVal = "CREATE "+amt;
            amt.clear();
            break;
        case 2 :
            cout<<"Selected transaction type: UPDATE"<<endl;
            cout<<"Enter account number: ";
            cin.ignore();
            getline(cin,accNo);
            cout<<"Enter new amount: ";
            getline(cin, amt);
            retVal = "UPDATE "+accNo+" "+amt;
            amt.clear();
            accNo.clear();
            break;
        case 3 :
            cout<<"Selected transaction type: QUERY"<<endl;
            cout<<"Enter account number: ";
            cin.ignore();
            getline(cin,accNo);
            retVal = "QUERY "+accNo;
            accNo.clear();
            break;
        case 4 :
            cout<<"Selected transaction type: QUIT"<<endl;
            retVal = "QUIT";
            break;
        default:
            cout<<"Invalid option!"<<endl;
            break;
    }
    cout<<endl;
    return retVal;
}

Record::Record(){}

void Record::setAccountNumber(int const &accNum){
    this->_accNum = accNum;
}

int Record::getAccountNumber(){
    return this->_accNum;
}

void Record::setBalance(float const &balance){
    this->_balance = balance;
}

float Record::getBalance(){
    return this->_balance;
}

Record::~Record(){}

/** @brief Checks if file is empty or not
 *  @param pFile reference of file descriptor of type istream
 *  @return true if file is empty; false otherwise 
*/
// This function is cited from "https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c"
bool is_file_empty(ifstream& pFile){
    return pFile.peek() == ifstream::traits_type::eof();
}

/** @brief Opens a file in read mode
 *  @param fileDesc reference of file descriptor
 *  @param fileName string of file name
 *  @return 1 if file is opened successfully and the file is not empty; -1 otherwise
 */
int openFile(ifstream& fileDesc, string fileName){
    fileDesc.open(fileName, ios::in);
    if(fileDesc.is_open() && !is_file_empty(fileDesc)){
        return 1;
    }
    return -1;
}


bool sendMessage(int socketFd, const char* message){
    int value = send(socketFd, message, sizeof(message), 0);
    if(value <= 0){
        cout<<"ERROR:: Cannot send the data!"<<endl;
        return false;
    }
    return true;
}

char* receiveMessage(int socketFd){
    //char message[256];
    //bzero(message, 256);
    char* message = (char*)malloc(256*sizeof(char));
    size_t bytesRead;
    int messageSize = 0;

    while((bytesRead = recv(socketFd, message+messageSize, sizeof(message)-messageSize-1, 0)) > 0){
        messageSize += bytesRead;
        if(messageSize > 255 || message[messageSize-1] == '\n')
            break;
    }

    if(bytesRead < 0){
        cout<<"ERROR:: Cannot receive data!"<<endl;
        return 0;
    }
    cout<<"message in services is:: "<<*message<<endl;
    return message;
}

char* receiveMessage2(int socketFd){
    //char message[256];
    //bzero(message, 256);
    char* message = (char*)malloc(256*sizeof(char));
    int value = recv(socketFd, message, sizeof(message), 0);
    if(value < 0){
        cout<<"ERROR:: Cannot receive data!"<<endl;
        return 0;
    }
    cout<<"message in services is:: "<<*message<<endl;
    return message;
}

