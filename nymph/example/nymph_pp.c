#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

#include "error_checking.h"
#include "helper_func.h"

#define DEBUG 0

#define FUNCCALL 0
#define FUNCDEC 1
#define OBJDEC 2
#define OBJPUBDEC 3

char *parseLevel2(char *buffer, FILE *hFile);
char *parseLevel1(char *buffer, FILE *hFile);
char *parseLevel1Pre(char *buffer, FILE *hFile);
char *parseLevel2Pre(char *buffer, FILE *hFile);

char *parseStatement(char *statement, FILE *hFile);

void compileFile(const char *inputName, const char *outputName);

char *checkFunctionCall(char *statement);

struct variable
{
    char *dataType;
    char *name;
    char *value;
};

struct object
{
    char *name;
    struct variable **properties;
    int propertiesLen;
};

struct function
{
    char *returnDataType;
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
char *parentClass;

char *currentVar;
char *currentFunction;

struct chain
{
    struct chain *next;
    void *thing;
};

struct chain *master_chain;

void *chain_malloc(int size)
{
    struct chain *newChain = malloc(sizeof(struct chain));
    newChain->thing = malloc(size);
    newChain->next = NULL;
    struct chain *tmp = master_chain;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = newChain;

    return newChain->thing;
}

void free_chain()
{
    struct chain *chain = master_chain;
    while (chain != NULL)
    {
        struct chain *next = chain->next;
        if (chain != NULL)
        {
            if (chain->thing != NULL)
            {
                free(chain->thing);
            }
            free(chain);
        }
        chain = next;
    }
}

//Begin Transformation Functions---------------------------------------------------------------------------------------------

/* check the balance of two chars. i.e '(' and ')' */

int balanceOfCharsInString(char *string, char open, char close)
{
    int cnt = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] == open)
        {
            cnt++;
        }
        else if (string[i] == close)
        {
            cnt--;
        }
    }
    if (cnt < 0)
    {
        cnt *= -1;
    }
    return cnt;
}

char *getObjectStatementDataType(char *statement)
{
    char *dataType = chain_malloc(1000 * sizeof(char));
    int flag = 0;
    int i = strlen(statement) - 1;
    for (; i > -1; i--)
    {
        if (statement[i] == '=')
        {
            flag = 1;
        }
        else if (flag == 1)
        {
            if (isalpha(statement[i]))
            {
                flag = 2;
            }
        }
        else if (flag == 2)
        {
            if (isspace(statement[i]))
            {
                break;
            }
        }
    }
    for (int j = 0; j < i; j++)
    {
        dataType[j] = statement[j];
        dataType[j + 1] = '\0';
    }
    trim(dataType);
    return dataType;
}

char *getObjectStatementName(char *statement)
{
    char *name = chain_malloc(1000 * sizeof(char));
    int flag = 0;
    int i = strlen(statement) - 1;
    int j = 0;
    for (; i > -1; i--)
    {
        if (statement[i] == '=')
        {
            flag = 1;
        }
        else if (flag == 1)
        {
            if (isalpha(statement[i]))
            {
                flag = 2;
            }
        }
        else if (flag == 2)
        {
            if (isspace(statement[i]))
            {
                break;
            }
        }
    }
    i++;
    for (; isspace(statement[i]) == 0; i++, j++)
    {
        name[j] = statement[i];
        name[j + 1] = '\0';
    }
    trim(name);
    return name;
}

char *getObjectStatementValue(char *statement)
{
    char *value = chain_malloc(1000 * sizeof(char));
    int i = strlen(statement) - 1;
    int j = 0;
    for (; i > -1; i--)
    {
        if (statement[i] == '=')
        {
            break;
        }
    }
    i++;
    for (; statement[i] != '\0'; i++, j++)
    {
        value[j] = statement[i];
        value[j + 1] = '\0';
    }
    trim(value);
    return value;
}

void addStatementToObjects(char *statement)
{
    //printf("%s\n", statement);
    for (int i = 0; i < objectsLen; i++)
    {
        if (!strcmp(objects[i]->name, currentObj))
        {
            char *name = getObjectStatementName(statement);
            for (int j = 0; j < objects[i]->propertiesLen; j++)
            {
                //printf("%s %s\n", objects[i]->properties[j]->name, name);
                if (!strcmp(objects[i]->properties[j]->name, name))
                {
                    //printf("%s %s\n", objects[i]->properties[j]->name, name);
                    objects[i]->properties[j]->value = getObjectStatementValue(statement);
                    return;
                }
            }
            objects[i]->properties[objects[i]->propertiesLen] = chain_malloc(1000 * sizeof(char));
            objects[i]->properties[objects[i]->propertiesLen]->dataType = getObjectStatementDataType(statement);
            objects[i]->properties[objects[i]->propertiesLen]->name = getObjectStatementName(statement);
            objects[i]->properties[objects[i]->propertiesLen]->value = getObjectStatementValue(statement);
            printf("SUCCESS %s %s %s\n", objects[i]->properties[objects[i]->propertiesLen]->dataType, objects[i]->properties[objects[i]->propertiesLen]->name, objects[i]->properties[objects[i]->propertiesLen]->value);
            objects[i]->propertiesLen++;
            break;
        }
    }
}

char *getFunctionDataType(char *statement)
{
    char *dataType = chain_malloc(1000 * sizeof(char));
    int flag = 0;
    int i = strlen(statement) - 1;
    for (; i > -1; i--)
    {
        if (statement[i] == '(')
        {
            flag = 1;
        }
        else if (flag == 1)
        {
            if (isspace(statement[i]))
            {
                flag = 2;
                break;
            }
        }
    }
    for (int j = 0; j < i; j++)
    {
        dataType[j] = statement[j];
        dataType[j + 1] = '\0';
    }
    trim(dataType);
    return dataType;
}

