#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "helper_func.h"

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements
    
    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);
    
    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }
    
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);
    
    if (!result)
        return NULL;
    
    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

void trimAllButLetter(char* str) {
    if(!str)
        return;
    
    char* ptr = str;
    int len = strlen(ptr);
    
    while(len-1 > 0 && !isalpha(ptr[len-1]))
        ptr[--len] = 0;
    
    while(*ptr && !isalpha(*ptr))
        ++ptr, --len;
    
    memmove(str, ptr, len + 1);
}

void trimAllButLetterAndStar(char* str) {
    if(!str)
        return;
    
    char* ptr = str;
    int len = strlen(ptr);
    
    while(len-1 > 0 && (!isalpha(ptr[len-1]) && ptr[len-1] != '*'))
        ptr[--len] = 0;
    
    while(*ptr && (!isalpha(*ptr) && *ptr != '*'))
        ++ptr, --len;
    
    memmove(str, ptr, len + 1);
}

void trimAllButAlphaAndStar(char* str) {
    if(!str)
        return;
    
    char* ptr = str;
    int len = strlen(ptr);
    
    while(len-1 > 0 && (isspace(ptr[len-1]) || (ispunct(ptr[len-1]) && ptr[len-1] != '*')))
        ptr[--len] = 0;
    
    while(*ptr && (isspace(*ptr) || (ispunct(*ptr) && *ptr != '*')))
        ++ptr, --len;
    
    memmove(str, ptr, len + 1);
}

void trimAllButAlpha(char* str)
{
    if(!str)
        return;
    
    char* ptr = str;
    int len = strlen(ptr);
    
    while(len-1 > 0 && (isspace(ptr[len-1]) || ispunct(ptr[len-1])))
        ptr[--len] = 0;
    
    while(*ptr && (isspace(*ptr) || ispunct(*ptr)))
        ++ptr, --len;
    
    memmove(str, ptr, len + 1);
}

void trimAllButNumbers(char* str)
{
    if(!str)
        return;
    
    char* ptr = str;
    int len = strlen(ptr);
    
    while(len-1 > 0 && (!isdigit(ptr[len-1])))
        ptr[--len] = 0;
    
    while(*ptr && (!isdigit(*ptr)))
        ++ptr, --len;
    
    memmove(str, ptr, len + 1);
}

void trim(char* str)
{
    if(!str)
        return;
    
    char* ptr = str;
    int len = strlen(ptr);
    
    while(len-1 > 0 && isspace(ptr[len-1]))
        ptr[--len] = 0;
    
    while(*ptr && isspace(*ptr))
        ++ptr, --len;
    
    memmove(str, ptr, len + 1);
}

char *nSubString(char *string, char *end) {
    char *nSubString = (char *)malloc(1000*sizeof(char));
    int i = 0;
    char *tmp = (char *)malloc(1000*sizeof(char));
    strcpy(tmp, string);
    tmp[strlen(end)] = '\0';
    for(; i < strlen(string) && strcmp(tmp, end); i++) {
        nSubString[i] = string[i];
        strcpy(tmp, &string[i]);
        tmp[strlen(end)] = '\0';
    }
    nSubString[i-strlen(end)] = '\0';
    return nSubString;
}

char *nPostSubString(char *string, char *end) {
    char *nSubString = (char *)malloc(1000*sizeof(char));
    int i = 0;
    int j = 0;
    int flag = 0;
    char *tmp = (char *)malloc(1000*sizeof(char));
    for(;string[i] != '\0'; i++) {
        strcpy(tmp, &string[i]);
        tmp[strlen(end)] = '\0';
        if(!strcmp(tmp, end) && !flag) {
            flag = 1;
            i += strlen(end);
        }
        if (flag) {
            nSubString[j] = string[i];
            nSubString[j+1] = '\0';
            j++;
        }
    }
    return nSubString;
}

