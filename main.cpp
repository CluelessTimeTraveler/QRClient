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
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    FILE* QRcode;
    struct stat st;

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



    std::cout << "\nStarting connection with..";
    std::cout << "\nPort: " << port;
    std::cout << "\nAddress: " << address;
    std::cout << "\nQR Code File: " << filePath;


    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);




    if(inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }


    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    QRcode = fopen(filePath.c_str(),"r");
    stat(filePath.c_str(),&st);

    int byteSize = st.st_size;

    send(sock, &byteSize, sizeof(int), 0);
    std::cout << "\nSent Size, was " <<  byteSize << std::endl;




    char * imgBuffer = (char *)malloc(st.st_size);

    fread(imgBuffer,st.st_size,1, QRcode);



    send(sock, imgBuffer,st.st_size,0);
    std::cout << "Sent image";

    int URLsize;


    read(sock, &URLsize,sizeof(int));
    std::cout << "\nReceived URL size of: " << URLsize;

    char * recURL = static_cast<char *>(malloc(URLsize));

    read(sock, recURL, URLsize);
    std::cout << "\nRecieved URL: " << recURL;

    return 0;
}