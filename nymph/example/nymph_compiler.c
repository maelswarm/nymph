//
//  main.c
//  nymph
//
//  Created by fairy-slipper on 7/13/17.
//  Copyright © 2017 fairy-slipper. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#define DEBUG 1

struct dict {
    char key[1000];
    char val[1000];
};

struct sDict {
    char *objectName;
    char *statement;
};

struct functions {
    char *returnType;
    char *name;
};

//void parse(char *, FILE *, FILE *, struct dict **, int *, const char[]);
char *parse(char *token, int *pos, FILE *outputCFP, FILE *outputHFP, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar, char **filesCompiled, int *filesCompiledLength, struct functions **functions, int *functionsLength);

char *includeCreate(char *token, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar, char **filesCompiled, int *filesCompiledLength, struct functions **functions, int *functionsLength);

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

void checkForThis(char **str) {
    while(strstr(*str, "this") != NULL) {
        *str = str_replace(*str, "this", "rebrab");
    }
}

void checkForString(char **str) {
    while(strstr(*str, "String") != NULL) {
        *str = str_replace(*str, "String", "char *");
    }
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
    
    while(len-1 > 0 && (isspace(ptr[len-1]) || ispunct(ptr[len-1]) && ptr[len-1] != '*'))
        ptr[--len] = 0;
    
    while(*ptr && (isspace(*ptr) || ispunct(*ptr) && ptr[len-1] != '*'))
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
    
    while(len-1 > 0 && (!isnumber(ptr[len-1])))
        ptr[--len] = 0;
    
    while(*ptr && (!isnumber(ptr[len-1])))
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

char *subString(char *string, char end) {
    char *substring = (char *)malloc(1000*sizeof(char));
    int i = 0;
    for(; string[i] != '\0' && string[i] != end; i++) {
        substring[i] = string[i];
    }
    substring[i] = '\0';
    return substring;
}

char *postSubString(char *string, char end) {
    char *substring = (char *)malloc(1000*sizeof(char));
    int i = 0;
    int j = 0;
    int flag = 0;
    for(; string[i] != '\0'; i++) {
        if (string[i] == end && !flag) {
            flag = 1;
        } else if (flag) {
            substring[j] = string[i];
            j++;
        }
    }
    substring[j] = '\0';
    return substring;
}

char *subStringLastOccurance(char *string, char end) {
    char *substring = (char *)malloc(1000*sizeof(char));
    int i = strlen(string)-1;
    for(;string[i] != end; i--) {}
    for (int idx = 0; idx < i; idx++) {
        substring[idx] = string[idx];
    }
    
    substring[i] = '\0';
    return substring;
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
    char *substring = (char *)malloc(1000*sizeof(char));
    for (int i=0; i<strlen(str); i++) {
        if (str[i] == '(') {
            flag = 1;
            cnt++;
        } else if(str[i] == ')') {
            cnt--;
        }
        substring[i] = str[i];
        substring[i+1] = '\0';
        if (flag && cnt == 0) {
            break;
        }
    }
    return substring;
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

char *rightAssignmentCreate(char *token, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar) {
    char *returnStr = malloc(1000*sizeof(char));
    int spaceFlag = 0;
    char *str1 = malloc(1000*sizeof(char));
    char *str2 = malloc(1000*sizeof(char));
    char *origStr2 = malloc(1000*sizeof(char));
    trim(token);
    for (int i=0; i<strlen(token); i++) {
        if (isspace(token[i])) {
            spaceFlag = 1;
        }
        if (!spaceFlag) {
            str1[i] = token[i];
            str1[i+1] = '\0';
        }
        if (spaceFlag) {
            if (isalpha(token[i])) {
                spaceFlag = 2;
            } else {
                str1[i] = token[i];
                str1[i+1] = '\0';
            }
        }
        if (spaceFlag == 2) {
            str2[i-strlen(str1)] = token[i];
            str2[i-strlen(str1)+1] = '\0';
        }
    }
    str1 = str_replace(str1, " ", "");
    str1 = str_replace(str1, "*", "1");
    trim(str1);
    strcpy(origStr2, str2);
    trimAllButAlphaAndStar(origStr2);
    trimAllButAlpha(str2);
    if(strcmp(str1, "") && strcmp(str2, "")) {
        struct dict *newDict = malloc(sizeof(struct dict));
        strcpy(newDict->key, str1);
        strcpy(newDict->val, str2);
        myDict[*myDictLen] = newDict;
        (*myDictLen)++;
        if (DEBUG) {
            printf("String1: %s String2: %s\n", str1, str2);
        }
    }
    
    if (strstr(str1, "new") != NULL) {
        char *size = malloc(100*sizeof(char));
        strcpy(size, str1);
        trimAllButNumbers(size);
        strcmp(returnStr, "");
        strcat(returnStr, "malloc(");
        if (strcmp(size, "")) {
            strcat(returnStr, size);
            strcat(returnStr, "*");
        }
        strcat(returnStr, "sizeof(");
        trimAllButLetterAndStar(origStr2);
        strcat(returnStr, origStr2);
        strcat(returnStr, "));");
        for (int i=0; i<*mySDictLength; i++) {
            //printf("Checking for initial %s %s\n%s\n%s\n%s\n", mySDict[i]->objectName, str2, token, currentVar, mySDict[i]->statement);
            if(!strcmp(mySDict[i]->objectName, str2)) {
                if (strstr(mySDict[i]->statement, "=")) {
                    trim(mySDict[i]->statement);
                    char *var = postSubString(mySDict[i]->statement, ' ');
                    strcat(returnStr, currentVar);
                    for (int i=0; i<*myDictLen; i++) {
                        
                    }
                    strcat(returnStr, "->"); //need to detect "->" vs. "."
                    strcat(returnStr, var);
                    strcat(returnStr, ";");
                    //printf("%s\n",returnStr);
                    free(var);
                }
            }
        }
    } else {
        free(returnStr);
        returnStr = NULL;
    }
    free(str1);
    free(str2);
    free(origStr2);
    if (returnStr != NULL) {
        return returnStr;
    }
    return NULL;
}

void leftAssignmentCreate(char *token, struct dict **myDict, int *myDictLen, char *currentVar) { //consider rewriting
    int spaceFlag = 0;
    char *str1 = malloc(1000*sizeof(char));
    char *str2 = malloc(1000*sizeof(char));
    str2[0] = '\0';
    trim(token);
    for (int i=0; i<strlen(token); i++) {
        if (isspace(token[i])) {
            spaceFlag = 1;
        }
        if (!spaceFlag) {
            str1[i] = token[i];
            str1[i+1] = '\0';
        }
        if (spaceFlag) {
            if (isalpha(token[i]) || token[i] == '=') {
                spaceFlag = 2;
            } else {
                str1[i] = token[i];
                str1[i+1] = '\0';
            }
        }
        if (spaceFlag == 2) {
            str2[i-strlen(str1)] = token[i];
            str2[i-strlen(str1)+1] = '\0';
        }
    }
    //printf("String1: %s String2: %s\n", str1, str2);
    if(strcmp(str2, "")) {
        strcpy(currentVar, str2);
    } else {
        if(strcmp(str1, "=")) {
            strcpy(currentVar, str1);
        }
    }
    //printf("CURRENTVAR: %s\n", str1);
    str1 = str_replace(str1, " ", "");
    int numOfStar = numberOfcharInString(str1, '*');
    char *starStr = malloc(sizeof(char));
    snprintf(starStr, sizeof(int), "%i", numOfStar);
    strcat(str1, starStr);
    str1 = str_replace(str1, "*", "");
    trim(str1);
    trimAllButAlpha(str2);
    if(strcmp(str1, "") && strcmp(str2, "")) {
        struct dict *newDict = malloc(sizeof(struct dict));
        strcpy(newDict->key, str1);
        strcpy(newDict->val, str2);
        myDict[*myDictLen] = newDict;
        (*myDictLen)++;
        if (DEBUG) {
            printf("myDict1: %s myDict2: %s\n", str1, str2);
        }
    }
    free(str1);
    free(str2);
}

char *functionCreate(char *token, FILE *outputHFP, struct dict **myDict, int *myDictLen, struct functions **functions, int *functionsLength) {
    char *function = malloc(1000*sizeof(char));
    char **parameters = malloc(100*sizeof(char *));
    int parametersLength = 0;
    char functionH[1000];
    memset(functionH, '\0', sizeof(functionH));
    trim(function);
    
    if ((strstr(token, "private") != NULL || strstr(token, "public") != NULL)) {
        char *accessType = subString(token, ' ');
        token += strlen(accessType) + 1;
        trim(accessType);
        
        char *returnType = subString(token, ' ');
        token += strlen(returnType) + 1;
        
        char *functionName = subString(token, '(');
        token += strlen(functionName) + 1;
        
        trim(functionName);
        
        if (!strcmp(accessType, "private") && strcmp(functionName, "main")) {
            strcat(function, "static ");
        }
        
        functions[*functionsLength] = malloc(sizeof(struct functions));
        functions[*functionsLength]->name = malloc(1000*sizeof(char));
        functions[*functionsLength]->returnType = malloc(1000*sizeof(char));
        strcpy(functions[*functionsLength]->name, functionName);
        strcpy(functions[*functionsLength]->returnType, returnType);
        (*functionsLength)++;
        
        char *functionInner = subString(token, ')');
        
        if (strcmp(functionName, "main")) {
            while (strstr(token, ",") != NULL) {
                parameters[parametersLength] = subString(token, ',');
                token += strlen(parameters[parametersLength]) + 1;
                trim(parameters[parametersLength]);
                
                
                int starCnt = numberOfcharInString(parameters[parametersLength], '*');
                char *postType = postSubString(parameters[parametersLength], ' ');
                trimAllButAlpha(postType);
                parameters[parametersLength] = subString(parameters[parametersLength], ' '); // what if box* mybox,... !!!
                myDict[*myDictLen] = malloc(sizeof(struct sDict));
                strcpy(myDict[*myDictLen]->key, parameters[parametersLength]);
                strcpy(myDict[*myDictLen]->val, postType);
                free(postType);
                (*myDictLen)++;
                
                char *starStr = malloc(sizeof(char));
                snprintf(starStr, sizeof(int), "%i", starCnt);
                strcat(parameters[parametersLength], starStr);
                parametersLength++;
            }
            
            parameters[parametersLength] = subString(token, ')');
            token += strlen(parameters[parametersLength]) + 1;
            trim(parameters[parametersLength]);
            
            int starCnt = numberOfcharInString(parameters[parametersLength], '*');
            char *postType = postSubString(parameters[parametersLength], ' ');
            trimAllButAlpha(postType);
            parameters[parametersLength] = subString(parameters[parametersLength], ' '); // what if box* mybox,... !!!
            myDict[*myDictLen] = malloc(sizeof(struct sDict));
            strcpy(myDict[*myDictLen]->key, parameters[parametersLength]);
            strcpy(myDict[*myDictLen]->val, postType);
            free(postType);
            (*myDictLen)++;
            
            char *starStr = malloc(sizeof(char));
            snprintf(starStr, sizeof(int), "%i", starCnt);
            strcat(parameters[parametersLength], starStr);
            parametersLength++;
        }
        
        strcat(function, returnType);
        strcat(function, " ");
        strcat(function, functionName);
        //    if (strcmp(functionName, "main")) { //namespace
        //        for (int i=0; i<parametersLength; i++) {
        //            strcat(function, parameters[i]);
        //        }
        //    }
        strcat(function, "(");
        strcat(function, functionInner);
        strcat(function, ") {");
        
        if (strstr(accessType, "public")) {
            strcat(functionH, returnType);
            strcat(functionH, " ");
            strcat(functionH, functionName);
            //        for (int i=0; i<parametersLength; i++) { //namespace
            //            strcat(functionH, parameters[i]);
            //        }
            strcat(functionH, "(");
            strcat(functionH, functionInner);
            strcat(functionH, ");\n");
            fwrite(functionH , 1 , strlen(functionH) , outputHFP);
        }
    } else {
        function = subString(token, '{'); //doesn't recursively check function calls
        token += strlen(function) + 1;
        strcat(function, "{");
    }
    return function;
}

char *functionCall(char *token, int *pos, FILE *outputCFP, FILE *outputHFP, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar, char **filesCompiled, int *filesCompiledLength, struct functions **functions, int *functionsLength) {
    char *function = malloc(1000*sizeof(char));
    char **parameters = malloc(100*sizeof(char *));
    int parametersLength = 0;
    
    char *functionName = subString(token, '(');
    token += strlen(functionName) + 1;
    trim(functionName);
    
    while (strstr(token, ",") != NULL) {
        char *parameter = subString(token, ',');
        if (strstr(parameter, "(") != NULL) {
            char *nxtFunction = lastPtheses(token);
            strcat(nxtFunction, ";");
            parameters[parametersLength] = parse(nxtFunction, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength);
            
            parameters[parametersLength] = str_replace(parameters[parametersLength],";", "");
            nxtFunction = str_replace(nxtFunction, ";", "");
            token += strlen(nxtFunction);
            
            parametersLength++;
            
        } else {
            strcat(parameter, ";");
            parameters[parametersLength] = parse(parameter, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength);
            
            parameters[parametersLength] = str_replace(parameters[parametersLength],";", "");
            parameter = str_replace(parameter,";", "");
            token += strlen(parameter) + 1;
            
            strcat(parameters[parametersLength], ",");
            
            parametersLength++;
        }
    }
    char *parameter = subString(token, ')');
    parameters[parametersLength] = parse(parameter, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength);
    token += strlen(parameter) + 1;
    
    parametersLength++;
    
    
    strcat(function, functionName);
    strcat(function, "(");
    for (int i=0; i<parametersLength; i++) {
        strcat(function, parameters[i]);
    }
    strcat(function, ");");
    
    return function;
    
}

char *createStruct(char *token, FILE *outputHFP, struct sDict **mySDict, int *mySDictLength) {
    char *myStruct = malloc(1000*sizeof(char));
    char structArrH[1000];
    memset(structArrH, '\0', sizeof(structArrH));
    
    char *tmp = subString(token, ' '); //skip 'object'
    token += strlen(tmp) + 1;
    free(tmp);
    
    char *structName = subString(token, '{');
    token += strlen(structName) + 1;
    trim(structName);
    
    strcpy(myStruct, "\n");
    strcat(myStruct, "struct ");
    strcat(myStruct, structName);
    strcat(myStruct, "{ ");
    
    char *tokenCopy = malloc(1000*sizeof(char));
    strcpy(tokenCopy, token);
    for (int i = 0; i<strlen(token); i++) {
        if (token[i] == ';') {
            mySDict[*mySDictLength] = malloc(sizeof(struct sDict *));
            mySDict[*mySDictLength]->statement = malloc(1000*sizeof(char));
            mySDict[*mySDictLength]->statement = subString(tokenCopy, ';');
            tokenCopy += strlen(mySDict[*mySDictLength]->statement) + 1;
            mySDict[*mySDictLength]->objectName = malloc(1000*sizeof(char));
            strcpy(mySDict[*mySDictLength]->objectName, structName);
            
            tmp = subString(mySDict[*mySDictLength]->statement, '=');
            strcat(myStruct, tmp);
            strcat(myStruct, ";");
            free(tmp);
            
            (*mySDictLength)++;
        }
    }
    
    strcat(myStruct, "\n");
    strcat(myStruct, "};\n");
    
    strcpy(structArrH, "");
    strcat(structArrH, "typedef struct ");
    strcat(structArrH, structName);
    strcat(structArrH, " ");
    strcat(structArrH, structName);
    strcat(structArrH, ";\n");
    
    fwrite(structArrH , 1 , strlen(structArrH) , outputHFP);
    fwrite(myStruct , 1 , strlen(myStruct) , outputHFP);
    
    free(structName);
    
    return myStruct;
}

char *createForStatement(char *token) {
    
}

int findNearestSymbol(char *token, int *pos) {
    int openParaFlag = 0;
    int hashFlag = 0;
    int dotFlag = 0;
    *pos = 0;
    
    for(int i=0; i<strlen(token); i++,(*pos)++) {
        char tmp[1000];
        strncpy(tmp, &token[i], 6*sizeof(char));
        if(token[i] == '=') {
            return 0;
        } else if(token[i] == ';' && i != 0) {
            if(openParaFlag == 2) {
                return 2;
            } else {
                return 1;
            }
        } else if(token[i] == '{') {
            if(openParaFlag == 2) {
                return 3;
            } else {
                return -1;
            }
        } else if(token[i] == '(') {
            openParaFlag = 1;
        } else if(token[i] == ')' && !isalpha(token[i+1]) && openParaFlag == 1) {
            openParaFlag = 2;
        } else if(token[i] == '#') {
            hashFlag = 1;
        } else if(token[i] == '.') {
            dotFlag = 1;
        } else if((token[i] == '\"' && hashFlag) || (token[i] == '>' && hashFlag)) {
            if(dotFlag) {
                if (token[i] == '\"') {
                    return 4;
                } else if(token[i] == '>') {
                    return -1;
                }
            }
        } else if(!strcmp(tmp, "object")) {
            return 6;
        }
    }
    return 5;
}

char *parse(char *token, int *pos, FILE *outputCFP, FILE *outputHFP, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar, char **filesCompiled, int *filesCompiledLength, struct functions **functions, int *functionsLength) {
    int symbol = findNearestSymbol(token, pos);
    char *output = malloc(100000*sizeof(char));
    char *functionBody;
    char *myStruct;
    memset(output, '\0', sizeof(output));
    strcpy(output, "");
    (*pos)++;
    if (DEBUG) {
        printf("Symbol: %i token %s\n", symbol, token);
    }
    char str[1000];
    memset(str, '\0', sizeof(str));
    switch(symbol) {
        case -1: //do nothing
            strncpy(str, token, *pos*sizeof(char));
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            strcat(output, str);
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            break;
        case 0: //left assignment
            strncpy(str, token, *pos*sizeof(char));
            leftAssignmentCreate(str, myDict, myDictLen, currentVar);
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            strcat(output, str);
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            break;
        case 1: //right assignment or declaration
            strncpy(str, token, *pos*sizeof(char));
            char *rightAss = rightAssignmentCreate(str, myDict, myDictLen, mySDict, mySDictLength, currentVar);
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            if (rightAss != NULL) {
                strcat(output, rightAss);
                strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            } else {
                if (strcmp(str, ";")) {
                    strcat(output, str);
                }
                strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            }
            break;
        case 2: //function call
            strncpy(str, token, *pos*sizeof(char));
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            int a = *pos;
            char *function = functionCall(str, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength);
            strcat(output, function);
            strcat(output, parse(token+strlen(str), pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            if (DEBUG) {
                //printf("token1222: %s output: %s\n", token, output);
            }
            free(function);
            break;
        case 3: //function declaration
            strncpy(str, token, *pos*sizeof(char));
            strcat(output, functionCreate(str, outputHFP, myDict, myDictLen, functions, functionsLength));
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            break;
        case 4: //include
            strncpy(str, token, *pos*sizeof(char));
            strcat(output, includeCreate(str, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
            break;
        case 5: //end
            strcat(output, token);
            if (DEBUG) {
                printf("String: %s\n", output);
            }
            return output;
            break;
        case 6: //object
            if (DEBUG) {
                printf("String: %s\n", token);
            }
            //token = str_replace(token, "object", "struct");
            char *myStruct = createStruct(token, outputHFP, mySDict, mySDictLength);
            //strcat(output, myStruct);
            break;
//        case 6: //object
//            strncpy(str, token, *pos*sizeof(char));
//            if (DEBUG) {
//                printf("String: %s\n", str);
//            }
//            char *forStatement = createForStatement(str, outputHFP, mySDict, mySDictLength);
//            strcat(output, forStatement);
//            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength));
//            break;
    }
    return output;
}

char *includeCreate(char *token, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar, char **filesCompiled, int *filesCompiledLength, struct functions **functions, int *functionsLength) {
    char *str = malloc(1000*sizeof(char));
    memset(str, '\0', sizeof(str));
    char *statement = subString(token, '.');
    
    if(strstr(statement, "<") == NULL) {
        char *name = postSubString(token, '\"');
        name = subString(name, '.');
        char *newToken;
        int *pos = malloc(sizeof(int));
        const char s[2] = "}";
        FILE *inputFP;
        char hFile[100];
        char cFile[100];
        char nFile[100];
        strcpy(cFile, name);
        strcat(cFile, ".c");
        strcpy(hFile, name);
        strcat(hFile, ".h");
        strcpy(nFile, name);
        strcat(nFile, ".n");
        
        int fileCheck = 0;
        for (int i=0; i<*filesCompiledLength; i++) {
            if (!strcmp(nFile, filesCompiled[i])) {
                fileCheck = 1;
                break;
            }
        }
        
        if (!fileCheck) { //check for leaks
            
            filesCompiled[*filesCompiledLength] = malloc(100*sizeof(char));
            strcpy(filesCompiled[*filesCompiledLength], nFile);
            (*filesCompiledLength)++;
            
            FILE *newOutputCFP = fopen(cFile, "wb+");
            FILE *newOutputHFP = fopen(hFile, "wb+");
            
            inputFP = fopen(nFile, "rb");
            
            char str2[100];
            strcpy(str2, "#include \"");
            strcat(str2, hFile);
            strcat(str2, "\"\n");
            fwrite(str2 , 1 , strlen(str2) , newOutputCFP);
            
            char *buffer = loadFile(inputFP);
            newToken = strtok(buffer, s);
            buffer += strlen(newToken) + 1;
            
            while(newToken != NULL && strcmp(newToken, "")) {
                if (DEBUG) {
                    printf("TOKEN1: %s\n", newToken);
                }
                char *output = parse(newToken, pos, newOutputCFP, newOutputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar, filesCompiled, filesCompiledLength, functions, functionsLength);
                if (strstr(output, "#include") != NULL && strstr(output, "{") == NULL) {
                    strcat(output, "\n");
                } else {
                    strcat(output, "}\n");
                }
                newToken = strtok(buffer, s);
                buffer += strlen(newToken) + 1;
                trim(newToken);
//                if (newToken == NULL || !strcmp(newToken, "")) {
//                    strcat(output, "}");
//                }
                fwrite(output, 1, strlen(output), newOutputCFP);
                free(output);
            }
            
            fclose(inputFP);
            fclose(newOutputCFP);
            fclose(newOutputHFP);
        }
    }
    
    token += strlen(statement) + 1;
    strcpy(str, "");
    strcat(statement, ".h\"\n");
    strcat(str, statement);
    free(statement);
    
    return str;
}

int main(int argc, const char * argv[]) {
    
    struct dict **myDict;
    myDict = malloc(1000*sizeof(struct dict *));
    int myDictLen = 0;
    struct sDict **statements = malloc(100*sizeof(struct sDict *));
    int statementsLength = 0;
    char *currentVar = malloc(1000*sizeof(char));
    char **filesCompiled = malloc(1000*sizeof(char*));
    int filesCompiledLength = 0;
    struct functions **functions = malloc(1000*sizeof(struct functions*));
    int functionsLength = 0;
    
    char *token;
    int *pos = malloc(sizeof(int));
    const char s[2] = "}";
    FILE *inputFP;
    FILE *outputCFP;
    FILE *outputHFP;
    
    if (argc != 3) {
        printf("Input and Output parameter needed.\n");
        return 1;
    }
    
    inputFP = fopen (argv[1], "rb");
    filesCompiled[filesCompiledLength] = malloc(100*sizeof(char));
    strcpy(filesCompiled[filesCompiledLength], argv[1]);
    filesCompiledLength++;
    
    char hFile[100];
    char cFile[100];
    strcpy(cFile, argv[2]);
    strcat(cFile, ".c");
    strcpy(hFile, argv[2]);
    strcat(hFile, ".h");
    outputCFP = fopen(cFile, "wb+");
    outputHFP = fopen(hFile, "wb+");
    
    char str[100];
    strcpy(str, "#include \"");
    strcat(str, hFile);
    strcat(str, "\"\n");
    fwrite(str , 1 , strlen(str) , outputCFP);
    
    char *buffer = loadFile(inputFP);
    token = strtok(buffer, s);
    buffer += strlen(token) + 1;
    
    while(token != NULL && strcmp(token, "")) {
        if (DEBUG) {
            printf("TOKEN: %s\n", token);
        }
        char *output = parse(token, pos, outputCFP, outputHFP, myDict, &myDictLen, statements, &statementsLength, currentVar, filesCompiled, &filesCompiledLength, functions, &functionsLength);
        printf("MAIN OUT: %s\n", output);
        if (strstr(output, "#include") != NULL && strstr(output, "{") == NULL) {
            strcat(output, "\n");
        } else {
            strcat(output, "}\n");
        }
        //printf("TRUE OUTPUT: %s", output);
        token = strtok(buffer, s);
        buffer += strlen(token) + 1;
        trim(token);
//        if (token == NULL || !strcmp(token, "")) {
//            strcat(output, "}");
//        }
        fwrite(output, 1, strlen(output), outputCFP);
        free(output);
    }
    
    fclose(inputFP);
    fclose(outputCFP);
    fclose(outputHFP);
    
    return 0;
}