char *getFunctionName(char *statement)
{
    char *name = chain_malloc(1000 * sizeof(char));
    int flag = 0;
    int i = strlen(statement) - 1;
    for (; i > -1; i--)
    {
        if (statement[i] == '(')
        {
            flag = 1;
        }
        else if (flag == 1)
        {
            if (isspace(statement[i]))
            {
                flag = 2;
                break;
            }
        }
    }
    int k = 0;
    for (int j = i; statement[j] != '('; j++, k++)
    {
        name[k] = statement[j];
        name[k + 1] = '\0';
    }
    trim(name);
    return name;
}

char *getParameterName(char *statement)
{
    char *dataType = chain_malloc(1000 * sizeof(char));
    int flag = 0;
    int i = strlen(statement) - 1;
    for (; i > -1; i--)
    {
        if (statement[i] == '*')
        {
            break;
        }
        else if (isspace(statement[i]))
        {
            break;
        }
    }
    i++;
    int k = 0;
    for (int j = i; j < strlen(statement); j++, k++)
    {
        dataType[k] = statement[j];
        dataType[k + 1] = '\0';
    }
    trim(dataType);
    return dataType;
}

char *getParameterDataType(char *statement)
{
    char *dataType = chain_malloc(1000 * sizeof(char));
    int flag = 0;
    int i = strlen(statement) - 1;
    for (; i > -1; i--)
    {
        if (statement[i] == '*')
        {
            i++;
            break;
        }
        else if (isspace(statement[i]))
        {
            break;
        }
    }
    for (int j = 0; j < i; j++)
    {
        dataType[j] = statement[j];
        dataType[j + 1] = '\0';
    }
    trim(dataType);
    return dataType;
}

void getFunctionParameters(char *function)
{
    struct variable **parameters = chain_malloc(1000 * sizeof(struct variable *));
    functions[functionsLen]->parametersLen = 0;
    char *innerStr = chain_malloc(1000 * sizeof(char));
    innerStr = nPostSubString(function, "(");

    while (strstr(innerStr, ",") != NULL)
    {
        char *str = nSubString(innerStr, ",");
        innerStr += strlen(str) + 1;
        trim(str);
        char *dataType = getParameterDataType(str);
        dataType = str_replace(dataType, "*", "1");
        dataType = str_replace(dataType, " ", "");
        char *name = getParameterName(str);
        struct variable *parameter = chain_malloc(sizeof(struct variable));
        parameter->dataType = dataType;
        parameter->name = name;
        parameters[functions[functionsLen]->parametersLen] = parameter;
        functions[functionsLen]->parametersLen++;
    }
    char *str = nSubString(innerStr, ")");
    innerStr += strlen(str) + 1;
    trim(str);
    char *dataType = getParameterDataType(str);
    dataType = str_replace(dataType, "*", "1");
    dataType = str_replace(dataType, " ", "");
    char *name = getParameterName(str);
    struct variable *parameter = chain_malloc(sizeof(struct variable));
    parameter->dataType = dataType;
    parameter->name = name;
    parameters[functions[functionsLen]->parametersLen] = parameter;
    functions[functionsLen]->parametersLen++;

    functions[functionsLen]->parameters = parameters;
}

void addFunctionToFunctions(char *function)
{
    functions[functionsLen] = chain_malloc(sizeof(struct function));
    functions[functionsLen]->returnDataType = getFunctionDataType(function);
    functions[functionsLen]->name = getFunctionName(function);
    currentFunction = functions[functionsLen]->name;
    getFunctionParameters(function);
    //printf("Function noted: %s %s\n", functions[functionsLen]->returnDataType, functions[functionsLen]->name);
    for (int i = 0; i < functions[functionsLen]->parametersLen; i++)
    {
        //printf("with param: %s\n", functions[functionsLen]->parameters[i]->dataType);
    }
    functionsLen++;
}

char *parseObj(char *buffer, FILE *hFile)
{ //struct union enum
    // printf("Parsing Object\n%s\n",buffer);
    if (strstr(buffer, "pub") != NULL)
    {
        status = OBJPUBDEC;
        char *name = nPostSubString(buffer, " ");
        if (strstr(name, ":") != NULL)
        {
            name = nPostSubString(name, " ");
            parentClass = nPostSubString(name, ":");
            trim(parentClass);
            name = nSubString(name, ":");
        }
        else
        {
            name = nPostSubString(name, " ");
        }
        trim(name);
        currentObj = name;
        objects[objectsLen] = chain_malloc(sizeof(struct object));
        objects[objectsLen]->name = name;
        objects[objectsLen]->properties = chain_malloc(1000 * sizeof(struct variable *));
        objects[objectsLen]->propertiesLen = 0;
        objectsLen++;

        char *str = chain_malloc(1000 * sizeof(char));
        strcat(str, "typedef struct ");
        strcat(str, name);
        strcat(str, " ");
        strcat(str, name);
        strcat(str, ";\n");
        fwrite(str, 1, strlen(str), hFile);

        strcpy(str, "struct ");
        strcat(str, name);
        strcat(str, " {\n");
        fwrite(str, 1, strlen(str), hFile);
    }
    else
    {
        status = OBJDEC;
        char *name;
        if (strstr(name, ":") != NULL)
        {
            parentClass = nSubString(nPostSubString(name, ":"), ";");
            trim(parentClass);
            name = nPostSubString(buffer, ":");
        }
        else
        {
            name = nPostSubString(buffer, " ");
        }
        char *str = chain_malloc(1000 * sizeof(char));
        strcat(str, "typedef struct ");
        strcat(str, name);
        strcat(str, " ");
        strcat(str, name);
        strcat(str, ";\n");
        buffer = str_replace(buffer, "obj", "struct");
        strcat(str, buffer);
        return str;
    }

    return "";
}

