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
enum type {
    sql,
    ping
};
struct config {
    char alreadySet;
    type service;
    int flushafter;//offline after x attempts
    int refreshrate;//ttl
    int port;
    const char* server;
    const char* uname;
    const char* pass;
    char* query;//no free memory
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