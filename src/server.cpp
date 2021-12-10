#include "../header/common.hpp"
#include<pthread.h>

using namespace std;

int initAccNum = 100; //initial account number
vector<Record>records1;
pthread_mutex_t mx;

void* backendServer(void*);
int generateRandomNumber();
int isRecord(int, vector<Record>&);

/**
 * @brief Generate random number
 * @return integer random number between 0 and 2, inclusive.
*/
int generateRandomNumber(){
	time_t t;
    srand((unsigned)time(&t));
    return (((rand()%100)*7) % 3);
}

/**
 * @brief Checks if user exists in vector<User> by comparing account number
 * @param accNum integer account number
 * @param recordVec reference of vector<User>
 * @return index if user exists; -1 otherwise
*/
int isRecord(int accNum, vector<Record> &recordVec){
    int counter = 0;
    for(Record &rc : recordVec){
        if(rc.getAccountNumber() == accNum){
            return counter;
        }
        counter++;
    }
    return -1;
}

/**
 * @brief backend server functionality: Performs COMMIT, ABORT operations
 * 
 * @param arg 
 * @return void* 
 */
void* backendServer(void* arg){
    int clientSocket = *(int*)arg;
    while(true){
        char buffer[256], txnStatus[256], txnData[256], ackData[256];
        bzero(buffer, 256);
        bzero(txnStatus, 256);
        bzero(txnData, 256);
        bzero(ackData, 256);
        int recvVal, abortValue = -1;
        recvVal = recv(clientSocket, buffer, sizeof(buffer), 0);
        cout<<endl<<"CO-ORDINATOR:: "<<buffer<<endl;
        abortValue = generateRandomNumber();
        cout<<endl<<"SERVER:: Random number generated to show Abort:: "<<abortValue<<endl;
        if(abortValue == 0){
            cout<<endl<<"SERVER:: ABORT"<<endl;
            int sendVal = send(clientSocket, "ABORT", 256, 0);
        }
		if(recvVal != 0 && abortValue!=0){
            cout<<endl<<"SERVER:: READY"<<endl;
            int sendVal = send(clientSocket, "READY", 256, 0);
        }

        recvVal = recv(clientSocket, txnStatus, sizeof(txnStatus), 0);
        cout<<endl<<"CO-ORDINATOR::"<<txnStatus<<endl;

        if((strcmp(txnStatus, "ABORT")) == 0){
            bzero(buffer, 256);
            bzero(txnStatus, 256);
            bzero(ackData, 256);
            bzero(txnData, 256);
        } else if((strcmp(txnStatus, "ABORT")) != 0){
            int recvData = recv(clientSocket, txnData, sizeof(txnData), 0);
            cout<<endl<<"SERVER:: Received transaction details:: "<<txnData<<endl;
        }

		if(((strcmp(txnStatus, "COMMIT")) == 0) && ((strcmp(txnStatus, "ABORT")) != 0)){
            char *splitTxn;
            float balance;
            splitTxn = strtok(txnData, " ");
            if(splitTxn != NULL){
                //If TRANSACTION == CREATE
                if(strcmp(splitTxn, "CREATE") == 0){
                    pthread_mutex_lock(&mx);
                    balance = atof(strtok(NULL, " "));

                    Record* rc = new Record();
                    rc->setAccountNumber(initAccNum);
                    rc->setBalance(balance);
                    records1.push_back(*rc);

                    int newAccount = initAccNum;
                    initAccNum = initAccNum + 1;
                    sprintf(ackData, "OK %d", newAccount);
                    int sendVal = send(clientSocket, ackData, sizeof(ackData), 0);
                    pthread_mutex_unlock(&mx);
                    cout<<endl<<"SERVER:: Transaction Successfull!"<<endl;
                }
                //If TRANSACTION == UPDATE
                else if(strcmp(splitTxn, "UPDATE") == 0){
                    pthread_mutex_lock(&mx);
                    int accNum = atoi(strtok(NULL, " "));
                    float newAmount = atof(strtok(NULL, " "));
                    //Citation:: https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
                    auto it = find_if(records1.begin(), records1.end(), [&accNum](Record& obj) {return obj.getAccountNumber() == accNum;});
                    if(it != records1.end()){
                        // found element. it is an iterator to the first matching element.
                        auto index = distance(records1.begin(), it);
                        (*it).setBalance(newAmount);
                        sprintf(ackData, "OK %.2f", (*it).getBalance());
                        int sendVal = send(clientSocket, ackData, sizeof(ackData), 0);
                        cout<<endl<<"SERVER:: Transaction Successfull!"<<endl;
                    } else{
                        sprintf(ackData, "ERR:: Account %d does not exist", accNum);
                        int sendVal = send(clientSocket, ackData, sizeof(ackData), 0);
                        cout<<endl<<"SERVER:: Account "<<accNum<<" does not exist!"<<endl;
                        cout<<endl<<"SERVER:: Transaction unsuccessful!"<<endl;
                    }
                    pthread_mutex_unlock(&mx);
                }
                //If TRANSACTION == QUERY
                else if(strcmp(splitTxn, "QUERY") == 0){
                    pthread_mutex_lock(&mx);
                    int accNum = atoi(strtok(NULL, " "));
                    //Citation:: https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
                    auto it = find_if(records1.begin(), records1.end(), [&accNum](Record& obj) {return obj.getAccountNumber() == accNum;});
                    if(it != records1.end()){
                        // found element. it is an iterator to the first matching element.
                        // if you really need the index, you can also get it:
                        auto index = distance(records1.begin(), it);
                        cout<<"Index:: "<<index<<endl;
                        cout<<"Account Number:: "<<(*it).getAccountNumber()<<endl;
                        cout<<"Balance:: "<<(*it).getBalance()<<endl;

                        sprintf(ackData, "OK %.2f", (*it).getBalance());
                        int sendVal = send(clientSocket, ackData, sizeof(ackData), 0);
                        cout<<endl<<"SERVER:: Transaction Successfull!"<<endl;
                    } else{
                        sprintf(ackData, "ERR:: Account %d does not exist", accNum);
                        int sendVal = send(clientSocket, ackData, sizeof(ackData), 0);
                        cout<<endl<<"SERVER:: Account "<<accNum<<" does not exist!"<<endl;
                        cout<<endl<<"SERVER:: Transaction unsuccessful!"<<endl;
                    }
                    pthread_mutex_unlock(&mx);
                } 
                else{
                    sprintf(ackData, "ERR:: Invalid Transaction!");
                    int sendVal = send(clientSocket, ackData, sizeof(ackData), 0);
                    cout<<"SERVER:: Invalid Transaction!"<<endl;
                }
            }
        }
        bzero(ackData, 256);
    }
	close(clientSocket);
    return NULL;
}