char *subStringPostLastOccurance(char *string, char end) {
    char *nSubString = (char *)malloc(1000*sizeof(char));
    int i = strlen(string)-1;
    int j = 0;
    for(;string[i] != end; i--) {
    }
    for (int idx = i; idx < strlen(string); idx++, j++) {
        nSubString[j] = string[idx];
        nSubString[j+1] = '\0';
    }
    return nSubString;
}

char *subStringLastOccurance(char *string, char end) {
    char *nSubString = (char *)malloc(1000*sizeof(char));
    int i = strlen(string)-1;
    for(;string[i] != end; i--) {
    }
    for (int idx = 0; idx < i; idx++) {
        nSubString[idx] = string[idx];
    }
    
    nSubString[i] = '\0';
    return nSubString;
}

char *loadFile(FILE *fp) {
    
    long fileSize;
    char *buffer;
    
    fseek(fp , 0L , SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);
    
    buffer = calloc(1, fileSize+1);
    fread(buffer, fileSize, 1 , fp);
    
    return buffer;
}

char *lastPtheses(char *str) {
    int flag = 0;
    int cnt = 0;
    char *nSubString = (char *)malloc(1000*sizeof(char));
    for (int i=0; i<strlen(str); i++) {
        if (str[i] == '(') {
            flag = 1;
            cnt++;
        } else if(str[i] == ')') {
            cnt--;
        }
        nSubString[i] = str[i];
        nSubString[i+1] = '\0';
        if (flag && cnt == 0) {
            break;
        }
    }
    return nSubString;
}

char *postLastPtheses(char *str) {
    int flag = 0;
    int cnt = 0;
    int j = 0;
    char *nSubString = (char *)malloc(1000*sizeof(char));
    for (int i=0; i<strlen(str); i++) {
        if (str[i] == '(' && flag != 2) {
            flag = 1;
            cnt++;
        } else if(str[i] == ')' && flag != 2) {
            cnt--;
        }
        if (flag == 1 && cnt == 0) {
            flag = 2;
        }
        if (flag == 2) {
            nSubString[j] = str[i];
            nSubString[j+1] = '\0';
            j++;
        }
    }
    return nSubString;
}

char *lastBracket(char *str) {
    int flag = 0;
    int cnt = 0;
    char *nSubString = (char *)malloc(10000*sizeof(char));
    for (int i=0; i<strlen(str); i++) {
        if (str[i] == '{') {
            flag = 1;
            cnt++;
        } else if(str[i] == '}') {
            cnt--;
        }
        nSubString[i] = str[i];
        nSubString[i+1] = '\0';
        if (flag && cnt == 0) {
            break;
        }
    }
    return nSubString;
}

char *postLastBracket(char *str) {
    int flag = 0;
    int cnt = 0;
    int j = 0;
    char *nSubString = (char *)malloc(10000*sizeof(char));
    for (int i=0; i<strlen(str); i++) {
        if (str[i] == '{' && flag != 2) {
            flag = 1;
            cnt++;
        } else if(str[i] == '}' && flag != 2) {
            cnt--;
        } else if ((flag && cnt < 1) || flag == 2) {
            flag = 2;
            nSubString[j] = str[i];
            nSubString[j+1] = '\0';
            j++;
        }
    }
    return nSubString;
}

int determinePointer(char* str, char c) {
    int cnt = 0;
    for (int i=strlen(str)-1; i>-1; i--) {
        if(str[i] == c) {
            cnt++;
        } else {
            return cnt;
        }
    }
    return cnt;
}

int numberOfcharInString(char* str, char c) {
    int cnt = 0;
    for (int i=0; i<strlen(str); i++) {
        if(str[i] == c) {
            cnt++;
        }
    }
    return cnt;
}

