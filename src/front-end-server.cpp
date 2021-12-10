#include "../header/common.hpp"
#include<pthread.h>

using namespace std;

int serverFd, clientSocketFd;
int portArr[3] = {0};
char* serverIP;

void* frontEndServer(void*);

void* frontEndServer(void* arg){
    int clientSocket = *(int*)arg;
    int totalServers = 0;
    int backEndServer[3] = {0}, serverConnectionPort[3] = {0}, serversOnline[3] = {0};
    serverConnectionPort[0] = htons(portArr[0]);
    serverConnectionPort[1] = htons(portArr[1]);
    serverConnectionPort[2] = htons(portArr[2]);
    for(int i=0; i<3; i++){
        struct sockaddr_in backEndServerAddr;
        bzero(&backEndServerAddr, sizeof(backEndServerAddr));
        backEndServerAddr.sin_family = AF_INET;
        backEndServerAddr.sin_addr.s_addr = inet_addr(serverIP);
        backEndServerAddr.sin_port = ((serverConnectionPort[i]));

        if((backEndServer[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		    cout<<"ERROR:: THREAD:: Front-end server cannot create socket from backend server"<<endl;
            return NULL;
	    }
		if(connect(backEndServer[i], (struct sockaddr *)&backEndServerAddr, sizeof(backEndServerAddr)) < 0){
			cout<<"ERROR:: Front-end server failed to connect to backend server:: "<<serverIP<<" : "<<serverConnectionPort[i]<<endl;
			close(backEndServer[i]);
			return NULL;
		} else {
			cout<<"SUCCESS:: Front-end server connected to backend server:: "<<serverIP<<" : "<<serverConnectionPort[i]<<endl;
			serversOnline[i] = 1;
			totalServers++;
		}
    }

	 while(true){
        char txnMsg[256];
        char txnStatus[256], buffer1[256];
        bzero(txnMsg, 256);
        bzero(buffer1, 256);
        int count = 0, abortCount = 0;
        //Receive transaction details from client
        if((recv(clientSocket, txnMsg, sizeof(txnMsg), 0)) < 0){
            cout<<"ERROR:: Could not receive data"<<endl;
        }
        cout<<endl<<"Transaction to be processed: "<< txnMsg << endl;
        if(strcmp(txnMsg, "QUIT") == 0){
            if((send(clientSocket, "QUIT", sizeof(4), 0))<0){
                cout<<"ERROR:: Could not send the data"<<endl;
            }
            close(clientSocket);
            break;
        }

        //Send VOTE to backend servers to get their status
        for(int i=0; i<3; i++){
            if(serversOnline[i] == 1){
                if((send(backEndServer[i], "VOTE", 256, 0)) < 0){
                    cout<<"ERROR:: Could not send the data"<<endl;
                }
            }
        }

        //Recieve status reply from the backend servers
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        memset(serversOnline, 0, sizeof(serversOnline));
        //
        for(int i=0; i<3; i++){
            if(setsockopt(backEndServer[i], SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
                cout<<"ERROR:: Timeout"<<endl;
                return 0;
            }
            while(recv(backEndServer[i], buffer1, 256, 0) > 0){
                if(strcmp(buffer1, "ABORT") == 0){
                    abortCount++;
                }
                count++;
                serversOnline[i] = 1;
            }
        }
        if(abortCount > 0){
            char abortMsg[] = "ABORT";
            for(int i = 0; i < 3; i++){
                send(backEndServer[i], abortMsg, sizeof(abortMsg), 0);
            }
            int sendVal = send(clientSocket, abortMsg, sizeof(abortMsg), 0);
            cout<<"Transaction Aborted"<<endl;
        } else{
            cout<<"Active backend servers:: "<<count;
            if(count == 0){
                cout<<"No active backend servers"<<endl;
                sprintf(txnStatus, "ERR:: No Server is active");
                int sendVal = send(clientSocket, txnStatus, sizeof(txnStatus), 0);
            } else{
                if(count <= totalServers){
                    char buffer2[256];
                    bzero(buffer2, 256);
                    int j = 0;
                    while(recv(backEndServer[j], buffer2, 256, 0) != 0){
                        if(j == 3){
                            break;
                        }
                        if(serversOnline[j] == 1){
                            int sendVal = send(backEndServer[j], "COMMIT", 256, 0);
                            totalServers++;
                        }
                        j++;
                    }
                    //char buffer3[256];
                    bzero(buffer2, 256);
                    totalServers = 0;
                    for (int i=0; i<3; i++){
                        if(recv(backEndServer[i], buffer2, 256, 0) != 0){
                            if(serversOnline[i] == 1){
                                int sendVal = send(backEndServer[i], txnMsg, sizeof(txnMsg), 0);
                                totalServers++;
                            }
                        } else{
                            serversOnline[i] = 0;
                        }
                    }

                    //Receive the updated transaction from the backend servers
                    totalServers = 0;

                    bzero(txnStatus, 256);
                    for(int i=0; i<3; i++){
                        if(recv(backEndServer[i], txnStatus, 256, 0) != 0){
                            if(serversOnline[i] == 1){
                                totalServers++;
                            }
                        } else{
                            serversOnline[i] = 0;
                        }
                    }
                    cout<<endl;
                    int sendVal = send(clientSocket, txnStatus, sizeof(txnStatus), 0);
                }
            }
            bzero(txnMsg, 256);
            bzero(buffer1, 256);
        }
    }
    close(clientSocket);
    return NULL;
}

int main(int argc, char** argv){
    // exit the program if invalid arguments are passed
	if(argc != 3 ){
		cout<<"ERROR: Invalid Arguments! Please run the program with args as ./<exe file> <IP address> <front-end-server port>"<<endl;
		exit(EXIT_FAILURE);
	}

	string portsFilePath = "portsFile.txt"; //backend-servers ports
    ifstream portsFd;
    //open ports file and read the records
    if(openFile(portsFd, portsFilePath) == 1){
        cout<<"SUCCESS:: Ports file opened!!"<<endl;
        string currLine;
        int i = 0;
        do{ 
            portsFd>>currLine;
            if(!currLine.empty()){
                portArr[i] = stoi(currLine);
                i++;
            }
            getline(portsFd,currLine);
        }while(!portsFd.eof());
    }
    else{
		cout<<"ERROR:: File is either not opened or file is empty!!"<<endl;
        cout<<"ERROR:: Exiting the program. First load the records file!"<<endl;
        exit(EXIT_FAILURE);
    }

	int setReuseAddr = 1; // ON == 1  
	int maxPendingConnections = 1;
	serverIP = argv[1];
    int serverPort = stoi(argv[2]);
    int threadCount = 0;
    pthread_t t[999] = {0};

	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t clientAddrLen; 

    //int* sock;

    // create socket (IPv4, stream-based, protocol set to TCP)
	if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"ERROR:: Front-end server failed to create the listening socket"<<endl;
		exit(EXIT_FAILURE);
	}

    // set socket option to reuse the address passed in terminal args
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &setReuseAddr, sizeof(setReuseAddr)) != 0){
		cout<<"ERROR:: Front-end server failed to set SO_REUSEADDR socket option"<<endl;
	}

	// set socket option to reuse the address passed in terminal args
	// wrote seperate if block fo SO_REUSEPORT because SO_REUSEADDR | SO_REUSEPORT in single if block was producing errors
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEPORT, &setReuseAddr, sizeof(setReuseAddr)) != 0){
		cout<<"ERROR:: Front-end server failed to set SO_REUSEPORT socket option"<<endl;
	}

    // configure server socket address structure (init to zero, IPv4,
	// network byte order for port and address)
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(serverIP);
	server.sin_port = htons(serverPort); //use port specified in terminal args

    // bind the socket
	if(::bind(serverFd, (struct sockaddr*) &server, sizeof(server)) < 0){ //c++11
		cout<<"ERROR:: Front-end server failed to bind"<<endl;
		exit(EXIT_FAILURE);
	}

    // listen on the socket for up to some maximum pending connections
	if(listen(serverFd, maxPendingConnections) < 0){
		cout<<"ERROR:: Front-end server failed to listen"<<endl;
		exit(EXIT_FAILURE);
	} else{
		cout<<"SUCCESS:: Front-end server listening for a connection on port:: "<<serverPort<<endl;
	}

    // get the size client's address struct
	clientAddrLen = sizeof(client);

    while((clientSocketFd = accept(serverFd, (struct sockaddr*) &client, &clientAddrLen))){
        cout<<"SUCCESS:: Front-end server accepted a client"<<endl;
        // sock = new int;
        // *sock = clientSocketFd;
        pthread_create(&t[threadCount++], NULL, frontEndServer, (void*) &clientSocketFd);
    }
    return 0;
}