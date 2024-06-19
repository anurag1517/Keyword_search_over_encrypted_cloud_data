#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include "encrypt.cpp" // Include encryption functions
#include "decrypt.cpp" // Include decryption functions
#include "client_socket.cpp" // Include client socket functions
using namespace std;

// Class for personal encryption functions
class personal_encryption {
private:
    // Simple XOR-based encryption function
    std::string encrypt(const std::string& message, const std::string& key) {
        std::string encrypted;
        for (size_t i = 0; i < message.size(); ++i) {
            encrypted += message[i] ^ key[i % key.size()]; // XOR with corresponding character from key
        }
        return encrypted;
    }

    // Simple XOR-based decryption function
    std::string decrypt(const std::string& encrypted, const std::string& key) {
        std::string decrypted;
        for (size_t i = 0; i < encrypted.size(); ++i) {
            decrypted += encrypted[i] ^ key[i % key.size()]; // XOR with corresponding character from key
        }
        return decrypted;
    }

    // Base64 encoding table
    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    // Function to encode a string to Base64
    std::string base64_encode(const std::string &input) {
        std::string encoded;
        size_t i = 0;
        size_t j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        for (auto c : input) {
            char_array_3[i++] = c;
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++) {
                    encoded += base64_chars[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++) {
                char_array_3[j] = '\0';
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

            for (j = 0; j < i + 1; j++) {
                encoded += base64_chars[char_array_4[j]];
            }

            while (i++ < 3) {
                encoded += '=';
            }
        }

        return encoded;
    }

    // Function to decode a Base64 string
    std::string base64_decode(const std::string &encoded_string) {
        size_t in_len = encoded_string.size();
        size_t i = 0;
        size_t j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string decoded;

        while (in_len-- && (encoded_string[in_] != '=') &&
               (isalnum(encoded_string[in_]) || (encoded_string[in_] == '+') || (encoded_string[in_] == '/'))) {
            char_array_4[i++] = encoded_string[in_];
            in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++) {
                    char_array_4[i] = base64_chars.find(char_array_4[i]);
                }

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; i < 3; i++) {
                    decoded += char_array_3[i];
                }
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++) {
                char_array_4[j] = 0;
            }

            for (j = 0; j < 4; j++) {
                char_array_4[j] = base64_chars.find(char_array_4[j]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

            for (j = 0; j < i - 1; j++) {
                decoded += char_array_3[j];
            }
        }

        return decoded;
    }

public:
    std::string key;
    personal_encryption(string key) {
        this->key = key;
    }

    // Function for encryption
    string encryption(string message) {
        string a = base64_encode(encrypt(message, key));
        if (a.length() >= 16)
            return a.substr(0, 16);
        return a + a.substr(0, 16 - a.length());
    }

    // Function for decryption
    string decryption(string cyphertext) {
        return decrypt(base64_decode(cyphertext), key);
    }
};

// Class for inverted index
class InvertedIndex {
private:
    unordered_map<string, unordered_set<string>> invIndex; // Inverted Index
    unordered_set<string> stop_word; // Set of stop words
    string password; // Password for encryption

public:
    vector<string> file_names; // Vector to store file names

    // Function to create inverted index
    void create_inv_index() {
        int noFile;
        cout << "ENTER NUMBER OF FILES FOR OUTSOURCING: ";
        cin >> noFile;

        // Storing stop words in set
        ifstream file("stop_word.txt");
        if (!file.is_open()) {
            cout << "Error opening file: stop_word.txt\n";
            exit(1); // Exit with error code 1
        }
        string word;
        while (file >> word) {
            stop_word.insert(word);
        }

        for (int i = 0; i < noFile; i++) {
            string file_name;
            cout << "ENTER FILE NAME: ";
            cin >> file_name;

            ifstream file(file_name + ".txt");
            if (!file.is_open()) {
                cout << "Error opening file: " << file_name << endl;
                exit(1); // Exit with error code 1
            }

            // Read file contents into stringstream
            stringstream buffer;
            buffer << file.rdbuf();

            // Tokenize and index the file content
            makeIndex(buffer.str(), file_name);

            file_names.push_back(file_name + ".dat");

            // Encrypt the file using AES cbc
            AES_encrypt(file_name);

            file.close();
        }

        file_names.push_back("encrypted_inverted_index.txt");
    }

