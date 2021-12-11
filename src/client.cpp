#include "../header/common.hpp"

using namespace std;

int clientFd;

void sendTxnToFrontServer();

/**
 * @brief Sends transaction details to a front-end server.
 * Transaction Details: CREATE <initial deposit to account num 100>, 
 * QUERY <account number>, 
 * UPDATE <account number> <new amount>, 
 * QUIT
 */
void sendTxnToFrontServer(){
    char message[256];
    char ans;
    while(true){
        bzero(message, 256);
        //accept transaction details from user
        cout<<endl;
        string retVal = userInput();
        cout<<"retVal:: "<<retVal<<endl;
        strcpy(message, retVal.c_str());
        if(send(clientFd, message, sizeof(message), 0) < 0){
            cout<<"ERROR:: Cannot send transaction details to front-end server!"<<endl;
        }
        else{
            cout<<"SUCCESS:: Transaction details sent to front-end server"<<endl;
            bzero(message, 256);
            if(recv(clientFd, message, sizeof(message), 0) < 0){
                cout<<"ERROR:: Cannot receive response from front-end server!"<<endl;
            }
            else{
                if(strcmp(message, "QUIT") == 0){
                    cout<<"OK"<<endl;
                    close(clientFd);
                    break;
                }
                if(strcmp(message, "ABORT") == 0){
                    cout<<"ABORT"<<endl;
                    cout<<"Transaction aborted!"<<endl;
                }
                else{
                    cout<<message<<endl;
                }
            }
        }
        // cout<<endl<<"Do you want to continue? (Y/N)";
        // cin>>ans;
        // if(toupper(ans) != 'Y'){
        //     cin.clear();
        //     exit(EXIT_SUCCESS);
        // }
    }
}

int main(int argc, char** argv){

    // exit the program if invalid arguments are passed
	if(argc != 3 ){
		cout<<"ERROR: Invalid Arguments! Please run the program with args as ./<exe file> <IP address> <front-end-server port>"<<endl;
		exit(EXIT_FAILURE);
	}

	int serverPort = stoi(argv[2]);
	char* serverAddress = argv[1];
	struct sockaddr_in serverSocketAddr;

    // Create socket (IPv4, stream-based, protocol set to TCP)
	if((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"ERROR:: Client failed to create socket"<<endl;
		exit(EXIT_FAILURE);
	}

    // Configure server socket address structure (init to zero, IPv4,
	// network byte order for port and address) 
	bzero(&serverSocketAddr, sizeof(serverSocketAddr));
	serverSocketAddr.sin_family = AF_INET;
	serverSocketAddr.sin_port = htons(serverPort);
	serverSocketAddr.sin_addr.s_addr = inet_addr(serverAddress);

    // Connect socket to server
	if(connect(clientFd, (struct sockaddr *)&serverSocketAddr, sizeof(serverSocketAddr)) < 0){
		cout<<"ERROR:: Client failed to connect to frontend server:: "<<serverAddress<<" : "<<serverPort<<endl;
		close(clientFd);
		exit(EXIT_FAILURE);
	} else {
		cout<<"SUCCESS:: Client connected to frontend server:: "<<serverAddress<<" : "<<serverPort<<endl;
        sendTxnToFrontServer();
    }

    // Close the socket and return 0
	close(clientFd);
    return 0;
}