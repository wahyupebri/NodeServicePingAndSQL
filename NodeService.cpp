// NodeService.cpp : Defines the entry point for the application.
//
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#include <stdio.h>
#include "NodeService.h"
#include "Tools.h"


using namespace std;
extern sqlite3* db;
extern int gloId;
extern struct config gloConfig;
int main(int argc, char* argv[])
{
    if (argc < 2)
        gloConfig.port = 8080;
    else
        gloConfig.port = atoi(argv[1]);
    if (strlen(argv[2])>0) {
        if (strcmp(argv[2], "ping") == 0) {
            gloConfig.service = type::ping;
            printf("This program is running with Web port %d and accept Ping monitoring\n", gloConfig.port);
        }
        else if (strcmp(argv[2], "sql") == 0) {
            gloConfig.service = type::sql;
            gloConfig.server = argv[3];
            gloConfig.uname = argv[4];
            gloConfig.pass = argv[5];
            printf("This program is running with Web port %d and accept SQL monitoring\n", gloConfig.port);
        }
    }
    
    if(argc<2) {
        printf("How to run: NodeService.exe <port web service> <ping|sql> <sql server> <sql uname> <sql password>\n");
        printf("Even you didn't set parameter correctly, this program is still running with Web port 8080 and accept Ping monitoring\n");

        //return 1;
    }
    
    //if (argc > 1) {
    //    //port=argv[1]
    //    if (strcmp(argv[2], "ping") == NULL) {
    //gloConfig.service = type::ping;
    //    }
    //    else if (strcmp(argv[2], "sql") == NULL) {
    // gloConfig.service = type::sql;
    //        //db server argv[3]
    //        //uname argv[4]
    //        //password argv[5]
    //    }
    //    else {
    //        printf("ERROR: Unknown argument\n");
    //        return 1;
    //    }
    //}
    //else {
    //    printf("ERROR: Need argument\n");
    //    return 1;
    //}
    gloId = 1;
    printf("INFO: NodeService ver. 1.0. Dibuat oleh Wahyu Pebrian.\n");
    prepareDB();
    //insertToPing(NULL, NULL);
    
    // Your main service logic goes here
    runWebAndServices();
    sqlite3_close(db);
    return 0;
}
