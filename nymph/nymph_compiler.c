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

#define FUNCCALL  0
#define FUNCDEC  1
#define OBJDEC  2

char *parseLevel2(char *buffer);
char *parseLevel1(char *buffer);
char *parseLevel1Pre(char *buffer);
char *parseLevel2Pre(char *buffer);

char *parseStatement(char *statement);

void compileFile(const char *inputName, const char *outputName);

struct variable {
    char *dataType;
    char *name;
    char *value;
};

struct object {
    char *name;
    struct variable **properties;
    int propertiesLen;
};

struct function {
    char *returnType;
    char *name;
    struct variable **parameters;
    int parametersLen;
};

char **filesCompiled;
int filesCompiledLen;

struct variable **variables;
int variablesLen;

struct object **objects;
int objectsLen;
char *currentObj;

struct function **functions;
int functionsLen;

int status;

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
    
    while(*ptr && (!isdigit(ptr[len-1])))
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

//---------------------------------------------------------------------------------------------

int isDataType(char *str) {
    if (!strcmp(str, "int") || !strcmp(str, "char") || !strcmp(str, "long") || !strcmp(str, "double") || !strcmp(str, "float") || !strcmp(str, "short") || !strcmp(str, "unsigned") || !strcmp(str, "signed") || !strcmp(str, "void")) {
        return 1;
    }
    return 0;
}

int containsDataType(char *str) {
    if (strstr(str, "int") != NULL || strstr(str, "char") != NULL || strstr(str, "long") != NULL || strstr(str, "double") != NULL || strstr(str, "float") != NULL || strstr(str, "short") != NULL || strstr(str, "unsigned") != NULL || strstr(str, "signed") != NULL || strstr(str, "void") != NULL) {
        return 1;
    }
    return 0;
}

void addStatementToObjects(char *statement) {
    for (int i=0; i<objectsLen; i++) {
        if (!strcmp(objects[i]->name, currentObj)) {
            objects[i]->properties[objects[i]->propertiesLen] = malloc(1000*sizeof(char));
            strcpy(objects[i]->properties[objects[i]->propertiesLen], statement);
            printf("SUCCESS %s\n", objects[i]->properties[objects[i]->propertiesLen]);
            objects[i]->propertiesLen++;
            break;
        }
    }
}

char *parseObj(char *buffer) { //struct union enum
    // printf("Parsing Object\n%s\n",buffer);
    if (strstr(buffer, "pub") != NULL) {
        
    } else {
        buffer = str_replace(buffer, "obj", "struct");
    }
    char *name = nPostSubString(buffer," ");
    trim(name);
    currentObj = name;
    objects[objectsLen] = malloc(sizeof(struct object));
    objects[objectsLen]->name = name;
    objects[objectsLen]->properties = malloc(1000*sizeof(char *));
    objects[objectsLen]->propertiesLen = 0;
    objectsLen++;

    return buffer;
}

char *parseSUE(char *buffer) { //struct union enum
    // printf("Parsing SUE\n%s\n",buffer);
    return buffer;
}

char *parseConditionalFunction(char *function) {
    // printf("Parsing Conditional\n%s\n",function);
    char *str = malloc(100000*sizeof(char));
    strcat(str, "if(");
    strcat(str, nPostSubString(function, "("));
    strcat(str, ")");
    return parseStatement(str);
}

char *parseFunction(char *function) {
    // printf("Parsing Function\n%s\n\n", function);
    return function;
}

char *parseLoopFunction(char *function) {
    // printf("Parsing Loop\n%s\n\n", function);
    char *str = malloc(100000*sizeof(char));
    strcat(str, "for(");
    function = nPostSubString(function, "(");
    while(strstr(function, ";") != NULL) { //remember to add def
        char *statement = nSubString(function, ";");
        function += strlen(statement) + 1;
        strcat(statement, ";");
        strcat(str, parseStatement(statement));
    }
    char *statement = nSubString(function, ")");
    strcat(str, parseStatement(statement));
    strcat(str, ")");

    return str;
}

