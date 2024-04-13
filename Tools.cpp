#include "microhttpd.h"

#include <stdio.h>
#include <stdlib.h>
#include "pingext.h"
#include "Tools.h"
#include "Sqlcon.h"
#include <string.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>node service</body></html>"
#define SUCCESSPAGE "1"
#define PORT 8080
sqlite3* db; // Declare db as a global variable
//extern struct config gloConfig;
struct config gloConfig;
int gloId;
DWORD WINAPI ThreadFunction(LPVOID lpParam);
PingNode* createPing(char* ip, int result) {
    printf("INFO: Insert to struct-%d %s\n", gloId,ip);
    PingNode* newNode = (PingNode*)malloc(sizeof(PingNode));
    LPVOID parameter = (LPVOID)newNode;
    if (newNode != NULL) {
        newNode->ip = ip;
        newNode->result = -1;
        newNode->next = NULL;
        newNode->id = gloId;
        insertToPing(&newNode->id, newNode->ip);
        newNode->attempt = 0;
        newNode->thread = CreateThread(NULL, 0, ThreadFunction, parameter, 0, NULL);
        if (newNode->thread == NULL) {
            fprintf(stderr, "ERROR: Error creating thread (%lu)\n", GetLastError());
            return NULL;
        }

    }
    gloId++;
    return newNode;
}

// Function to insert a new node at the beginning of the linked list
PingNode* insertAtPingBeginning(PingNode* head, int result, char* ip) {
    PingNode* newNode = createPing(ip, result);
    if (newNode != NULL) {
        newNode->next = head;
        head = newNode;
    }
    return head;
}

// Function to print the elements of the linked list
void printLinkedListPing(PingNode* head) {
    PingNode* current = head;
    while (current != NULL) {
        printf("Name %d ip %s;", current->result, current->ip);
        current = current->next;
    }
    printf("NULL\n");
}
void stopThreadLinkedListPing(PingNode* head) {
    PingNode* current = head;
    while (current != NULL) {
        printf("Name %d ip %s;", current->result, current->ip);
        WaitForSingleObject(current->thread, INFINITE);
        // Clean up
        CloseHandle(current->thread);
        current = current->next;
    }
    printf("NULL\n");
}
// Function to free the memory allocated for the linked list
void freeLinkedListPing(PingNode* head) {
    PingNode* current = head;
    while (current != NULL) {
        PingNode* temp = current;
        current = current->next;
        free(temp);
    }
}

//int generateArray() {
//    // Number of strings in the array
//    int num_strings = 5;
//     
//    // Allocate memory for the array of strings (array of char pointers)
//    char** array_of_strings = (char**)malloc(num_strings * sizeof(char*));
//    if (array_of_strings == NULL) {
//        fprintf(stderr, "Memory allocation failed\n");
//        return 1;
//    }
//
//    // Allocate memory for each individual string
//    for (int i = 0; i < num_strings; i++) {
//        // Example string length
//        int string_length = 20;
//
//        // Allocate memory for the string
//        array_of_strings[i] = (char*)malloc((string_length + 1) * sizeof(char));
//        if (array_of_strings[i] == NULL) {
//            fprintf(stderr, "Memory allocation failed\n");
//
//            // Free previously allocated memory
//            for (int j = 0; j < i; j++) {
//                free(array_of_strings[j]);
//            }
//            free(array_of_strings);
//
//            return 1;
//        }
//
//        // Populate the string with some data
//        snprintf(array_of_strings[i], string_length + 1, "String %d", i + 1);
//    }
//
//    // Print the array of strings
//    for (int i = 0; i < num_strings; i++) {
//        printf("String %d: %s\n", i + 1, array_of_strings[i]);
//    }
//
//    // Free allocated memory
//    for (int i = 0; i < num_strings; i++) {
//        free(array_of_strings[i]);
//    }
//    free(array_of_strings);
//
//    return 0;
//}

