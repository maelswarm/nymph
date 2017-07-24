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

void createFunctionCall(FILE *outputFP, char *token, int lastIDX) {
    char function[1000];
    char **parameters = (char **)malloc(100*sizeof(char *));
    int parametersLength = 0;
    memset(function, '\0', sizeof(function));
    
    char *objectName = subString(token, '.');
    token += strlen(objectName) + 1;
    
    char *functionName = subString(token, '(');
    token += strlen(functionName) + 1;
    
    
    while (strchr(token, ',') != NULL) {
        char *parameterName = subString(token, ',');
        token += strlen(parameterName) + 1;
        parameters[parametersLength] = parameterName;
        parametersLength++;
    }
    
    char *parameterName = subStringLastOccurance(token, ')');
    parameters[parametersLength] = parameterName;
    parametersLength++;
    token += strlen(parameterName) + 1;
    
    for(int i=0; i<parametersLength; i++) {
        //printf("PARAMETER: %s\n",parameters[i]);
    }
    
    trim(objectName);
    
    strcpy(function, "\n");
    strcat(function, functionName);
    strcat(function, "(");
    strcat(function, objectName);
    strcat(function, ",");
    for (int i=0; i<parametersLength; i++) {
        strcat(function, parameters[i]);
        if(i != parametersLength - 1) {
            strcat(function, ",");
        } else {
            strcat(function, ");");
        }
    }
    //printf("FUNCTION: %s", function);
    fwrite(function , 1 , strlen(function) , outputFP);
    free(objectName);
    free(functionName);
}

void createFunction(FILE *outputFP, char *token, int lastIDX) {
    char function[1000];
    char **parameters = (char **)malloc(100*sizeof(char *));
    int parametersLength = 0;
    memset(function, '\0', sizeof(function));
    
    char *returnType = subString(token, ' ');
    token += strlen(returnType) + 1;
    
    char *objectType = subString(token, '.');
    token += strlen(objectType) + 1;
    
    char *functionName = subString(token, '(');
    token += strlen(functionName) + 1;
    
    while (strchr(token, ',') != NULL) {
        char *parameterName = subString(token, ',');
        token += strlen(parameterName) + 1;
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
    strcat(function, ",");
    for (int i=0; i<parametersLength; i++) {
        strcat(function, parameters[i]);
        if(i != parametersLength - 1) {
            strcat(function, ",");
        } else {
            strcat(function, ");");
        }
    }
    
    fwrite(function , 1 , strlen(function) , outputFP);
    free(returnType);
    free(functionName);
    free(objectType);
}

int main(int argc, const char * argv[]) {
    
    char *token;
    const char s[2] = ";";
    FILE *inputFP;
    FILE *outputFP;
    
    if (argc != 2) {
        return 1;
    }

    inputFP = fopen (argv[1], "rb");
    outputFP = fopen("trans.c", "wb+");
    
    char *buffer = loadFile(inputFP);
    token = strtok(buffer, s);
    int lastIDX = 0;
    
    while(token != NULL) {
        
        
        if (strstr(token, ".") != NULL && strstr(token, "(") != NULL && strstr(token, ">") == NULL && strstr(token, "{") < strstr(token, ")")) {
            checkForThis(&token);
            checkForString(&token);
            createFunctionCall(outputFP, token, lastIDX);
        } else if(strstr(token, ".") != NULL && strstr(token, "(") != NULL && strstr(token, ">") == NULL) {
            checkForThis(&token);
            checkForString(&token);
            createFunction(outputFP, token, lastIDX);
        } else {
            
            checkForThis(&token);
            checkForString(&token);
            
            char newToken[10000];
            memset(newToken, '\0', sizeof(newToken));
            strcpy(newToken, token);
            strcat(newToken, ";");
            fwrite(newToken , 1 , strlen(newToken) , outputFP);
        }
        token = strtok(NULL, s);
    }
    
    fclose(inputFP);
    fclose(outputFP);
    
    return 0;
};
