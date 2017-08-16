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

#define DEBUG 0

struct dict {
    char key[1000];
    char val[1000];
};

struct sDict {
    char *objectName;
    char *statement;
};

//void parse(char *, FILE *, FILE *, struct dict **, int *, const char[]);
char *parse(char *token, int *pos, FILE *outputCFP, FILE *outputHFP, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar);

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
        substring[i] = str[i];
        substring[i+1] = '\0';
        if (str[i] == '(') {
            flag = 1;
            cnt++;
        } else if(str[i] == ')') {
            cnt--;
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
    if (!strcmp(str1, "new")) {
        strcmp(returnStr, "");
        strcat(returnStr, "malloc(sizeof(");
        strcat(returnStr, origStr2);
        strcat(returnStr, "));");
        for (int i=0; i<*mySDictLength; i++) {
            if(!strcmp(mySDict[i]->objectName, str2)) {
                if (strstr(mySDict[i]->statement, "=")) {
                    trim(mySDict[i]->statement);
                    char *var = postSubString(mySDict[i]->statement, ' ');
                    strcat(returnStr, currentVar);
                    strcat(returnStr, "->"); //need to detect "->" vs. "."
                    strcat(returnStr, var);
                    strcat(returnStr, ";");
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
    if(strcmp(str1, "=")) {
        strcpy(currentVar, str1);
    }
    printf("CURRENTVAR: %s\n", str1);
    str1 = str_replace(str1, " ", "");
    str1 = str_replace(str1, "*", "1");
    trim(str1);
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
    free(str1);
    free(str2);
}

char *includeCreate(char *token) {
    char *str = malloc(1000*sizeof(char));
    memset(str, '\0', sizeof(str));
    char *name = subString(token, '.');
    token += strlen(name) + 1;
    
    strcpy(str, "");
    strcat(str, name);
    strcat(str, ".h\"\n");
    free(name);
    
    return str;
}

char *functionCreate(char *token, FILE *outputHFP) {
    char *function = malloc(1000*sizeof(char));
    char functionH[1000];
    char **parameters = (char **)malloc(100*sizeof(char *));
    int parametersLength = 0;
    //memset(function, '\0', sizeof(function));
    memset(functionH, '\0', sizeof(functionH));
    
    char *accessType = subString(token, ' ');
    token += strlen(accessType) + 1;
    
    char *returnType = subString(token, ' ');
    token += strlen(returnType) + 1;
    
    char *objectType = subString(token, '.');
    token += strlen(objectType) + 1;
    
    char *functionName = subString(token, '(');
    token += strlen(functionName) + 1;
    char *objectTypeTmp = str_replace(objectType, "*", "1");
    trim(objectTypeTmp);
    strcat(functionName, objectTypeTmp);
    
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
    trim(parameterName);
    if(strcmp(parameterName, "")) {
        parametersLength++;
    }
    token += strlen(parameterName) + 1;
    
    strcat(function, returnType);
    strcat(function, " ");
    strcat(function, functionName);
    strcat(function, "(");
    strcat(function, objectType);
    strcat(function, " rebrab");
    if (parametersLength > 0) {
        strcat(function, ",");
    }
    for (int i=0; i<parametersLength; i++) {
        strcat(function, parameters[i]);
        if (parametersLength - 1 != i) {
            strcat(function, ",");
        }
    }
    strcat(function, ") {");
    
    trim(accessType);
    if (!strcmp(accessType, "public")) {
        strcpy(functionH, "");
        strcat(functionH, returnType);
        strcat(functionH, " ");
        strcat(functionH, functionName);
        strcat(functionH, "(");
        strcat(functionH, objectType);
        strcat(functionH, " rebrab");
        if (parametersLength > 0) {
            strcat(functionH, ",");
        }
        for (int i=0; i<parametersLength; i++) {
            strcat(functionH, parameters[i]);
            if (parametersLength - 1 != i) {
                strcat(functionH, ",");
            }
        }
        strcat(functionH, ");\n");
        fwrite(functionH , 1 , strlen(functionH) , outputHFP);
    }
    
    free(returnType);
    free(functionName);
    free(objectType);
    free(accessType);
    for (int i=0; i<parametersLength; i++) {
        free(parameters[i]);
    }
    free(parameters);
    
    return function;
}

char *functionCall(char *token, int *pos, FILE *outputCFP, FILE *outputHFP, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar) {
    char *function = malloc(1000*sizeof(char));
    
    char *preObject = malloc(1000*sizeof(char));
    char *object = subString(token, '.');
    token += strlen(object) + 1;
    trim(object);
    
    int flag = 0;
    int i = strlen(object)-1;
    for (i; i>-1; i--) { if(isalpha(object[i]) || object[i] == '&' || object[i] == '*') {} else {break;} }
    if (i > -1) {strncpy(preObject, object, (i + 1)*sizeof(char));object += i + 1;}
    
    char *functionName = subString(token, '(');
    token += strlen(functionName) + 1;
    trim(functionName);
    int pcnt = numberOfcharInString(object, '&');
    char *tmpObject = str_replace(object, "*", "1");
    tmpObject = str_replace(object, " ", "");
    tmpObject = str_replace(object, "&", "");
    
    trim(tmpObject);
    for (int i=0; i<*myDictLen; i++) {
        if(!strcmp(myDict[i]->val, tmpObject)) {
            int n = determinePointer(myDict[i]->key, '1');
            strcpy(tmpObject, myDict[i]->key);
            for(int i=0; i<pcnt;i++) {
                strcat(tmpObject, "1");
            }
            trim(myDict[i]->key);
            strcat(functionName, tmpObject);
            break;
        }
    }
    strcpy(function, "");
    strcat(function, preObject);
    strcat(function, functionName);
    strcat(function, "(");
    strcat(function, object);
    while(strstr(token, ",")) {
        char *parameter = subString(token, ',');
        strcat(parameter, ";");
        char *output = parse(parameter, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar);
        if (strstr(output, "(") == NULL) {
            char *str = str_replace(output, ";", "");
            free(output);
            output = str;
        }
        strcat(function, ",");
        strcat(function, output);
        free(parameter);
        free(output);
    }
    char *lastParam = lastPtheses(token);
    token += strlen(lastParam);
    trim(lastParam);
    if (strcmp(lastParam, "")) {
        if (lastParam[0] != ')') {
            strcat(function, ",");
        }
        char *output = parse(lastParam, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar);
        if (strstr(output, "(") == NULL) {
            char *str = str_replace(output, ";", "");
            free(output);
            output = str;
        }
        strcat(function, output);
        free(output);
    }
    free(lastParam);
    free(preObject);
    free(tmpObject);
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
    
    strcpy(structArrH, "");
    strcat(structArrH, "typedef struct ");
    strcat(structArrH, structName);
    strcat(structArrH, " ");
    strcat(structArrH, structName);
    strcat(structArrH, ";\n");
    
    fwrite(structArrH , 1 , strlen(structArrH) , outputHFP);
    
    free(structName);
    
    return myStruct;
}

int findNearestSymbol(char *token, int *pos) {
    int dotFlag = 0;
    int hashFlag = 0;
    *pos = 0;
    
    for(int i=0; i<strlen(token); i++,(*pos)++) {
        char tmp[1000];
        strncpy(tmp, &token[i], 6*sizeof(char));
        if(token[i] == '=') {
            return 0;
        } else if(token[i] == ';') {
            if(dotFlag) {
                return 2;
            } else {
                return 1;
            }
        } else if(token[i] == '{') {
            if(dotFlag) {
                return 3;
            } else {
                return -1;
            }
        } else if(token[i] == '.') {
            dotFlag = 1;
        } else if(token[i] == '#') {
            hashFlag = 1;
        } else if((token[i] == '\"' && hashFlag) || (token[i] == '>' && hashFlag)) {
            if(dotFlag) {
                if (token[i] == '\"') {
                    return 4;
                } else {
                    return -1;
                }
            }
        } else if(!strcmp(tmp, "object")) {
            return 6;
        }
    }
    return 5;
}

char *parse(char *token, int *pos, FILE *outputCFP, FILE *outputHFP, struct dict **myDict, int *myDictLen, struct sDict **mySDict, int *mySDictLength, char *currentVar) {
    int symbol = findNearestSymbol(token, pos);
    char *output = malloc(100000*sizeof(char));
    char *functionBody;
    char *myStruct;
    memset(output, '\0', sizeof(output));
    strcpy(output, "");
    (*pos)++;
    if (DEBUG) {
        printf("Symbol: %i\n", symbol);
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
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar));
            break;
        case 0: //left assignment
            checkForThis(&token);
            strncpy(str, token, *pos*sizeof(char));
            leftAssignmentCreate(str, myDict, myDictLen, currentVar);
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            strcat(output, str);
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar));
            break;
        case 1: //right assignment or declaration
            checkForThis(&token);
            strncpy(str, token, *pos*sizeof(char));
            char *rightAss = rightAssignmentCreate(str, myDict, myDictLen, mySDict, mySDictLength, currentVar);
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            if (rightAss != NULL) {
                strcat(output, rightAss);
                strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar));
            } else {
                if (strcmp(str, ";")) {
                    strcat(output, str);
                }
                strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar));
            }
            break;
        case 2: //function call
            functionBody = lastPtheses(token);
            if (DEBUG) {
                printf("String: %s\n", functionBody);
            }
            int a = *pos;
            char *function = functionCall(functionBody, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar);
            strcat(output, function);
            strcat(output, parse(token+strlen(output)+strlen(functionBody), pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar));
            free(function);
            break;
        case 3: //function declaration
            strncpy(str, token, *pos*sizeof(char));
            strcat(output, functionCreate(str, outputHFP));
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar));
            break;
        case 4: //include
            strncpy(str, token, *pos*sizeof(char));
            strcat(output, includeCreate(str));
            if (DEBUG) {
                printf("String: %s\n", str);
            }
            strcat(output, parse(token+*pos, pos, outputCFP, outputHFP, myDict, myDictLen, mySDict, mySDictLength, currentVar));
            break;
        case 5: //end
            break;
        case 6: //object
            if (DEBUG) {
                printf("String: %s\n", token);
            }
            //token = str_replace(token, "object", "struct");
            char *myStruct = createStruct(token, outputHFP, mySDict, mySDictLength);
            strcat(output, myStruct);
            break;
    }
    return output;
}

int main(int argc, const char * argv[]) {
    
    struct dict **myDict;
    myDict = malloc(1000*sizeof(struct dict *));
    int *myDictLen = malloc(sizeof(int));
    *myDictLen = 0;
    struct sDict **statements = malloc(100*sizeof(struct sDict *));
    int *statementsLength = malloc(sizeof(int));
    *statementsLength = 0;
    char *currentVar = malloc(1000*sizeof(char));
    
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
    int lastIDX = 0;
    
    while(token != NULL && strcmp(token, "")) {
        if (DEBUG) {
            printf("TOKEN: %s\n", token);
        }
        char *output = parse(token, pos, outputCFP, outputHFP, myDict, myDictLen, statements, statementsLength, currentVar);
        strcat(output, "};\n");
        fwrite(output, 1, strlen(output), outputCFP);
        token = strtok(NULL, s);
        trim(token);
        free(output);
    }
    
    fclose(inputFP);
    fclose(outputCFP);
    fclose(outputHFP);
    
    return 0;
}
