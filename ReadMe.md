# Distributed Banking Service

This project implements distributed banking service to show how 2-phase commit protocol, and simple fault torelence mechanism work.

## Steps to execute the project 
1.	Build the project and generates server and client executables
    - ``make compile``
2.	Start the backend servers first.
    - ``./bin/server <IP ADDRESS> <BACKEND PORT>``

    NOTE: Before starting the servers, make sure you have entered port numbers in ***portsfile.txt***.
3.	Start the frontend server first.
    - ``./bin/front <IP ADDRESS> <FRONTEND PORT>``
4.	Run client
    - ``./bin/client <IP ADDRESS> <FRONTEND PORT>``
5.	To clean all object and executable files
    - make clean

OR

> You can also use ``exe-script.py`` to run the project. This script will create 5 new terminals- 1 for client, 1 for frontend server, and 3 for backend servers. Above all shell commands will get executed automatically thus starting client, frontend server and backend servers. After running this script successfully, you will MENU in client’s terminal.

> NOTE: This script creates portsfile.txt with backend server ports.
	
> ### Steps to run the script:
1.	``make compile``
2.	``python3 exe-script.py <IP ADDRESS> <FRONTEND PORT>``

> Example: If you take frontend server’s port=8080 then the script will create port numbers 8081, 8082, 8083 for 3 backend servers. IP ADDRESS will remain same for each.
(Python version used: 3.8.9)
3. ``make clean``

 [Implementation.md](https://github.com/BhushanMahajan25/distributed-banking-service/blob/main/Implementation.md) explains the working of multi-threading in detail.