int main(int argc, char** argv){
    // exit the program if invalid arguments are passed
	if(argc != 3 ){
        cout<<"ERROR: Invalid Arguments! Please run the program with args as ./<executable server file> <IP address> <server port>"<<endl;
        ::exit(EXIT_FAILURE);
    }

    /*socket variables*/
    int serverFd, serverSocketFd;
	int serverPort = stoi(argv[2]);
	int setReuseAddr = 1; // ON == 1  
	int maxPendingConnections = 1;
	char* serverIP = argv[1];
    //int* sock;
    int threadCount = 0;
    pthread_t t[999];

	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t clientAddrLen; 

    // create socket (IPv4, stream-based, protocol set to TCP)
	if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"ERROR:: Server failed to create the listening socket"<<endl;
		::exit(EXIT_FAILURE);
	}

	// set socket option to reuse the address passed in terminal args
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &setReuseAddr, sizeof(setReuseAddr)) != 0){
		cout<<"ERROR:: Server failed to set SO_REUSEADDR socket option"<<endl;
	}
	// set socket option to reuse the address passed in terminal args
	// wrote seperate if block fo SO_REUSEPORT because SO_REUSEADDR | SO_REUSEPORT in single if block was producing errors
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEPORT, &setReuseAddr, sizeof(setReuseAddr)) != 0){
		cout<<"ERROR:: Server failed to set SO_REUSEPORT socket option"<<endl;
	}

    // configure server socket address structure (init to zero, IPv4,
	// network byte order for port and address)
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(serverIP);
	server.sin_port = htons(serverPort); //use port specified in terminal args

	// bind the socket
	if(::bind(serverFd, (struct sockaddr*) &server, sizeof(server)) < 0){ //c++11
		cout<<"ERROR:: Server failed to bind"<<endl;
		::exit(EXIT_FAILURE);
	}

	// listen on the socket for up to some maximum pending connections
	if(listen(serverFd, maxPendingConnections) < 0){
		cout<<"ERROR:: Server failed to listen"<<endl;
		::exit(EXIT_FAILURE);
	} else{
		cout<<"SUCCESS:: Server listening for a connection on port:: "<<serverPort<<endl;
	}

	// get the size client's address struct
	clientAddrLen = sizeof(client);
    while((serverSocketFd = accept(serverFd, (struct sockaddr*) &client, &clientAddrLen))){
        // sock = new int;
        // *sock = serverSocketFd;
        pthread_create(&t[threadCount++], NULL, backendServer, (void*) &serverSocketFd);
    }
}
