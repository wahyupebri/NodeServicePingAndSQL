#pragma comment(lib, "odbc32.lib")
#include <stdio.h>
#include <stdlib.h>
#include "Tools.h"
#include <sql.h>
#include <sqlext.h>
#define MAX_NUMS 800
#define MAX_STR_LEN 800
void reverseNumbersInText(char* input, char* output) {
    char* token;
    int numbers[MAX_NUMS];
    int count = 0;
    int i, j;

    // Use strtok to split the input string by semicolon and convert each token to an integer
    token = strtok(input, ";");
    while (token != NULL && count < MAX_NUMS) {
        // Convert token to integer and store it in the numbers array
        numbers[count] = atoi(token);
        count++;
        // Move to the next token
        token = strtok(NULL, ";");
    }

    // Reverse the order of numbers in the array
    for (i = 0, j = count - 1; i < j; i++, j--) {
        int temp = numbers[i];
        numbers[i] = numbers[j];
        numbers[j] = temp;
    }

    // Build the output string from the reversed numbers array
    output[0] = '\0'; // Start with an empty string
    for (i = 0; i < count; i++) {
        char tempStr[MAX_STR_LEN];
        // Append each number to the output string with semicolon as separator
        if (i != 0) {
            strcat(output, ";");
        }
        sprintf(tempStr, "%d", numbers[i]);
        strcat(output, tempStr);
    }
}
void removeSpaces(char* str) {
    int i, j = 0;
    char temp;

    // Iterate through each character in the string
    for (i = 0; str[i] != '\0'; i++) {
        // If the current character is not a space, copy it to the front
        if (str[i] != ' ') {
            str[j] = str[i];
            j++;
        }
    }
    // Terminate the string with a null character
    str[j] = ',';
    str[j+1] = '\0';
}
char* joinResult_(char* collector, char* entry) {
    //removeSpaces(entry);
    char* pjoin = NULL;
    if (collector == NULL) {
        pjoin = (char*)malloc(sizeof(char) * (strlen(entry) + 2));
        strcpy(pjoin, entry);
        //printf("INFO: SQL result %d %d\n", strlen(entry), strlen(pjoin));
        *(pjoin + strlen(entry)) = ';';
        *(pjoin + strlen(entry)+1) = '\0';
        
    }
    else {
        pjoin = (char*)malloc(sizeof(char) * (strlen(entry) + strlen(collector) + 2));
        char* ptemp = pjoin;
        strcpy(pjoin, collector);
        pjoin += strlen(collector);
        strcpy(pjoin, entry);
        *(ptemp + strlen(entry)+ strlen(collector)) = ';';
        *(ptemp + strlen(entry) + strlen(collector)+1) = '\0';
        //printf("INFO: SQL result %d %d\n", strlen(entry), strlen(pjoin));
        pjoin = ptemp;
        free(collector);
    }
    //printf("INFO: SQL result %s\n",pjoin);
    return pjoin;
}
char* joinResult(char * collector, char * entry) {
    //removeSpaces(entry);
    char* pjoin = NULL;
    if (collector == NULL) {
        pjoin = (char*)malloc(sizeof(char) * (strlen(entry)+1));
        strcpy(pjoin, entry);
        *(pjoin + strlen(pjoin)) = '\0';
        //printf("INFO: SQL result %d %d\n", strlen(entry),strlen(pjoin));
    }
    else {
        pjoin = (char*)malloc(sizeof(char) * (strlen(entry)+strlen(collector)+1));
        char* ptemp = pjoin;
        strcpy(pjoin, collector);
        pjoin += strlen(collector);
        strcpy(pjoin, entry);
        *(ptemp + strlen(ptemp)) = '\0';
        //printf("INFO: SQL result %d %d\n", strlen(entry), strlen(pjoin));
        pjoin = ptemp;
        free(collector);
    }
    //printf("INFO: SQL result %s\n",pjoin);
    return pjoin;
}
char * getDataFromSQL(const char* constring, char* query, char* result) {
   //Server=localhost\SQLEXPRESS;Database=master;Trusted_Connection=True;
    //printf("INFO: SQL function...\n");
    static const char* EXIST = "1";
    static const char* NOTEXIST = "0";
    SQLHENV henv = SQL_NULL_HENV; // ODBC environment handle
    SQLHDBC hdbc = SQL_NULL_HDBC; // ODBC connection handle
    SQLHSTMT hstmt = SQL_NULL_HSTMT; // ODBC statement handle
    SQLRETURN retcode; // Return code for ODBC functions
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
    }
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLDriverConnect(hdbc, NULL, (SQLCHAR*)constring,
            SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    }
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLExecDirect(hstmt, (SQLCHAR*)query, SQL_NTS);
    }
    SQLCHAR column1[100]; // Adjust size according to your data
    SQLCHAR column2[100]; // Adjust size according to your data
    char * buffer = result;
    short col1 = 0;
    short col2 = 0;
    while (SQLFetch(hstmt) == SQL_SUCCESS) {
        col1=SQLGetData(hstmt, 1, SQL_C_CHAR, column1, sizeof(column1), NULL);
        col2=SQLGetData(hstmt, 2, SQL_C_CHAR, column2, sizeof(column2), NULL);
        //removeSpaces((char*)column2);
        /*if (strlen((char *)column2)>0) {*/
        if (col1==0 && col2==0) {
            buffer = joinResult_(buffer, (char*)EXIST);
        }
        else {
            buffer = joinResult_(buffer, (char*)NOTEXIST);
        }
            
        //}
        // Process the data (e.g., print it to the console)
       printf("Column 1: %d-%s, Column 2: %d-%s\n", col1,column1, col2,column2);
    }
    *(buffer + strlen(buffer)-1) = '\0';
    char output[MAX_STR_LEN];

    // Reverse the order of numbers in the input text
    reverseNumbersInText(buffer, output);
    free(buffer);
    char* newbuffer = (char*)malloc(sizeof(char) * strlen(output)+1);
    strcpy(newbuffer, output);
    *(newbuffer + strlen(newbuffer)) = '\0';
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);

    return newbuffer;
}