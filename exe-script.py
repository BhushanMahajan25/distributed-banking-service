import os
import sys

n = len(sys.argv)
if n < 3:
    sys.exit("Invalid number of args!")

backend_server_count = 3
file_name = "portsFile.txt"
ip_address = sys.argv[1]
frontend_server_port = int(sys.argv[2])

print("Frontend server's port is :: {}".format(frontend_server_port))

f = open(file_name, 'w')

for i in range(backend_server_count):
    frontend_server_port += 1
    print("backend server-{}'s port number :: {}".format(i+1, frontend_server_port))
    f.write(str(frontend_server_port))
    f.write("\n")
f.close()

# start backend servers
frontend_server_port = int(sys.argv[2])
for i in range(backend_server_count):
    frontend_server_port += 1
    cmd = '''
        gnome-terminal -- bash -c "./bin/server {} {}; exec bash"
    '''.format(ip_address, frontend_server_port)
    os.system(cmd)

#start frontend server
frontend_server_port = int(sys.argv[2])
print("frontend server's port number :: {}".format(frontend_server_port))
cmd = '''
        gnome-terminal -- bash -c "./bin/front {} {}; exec bash"
    '''.format(ip_address, frontend_server_port)
os.system(cmd)

#start client
print("client connecting to port number :: {}".format(frontend_server_port))
cmd = '''
        gnome-terminal -- bash -c "./bin/client {} {}; exec bash"
    '''.format(ip_address, frontend_server_port)
os.system(cmd)

os._exit(0)
