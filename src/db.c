#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *NU, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){
        printf("%s == %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(){
    int return_code;
    sqlite3 *db;
    char* errmsg = 0;
    char* query;

    return_code = sqlite3_open("test.db", &db);

    query = "SELECT * FROM message";

    return_code = sqlite3_exec(db, query, callback, 0, &errmsg);

    sqlite3_close(db);
    return 0;
    
}