char *parseSUE(char *buffer, FILE *hFile)
{ //struct union enum
    // printf("Parsing SUE\n%s\n",buffer);
    return buffer;
}

char *parseIfConditionalFunction(char *function, FILE *hFile)
{
    //printf("Parsing Conditional\n%s\n",function);
    return function;
}

char *parseElseIfConditionalFunction(char *function, FILE *hFile)
{
    //printf("Parsing Conditional\n%s\n",function);
    return function;
}

char *parseElseConditionalFunction(char *function, FILE *hFile)
{
    //printf("Parsing Conditional\n%s\n",function);

    return function;
}

char *parseFunction(char *function, FILE *hFile)
{
    char *str = chain_malloc(1000 * sizeof(char));
    int pubFlag = 0;
    if (strstr(function, "pub") != NULL)
    {
        function = str_replace(function, "pub", "");
        pubFlag = 1;
    }
    addFunctionToFunctions(function);
    if (strstr(function, "main") == NULL)
    { //for function overloading
        char *functionFront = chain_malloc(1000 * sizeof(char));
        char *functionBack = chain_malloc(1000 * sizeof(char));
        functionFront = nSubString(function, "(");
        functionBack = nPostSubString(function, "(");
        for (int i = 0; i < functions[functionsLen - 1]->parametersLen; i++)
        {
            strcat(functionFront, functions[functionsLen - 1]->parameters[i]->dataType);
        }
        strcat(functionFront, "(");
        strcat(functionFront, functionBack);
        function = functionFront;
    }
    if (pubFlag)
    {
        strcpy(str, function);
        strcat(str, ";");
        fwrite(str, 1, strlen(str), hFile);
    }
    //printf("Parsing Function\n%s\n\n", function);
    return function;
}

char *parseWhileLoopFunction(char *function, FILE *hFile)
{
    //printf("Parsing While Loop\n%s\n",function);
    return function;
}

char *parseForLoopFunction(char *function, FILE *hFile)
{
    //printf("Parsing Loop\n%s\n\n", function);
    char *str = chain_malloc(100000 * sizeof(char));
    strcat(str, "for(");
    function = nPostSubString(function, "(");
    while (strstr(function, ";") != NULL)
    { //remember to add def
        char *statement = nSubString(function, ";");
        function += strlen(statement) + 1;
        strcat(statement, ";");
        //strcat(str, parseStatement(statement, hFile));
        strcat(str, statement);
    }
    char *statement = nSubString(function, ")");
    //strcat(str, parseStatement(statement, hFile));
    strcat(str, statement);
    strcat(str, ")");

    return str;
}

char *getVariableName(char *statement)
{
    char *name = chain_malloc(1000 * sizeof(char));
    int i = strlen(statement) - 1;
    for (; i > -1; i--)
    {
        if (isspace(statement[i]) || statement[i] == '*')
        {
            break;
        }
    }
    i++;
    int k = 0;
    for (int j = i; j < strlen(statement); j++, k++)
    {
        name[k] = statement[j];
        name[k + 1] = '\0';
    }
    trim(name);
    return name;
}

char *getVariableDataType(char *statement)
{
    char *dataType = chain_malloc(1000 * sizeof(char));
    int i = strlen(statement) - 1;
    for (; i > -1; i--)
    {
        if (isspace(statement[i]) || statement[i] == '*')
        {
            i++;
            break;
        }
    }
    for (int j = 0; j < i; j++)
    {
        dataType[j] = statement[j];
        dataType[j + 1] = '\0';
    }
    trim(dataType);
    return dataType;
}

void addVariable(char *statement)
{
    trim(statement);
    struct variable *newVar = chain_malloc(sizeof(struct variable));
    newVar->name = getVariableName(statement);
    newVar->dataType = getVariableDataType(statement);
    newVar->dataType = str_replace(newVar->dataType, " ", "");
    newVar->dataType = str_replace(newVar->dataType, "*", "1");
    if (strcmp(newVar->dataType, "") && strcmp(newVar->name, ""))
    {
        variables[variablesLen] = newVar;
        variablesLen++;
    }
}

char *getCurrentVar(char *statement)
{
    char *curVar = chain_malloc(1000 * sizeof(char));
    int i = strlen(statement) - 1;
    int k = 0;
    trim(statement);
    for (; i > -1; i--)
    {
        if (isspace(statement[i]) || statement[i] == '*')
        {
            break;
        }
    }
    i++;
    int end = strlen(statement);
    for (int j = i; j < end; j++, k++)
    {
        curVar[k] = statement[j];
        curVar[k + 1] = '\0';
    }
    trim(curVar);
    return curVar;
}

char *addDefaultObjectValues(char *statement)
{
    currentVar = getCurrentVar(nSubString(statement, "="));
    trim(currentVar);
    char *dataType = nPostSubString(statement, "sizeof(");
    dataType = nSubString(dataType, ")");
    char *newStatement = chain_malloc(1000 * sizeof(char));
    strcat(newStatement, statement);
    strcat(newStatement, ";");

    for (int i = 0; i < objectsLen; i++)
    {
        if (!strcmp(objects[i]->name, dataType))
        {
            for (int j = 0; j < objects[i]->propertiesLen; j++)
            {
                strcat(newStatement, currentVar);
                strcat(newStatement, "->");
                strcat(newStatement, objects[i]->properties[j]->name);
                strcat(newStatement, "=");
                strcat(newStatement, objects[i]->properties[j]->value);
                strcat(newStatement, ";");
            }
            break;
        }
    }
    return newStatement;
}

