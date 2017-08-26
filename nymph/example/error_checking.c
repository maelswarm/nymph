#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error_checking.h"
#include "helper_func.h"

int checkBalanceOfChars(char *string, char open, char close) {
    int cnt = 0;
    for (int i=0; i<strlen(string); i++) {
        if (string[i] == open) {
            cnt++;
        } else if(string[i] == close) {
            cnt--;
        }
    }
    return cnt;
}

int checkStatement(char *statement) {
    
    char *stmt = malloc(1000*sizeof(char));
    strcpy(stmt, statement);
    trim(stmt);
    //printf("orig stmt: !%s!\n", stmt);
    int statementArr[1000];
    int statementArrLen = 0;
    char *word;
    word = getToken(stmt);
    if (word != NULL) {
        stmt+=strlen(word);
        trim(stmt);
    }
    //printf("stmt: !%s!\n", stmt);
    trim(word);
    while(word != NULL) {
        if (isDataType(word)) {
            statementArr[statementArrLen] = 0;
        } else if(isStorageC(word)) {
            statementArr[statementArrLen] = 3;
        } else if(isOper(word)) {
            statementArr[statementArrLen] = 4;
        } else if(isVal(word)) {
            statementArr[statementArrLen] = 5;
        } else if(isReturn(word)) {
            statementArr[statementArrLen] = 6;
        } else if(isFuncCall(word)) {
            statementArr[statementArrLen] = 2;
        } else if(isVar(word)) {
            statementArr[statementArrLen] = 1;
        }
        
        statementArrLen++;
        
        word = getToken(stmt);
        if (word != NULL) {
            //printf("prestmt: !%s! len:%i\n", stmt, strlen(word));
            stmt+=strlen(word);
            trim(stmt);
        }
        //printf("stmt: !%s!\n", stmt);
        trim(word);
    }
    
    for (int i = 0; i< statementArrLen; i++) {
        //printf("%i ", statementArr[i]);
    }
    //printf("\n");
    
    for (int i = 0; i<statementArrLen; i++) {
        
        if (statementArr[i] == 0) {
            if (i+1 < statementArrLen) {
                if (statementArr[i+1] != 1) {
                    return 0;
                }
            } else {
                return 0;
            }
        } else if (statementArr[i] == 1) {
            if (i+1 < statementArrLen) {
                if (statementArr[i+1] != 4 && statementArr[i+1] != 1) {
                    return 0;
                }
            } else {
                return 1;
            }
        } else if (statementArr[i] == 2) {
            if (i+1 < statementArrLen) {
                if (statementArr[i+1] != 4) {
                    return 0;
                }
            } else {
                return 1;
            }
        } else if (statementArr[i] == 3) {
            if (i+1 < statementArrLen) {
                if (statementArr[i+1] != 0) {
                    return 0;
                }
            } else {
                return 0;
            }
        } else if (statementArr[i] == 4) {
            if (i+1 < statementArrLen) {
                if (statementArr[i+1] != 1 && statementArr[i+1] != 2 && statementArr[i+1] != 5) {
                    return 0;
                }
            } else {
                return 0;
            }
        } else if (statementArr[i] == 5) {
            if (i+1 < statementArrLen) {
                if (statementArr[i+1] != 4) {
                    return 0;
                }
            } else {
                return 1;
            }
        } else if (statementArr[i] == 6) {
            if (i+1 < statementArrLen) {
                if (statementArr[i+1] != 2 && statementArr[i+1] != 1 && statementArr[i+1] != 5) {
                    return 0;
                }
            } else {
                return 1;
            }
        }
    }
    return 1;
}
