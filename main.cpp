#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fstream>

#define PORT 2012

int main() {
    std::cout << "Hello, World!" << std::endl;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    FILE* QRcode;
    struct stat st;


    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);




    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }


    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
//    send(sock , hello , strlen(hello) , 0 );
//    printf("Hello message sent\n");
//    valread = read( sock , buffer, 1024);
//    printf("%s\n",buffer );

    QRcode = fopen("google.png","r");
    stat("google.png",&st);

    int byteSize = st.st_size;

    send(sock, &byteSize, sizeof(int), 0);
    std::cout << "Sent Size, was " <<  byteSize << std::endl;

//    int gamerNumber = 69420;
//    send(sock, &gamerNumber, sizeof(int),0);


    char * imgBuffer = (char *)malloc(st.st_size);
    //std::cout << "size of buffer: " << sizeof(imgBuffer);
    //printf("File Size: %d bytes\n",st.st_size);
    fread(imgBuffer,st.st_size,1, QRcode);

//    FILE * recievedFile;
//    recievedFile = fopen("googleRec.png","w");
//    fwrite(imgBuffer, st.st_size,1,recievedFile);

//    std::ofstream image;
//    image.open("googleRec.png");
//    image << imgBuffer;
//    image.close();

    send(sock, imgBuffer,st.st_size,0);
    std::cout << "Sent image";

    return 0;
}