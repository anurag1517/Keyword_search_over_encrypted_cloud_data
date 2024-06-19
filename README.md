# Keyword_search_over_encrypted_cloud_data

---

## Technical Description

-This project uses inverted index table to store keywords and encryption technique to encrypt the same and upload it on the server along with the original file of the user.

-The uploaded file can be fetched later via connecting to server with the help of socket .

-Client can download their uploaded file by passing keywords as query and the files containing those keywords will be decrypted and will be sent to the client.

-Language used:C++

---

### Inspiration

To avoid creating backups of backups on local storage devices which is a burden to carry.
Storing files on cloud provide security along with ease of access and can be accessed from anyplace just with the help of an active internet connection.

### What it does

-Server and client are connected via socket programming.
-Client uploads the encrypted file and inverted index on the server.
-The files are stored securely on the server and can be fetched via query whenever needed.

### How we built it

-We used socket programming,AES encryption scheme and inverted index.

###challenges we ran into
-Choosing the appropriate encryption scheme.
-Splitting text files into keywords and creating an inverted index of it.
-Creating Socket between client and server.

### Accomplishments

We overcome all the challenges and built a full functioning client-server model where transaction of data can take place.

---