void parsingConfigPing(const char* url) {
    //int number = atoi(char_array);
    printf("INFO: url to parsing is Ping %s\n", url);
    int pcounter = 0;
    int lastIPcounter = 0;
    int param = 0;
    PingNode* head = NULL;
    while (*url != '\0') {
        //printf("%c", *url);
        if (*url == ',') {
            //lastIPcounter=createNewIPMember(lastIPcounter,pcounter,url);
            char* temp = (char*)malloc((pcounter - lastIPcounter + 1) * sizeof(char));
            const char* temp_url = url - (pcounter - lastIPcounter);
            strncpy(temp, temp_url, (pcounter - lastIPcounter + 1));
            char* temp_first = temp;
            temp += (pcounter - lastIPcounter);
            *temp = '\0';
            printf("INFO: Setting Param %s\n", temp_first);

            if (param == 0) {
                int number = atoi(temp_first);
                gloConfig.flushafter = number;
                param++;
                free(temp_first);
            }
            else if (param == 1) {
                int number = atoi(temp_first);
                gloConfig.refreshrate = number;
                param++;
                free(temp_first);
            }
            else {
                if (strcmp(temp, "end") != NULL) {
                    printf("INFO: Setting IP %s\n", temp_first);
                    head = insertAtPingBeginning(head, -1, temp_first);
                    gloConfig.head = head;
                }
            }

            lastIPcounter = pcounter + 1;
            //printf("INFO: Setting number %d\n", flushafter);
        }
        url++;
        pcounter++;
    }
}
void parsingConfigSQL(const char* url) {
    //int number = atoi(char_array);
    printf("INFO: url to parsing is SQL %s\n", url);
    int pcounter = 0;
    int lastIPcounter = 0;
    int param = 0;
    char* pquery = NULL;
    char* pkeys = NULL;
    int oldlen = 0;
    int numkey = 0;
    while (*url != '\0') {
        //printf("%c", *url);
        if (*url == ';') {
            //lastIPcounter=createNewIPMember(lastIPcounter,pcounter,url);
            char* temp = (char*)malloc((pcounter - lastIPcounter + 1) * sizeof(char));
            const char* temp_url = url - (pcounter - lastIPcounter);
            strncpy(temp, temp_url, (pcounter - lastIPcounter + 1));
            char* temp_first = temp;
            temp += (pcounter - lastIPcounter);
            *temp = '\0';
            printf("INFO: Setting Param %s\n", temp_first);

            if (param == 0) {
                int number = atoi(temp_first);
                gloConfig.flushafter = number;
                param++;
                free(temp_first);
            }
            else if (param == 1) {
                int number = atoi(temp_first);
                gloConfig.refreshrate = number;
                param++;
                free(temp_first);
            }
            else if (param == 2) {
                //int number = atoi(temp_first);
                pquery = (char*)malloc((strlen(temp_first)+1) * sizeof(char));
                strncpy(pquery, temp_first, strlen(temp_first) - 1);
                *(pquery + strlen(temp_first) - 1) = '\0';
                //gloConfig.query = pquery;
                //gloConfig.refreshrate = number;
                //printf("INFO: Setting KEY %s %p\n", pquery, &pquery);
                param++;
                free(temp_first);
                //printf("INFO: Setting KEY %s %p\n", pquery, &pquery);
                //free(pquery);
            }
            else {
                printf("INFO: Setting KEY %s\n", temp_url);
                int len_url = strlen(temp_url);
                int len_end = strlen(";end");
                char* new_url = (char*)malloc(sizeof(char) * (len_url - len_end));
                strncpy(new_url, temp_url, (len_url - len_end));
                *(new_url + strlen(new_url)-2) = '\0';
                printf("INFO: New Setting KEY %s\n", new_url);
                free(temp_first);
                break;
            }

            lastIPcounter = pcounter + 1;
            //printf("INFO: Setting keys %d\n", pkeys);
        }
        url++;
        pcounter++;
    }
    if (numkey > 0) {
        /**(pquery + strlen(pquery) - 2) = ')';
        *(pquery + strlen(pquery) - 1) = '\0';*/
        gloConfig.query = pquery;
        printf("INFO: query is %s\n", pquery);
    }
    else {
        printf("ERROR: No primary keys in the URL\n");
    }
}
static void parsingConfig(const char* url) {
    if (gloConfig.alreadySet == 0) {
        if (url == NULL)
            return;
        //printf("INFO: url to parsing %s\n", url);
        if (strstr(url, "sql")) {
            printf("INFO: url to parsing is SQL\n");
            if (gloConfig.service != type::sql) {
                printf("ERROR: Accept URL config from user for SQL monitoring but it is different with parameters. Please restart this service\n");
            }
            url += strlen("sql,");
            parsingConfigSQL(url);
        }
        if (strstr(url, "ping")) {
            printf("INFO: url to parsing is PING\n");
            if (gloConfig.service != type::ping) {
                printf("ERROR: Accept URL config from user for Ping monitoring but it is different with parameters. Please restart this service\n");
            }
            url += strlen("ping,");
            parsingConfigPing(url);
        }
        gloConfig.alreadySet = 1;
    }
    else {
        printf("WARNING: Can not rewrite config. User might changed the config.xml. Please restart this service\n");
    }

}
static enum MHD_Result ahc_echo(void* cls,
    struct MHD_Connection* connection,
    const char* url,
    const char* method,
    const char* version,
    const char* upload_data,
    size_t* upload_data_size,
    void** ptr) {
    static int dummy;
    const char* page = (const char*)cls;
    struct MHD_Response* response;
    int ret;
    char* result = NULL;
    //printf("INFO: Request URL %s\n", url);
    if (0 != strcmp(method, "GET"))
        return MHD_NO; /* unexpected method */
    if (&dummy != *ptr)
    {
        /* The first time only the headers are valid,
           do not respond in the first round... */
        *ptr = &dummy;
        return MHD_YES;
    }
    if (0 != *upload_data_size)
        return MHD_NO; /* upload data in a GET!? */
    *ptr = NULL; /* clear context pointer */
    //-----logic
    if (strstr(url, "setup") != NULL) {
        const char* url_param = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "config");
        parsingConfig(url_param);
        response = MHD_create_response_from_buffer(strlen(SUCCESSPAGE),
            (void*)SUCCESSPAGE,
            MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection,
            MHD_HTTP_OK,
            response);
        MHD_destroy_response(response);
        return (enum MHD_Result)ret;
    }
    if (strstr(url, "getdata") != NULL) {
        printResult();
        result = encodeResult();
        printf("%s\n", result);
        response = MHD_create_response_from_buffer(strlen(result),
            (void*)result,
            MHD_RESPMEM_MUST_COPY);
        ret = MHD_queue_response(connection,
            MHD_HTTP_OK,
            response);
        MHD_destroy_response(response);
        free(result);//ADD
        return (enum MHD_Result)ret;
    }
    //-----end
    response = MHD_create_response_from_buffer(strlen(PAGE),
        (void*)PAGE,
        MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection,
        MHD_HTTP_OK,
        response);
    MHD_destroy_response(response);
    return (enum MHD_Result)ret;
}
// Your service logic goes here
int runWebService() {
    struct MHD_Daemon* d;
    d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
        gloConfig.port,
        NULL,
        NULL,
        &ahc_echo,
        (char*)PAGE,
        MHD_OPTION_END);
    if (d == NULL)
        return 1;
    (void)getc(stdin);
    MHD_stop_daemon(d);
    return 0;
}

