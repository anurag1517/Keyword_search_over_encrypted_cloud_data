#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>     

using namespace std;

vector<string> received_files;

// Class for handling server connection
class server_connection {
private:
    int serverPort;
    string serverAddress;

public:
    server_connection(const string& address, int port) : serverAddress(address), serverPort(port) {}

    // Function to send files to the server
    bool sendFiles(const vector<string>& fileNames) {
        // Create socket
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Error creating socket." << endl;
            return false;
        }

        // Server address
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

        // Connect to server
        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            cerr << "Error connecting to server." << endl;
            close(clientSocket);
            return false;
        }

        // Send files
        for (const string& fileName : fileNames) {
            ifstream file(fileName, ios::binary);
            if (!file.is_open()) {
                cerr << "Error opening file: " << fileName << endl;
                continue;
            }

            // Send file name length and file name
            uint32_t nameLen = fileName.length();
            nameLen = htonl(nameLen);
            send(clientSocket, &nameLen, sizeof(nameLen), 0);
            send(clientSocket, fileName.c_str(), fileName.length(), 0);

            // Send file contents
            stringstream buffer;
            buffer << file.rdbuf();
            string fileContents = buffer.str();
            uint32_t fileLen = fileContents.length();
            fileLen = htonl(fileLen);
            send(clientSocket, &fileLen, sizeof(fileLen), 0);
            send(clientSocket, fileContents.c_str(), fileContents.length(), 0);

            file.close();
        }

        // Close socket
        close(clientSocket);
        return true;
    }

    // Function to send queries to the server
    bool send_query(const vector<string>& querys) {
        // Create socket
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Error creating socket." << endl;
            return false;
        }

        // Server address
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

        // Connect to server
        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            cerr << "Error connecting to server." << endl;
            close(clientSocket);
            return false;
        }

        // sending query size
        uint32_t queryLen = querys.size();
        queryLen = htonl(queryLen);
        send(clientSocket, &queryLen, sizeof(queryLen), 0);

        // Send queries
        for (const string& query : querys) {
            // Send query length and query
            uint32_t queryLen = query.length();
            queryLen = htonl(queryLen);
            send(clientSocket, &queryLen, sizeof(queryLen), 0);
            send(clientSocket, query.c_str(), query.length(), 0);
        }

        receive_files(clientSocket);

        // Close socket
        close(clientSocket);
        return true;
    }

    // Function to receive files from the server
    void receive_files(int clientSocket) {
        uint32_t files_size;
        recv(clientSocket, &files_size, sizeof(files_size), 0);
        files_size = ntohl(files_size);

        while (files_size--) {
            // Receive file name length
            uint32_t nameLen;
            recv(clientSocket, &nameLen, sizeof(nameLen), 0);
            nameLen = ntohl(nameLen);

            // Receive file name
            char fileName[nameLen + 1];
            recv(clientSocket, fileName, nameLen, 0);
            fileName[nameLen] = '\0';

            received_files.push_back(string(fileName));

            // Receive file content length
            uint32_t fileLen;
            recv(clientSocket, &fileLen, sizeof(fileLen), 0);
            fileLen = ntohl(fileLen);

            // Receive file content
            char fileContents[fileLen + 1];
            recv(clientSocket, fileContents, fileLen, 0);
            fileContents[fileLen] = '\0';

            // Write received file content to file
            ofstream outFile(string(fileName) + ".dat", ios::binary);
            outFile.write(fileContents, fileLen);
            outFile.close();

            cout << "Received file: " << fileName << endl;
        }
    }
};

// Function to send files or queries to the server
int sendFiles_to_server(vector<string>& content, bool flag) {
    // Server address and port
    string serverAddress = "127.0.0.1"; // Loopback address for local communication
    int serverPort = 12345; // Change to your server port

    // Initialize file sender
    server_connection fileSender(serverAddress, serverPort);

    // Choose to send files or queries based on flag
    bool success = flag ? fileSender.sendFiles(content) : fileSender.send_query(content);

    if (success) {
        cout << "Files sent successfully." << endl;
    } else {
        cerr << "Error sending files." << endl;
    }

    return 0;
}
