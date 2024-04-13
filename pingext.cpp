#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include "Tools.h"
extern struct config gloConfig;
// Function to perform ICMP ping
int doPing(void* ptr) {
    HANDLE hIcmpFile;
    DWORD dwRetVal = 0;
    char SendData[32] = "Data Buffer";
    LPVOID ReplyBuffer = NULL;
    DWORD ReplySize = 0;
    PingNode* pping = (PingNode*)ptr;
    // Set the number of pings to 1
    unsigned int nPingCount = 1;

    // Allocate memory for the reply buffer
    ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
    ReplyBuffer = (VOID*)malloc(ReplySize);
    if (ReplyBuffer == NULL) {
        printf("Memory allocation failed\n");
        return -1;
    }

    // Open a handle to the ICMP library
    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        printf("Unable to open handle.\n");
        free(ReplyBuffer);
        return -1;
    }

    // Send the ICMP echo request
    dwRetVal = IcmpSendEcho(hIcmpFile, inet_addr(pping->ip), SendData, sizeof(SendData), NULL, ReplyBuffer, ReplySize, gloConfig.refreshrate);
    if (dwRetVal != 0) {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
        printf("Received ICMP Echo Reply from %s\n", pping->ip);
        //printf("  Status: %ld\n", pEchoReply->Status);
        //printf("  Roundtrip time: %ld milliseconds\n", pEchoReply->RoundTripTime);
        updatePing(pping->ip, &pEchoReply->RoundTripTime);
        pping->attempt = 0;
    }
    else {
        printf("Failed to receive ICMP Echo Reply from %s. Error: %ld\n", pping->ip, GetLastError());
        pping->attempt += 1;
        if (pping->attempt >= gloConfig.flushafter) {
            updatePing(pping->ip, NULL);
        }
    }

    // Close the ICMP handle and free the reply buffer
    if (hIcmpFile != INVALID_HANDLE_VALUE)
        IcmpCloseHandle(hIcmpFile);
    if (ReplyBuffer != NULL)
        free(ReplyBuffer);
    if (dwRetVal != 0)
        return 0;
    else
        return 1;
}
