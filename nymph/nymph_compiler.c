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

struct dict {
    char *key;
    char *val;
};

regex_t functionRegex; //function
regex_t functionCallRegex; //function
regex_t objectRegex; //function
regex_t includeRegex; //function
regex_t objectDictRegex; //function

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

void createInclude(FILE *outputCFP, char *token) {
    char includeArr[1000];
    memset(includeArr, '\0', sizeof(includeArr));
    
    char *name = subString(token, '.');
    token += strlen(name) + 1;
    
    strcpy(includeArr, "");
    strcat(includeArr, name);
    strcat(includeArr, ".h\"\n");
    
    fwrite(includeArr , 1 , strlen(includeArr) , outputCFP);
    
    free(name);
}

void createStruct(FILE *outputCFP, char *token, FILE *outputHFP) {
    char structArr[1000];
    char structArrH[1000];
    memset(structArr, '\0', sizeof(structArr));
    memset(structArrH, '\0', sizeof(structArrH));
    
    
    char *tmp = subString(token, ' '); //skip 'object'
    token += strlen(tmp) + 1;
    
    char *structName = subString(token, '{');
    token += strlen(structName) + 1;
    
    trim(structName);
    
    strcpy(structArr, "\n");
    strcat(structArr, "struct ");
    strcat(structArr, structName);
    strcat(structArr, " {\n");
    
    strcpy(structArrH, "");
    strcat(structArrH, "typedef struct ");
    strcat(structArrH, structName);
    strcat(structArrH, " ");
    strcat(structArrH, structName);
    strcat(structArrH, ";\n");
    
    fwrite(structArr , 1 , strlen(structArr) , outputCFP);
    fwrite(structArrH , 1 , strlen(structArrH) , outputHFP);
    
    free(tmp);
    free(structName);
}

void parser(char *str) {
    
}

char *createFunctionCall(char *token, struct dict myDict[], int myDictLen) { // hello.world(a,b.print());
    char *function = malloc(1000*sizeof(char));
    function[0] = '\0';
    int i = 0;
    int j = 0;
    char *objectName = malloc(1000*sizeof(char));
    char *functionName = malloc(1000*sizeof(char));
    char *functionInner = malloc(1000*sizeof(char));
    objectName[0] = '\0';
    functionName[0] = '\0';
    functionInner[0] = '\0';
    char *str = malloc(1000*sizeof(char));
    str[0] = '\0';
    for (; i<strlen(token); i++, j++) {
        if (token[i] == '.') {
            str[j] = '\0';
            strcpy(objectName, str);
            j = -1;
        } else if(token[i] == '(') {
            str[j] = '\0';
            strcpy(functionName, str);
            if (functionName[0] != '\0') {
                trim(functionName);
                char tmpObjectName[100];
                strcpy(tmpObjectName, objectName);
                trimAllButAlpha(tmpObjectName);
                for (int i=0; i<myDictLen; i++) {
                    if(!strcmp(myDict[i].val, tmpObjectName)) {
                        trim(myDict[i].key);
                        strcat(functionName, myDict[i].key);
                        break;
                    }
                }
            }
            strcat(functionName, "(");
            strcpy(function, functionName);
            if (objectName[0] != '\0') {
                trim(objectName);
                strcat(function, objectName);
                if (token[i+1] != ')') {
                    strcat(function, ",");
                }
            }
            if (functionInner[0] != '\0') {
                strcat(function, ",");
                strcat(function, functionInner);
            }
            j = 0;
            return strcat(function, createFunctionCall(&token[i+1], myDict, myDictLen));
        } else if(token[i] == ',') {
            str[j] = token[i];
            str[j+1] = '\0';
            strcpy(functionInner, str);
            if (functionName[0] != '\0') {
                trimAllButAlpha(functionName);
                strcat(function, functionName);
                strcat(function, "(");
            }
            if (objectName[0] != '\0') {
                trim(objectName);
                strcat(function, objectName);
            }
            if (functionInner[0] != '\0') {
                strcat(function, functionInner);
            }
            j = 0;
            return strcat(function, createFunctionCall(&token[i+1], myDict, myDictLen));
        } else if(token[i] == ')') {
            str[j] = token[i];
            str[j+1] = '\0';
            strcpy(functionInner, str);
            if (functionName[0] != '\0') {
                trimAllButAlpha(functionName);
                strcat(function, functionName);
                strcat(function, "(");
            }
            if (objectName[0] != '\0') {
                trim(objectName);
                strcat(function, objectName);
                strcat(function, ",");
            }
            if (functionInner[0] != '\0') {
                strcat(function, functionInner);
            }
            j = 0;
            return strcat(function, createFunctionCall(&token[i+1], myDict, myDictLen));
        } else if(token[i] == ';') {
            return ";";
        } else {
            str[j] = token[i];
        }
    }
    return NULL;
}

