#include<iostream>
#include<fstream>
#include<openssl/evp.h>
#include<openssl/rand.h>
#include<vector>

using namespace std;

unsigned char key[16], iv[16]; // Key and IV for AES encryption (128-bit)

// Function to encrypt data using AES CBC mode
string encryptData(const string& plaintext, const unsigned char* key, const unsigned char* iv) {
    // Create a new cipher context for encryption
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    // Initialize the encryption operation with AES CBC mode, key, and IV
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv);

    // Allocate memory for the ciphertext buffer
    int ciphertextLen = plaintext.length() + EVP_MAX_BLOCK_LENGTH; // Account for possible padding
    unsigned char* ciphertext = new unsigned char[ciphertextLen];

    // Perform the encryption operation
    int len;
    EVP_EncryptUpdate(ctx, ciphertext, &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
    int ciphertextLen1 = len;

    // Finalize the encryption operation
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertextLen1 += len;

    // Convert ciphertext to a string
    string encryptedData(reinterpret_cast<char*>(ciphertext), ciphertextLen1);

    // Deallocate memory for ciphertext
    delete[] ciphertext;

    // Free the cipher context to release allocated resources
    EVP_CIPHER_CTX_free(ctx);

    return encryptedData;
}

// Function to generate a random key and IV for AES encryption
void key_generate(){
    // Generate a random key and IV using OpenSSL's RAND_bytes
    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));
    cout<< "KEY generated\n";

    // Write the generated key to a text file
    ofstream keyFile("key.txt");
    if (!keyFile.is_open()) {
        cerr << "Error: Unable to create key file.\n" << endl;
        exit(0);
    }
    keyFile.write(reinterpret_cast<const char*>(key), sizeof(key));
    keyFile.close();
}

// Function to perform AES encryption on a file
int AES_encrypt(string& file_name) {
    // Read plaintext from input file
    ifstream inputFile(file_name+".txt");
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open "<<file_name<<" file." << endl;
        return 1;
    }
    string plaintext((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
    inputFile.close();

    // Encrypt the plaintext using AES CBC mode
    string ciphertext = encryptData(plaintext, key, iv);

    // Write IV and ciphertext to output file
    ofstream outputFile(file_name+".dat", ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to create output file." << endl;
        return 1;
    }
    outputFile.write(reinterpret_cast<const char*>(iv), sizeof(iv)); // Write IV to file
    outputFile.write(ciphertext.c_str(), ciphertext.length()); // Write ciphertext to file
    outputFile.close();

    cout << "Encryption of "<<file_name<<" completed successfully.\n" << endl;
    return 0;
}

/*
Compilation and execution instructions:
g++ -o program encrypt.cpp -lssl -lcrypto
./program
*/