    // Function to tokenize and index the file content
    void makeIndex(const string& s, string Id) {
        string st = "";
        for (char c : s) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '"' || c == '\'') {
                if (c >= 'A' && c <= 'Z')
                    c = tolower(c);
                st += c; // Convert characters to lowercase
            } else if (!st.empty()) {
                if (stop_word.find(st) == stop_word.end())
                    invIndex[st].insert(Id); // Store document ID for the current word
                st = ""; // Reset the word
            }
        }
        if (!st.empty()) {
            if (stop_word.find(st) == stop_word.end())
                invIndex[st].insert(Id); // Store document ID for the last word
        }
    }

    // Function to print the inverted index
    void printInvertedIndex() {
        for (const auto& word : invIndex) {
            cout << word.first << " --> ";
            for (const string& docId : word.second) {
                cout << docId << " ";
            }
            cout << endl;
        }
    }

    // Serialize the encrypted inverted index to a file
    void store_encrypted_invindex(const string& filename) {
        personal_encryption enc_dec(password);
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error opening file for serialization: " << filename << endl;
            return;
        }
        for (const auto& word : invIndex) {
            outFile << enc_dec.encryption(word.first) << " ";
            // Encrypt document IDs and store them in the file
            for (const string& file_id : word.second) {
                outFile << file_id << " ";
            }
            outFile << endl;
        }
        outFile.close();
    }

    // Function to search for a keyword in the encrypted inverted index
    void search(const string& keyword) {
        // Encrypt keyword
        personal_encryption enc_dec(password);
        string encryptedKeyword = enc_dec.encryption(keyword);

        // Perform search using encrypted keyword
        auto it = invIndex.find(keyword);

        if (it != invIndex.end()) {
            cout << "Keyword found. Files present in location: ";
            for (const string& loc : it->second) {
                cout << loc << " ";
            }
            cout << endl;
        } else {
            cout << "Keyword not found." << endl;
        }
    }

    // Function to set the password
    void get_password(const string password) {
        this->password = password;
    }
};

int main() {

    InvertedIndex indexing;
    string pass, keyword;
    vector<string> querys;
    cout << "ENTER PASSWORD\n";
    cin >> pass;
    personal_encryption hash(pass);

    int token;
    cout << "ENTER OPTION OF CLIENT \n";
    cout << "PRESS \n 1 --> FOR ENCRYPTING FILES with INVERTED INDEX\n 2 --> FOR QUERY RETRIEVAL\n 3 --> FOR UPLOADING FILES\n 4 --> DECRYPTING RECEIVED FILES\n";
    while (true) {
        cin >> token;
        switch (token) {
            case 1:
                cout << "ENCRYPTING FILES.. \n";
                indexing.get_password(pass);
                key_generate();

                // Create index from files
                indexing.create_inv_index();

                // Print index
                // indexing.printInvertedIndex();

                // Serialize and store the encrypted index to a file
                indexing.store_encrypted_invindex("encrypted_inverted_index.txt");

                break;
            case 2:
                cout << "QUERY RETRIEVAL \n";
                // Search for a keyword
                cout << "ENTER KEYWORDS TO RETRIEVE DATA (press (end) to terminate query at last )\n";
                cin >> keyword;
                while (keyword != "end") {
                    string enc_key = hash.encryption(keyword);
                    cout << enc_key << " as query \n";
                    querys.push_back(enc_key);
                    cin >> keyword;
                }

                sendFiles_to_server(querys, false);
                cout << "query received\n";
                break;

            case 3:
                cout << "UPLOADING FILES... \n";
                // upload files to server
                sendFiles_to_server(indexing.file_names, true);
                break;

            case 4:
                cout << "DECRYPTING RECEIVED FILES\n";
                AES_decrypt(received_files);
                break;

            default:
                return 0;
        }
    }

    return 0;
}