void createFunction(FILE *outputCFP, char *token, FILE *outputHFP, struct dict myDict[], int myDictLen) {
    char function[1000];
    char functionH[1000];
    char **parameters = (char **)malloc(100*sizeof(char *));
    int parametersLength = 0;
    memset(function, '\0', sizeof(function));
    memset(functionH, '\0', sizeof(functionH));
    
    char *accessType = subString(token, ' ');
    token += strlen(accessType) + 1;
    
    char *returnType;
    returnType = subString(token, ' ');
    token += strlen(returnType) + 1;
    
    char *objectType = subString(token, '.');
    token += strlen(objectType) + 1;
    
    char *functionName = subString(token, '(');
    token += strlen(functionName) + 1;
    if (strchr(objectType, '*') != NULL) {
        //printf("OBJECTTYPE: %s\n", objectType);
        char objectTypeTmp[1000];
        strcpy(objectTypeTmp, objectType);
        trimAllButAlpha(objectTypeTmp);
        strcat(functionName, objectTypeTmp);
    }
    
    checkForString(&returnType);
    checkForString(&objectType);
    
    while (strchr(token, ',') != NULL) {
        char *parameterName = subString(token, ',');
        token += strlen(parameterName) + 1;
        checkForString(&parameterName);
        parameters[parametersLength] = parameterName;
        parametersLength++;
    }
    
    char *parameterName = subString(token, ')');
    parameters[parametersLength] = parameterName;
    parametersLength++;
    token += strlen(parameterName) + 1;
    
    for(int i=0; i<parametersLength; i++) {
        //printf("PARAMETER: %s\n",parameters[i]);
    }
    
    strcpy(function, "\n");
    strcat(function, returnType);
    strcat(function, " ");
    strcat(function, functionName);
    strcat(function, "(");
    strcat(function, objectType);
    strcat(function, " rebrab");
    if (parametersLength > 1) {
        strcat(function, ",");
    }
    for (int i=0; i<parametersLength; i++) {
        if (parametersLength > 1) {
            strcat(function, parameters[i]);
        }
        if(i == parametersLength - 1) {
            strcat(function, ") {\n");
        } else {
            strcat(function, ",");
        }
    }
    
    fwrite(function , 1 , strlen(function) , outputCFP);
    
    trim(accessType);
    printf("%s\n", accessType);
    if (!strcmp(accessType, "public")) {
        strcpy(functionH, "");
        strcat(functionH, returnType);
        strcat(functionH, " ");
        strcat(functionH, functionName);
        strcat(functionH, "(");
        strcat(functionH, objectType);
        strcat(functionH, " rebrab");
        if (parametersLength > 1) {
            strcat(functionH, ",");
        }
        for (int i=0; i<parametersLength; i++) {
            if (parametersLength > 1) {
                strcat(functionH, parameters[i]);
            }
            if(i == parametersLength - 1) {
                strcat(functionH, ");\n");
            } else {
                strcat(functionH, ",");
            }
        }
        fwrite(functionH , 1 , strlen(functionH) , outputHFP);
    }
    
    free(returnType);
    free(functionName);
    free(objectType);
}