char *parseLeftStatement(char *statement) {
    // printf("Parsing Left\n%s\n", statement);
    if (strstr(statement, "=") != NULL || strstr(statement, "&&") != NULL || strstr(statement, "||") != NULL || strstr(statement, "<") != NULL || strstr(statement, ">") != NULL) { //add other ones
        return parseStatement(statement);
    }
    return statement;
}

char *parseRightStatement(char *statement) {
    // printf("Parsing Right\n%s\n", statement);
    if (strstr(statement, "=") != NULL || strstr(statement, "&&") != NULL || strstr(statement, "||") != NULL || strstr(statement, "<") != NULL || strstr(statement, ">") != NULL) { //add other ones
        return parseStatement(statement);
    }
    return statement;
}

char *parseStatement(char *statement) {
    //printf("STATEMENT: %s\n", statement);
    trim(statement);
    char *str = malloc(100000*sizeof(char));
    char *left = malloc(1000*sizeof(char));
    char *right = malloc(1000*sizeof(char));
    if(strstr(statement, "==") != NULL) { //1. what if these are in strings? 2. Must take comma into account
        left = nSubString(statement, "==");
        left = parseLeftStatement(left);
        right = nPostSubString(statement, "==");
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "==");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "!=") != NULL) {
        left = nSubString(statement, "!=");
        left = parseLeftStatement(left);
        right = nPostSubString(statement, "!=");
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "!=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "<=") != NULL) {
        left = nSubString(statement, "<=");
        left = parseLeftStatement(left);
        right = nPostSubString(statement, "<=");
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "<=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, ">=") != NULL) {
        left = nSubString(statement, ">=");
        left = parseLeftStatement(left);
        right = nPostSubString(statement, ">=");
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, ">=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "&&") != NULL) {
        left = nSubString(statement, "&&");
        right = nPostSubString(statement, "&&");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "&&");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "||") != NULL) {
        left = nSubString(statement, "||");
        right = nPostSubString(statement, "||");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "||");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "+=") != NULL) {
        left = nSubString(statement, "+=");
        right = nPostSubString(statement, "+=");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "+=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "-=") != NULL) {
        left = nSubString(statement, "-=");
        right = nPostSubString(statement, "-=");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "-=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "/=") != NULL) {
        left = nSubString(statement, "/=");
        right = nPostSubString(statement, "/=");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "/=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "*=") != NULL) {
        left = nSubString(statement, "*=");
        right = nPostSubString(statement, "*=");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "*=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "<<=") != NULL) {
        left = nSubString(statement, "<<=");
        right = nPostSubString(statement, "<<=");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "<<=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, ">>=") != NULL) {
        left = nSubString(statement, ">>=");
        right = nPostSubString(statement, ">>=");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, ">>=");
        strcat(str, right);
        return str;
    } else if(strstr(statement, ">>") != NULL) {
        left = nSubString(statement, ">>");
        right = nPostSubString(statement, ">>");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, ">>");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "<<") != NULL) {
        left = nSubString(statement, "<<");
        right = nPostSubString(statement, "<<");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "<<");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "+") != NULL) {
        left = nSubString(statement, "+");
        right = nPostSubString(statement, "+");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "+");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "-") != NULL) {
        left = nSubString(statement, "-");
        right = nPostSubString(statement, "-");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "-");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "/") != NULL) {
        left = nSubString(statement, "/");
        right = nPostSubString(statement, "/");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "/");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "*") != NULL) {
        left = nSubString(statement, "*");
        right = nPostSubString(statement, "*");
        left = parseLeftStatement(left);
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "*");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "<") != NULL) {
        left = nSubString(statement, "<");
        left = parseLeftStatement(left);
        right = nPostSubString(statement, "<");
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "<");
        strcat(str, right);
        return str;
    } else if(strstr(statement, ">") != NULL) {
        left = nSubString(statement, ">");
        left = parseLeftStatement(left);
        right = nPostSubString(statement, ">");
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, ">");
        strcat(str, right);
        return str;
    } else if(strstr(statement, "=") != NULL) {
        left = nSubString(statement, "=");
        left = parseLeftStatement(left);
        right = nPostSubString(statement, "=");
        right = parseRightStatement(right);
        strcat(str, left);
        strcat(str, "=");
        strcat(str, right);
        return str;
    } else {
        if (strstr(statement, ";") != NULL) {
            left = nSubString(statement, ";");
            left = parseLeftStatement(left);
            strcat(str, left);
            strcat(str, ";");
        } else {
            left = parseLeftStatement(statement);
            strcat(str, left);
        }
        return str;
    }
    return NULL;
}

