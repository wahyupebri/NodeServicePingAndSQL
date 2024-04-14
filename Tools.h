#ifndef TOOLS_H
#define TOOLS_H
#include <windows.h>
#include <sqlite3.h>

struct pingnode {
    char* ip;//no free memory
    int id;
    int result;
    int attempt;
    HANDLE thread;
    DWORD thread_id;
    struct pingnode* next;
};
typedef struct pingnode PingNode;
struct sqlnode {
    char dbresult[800];
};
typedef struct sqlnode SQLNode;
enum type {
    sql,
    ping
};
struct config {
    char alreadySet;
    type service;
    int flushafter;//PING offline after x attempts
    int refreshrate;//PING ttl, SQL looping delay
    int port;
    char* connectionstring;//no free, SQL
    char* query;//no free memory, SQL
    void* head;
};

int printResult();
char* encodeResult();
int updatePing(char* pip, unsigned long* value);
int insertToPing(int* pid, char* pip);
int insertToPingResult(char* pip, int* result);
int prepareDB();
int runWebAndServices();
#endif