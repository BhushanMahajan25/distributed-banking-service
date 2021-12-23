# Implementation

## 2-phase commit protocol:
To ensure correctness of and fault tolerance in a distributed database system, the protocol breaks database commit into 2 phases. Coordinator is responsible to manage the commits to database system.

PHASE-1:

For the sake of simplicity, coordinator sends VOTE to slave servers
after receiving transaction details from the client. Slave servers respond to VOTE by replying either READY or ABORT to the coordinator. In this implementation, to show ABORT scenario random number is generated using UNIX Epoch. The epoch is divided by 4 and if the remainder is 0 then server sends ABORT to the coordinator; else, the remainder is other than 0, then server sends READY to the coordinator. (NOTE: Since, occurrence of random number is between 0 and 3, both inclusive, it may happen that you get ABORT many times, however, you may get READY scenario more than ABORT.) If any of the slave servers sends ABORT to the coordinator then, coordinator sends global ABORT to all other active slave servers and the transaction is aborted. In the end, client is notified with aborted transaction.

PHASE-2:
After receiving READY from active slave servers, coordinator sends
COMMIT to active slave servers. Each active slave server applies the transaction and sends OK acknowledgement to the coordinator. Coordinator sends this acknowledgement further to the client.

1. Client
    
    Client has implementation of menu in which user is given 4 options:
   1. CREATE, 2. UPDATE, 3. QUERY, 4. QUIT.
    
    This menu is implemented using SWITCH CASE. After selecting one of the above options, user is asked to enter initial deposit amount in case of option 1, or account number and new amount in case of option 2, or account number in case of option 3. If user selects option 4, then client gets closed.
    
    After selecting menu, client sends transaction details to frontend server.

    NOTE: Initial account number is 100 (hardcoded). Account number gets incremented each time whenever option CREATE is opted.

2. Frontend server
 
    Frontend server acts as COORDINATOR in 2-phase commit protocol. 
    
    The frontend server performs phase-1 of the above 2-phase commit protocol.

    During PHASE-2, frontend server also implements simple fault tolerance mechanism by keeping track of active backend server. If there at least an active slave server then the coordinator sends COMMIT to slave server.
3. Backend servers

    Backend servers act as slave servers (database stores) in 2-phase commit protocol. The servers respond READY, or ABORT based on random number generated using UNIX Epoch % 4. Also, to handle fault tolerance, data is replicated in every backend server. Backend server keeps transaction details- account number and account balance – in object of Record class. After either ABORT or COMMIT, the active servers send acknowledgement to the coordinator.