char *parseLevel2Pre(char *buffer) {
    char *str = malloc(100000*sizeof(char));

    while(strstr(buffer, ";") != NULL && strstr(buffer, ";")) {
        char *statement = strchr(buffer, ';');
        buffer = statement + 1;
        statement--;
        for(;statement[0] != '\n' && statement[0] != ';'; statement--) {}
        statement++;
        statement = nSubString(statement, ";");
        if (status == OBJDEC) {
            addStatementToObjects(statement);
        }
        strcat(statement, ";");
        strcat(str, parseStatement(statement));
    }
    status = -1;
    if (strstr(buffer, "}") != NULL) { // what if } is in a string?
        strcat(str, "}");
    }
    if (strstr(buffer, ")") != NULL) {
        char *function = nSubString(buffer, ")");
        strcat(function, ")");
        if (strstr(function, "if") != NULL || strstr(function, "else") != NULL || strstr(function, "while") != NULL) {
            strcat(str, parseConditionalFunction(function));
        } else if(strstr(function, "for") != NULL) {
            strcat(str, parseLoopFunction(function));
        } else {
            strcat(str, parseFunction(function));
        }
    }
    return str;
}

char *parseLevel2Post(char *buffer) {
    return parseLevel2(buffer);
}

char *parseLevel2(char *buffer) {
    char *str = malloc(100000*sizeof(char));
    char *pre;
    char *body;
    char *post;
    if(strstr(buffer, "{") != NULL) {
        pre = nSubString(buffer, "{");
        body = lastBracket(buffer);
        body = nPostSubString(body, "{");
        post = postLastBracket(buffer);
        strcat(str, parseLevel2Pre(pre));
        strcat(str, "{");
        strcat(str, parseLevel2(body));
        strcat(str, parseLevel2Post(post));
    } else {
        strcat(str, parseLevel2Pre(buffer));
    }
    return str;
}

char *parseLevel1Pre(char *buffer) {
    char *str = malloc(100000*sizeof(char));
    while(strstr(buffer, "#include") != NULL) {
        char *inputname = malloc(1000*sizeof(char));
        char *outputname = nSubString(buffer, "\n");
        buffer += strlen(outputname) + 1;
        if(strstr(outputname, "\"") != NULL) {
            outputname = nPostSubString(outputname, "\"");
            outputname = nSubString(outputname, ".");
            strcpy(inputname, outputname);
            strcat(inputname, ".n");
            int compiledFlag = 0;
            for (int i = 0; i < filesCompiledLen; i++) {
                if(!strcmp(filesCompiled[i], inputname)) {
                    compiledFlag = 1;
                    break;
                }
            }
            if(!compiledFlag) {
                printf("Found %s to compile\n", inputname);
                compileFile(inputname, outputname);
            } else {
                printf("File %s already compiling/compiled\n", inputname);
            }
        }
    }

    while(strstr(buffer, ";") != NULL) { //remember to add def
        char *statement = strchr(buffer, ';');
        buffer = statement + 1;
        statement--;
        for(;statement[0] != '\n' && statement[0] != ';'; statement--) {}
        statement++;
        statement = nSubString(statement, ";");
        strcat(statement, ";");
        strcat(str, parseStatement(statement));
    }

    if (strstr(buffer, ")") != NULL) {
        char *function = nSubString(buffer, ")");
        strcat(function, ")");
        strcat(str, parseFunction(function));
    } else {
        if (strstr(buffer, "struct") != NULL || strstr(buffer, "enum") != NULL || strstr(buffer, "union") != NULL) {
            strcat(str, parseSUE(buffer));
        }  else if(strstr(buffer, "obj") != NULL) {
            status = OBJDEC;
            strcat(str, parseObj(buffer));
        }
    }

    return str;
}