int isVal(char *str) { //not full proof
    if(str[0] == '\"') {
        if (2 == numberOfcharInString(str, '\"') && str[strlen(str)-1] == '\"') {
            return 1;
        }
    } else if(str[0] == '\'') {
        if (2 == numberOfcharInString(str, '\'') && str[strlen(str)-1] == '\'') {
            return 1;
        }
    } else if(isdigit(str[0])) {
        for (int i = 0; i<strlen(str); i++) {
            if (!isdigit(str[i])) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int isReturn(char *str) { //not full proof
    if (!strcmp(str, "return")) {
        return 1;
    }
    return 0;
}

int isOper(char *str) { //not full proof
    if (!strcmp(str, "=") || !strcmp(str, "+") || !strcmp(str, "-") || !strcmp(str, "*") || !strcmp(str, "/") || !strcmp(str, "%") || !strcmp(str, "+=") || !strcmp(str, "-=") || !strcmp(str, "*=") || !strcmp(str, "/=") || !strcmp(str, "%=") || !strcmp(str, "<") || !strcmp(str, ">") || !strcmp(str, "<=") || !strcmp(str, ">=")) {
        return 1;
    }
    return 0;
}

int isStorageC(char *str) { //not full proof
    if (!strcmp(str, "static") || !strcmp(str, "extern") || !strcmp(str, "auto") || !strcmp(str, "register")) {
        return 1;
    }
    return 0;
}

int isFuncCall(char *str) { //not full proof
    if(strstr(str, "(") == NULL || strstr(str, ")") == NULL || !isalnum(*(strstr(str, "(")-1))) {
        return 0;
    }
    return 1;
}

int isVar(char *str) { //not full proof
    for (int i = 0; i<strlen(str); i++) {
        if (!isalnum(str[i]) && str[i] != '&' && str[i] != '*') {
            return 0;
        }
    }
    return 1;
}

int isDataType(char *str) { //not full proof
    if (!strcmp(str, "int") || !strcmp(str, "char") || !strcmp(str, "long") || !strcmp(str, "double") || !strcmp(str, "float") || !strcmp(str, "short") || !strcmp(str, "unsigned") || !strcmp(str, "signed") || !strcmp(str, "void")) {
        return 1;
    }
    return 0;
}

int containsDataType(char *str) { //not full proof
    if (strstr(str, "int") != NULL || strstr(str, "char") != NULL || strstr(str, "long") != NULL || strstr(str, "double") != NULL || strstr(str, "float") != NULL || strstr(str, "short") != NULL || strstr(str, "unsigned") != NULL || strstr(str, "signed") != NULL || strstr(str, "void") != NULL) {
        return 1;
    }
    return 0;
}

//enum tokType{string, chr, num, var, oper, func};

char *getToken(char *str) {
    if (strlen(str) == 0) {
        return NULL;
    }
    
    int paraCnt = 0;
    
    char *token = malloc(1000*sizeof(char));
    token[0] = '\0';
    int chrFlag = 0;
    int strFlag = 0;
    for (int i = 0; i<strlen(str); i++) {
        
        if (str[i] == '\'' && paraCnt == 0) {
            if (chrFlag) {
                chrFlag = 0;
            } else {
                chrFlag = 1;
            }
        }
        
        if (str[i] == '\"' && paraCnt == 0) {
            if (strFlag) {
                strFlag = 0;
            } else {
                strFlag = 1;
            }
        }
        
        if((str[i] == '=' || str[i] == '+' || str[i] == '-' || (str[i] == '*' && !isalpha(str[i+1])) || str[i] == '/' || str[i] == '%' || str[i] == '<' || str[i] == '>' || str[i] == '^') && paraCnt == 0) {
            if (i == 0) {
                token[i] = str[i];
                token[i+1] = '\0';
                i++;
                for (; i<strlen(str); i++) {
                    if (isalnum(str[i]) || str[i] == '&' || str[i] == '*' || str[i] == ' ') {
                        break;
                    }
                    token[i] = str[i];
                    token[i+1] = '\0';
                }
            }
            break;
        }
        
        if (!chrFlag && !strFlag && paraCnt == 0) {
            if(str[i] == ' ') {
                break;
            }
        }
        token[i] = str[i];
        token[i+1] = '\0';
        if (str[i] == '(') {
            paraCnt++;
        }
        if (str[i] == ')') {
            paraCnt--;
        }
    }
    //printf("Word: !%s!\n", token);
    return token;
}