char *postPrepareFunction(char *statement)
{
    char *tmp = strstr(statement, "(") - 1;
    for (; isalnum(tmp[0]) != 0; tmp--)
    {
    }
    tmp++;
    char *function = chain_malloc(1000 * sizeof(char));
    for (int i = 0; i < strlen(tmp); i++)
    {
        function[i] = tmp[i];
        function[i + 1] = '\0';
    }
    //trim(function);
    //printf("POST: %s\n", function);
    return function;
}

char *prePrepareFunction(char *statement)
{
    char *function = chain_malloc(1000 * sizeof(char));
    strcpy(function, statement);
    int i = strlen(function) - 1;
    for (; isalnum(function[i]); i--)
    {
    }
    strcpy(function, statement);
    function[i + 1] = '\0';
    trim(function);
    return function;
}

char *parseFunctionCall(char *statement)
{
    char *returnStr = chain_malloc(1000 * sizeof(char));
    char *statementCopy = chain_malloc(1000 * sizeof(char));
    strcpy(statementCopy, statement);

    int numOfParam = 0;

    for (int i = 0; i < functionsLen; i++)
    {
        if (!strcmp(functions[i]->name, nSubString(postPrepareFunction(statementCopy), "(")))
        {
            numOfParam = functions[i]->parametersLen;
        }
    }

    while (strstr(statementCopy, "(") != NULL)
    {

        char **parameters = chain_malloc(100 * sizeof(char *));
        int parametersLen = 0;

        strcat(returnStr, prePrepareFunction(nSubString(statementCopy, "(")));
        char *str = postPrepareFunction(statementCopy);
        char *postName = chain_malloc(1000 * sizeof(char));
        strcat(postName, lastPtheses(statementCopy));
        postName = nPostSubString(postName, "(");
        //printf("Statement to parse: %s\n", str);

        char *functionName = nSubString(str, "(");
        char *funcName = nSubString(str, "(");
        if (!strcmp(functionName, "for") || !strcmp(functionName, "while"))
        { //prevent for statements!!!
            strcpy(returnStr, statementCopy);
            return returnStr;
        }

        char *functionParams = lastPtheses(str);
        str += strlen(functionParams) + 1;
        functionParams = nPostSubString(functionParams, "(");
        //printf("Function Params: %s\n", functionParams);

        char *param;
        int pFlag = 0;
        int ppFlag = 0;
        while (strstr(functionParams, ",") != NULL)
        {

            char *preParam = chain_malloc(1000 * sizeof(char));

            if (strstr(functionParams, "(") != NULL && strstr(functionParams, ",") > strstr(functionParams, "(") && functionParams[0] != '(')
            {
                if (!isalnum(*(strstr(functionParams, "(") - 1)))
                {
                    param = lastPtheses(functionParams);
                    strcat(preParam, param);
                    functionParams += strlen(param);
                }
                param = lastPtheses(functionParams);
                functionParams += strlen(param);
                pFlag = 0;
            }
            else
            {
                if (functionParams[0] == '(')
                {
                    param = lastPtheses(functionParams);
                    strcat(preParam, param);
                    functionParams += strlen(param);
                }
                param = nSubString(functionParams, ",");
                functionParams += strlen(param) + 1;
                pFlag = 1;
            }
            //printf("functionParams: %s\n", functionParams);
            //trim(param);
            //printf("Param: %s\n", param);
            //printf("functionParams: %s\n", functionParams);

            if (strstr(param, "+") != NULL || strstr(param, "-") != NULL || strstr(param, "-") != NULL || strstr(param, "*") != NULL || strstr(param, "/") != NULL || strstr(param, "%") != NULL)
            {
                if (strstr(param, "+") < strstr(param, "("))
                {
                    preParam = nSubString(param, "+");
                    strcat(preParam, "+");
                    param = nPostSubString(param, "+");
                    trim(param);
                }
                else if (strstr(param, "-") < strstr(param, "(") && *(strstr(param, "-") + 1) != '>')
                {
                    preParam = nSubString(param, "-");
                    strcat(preParam, "-");
                    param = nPostSubString(param, "-");
                    trim(param);
                }
                else if (strstr(param, "*") < strstr(param, "("))
                {
                    preParam = nSubString(param, "*");
                    strcat(preParam, "*");
                    param = nPostSubString(param, "*");
                    trim(param);
                }
                else if (strstr(param, "/") < strstr(param, "("))
                {
                    preParam = nSubString(param, "/");
                    strcat(preParam, "/");
                    param = nPostSubString(param, "/");
                    trim(param);
                }
                else if (strstr(param, "%") < strstr(param, "("))
                {
                    preParam = nSubString(param, "%");
                    strcat(preParam, "%");
                    param = nPostSubString(param, "%");
                    trim(param);
                }
            }
            //printf("PPPARAM: %s, %i\n", param, ppFlag);
            if (isdigit(param[0]))
            {
                if (numOfParam > ppFlag)
                {
                    strcat(functionName, "int");
                }
            }
            else if (param[0] == '\"')
            {
                if (numOfParam > ppFlag)
                {
                    strcat(functionName, "char1");
                }
            }
            else if (param[0] == '\'')
            {
                if (numOfParam > ppFlag)
                {
                    strcat(functionName, "char");
                }
            }
            else if (strstr(param, "(") != NULL)
            {
                char *paramFunctionName = nSubString(param, "(");
                for (int i = 0; i < functionsLen; i++)
                {
                    if (!strcmp(functions[i]->name, paramFunctionName))
                    {
                        //printf("Function Match!\n");
                        if (numOfParam > ppFlag)
                        {
                            strcat(functionName, functions[i]->returnDataType);
                        }
                        param = checkFunctionCall(param);
                        break;
                    }
                }
            }
            else if (strstr(param, "->") != NULL)
            {
                char *objectValue = nPostSubString(param, ">");
                trimAllButAlpha(objectValue);
                char *var = nSubString(param, "-");
                trimAllButAlpha(var);

                int foundFlag = 0;

                for (int i = 0; i < functionsLen; i++)
                {
                    if (!strcmp(functions[i]->name, currentFunction))
                    {
                        for (int j = 0; j < functions[i]->parametersLen; j++)
                        {
                            if (!strcmp(functions[i]->parameters[j]->name, var))
                            {
                                for (int k = 0; k < objectsLen; k++)
                                {
                                    char *dt = chain_malloc(1000 * sizeof(char));
                                    strcpy(dt, functions[i]->parameters[j]->dataType);
                                    dt = str_replace(dt, "1", "");
                                    if (!strcmp(objects[k]->name, dt))
                                    {
                                        for (int m = 0; m < objects[k]->propertiesLen; m++)
                                        {
                                            if (!strcmp(objects[k]->properties[m]->name, objectValue))
                                            {
                                                if (numOfParam > ppFlag)
                                                {
                                                    strcat(functionName, objects[k]->properties[m]->dataType);
                                                }
                                                foundFlag = 1;
                                                break;
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                if (!foundFlag)
                {
                    for (int i = 0; i < variablesLen; i++)
                    {
                        if (!strcmp(variables[i]->name, var))
                        {
                            char *dt = chain_malloc(1000 * sizeof(char));
                            strcpy(dt, variables[i]->dataType);
                            dt = str_replace(dt, "1", "");
                            for (int j = 0; j < objectsLen; j++)
                            {
                                if (!strcmp(objects[j]->name, dt))
                                {
                                    for (int k = 0; k < objects[j]->propertiesLen; k++)
                                    {
                                        if (!strcmp(objects[j]->properties[k]->name, objectValue))
                                        {
                                            if (numOfParam > ppFlag)
                                            {
                                                strcat(functionName, objects[j]->properties[k]->dataType);
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }
            else if (isalpha(param[0]))
            {
                char *paramFunctionName = nSubString(param, "(");
                int derefCnt = 0;
                if (strstr(paramFunctionName, "[") != NULL)
                {
                    paramFunctionName = nSubString(paramFunctionName, "[");
                    derefCnt++;
                }
                if (strstr(paramFunctionName, "*") != NULL)
                {
                    paramFunctionName = str_replace(paramFunctionName, "*", "");
                    derefCnt++;
                }
                for (int i = 0; i < variablesLen; i++)
                {
                    if (!strcmp(variables[i]->name, paramFunctionName))
                    {
                        char *dType = chain_malloc(1000 * sizeof(char));
                        strcpy(dType, variables[i]->dataType);
                        for (int i = 0; i < derefCnt; i++)
                        {
                            dType[strlen(dType) - 1] = '\0';
                        }
                        if (numOfParam > ppFlag)
                        {
                            strcat(functionName, dType);
                        }
                        break;
                    }
                }
            }
            if (strcmp(preParam, ""))
            {
                char *tmp = chain_malloc(1000 * sizeof(char));
                strcat(tmp, preParam);
                strcat(tmp, param);
                strcpy(param, tmp);
            }
            if (pFlag)
            {
                strcat(param, ",");
            }
            if (strcmp(param, ""))
            {
                parameters[parametersLen] = param;
                parametersLen++;
            }
            ppFlag++;
        }

        param = lastPtheses(functionParams);
        functionParams += strlen(param) + 1;
        trim(param);
        //printf("Param2: %s\n", param);

        if (isdigit(param[0]))
        {
            strcat(functionName, "int");
        }
        else if (param[0] == '\"')
        {
            strcat(functionName, "char1");
        }
        else if (param[0] == '\'')
        {
            strcat(functionName, "char");
        }
        else if (strstr(param, "(") != NULL)
        {
            char *paramFunctionName = nSubString(param, "(");
            for (int i = 0; i < functionsLen; i++)
            {
                if (!strcmp(functions[i]->name, paramFunctionName))
                {
                    //printf("Function Match1!\n");
                    strcat(functionName, functions[i]->returnDataType);
                    param = checkFunctionCall(param);
                    break;
                }
            }
        }
        else if (strstr(param, "->") != NULL)
        {
            char *objectValue = nPostSubString(param, ">");
            trimAllButAlpha(objectValue);
            char *var = nSubString(param, "-");
            trimAllButAlpha(var);

            int foundFlag = 0;

            for (int i = 0; i < functionsLen; i++)
            {
                if (!strcmp(functions[i]->name, currentFunction))
                {
                    for (int j = 0; j < functions[i]->parametersLen; j++)
                    {
                        if (!strcmp(functions[i]->parameters[j]->name, var))
                        {
                            for (int k = 0; k < objectsLen; k++)
                            {
                                char *dt = chain_malloc(1000 * sizeof(char));
                                strcpy(dt, functions[i]->parameters[j]->dataType);
                                dt = str_replace(dt, "1", "");
                                if (!strcmp(objects[k]->name, dt))
                                {
                                    for (int m = 0; m < objects[k]->propertiesLen; m++)
                                    {
                                        if (!strcmp(objects[k]->properties[m]->name, objectValue))
                                        {
                                            strcat(functionName, objects[k]->properties[m]->dataType);
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }

            if (!foundFlag)
            {
                for (int i = 0; i < variablesLen; i++)
                {
                    if (!strcmp(variables[i]->name, var))
                    {
                        char *dt = chain_malloc(1000 * sizeof(char));
                        strcpy(dt, variables[i]->dataType);
                        dt = str_replace(dt, "1", "");
                        for (int j = 0; j < objectsLen; j++)
                        {
                            if (!strcmp(objects[j]->name, dt))
                            {
                                for (int k = 0; k < objects[j]->propertiesLen; k++)
                                {
                                    if (!strcmp(objects[j]->properties[k]->name, objectValue))
                                    {
                                        if (numOfParam > ppFlag)
                                        {
                                            strcat(functionName, objects[j]->properties[k]->dataType);
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }
        else if (isalpha(param[0]))
        {
            char *paramFunctionName = nSubString(param, "(");
            int derefCnt = 0;
            if (strstr(paramFunctionName, "[") != NULL)
            {
                paramFunctionName = nSubString(paramFunctionName, "[");
                derefCnt++;
            }
            if (strstr(paramFunctionName, "*") != NULL)
            {
                paramFunctionName = str_replace(paramFunctionName, "*", "");
                derefCnt++;
            }
            for (int i = 0; i < variablesLen; i++)
            {
                if (!strcmp(variables[i]->name, paramFunctionName))
                {
                    char *dType = chain_malloc(1000 * sizeof(char));
                    strcpy(dType, variables[i]->dataType);
                    for (int i = 0; i < derefCnt; i++)
                    {
                        dType[strlen(dType) - 1] = '\0';
                    }
                    strcat(functionName, dType);
                    break;
                }
            }
        }
        if (strcmp(param, ""))
        {
            parameters[parametersLen] = param;
            parametersLen++;
        }

        strcat(functionName, "(");
        for (int i = 0; i < parametersLen; i++)
        {
            strcat(functionName, parameters[i]);
            //            if (parametersLen - 1 != i) {
            //                strcat(functionName, ",");
            //            }
        }

        //strcat(functionName, postName);
        strcat(returnStr, functionName);
        statementCopy += strlen(lastPtheses(statementCopy)) + 1;
        //printf("New Function: %s\n", returnStr);
        //printf("Function to parse: %s\n", returnStr);
    }
    return returnStr;
}

/* check statement for functions to transform */

char *checkFunctionCall(char *statement)
{

    char *returnStr = chain_malloc(1000 * sizeof(char));
    char *statementCopy = chain_malloc(1000 * sizeof(char));
    strcpy(statementCopy, statement);
    int cnt = 0;
    while (strstr(statementCopy, "(") != NULL)
    {
        if (!isalpha(*(strstr(statementCopy, "(") - 1)))
        { //not a function, move on
            strcat(returnStr, nSubString(statementCopy, "("));
            strcat(returnStr, "(");
            statementCopy = nPostSubString(statementCopy, "(");
        }
        else
        {
            //printf("This statement has function calls. %s\n", statementCopy); //function calls detected
            char *functionName = nSubString(postPrepareFunction(statementCopy), "("); // get function name
            trim(functionName);
            int flag = 0;
            int flag2 = 1;
            for (int i = 0; i < functionsLen; i++)
            {
                if (!strcmp(functions[i]->name, functionName))
                {                                                           //see if function needs to be transformed
                    cnt += balanceOfCharsInString(statementCopy, '(', ')'); //take note of any trailing ')'
                    char *function = parseFunctionCall(statementCopy);      //transform function
                    for (int i = 0; i < cnt; i++)
                    {
                        strcat(function, ")"); //add back trailing ')'
                    }
                    strcat(returnStr, function);
                    statementCopy += strlen(function);
                    flag = 1;
                    break;
                }
            }
            if (!flag)
            { //first function detected didn't need to be transformed, let's move on to the other functions
                char *param;
                char **parameters = chain_malloc(100 * sizeof(char *));
                int parametersLen = 0;
                trim(statementCopy);
                strcat(returnStr, nSubString(statementCopy, "("));
                strcat(returnStr, "(");
                statementCopy = nPostSubString(statementCopy, "(");
                while (strstr(statementCopy, ",") != NULL)
                { //advance past ','
                    if (strstr(statementCopy, "(") != NULL && strstr(statementCopy, ",") > strstr(statementCopy, "("))
                    {
                        param = lastPtheses(statementCopy);
                    }
                    else
                    {
                        param = nSubString(statementCopy, ",");
                    }

                    statementCopy += strlen(param) + 1;
                    trim(param);
                    //printf("Param: %s\n", param);

                    if (strstr(param, "=") != NULL)
                    {
                        strcat(returnStr, nSubString(param, "="));
                        strcat(returnStr, "=");
                        param = nPostSubString(param, "=");
                        flag2 = 0;
                    }

                    if (strstr(param, "(") != NULL)
                    {
                        char *paramFunctionName = nSubString(param, "(");
                        for (int i = 0; i < functionsLen; i++)
                        {
                            if (!strcmp(functions[i]->name, paramFunctionName))
                            { //see if function needs to be transformed
                                //printf("Function Match!\n");
                                strcat(functionName, functions[i]->returnDataType);
                                param = checkFunctionCall(param);
                                flag = 1;
                                break;
                            }
                        }
                    }
                    if (strcmp(param, ""))
                    {
                        parameters[parametersLen] = param;
                        parametersLen++;
                    }
                }
                // do the last parameter
                param = lastPtheses(statementCopy);
                statementCopy += strlen(param) + 1;
                trim(param);
                //printf("Param: %s\n", param);

                if (strstr(param, "=") != NULL)
                {
                    strcat(returnStr, nSubString(param, "="));
                    strcat(returnStr, "=");
                    param = nPostSubString(param, "=");
                    flag2 = 0;
                }

                if (strstr(param, "(") != NULL)
                {
                    char *paramFunctionName = nSubString(param, "(");
                    for (int i = 0; i < functionsLen; i++)
                    {
                        if (!strcmp(functions[i]->name, paramFunctionName))
                        { //see if function needs to be transformed
                            //printf("Function Match!\n");
                            strcat(functionName, functions[i]->returnDataType);
                            param = checkFunctionCall(param);
                            flag = 1;
                            break;
                        }
                    }
                }

                if (strcmp(param, ""))
                {
                    parameters[parametersLen] = param;
                    parametersLen++;
                }

                for (int i = 0; i < parametersLen; i++)
                {
                    strcat(returnStr, parameters[i]);
                    if (parametersLen - 1 != i)
                    {
                        strcat(returnStr, ",");
                    }
                }
                if (flag && flag2)
                { //consider removal
                    strcat(returnStr, ")");
                }
            }
            break;
        }
    }
    if (!strcmp(returnStr, ""))
    {
        strcat(returnStr, statementCopy);
    }
    return returnStr;
}

/* parse level 2 Pre. */
/*
 
 int add() {
 **************************
 **************************
 **************************
 **************************
 **************************
 }
 */

char *parseLevel2Pre(char *buffer, FILE *hFile)
{
    char *str = chain_malloc(100000 * sizeof(char));
    //printf("Buffer: %s\n", buffer);
    while (strstr(buffer, ";") != NULL)
    {
        char *statement = strchr(buffer, ';');
        buffer = statement + 1;
        statement--;
        for (; statement[0] != '\n' && statement[0] != ';'; statement--)
        {
        }
        statement++;
        statement = nSubString(statement, ";");

        int check = checkStatement(statement);
        if (check)
        {
            //printf("\"%s\" is a valid statment.\n", statement);
        }
        else
        {
            printf("\x1B[31m ERROR:\x1B[37m\n \"%s\" is not a valid statment.\n\n", statement);
        }

        if (status == OBJPUBDEC)
        {
            //printf("%s\n", statement);
            addStatementToObjects(statement);
            if (strstr(statement, "=") != NULL)
            {
                statement = nSubString(statement, "=");
            }
            trim(statement);
            strcat(statement, ";\n");
            fwrite(statement, 1, strlen(statement), hFile);
        }
        else if (status == OBJDEC)
        {
            //printf("%s\n", statement);
            addStatementToObjects(statement);
            if (strstr(statement, "=") != NULL)
            {
                statement = nSubString(statement, "=");
            }
            trim(statement);
            strcat(statement, ";");
            //strcat(str, parseStatement(statement, hFile));
            strcat(str, statement);
        }
        else
        {
            if (strstr(statement, "&") == NULL && strstr(statement, "/") == NULL && strstr(statement, "+") == NULL && strstr(statement, "-") == NULL && strstr(statement, "<") == NULL && strstr(statement, ">") == NULL && strstr(statement, "return") == NULL)
            {
                if (strstr(statement, "=") != NULL)
                { //filter out junk
                    addVariable(nSubString(statement, "="));
                }
                else
                {
                    addVariable(statement);
                }
            }
            if (strstr(statement, "alloc"))
            {
                statement = addDefaultObjectValues(statement);
            }
            else
            {
                if (strstr(statement, "(") != NULL)
                {
                    statement = checkFunctionCall(statement);
                }
            }
            strcat(statement, ";");
            //strcat(str, parseStatement(statement, hFile));
            strcat(str, statement);
        }
    }
    if (strstr(buffer, "(") != NULL && strstr(buffer, ";") == NULL)
    {
        trim(buffer);
        strcat(str, checkFunctionCall(buffer));
    }
    if (strstr(buffer, ")") != NULL && strstr(buffer, "(") == NULL)
    {
        strcat(str, checkFunctionCall(buffer));
    }
    if (status == OBJPUBDEC)
    {
        fwrite("};\n", 1, 3, hFile);
    }
    if (strstr(buffer, "}") != NULL)
    { // what if } is in a string?
        if (status == OBJDEC)
        {
            strcat(str, "};");
        }
        else
        {
            strcat(str, "}");
        }
    }
    status = -1;

    parentClass = NULL;

    return str;
}

/* parse level 2 Post. */
/*
 int add() {
 ...
 }
 **************************
 **************************
 **************************
 **************************
 */

char *parseLevel2Post(char *buffer, FILE *hFile)
{
    return parseLevel2(buffer, hFile);
}

/* parse level 2. */
/*
 
 #include <stdlib.c>
 
 
 int main() {
 **************************
 }
 
 int subtract() {
 ...
 }
 
 int add() {
 ...
 }
 */

char *parseLevel2(char *buffer, FILE *hFile)
{
    char *str = chain_malloc(100000 * sizeof(char));
    char *pre;
    char *body;
    char *post;
    if (strstr(buffer, "{") != NULL)
    {
        pre = nSubString(buffer, "{");
        body = lastBracket(buffer);
        body = nPostSubString(body, "{");
        post = postLastBracket(buffer);
        strcat(str, parseLevel2Pre(pre, hFile));
        strcat(str, "{");
        strcat(str, parseLevel2(body, hFile));
        strcat(str, parseLevel2Post(post, hFile));
    }
    else
    {
        strcat(str, parseLevel2Pre(buffer, hFile));
    }
    return str;
}

/* parse level 1 Pre. */
/*
 **************************
 #include <stdlib.c>
 **************************
 **************************
 int main() {
 
 }
 
 int subtract() {
 ...
 }
 
 int add() {
 ...
 }
 */

char *parseLevel1Pre(char *buffer, FILE *hFile)
{
    char *str = chain_malloc(100000 * sizeof(char));
    while (strstr(buffer, "#include") != NULL)
    {
        char *inputname = chain_malloc(1000 * sizeof(char));
        char *outputname = nSubString(buffer, "\n");
        buffer += strlen(outputname) + 1;
        if (strstr(outputname, "\"") != NULL)
        {
            outputname = nPostSubString(outputname, "\"");
            outputname = nSubString(outputname, ".");
            strcpy(inputname, outputname);
            strcat(inputname, ".n");
            int compiledFlag = 0;
            for (int i = 0; i < filesCompiledLen; i++)
            {
                if (!strcmp(filesCompiled[i], inputname))
                {
                    compiledFlag = 1;
                    break;
                }
            }
            if (!compiledFlag)
            {
                printf("Found %s to compile\n", inputname);
                compileFile(inputname, outputname);
            }
            else
            {
                printf("File %s already compiling/compiled\n", inputname);
            }
            strcat(outputname, ".h");
            strcat(str, "#include \"");
            strcat(str, outputname);
            strcat(str, "\"\n");
        }
        else
        {
            strcat(outputname, "\n");
            strcat(str, outputname);
        }
    }
    while (strstr(buffer, ";") != NULL)
    { //remember to add def
        char *statement = strchr(buffer, ';');
        buffer = statement + 1;
        statement--;
        for (; statement[0] != '\n' && statement[0] != ';'; statement--)
        {
        }
        statement++;
        if (strstr(statement, ")") != NULL)
        {
            char *function = nSubString(statement, ")");
            strcat(function, ")");
            strcat(str, parseFunction(function, hFile));
            strcat(str, ";");
        }
        else
        {
            statement = nSubString(statement, ";");
            strcat(statement, ";");
            strcat(str, statement);
        }
    }

    if (strstr(buffer, ")") != NULL)
    {
        char *function = nSubString(buffer, ")");
        strcat(function, ")");
        strcat(str, parseFunction(function, hFile));
    }
    else
    {
        if (strstr(buffer, "struct") != NULL || strstr(buffer, "enum") != NULL || strstr(buffer, "union") != NULL)
        {
            strcat(str, parseSUE(buffer, hFile));
        }
        else if (strstr(buffer, "obj") != NULL)
        {
            strcat(str, parseObj(buffer, hFile));
        }
    }

    return str;
}

char *parseLevel1Post(char *buffer, FILE *hFile)
{
    return parseLevel1(buffer, hFile);
}

/* parse level 1. */
/*
 **************************
 #include <stdlib.c>
 **************************
 **************************
 int main() {
 
 }
 **************************
 int subtract() {
 ...
 }
 **************************
 int add() {
 ...
 }
 */

char *parseLevel1(char *buffer, FILE *hFile)
{
    char *str = chain_malloc(100000 * sizeof(char));
    char *pre;
    char *body;
    char *post;
    if (strstr(buffer, "{") != NULL)
    {
        pre = nSubString(buffer, "{");
        body = lastBracket(buffer);
        body = nPostSubString(body, "{");
        post = postLastBracket(buffer);
        // printf("BUFFER: %s\n", buffer);
        // printf("PRE: %s\n", pre);
        // printf("BODY: %s\n", body);
        // printf("POST: %s\n", post);
        strcat(str, parseLevel1Pre(pre, hFile));
        char *tmp = parseLevel2(body, hFile);
        //printf("TMP: %s\n", tmp);
        if (strcmp(tmp, "}"))
        {
            strcat(str, "{");
            strcat(str, tmp);
        }
        strcat(str, parseLevel1Post(post, hFile));
    }
    else
    {
        strcat(str, parseLevel1Pre(buffer, hFile));
        //printf("STRING2%s\n", str);
    }

    return str;
}

/* compile a file when found. */

void compileFile(const char *inputName, const char *outputName)
{

    variables = chain_malloc(1000 * sizeof(struct variable *));

    variablesLen = 0;

    FILE *inputFP;
    FILE *outputCFP;
    FILE *outputHFP;

    inputFP = fopen(inputName, "rb");

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
    fwrite(str, 1, strlen(str), outputCFP);

    char *buffer = loadFile(inputFP);

    char *output = parseLevel1(buffer, outputHFP);

    //printf("Listing all variables.\n");
    for (int i = 0; i < variablesLen; i++)
    {
        //printf("%s %s = %s\n", variables[i]->dataType, variables[i]->name, variables[i]->value);
    }

    //printf("Listing all objects.\n");
    for (int i = 0; i < objectsLen; i++)
    {
        //printf("%s\n", objects[i]->name);
    }

    fwrite(output, 1, strlen(output), outputCFP);

    fclose(inputFP);
    fclose(outputCFP);
    fclose(outputHFP);
}

/* main */

int main(int argc, const char *argv[])
{

    master_chain = malloc(sizeof(struct chain));
    master_chain->thing = NULL;
    master_chain->next = NULL;

    currentVar = chain_malloc(1000 * sizeof(char));

    currentFunction = chain_malloc(1000 * sizeof(char));

    filesCompiled = chain_malloc(1000 * sizeof(char *));
    filesCompiledLen = 0;

    objects = chain_malloc(1000 * sizeof(struct object *));
    functions = chain_malloc(1000 * sizeof(struct function *));

    objectsLen = 0;
    functionsLen = 0;

    status = -1;

    if (argc != 3)
    {
        printf("Input and Output parameter needed.\n");
        return 1;
    }

    filesCompiled[filesCompiledLen] = chain_malloc(1000 * sizeof(char));
    strcpy(filesCompiled[filesCompiledLen], argv[1]);
    filesCompiledLen++;

    compileFile(argv[1], argv[2]);

    free_chain();
    return 0;
}
