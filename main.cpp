#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fstream>
#import <thread>
#include <getopt.h>

using namespace std;

#define PORT 2012

#define no_argument 0
#define required_argument 1
#define optional_argument 2


int main(int argc, char *argv[]) {
    std::cout << "-----QR Code Client-----" << std::endl;





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int opt;
    int port = 2012;
    std::string address = "127.0.0.1";
    std::string filePath = "google.png";


    const struct option longopts[] = {
            {"PORT", optional_argument, NULL, 'P'},
            {"ADDRESS", optional_argument, NULL, 'A'},
            {"FILE", required_argument, NULL, 'F'},
            {0, 0, 0, 0}
    };

    while(((opt = getopt_long(argc, argv, "PAF:", longopts, NULL)) != -1)){

        //std::cout << "Arguement is optarg: " << optarg << std::endl;


        switch(opt){
            case 'P':
                port = atoi(optarg);
                break;
            case 'A':
                address = optarg;
                break;
            case 'F':
                filePath = optarg;
                break;
            case '?':
                std::cout << "\nUnknown arguments";
                return 1;
            case ':':
                std::cout << "\nError: Need more arguments";
                break;

        }
    }

    if(opt == -1 && argc == 1){
        std::cout << "Error: Need more arguments";
        return 1;
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    std::cout << "\nStarting connection with..";
    std::cout << "\nPort: " << port;
    std::cout << "\nAddress: " << address;
    std::cout << "\nQR Code File: " << filePath;


    //Creates socket
    int sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }


    //Sets up Address struct
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }


    //Attempts to connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    //Opens the QRCode File for transmission
    FILE* QRcode;
    struct stat st;
    QRcode = fopen(filePath.c_str(),"r");

    //Gets size of QRCode image
    stat(filePath.c_str(),&st);
    std::uint32_t byteSize = st.st_size;


    //Sends size of QRCode Image
    int BS_QRImageSize = send(sock, &byteSize, sizeof(std::uint32_t), 0);
    std::cout << "\nSent Size, was " <<  byteSize << std::endl;



    //Creates and stores QRCode in Buffer
    char * imgBuffer = (char *)malloc(st.st_size);
    fread(imgBuffer,st.st_size,1, QRcode);


    //Sends QR Code from buffer and of appropriate size
    int BS_QRImage = send(sock, imgBuffer,st.st_size,0);
    std::cout << "Sent image";

    //Reads return code from server
    std::uint32_t returnCode = 1;
    int BR_returnCode = read(sock, &returnCode, sizeof(std::uint32_t));

    std::cout << "\nReturn code was: " << returnCode;

    switch(returnCode){
        case 0:
            break;
        case 1:
            std::cout << "\nFailed: Image is not valid or breaches security conduct";
            break;
        case 2:
            break;
        case 3:
            break;
    }

    //TO-DO
    //ADD SWITCH STATEMNT PRBABLY



    //Receives size of to be returned URL
    std::uint32_t URLsize;
    int BR_URLlength = read(sock, &URLsize,sizeof(std::uint32_t));
    std::cout << "\nReceived URL size of: " << URLsize;


    //Creates buffer for URL based on size and reads URL into buffer
    char * recURL = static_cast<char *>(malloc(URLsize));
    int BR_URL = read(sock, recURL, URLsize);
    std::cout << "\nRecieved URL: " << recURL;


    free(imgBuffer);
    free(recURL);
    return 0;
}