// Entry point for the additional thread
DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    // Your multithreaded logic goes here
    int returncode = -1;

    PingNode* ptr = (PingNode*)lpParam;
    //int counter = 0;
    //while(counter < 4){
    while (true) {
        ptr->result = -1;
        returncode = doPing(ptr);
        if (returncode == 0)
            Sleep(10000);
        else
            Sleep(500);
        //counter++;
    }

    return 0;
}

// Entry point for the main thread (service)
int runWebAndServices() {
    gloConfig.alreadySet = 0;

    // Your main service logic goes here
    printf("INFO: Main service thread running...\n");
    runWebService();

    // Wait for the additional thread to finish
    stopThreadLinkedListPing((PingNode*)gloConfig.head);



    return 0;
}
char* encodeResult() {
    sqlite3_stmt* stmt;
    int rc;
    // Prepare the SQL statement
    const char* sql = "SELECT ip, value from ping;";
    char* result = NULL;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // Execute the statement and print the results as a table
    int num_columns = sqlite3_column_count(stmt);
    char buffer[5];
    // Print rows
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        for (int i = 0; i < num_columns; i++) {
            if (sqlite3_column_type(stmt, i) == SQLITE_TEXT) {
                //printf("%s\t", sqlite3_column_text(stmt, i));
            }
            else if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER) {
                //printf("%d\t", sqlite3_column_int(stmt, i));
                sprintf(buffer, "%d", sqlite3_column_int(stmt, i));
                int oldlen = 0;
                if(result!=NULL)
                    oldlen = strlen(result);
                int newlen = strlen(buffer);
                char* newbuffer = (char*)malloc(sizeof(char) * (oldlen + newlen + 2));
                if (result != NULL) {
                    strcpy(newbuffer, result);
                    strcat(newbuffer, ";");
                    strcat(newbuffer, buffer);
                }
                else {
                    strcpy(newbuffer, buffer);
                    //strcat(newbuffer, ";");
                }
                if (result != NULL) {
                    free(result);
                }
                //*(newbuffer + oldlen + newlen+1) = ';';
                *(newbuffer + oldlen + newlen+1) = '\0';
                result = newbuffer;
            }
            else {
                printf("NULL\t");
            }
        }
        //printf("\n");
    }

    // Check for errors or end of data
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error reading data: %s\n", sqlite3_errmsg(db));
    }

    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    return result;
}
int printResult() {
    sqlite3_stmt* stmt;
    int rc;
    // Prepare the SQL statement
    const char* sql = "SELECT ip, value from ping;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Execute the statement and print the results as a table
    int num_columns = sqlite3_column_count(stmt);

    // Print column names
    for (int i = 0; i < num_columns; i++) {
        printf("%s\t", sqlite3_column_name(stmt, i));
    }
    printf("\n");

    // Print rows
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        for (int i = 0; i < num_columns; i++) {
            if (sqlite3_column_type(stmt, i) == SQLITE_TEXT) {
                printf("%s\t", sqlite3_column_text(stmt, i));
            }
            else if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER) {
                printf("%d\t", sqlite3_column_int(stmt, i));
            }
            else {
                printf("NULL\t");
            }
        }
        printf("\n");
    }

    // Check for errors or end of data
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error reading data: %s\n", sqlite3_errmsg(db));
    }

    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    return 0;
}
int insertToPingResult(char* pip, int *result) {
    const char* insert_sql = "INSERT INTO result (ip, value) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    int id = 0;
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, pip, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, *result);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_reset(stmt);
    //}

    // Finalize the statement
    sqlite3_finalize(stmt);

    printf("INFO: Data inserted successfully!\n");

    return 0;
}
int updatePing(char* pip, unsigned long* value) {
    sqlite3_stmt* stmt;
    int rc;
    // Prepare the SQL statement with a parameter
        const char* sql = "UPDATE ping SET value = ? WHERE ip = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Bind parameters to the prepared statement
    
    char new_value[10];
    if(value!=NULL)
        sprintf(new_value, "%d", *value);
    else
        sprintf(new_value, "%d", -1);
    const char* condition_value = pip;
    sqlite3_bind_text(stmt, 1, new_value, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, condition_value, -1, SQLITE_STATIC);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error updating data: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);

    return 0;
}
int insertToPing(int* pid, char* pip) {
    const char* insert_sql = "INSERT INTO ping (id, ip, value) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    int id = 0;
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }


        sqlite3_bind_int(stmt, 1, *pid);
        sqlite3_bind_text(stmt, 2, pip, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, -1);
        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return 1;
        }

        sqlite3_reset(stmt);
    //}

    // Finalize the statement
    sqlite3_finalize(stmt);

    printf("INFO: Data inserted successfully!\n");

    return 0;
}

int prepareDB() {
    // Open an in-memory SQLite database
    int rc = sqlite3_open(":memory:", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Create a table
    const char* create_table_sql_ping =
        "CREATE TABLE IF NOT EXISTS ping ("
        "id INTEGER PRIMARY KEY,"
        "ip TEXT NOT NULL,"
        "value INTEGER"
        ");";

    char* error_message = 0;
    rc = sqlite3_exec(db, create_table_sql_ping, 0, 0, &error_message);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        return 1;
    }
    // Create a table
    const char* create_table_sql_ping_result =
        "CREATE TABLE IF NOT EXISTS result ("
        "id INTEGER PRIMARY KEY,"
        "ip TEXT NOT NULL,"
        "value INTEGER"
        ");";

    error_message = 0;
    rc = sqlite3_exec(db, create_table_sql_ping_result, 0, 0, &error_message);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_message);
        sqlite3_free(error_message);
        sqlite3_close(db);
        return 1;
    }

    printf("Database initialized.\n");

    return 0;
}