int main(int argc, const char * argv[]) {
    
    struct dict myDict[10000];
    int myDictLen = 0;
    char *token;
    const char s[2] = "\n";
    FILE *inputFP;
    FILE *outputCFP;
    FILE *outputHFP;
    
    if (argc != 3) {
        printf("Input and Output parameter needed.\n");
        return 1;
    }
    
    inputFP = fopen (argv[1], "rb");
    char hFile[100];
    char cFile[100];
    strcpy(cFile, argv[2]);
    strcat(cFile, ".c");
    strcpy(hFile, argv[2]);
    strcat(hFile, ".h");
    outputCFP = fopen(cFile, "wb+");
    outputHFP = fopen(hFile, "wb+");
    
    char *buffer = loadFile(inputFP);
    token = strtok(buffer, s);
    int lastIDX = 0;
    
    int retFunction = regcomp(&functionRegex, "[[:alpha:]]\\{1,\\}[[:space:]]*[[:punct:]]*[[:space:]]*\\.[[:alpha:]]\\{1,\\}\\([[:print:]]*\\)[[:space:]]*{[[:space:]]*$", 0);
    int retFunctionCall = regcomp(&functionCallRegex, "^[[:space:]]*[[:print:]]\\{1,\\}[[:punct:]]*\\.[[:alpha:]]\\{1,\\}\\([[:print:]]*\\);[[:space:]]*$", 0);
    int retObject = regcomp(&objectRegex, "^[[:space:]]*object[[:space:]]\\{1,\\}[[:alpha:]]\\{1,\\}[[:space:]]\\{1,\\}{[[:space:]]*$", 0);
    int retInclude = regcomp(&includeRegex, "^[[:space:]]*#include[[:space:]]\\{1,\\}\"[[:alpha:]]\\{1,\\}\\.[[:alpha:]]\\{1,\\}\"[[:space:]]*$", 0);
    int retObjectDict = regcomp(&objectDictRegex, "^[[:space:]]*[[:alpha:]]\\{1,\\}[[:space:]]*[[:alpha:]]\\{1,\\}\\;[[:space:]]*$", 0);
    
    while(token != NULL) {
        
        retFunction = regexec(&functionRegex, token, 0, NULL, 0);
        
        retFunctionCall = regexec(&functionCallRegex, token, 0, NULL, 0);
        //printf("%s %i %i\n", token, retFunctionCall, retFunction);
        
        retObject = regexec(&objectRegex, token, 0, NULL, 0);
        
        retInclude = regexec(&includeRegex, token, 0, NULL, 0);
        
        checkForThis(&token);
        if(!retInclude) {
            createInclude(outputCFP, token);
        } else if (!retFunctionCall) {
            char *function = createFunctionCall(token, myDict, myDictLen);
            strcat(function, "\n");
            fwrite(function , 1 , strlen(function) , outputCFP);
            free(function);
            printf("\n");
        } else if(!retFunction) {
            createFunction(outputCFP, token, outputHFP, myDict, myDictLen);
        } else if(!retObject) {
            createStruct(outputCFP, token, outputHFP);
        } else if(!retObjectDict) {
            checkForString(&token);
            struct dict newDict;
            char *nToken = malloc(100*sizeof(char));
            char *vToken = malloc(100*sizeof(char));
            memset(nToken, '\0', 100*sizeof(char));
            memset(vToken, '\0', 100*sizeof(char));
            int wSpace = 0;
            int flag = 0;
            trim(token);
            for (int i=0; i<strlen(token); i++) {
                if (!flag) {
                    if (token[i] == '*') {
                        nToken[i] = token[i];
                        flag = 1;
                        continue;
                    }
                    if (wSpace == 1) {
                        flag = 1;
                        continue;
                    }
                    if (token[i] == ' ') {
                        wSpace++;
                    }
                    nToken[i] = token[i];
                } else {
                    vToken[i - strlen(nToken)] = token[i];
                    if (token[i] == ' ') {
                        break;
                    }
                }
            }
            trimAllButAlpha(nToken);
            trimAllButAlpha(vToken);
            newDict.key = nToken;
            newDict.val = vToken;
            //printf("NTOKEN: %s Token: %s\n",nToken, token);
            //printf("VTOKEN: %s Token: %s\n",vToken, token);
            
            myDict[myDictLen] = newDict;
            myDictLen++;
            //printf("DICT: %s %s\n", newDict.key, newDict.val);
            
            char newToken[10000];
            memset(newToken, '\0', sizeof(newToken));
            strcpy(newToken, token);
            if(newToken[strlen(newToken) - 1] != '>' && newToken[strlen(newToken) - 1] != ';' && newToken[strlen(newToken) - 1] != '{') {
                strcat(newToken, ";\n");
            } else {
                strcat(newToken, "\n");
            }
            fwrite(newToken , 1 , strlen(newToken) , outputCFP);
            //free(nToken);
            //free(vToken);
        } else {
            checkForString(&token);
            char newToken[10000];
            memset(newToken, '\0', sizeof(newToken));
            strcpy(newToken, token);
            //if(newToken[strlen(newToken) - 1] != '>') {
            //strcat(newToken, ";\n");
            //} else {
            strcat(newToken, "\n");
            //}
            fwrite(newToken , 1 , strlen(newToken) , outputCFP);
        }
        token = strtok(NULL, s);
    }
    
    fclose(inputFP);
    fclose(outputCFP);
    fclose(outputHFP);
    
    return 0;
}