char *parseLevel1Post(char *buffer) {
    return parseLevel1(buffer);
}

char *parseLevel1(char *buffer) {
    char *str = malloc(100000*sizeof(char));
    char *pre;
    char *body;
    char *post;
    if(strstr(buffer, "{") != NULL) {
        pre = nSubString(buffer, "{");
        body = lastBracket(buffer);
        body = nPostSubString(body, "{");
        post = postLastBracket(buffer);
        // printf("BUFFER: %s\n", buffer);
        // printf("PRE: %s\n", pre);
        // printf("BODY: %s\n", body);
        // printf("POST: %s\n", post);
        strcat(str, parseLevel1Pre(pre));
        strcat(str, "{");
        strcat(str, parseLevel2(body));
        strcat(str, parseLevel1Post(post));
    } else {
        strcat(str, parseLevel1Pre(buffer));
        //printf("STRING2%s\n", str);
    }

    return str;
}

void compileFile(const char *inputName, const char *outputName) {

    FILE *inputFP;
    FILE *outputCFP;
    FILE *outputHFP;

    inputFP = fopen (inputName, "rb");
    
    char hFile[100];
    char cFile[100];
    memset(hFile, '\0', sizeof(hFile));
    memset(cFile, '\0', sizeof(cFile));
    strcpy(cFile, outputName);
    strcat(cFile, ".c");
    strcpy(hFile, outputName);
    strcat(hFile, ".h");
    outputCFP = fopen(cFile, "wb+");
    outputHFP = fopen(hFile, "wb+");
    
    char str[100];
    memset(str, '\0', sizeof(str));
    strcpy(str, "#include \"");
    strcat(str, hFile);
    strcat(str, "\"\n");
    fwrite(str , 1 , strlen(str) , outputCFP);
    
    char *buffer = loadFile(inputFP);
    
    char *output = parseLevel1(buffer);
    
    printf("Listing all variables.\n");
    for(int i=0; i< variablesLen; i++) {
        printf("%s %s = %s\n", variables[i]->dataType, variables[i]->name, variables[i]->value);
    }

    printf("Listing all objects.\n");
    for(int i=0; i< objectsLen; i++) {
        printf("%s\n", objects[i]->name);
    }

    fwrite(output, 1, strlen(output), outputCFP);

    fclose(inputFP);
    fclose(outputCFP);
    fclose(outputHFP);

}

int main(int argc, const char * argv[]) {

    filesCompiled = malloc(1000*sizeof(char *));
    filesCompiledLen = 0;

    variables = malloc(1000*sizeof(struct variable*));
    objects = malloc(1000*sizeof(struct object*));
    functions = malloc(1000*sizeof(struct function*));

    variablesLen = 0;
    objectsLen = 0;
    functionsLen = 0;

    status = -1;

    if (argc != 3) {
        printf("Input and Output parameter needed.\n");
        return 1;
    }
    
    filesCompiled[filesCompiledLen] = malloc(1000*sizeof(char));
    strcpy(filesCompiled[filesCompiledLen], argv[1]);
    filesCompiledLen++;

    compileFile(argv[1], argv[2]);
    
    return 0;
}