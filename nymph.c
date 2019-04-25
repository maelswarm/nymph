#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "nymph.h"
#include "hmap.h"
/* HELPER FUNCTIONS BEGIN */

char *trim(char *str)
{
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}

/* HELPER FUNCTIONS END */

#define PUBLIC_CLASS_FUNC 0
#define PUBLIC_OBJECT_FUNC 1
#define PRIVATE_OBJECT_FUNC 2
#define PUBLIC_FUNC 3
#define PRIVATE_FUNC 4

#define PUBLIC_CLASS_VAR 0
#define PUBLIC_OBJECT_VAR 1
#define PRIVATE_OBJECT_VAR 2
#define PUBLIC_VAR 3
#define PRIVATE_VAR 4

void tokenize(char *, NFile *);
void addToken(NFile *, Token *, char);
void appendToken(NFile *, char *, char);

int isClassInstance(NFile *, char *);

char rootfilename[100];

struct NFile
{
    char *filename;
    FILE *fileN;
    FILE *fileC;
    FILE *fileH;

    Token *token;
    Token *endToken;
    Token *tokenC;
    Token *tokenH;

    Class **classes;
    int classesCnt;
    HMap **classes_hmap;
    NFile **inheritance;
    int inheritanceCnt;
};

struct Token
{
    Token *next;
    Token *prev;
    char *val;
};

struct Class
{
    char *name;
    Class *parent;
    Class *next;
    Property **properties;
    int propertiesCnt;
    Function **functions;
    int functionsCnt;
    HMap **object_hmap;
    HMap **class_hmap;
};

struct Property
{
    char *datatype;
    char *name;
    char *value;
    int encapsulation;
};

struct Function
{
    Token *bodyStart;
    Token *bodyEnd;
    char *datatype;
    char *name;
    char *overloadedName;
    Property **params;
    int paramsCnt;
    Property **properties;
    int propertiesCnt;
    int encapsulation;
    HMap **hmap;
    HMap **return_hmap;
};

NFile **fileList;
int fileListCnt;

HMap **classes_hmap;

Property *createProperty()
{
    Property *newProperty = (Property *)malloc(sizeof(Property));
    newProperty->datatype = (char *)malloc(10000 * sizeof(char));
    newProperty->name = (char *)malloc(10000 * sizeof(char));
    newProperty->value = (char *)malloc(10000 * sizeof(char));
    memset(newProperty->datatype, 0, sizeof(newProperty->datatype));
    memset(newProperty->name, 0, sizeof(newProperty->name));
    memset(newProperty->value, 0, sizeof(newProperty->value));
    newProperty->encapsulation = -1;
    return newProperty;
}

Function *createFunction()
{
    Function *newFunction = (Function *)malloc(sizeof(Function));
    newFunction->properties = (Property **)malloc(1000 * sizeof(Property));
    newFunction->propertiesCnt = 0;
    newFunction->params = (Property **)malloc(1000 * sizeof(Property));
    newFunction->paramsCnt = 0;
    newFunction->datatype = (char *)malloc(10000 * sizeof(char));
    newFunction->name = (char *)malloc(10000 * sizeof(char));
    newFunction->overloadedName = (char *)malloc(10000 * sizeof(char));
    newFunction->encapsulation = -1;
    newFunction->bodyStart = NULL;
    newFunction->bodyEnd = NULL;
    newFunction->hmap = create_hm();
    return newFunction;
}

Class *createClass()
{
    Class *newClass = (Class *)malloc(sizeof(Class));
    newClass->properties = (Property **)malloc(1000 * sizeof(Property));
    newClass->functions = (Function **)malloc(1000 * sizeof(Function));
    newClass->name = NULL;
    newClass->parent = NULL;
    newClass->next = NULL;
    newClass->propertiesCnt = 0;
    newClass->functionsCnt = 0;
    newClass->object_hmap = create_hm();
    newClass->class_hmap = create_hm();
    return newClass;
}

NFile *createNFile(char *filename)
{
    int len = strlen(filename);
    FILE *fileN = fopen(filename, "r");
    filename[len - 1] = 'c';
    FILE *fileC = fopen(filename, "w");
    filename[len - 1] = 'h';
    FILE *fileH = fopen(filename, "w");

    NFile *newFile = (NFile *)malloc(sizeof(NFile));
    newFile->classes = (Class **)malloc(1000 * sizeof(Class *));
    newFile->classesCnt = 0;
    newFile->fileN = fileN;
    newFile->fileC = fileC;
    newFile->fileH = fileH;
    newFile->token = NULL;
    newFile->tokenC = NULL;
    newFile->tokenH = NULL;
    newFile->filename = (char *)malloc((strlen(filename) + 1) * sizeof(char));
    memset(newFile->filename, 0, sizeof(newFile->filename));
    filename[len - 1] = 'n';
    strcpy(newFile->filename, filename);
    newFile->inheritance = (NFile **)malloc(1000 * sizeof(NFile *));
    ;
    newFile->inheritanceCnt = 0;

    return newFile;
}

Token *endStatement(Token *curs, Token *end)
{
    int catch = 0;
    while (curs != NULL)
    {
        if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
        {
            catch = (catch == 2) ? 0 : 2;
        }
        else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
        {
            catch = (catch == 1) ? 0 : 1;
        }
        else if (strcmp(curs->val, ";") == 0 && catch == 0)
        {
            if (curs == end)
            {
                return curs;
            }
            return curs->next;
        }
        curs = curs->next;
    }
}

Token *endScope(Token *curs)
{
    int balance = 1;
    while (curs != NULL && strcmp(curs->val, "{") != 0)
    {
        curs = curs->next;
    }
    Token *end = curs->next;

    while (end != NULL)
    {
        if (strcmp(end->val, "{") == 0)
        {
            ++balance;
        }
        else if (strcmp(end->val, "}") == 0)
        {
            --balance;
        }
        if (balance < 1)
        {
            return end;
        }
        end = end->next;
    }
    printf("UNBALANCED BRACKETS!\n");
    exit(10);
}

Token *endParenth(Token *curs)
{
    int balance = 1;
    while (curs != NULL && strcmp(curs->val, "(") != 0)
    {
        curs = curs->next;
    }
    Token *end = curs->next;

    while (end != NULL)
    {
        if (strcmp(end->val, "(") == 0)
        {
            ++balance;
        }
        else if (strcmp(end->val, ")") == 0)
        {
            --balance;
        }
        if (balance < 1)
        {
            return end;
        }
        end = end->next;
    }
    printf("UNBALANCED Parenth!\n");
    exit(10);
}

Token *createToken(char *content)
{
    Token *newToken = (Token *)malloc(sizeof(Token));
    newToken->next = NULL;
    newToken->prev = NULL;
    newToken->val = (char *)malloc((strlen(content) + 1) * sizeof(char));
    memset(newToken->val, 0, strlen(content) + 1);
    strcpy(newToken->val, content);
    return newToken;
}

void appendToken(NFile *file, char *content, char type)
{
    Token *start = NULL;
    if (type == 'N')
    {
        start = file->token;
    }
    else if (type == 'C')
    {
        start = file->tokenC;
    }
    else if (type == 'H')
    {
        start = file->tokenH;
    }
    if (start == NULL)
    {
        if (type == 'N')
        {
            file->token = createToken(content);
        }
        else if (type == 'C')
        {
            file->tokenC = createToken(content);
        }
        else if (type == 'H')
        {
            file->tokenH = createToken(content);
        }
        return;
    }
    while (start->next != NULL)
    {
        start = start->next;
    }
    file->endToken = start->next = createToken(content);
    start->next->prev = start;
}

Token *writeInclude(NFile *file, Token *start, Token *end)
{
    char str[1000];
    memset(str, 0, sizeof(str));
    while (strcmp(end->val, "\n") != 0 && strcmp(end->val, "\0") != 0)
    {
        end = end->next;
    }
    strcat(str, start->next->next->next->val);
    while (strcmp(start->val, ".") != 0)
    {
        fwrite(start->val, 1, strlen(start->val), file->fileC);
        start = start->next;
    }
    fwrite(start->val, 1, strlen(start->val), file->fileC);
    start = start->next;
    if (strcmp(start->val, "n") == 0)
    {
        strcat(str, ".n");
        tokenize(str, file);
        fwrite("h\"\n", 1, 3, file->fileC);
    }
    else
    {
        fwrite(start->val, 1, strlen(start->val), file->fileC);
    }
    start = start->next;

    return start;
}

void parseStatement(Class *class, Token *start, Token *end)
{
    char datatype[1000];
    memset(datatype, 0, sizeof(datatype));
    char name[1000];
    memset(name, 0, sizeof(name));
    char value[1000];
    memset(value, 0, sizeof(value));

    int encapsulation = -1;
    if (strcmp(start->val, "+") == 0)
    {
        encapsulation = PUBLIC_CLASS_VAR;
        start = start->next;
    }
    else if (strcmp(start->val, "-") == 0)
    {
        encapsulation = PUBLIC_OBJECT_VAR;
        start = start->next;
    }

    Token *curs = start;
    while (strcmp(curs->next->next->val, "=") != 0 && strcmp(curs->next->next->val, ";") != 0)
    {
        curs = curs->next;
    }
    curs = curs->next;
    while (start != curs)
    {
        strcat(datatype, start->val);
        start = start->next;
    }
    if (strcmp(curs->next->val, "=") == 0)
    {
        Property *prop = createProperty();
        strcpy(prop->datatype, datatype);
        strcpy(prop->name, curs->val);
        prop->encapsulation = encapsulation;

        curs = curs->next->next;
        int catch = 0;
        while ((strcmp(curs->val, ";") != 0 && catch == 0) || catch == 1 || catch == 2)
        {
            if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
            {
                catch = (catch == 2) ? 0 : 2;
            }
            else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
            {
                catch = (catch == 1) ? 0 : 1;
            }
            strcat(value, curs->val);
            curs = curs->next;
        }
        strcpy(prop->value, value);
        if (encapsulation == PUBLIC_CLASS_VAR)
        {
            update_hm(class->class_hmap, prop->name, (void *)prop->datatype);
        }
        else
        {
            update_hm(class->object_hmap, prop->name, (void *)prop->datatype);
        }
        class->properties[class->propertiesCnt++] = prop;
    }
    else if (strcmp(curs->next->val, ";") == 0)
    {
        Property *prop = createProperty();
        strcpy(prop->datatype, datatype);
        strcpy(prop->name, curs->val);
        strcpy(prop->value, "");
        prop->encapsulation = encapsulation;
        if (encapsulation == PUBLIC_CLASS_VAR)
        {
            update_hm(class->class_hmap, prop->name, (void *)prop->datatype);
        }
        else
        {
            update_hm(class->object_hmap, prop->name, (void *)prop->datatype);
        }
        class->properties[class->propertiesCnt++] = prop;
    }
}

// void parseFunctionBodyStatement(Class *class, Function *function, Token *start, Token *end)
// {
//     char datatype[1000];
//     memset(datatype, 0, sizeof(datatype));
//     char name[1000];
//     memset(name, 0, sizeof(name));
//     char value[1000];
//     memset(value, 0, sizeof(value));

//     Token *curs = start;
//     while (isspace(curs->val[0]) != 0 && curs != end)
//     {
//         curs = curs->next;
//     }
//     if (strcmp(curs->next->val, "=") == 0)
//     {
//         return;
//     }
//     while (strcmp(curs->next->next->val, "=") != 0 && strcmp(curs->next->next->val, ";") != 0)
//     {
//         curs = curs->next;
//     }
//     curs = curs->next;
//     while (start != curs)
//     {
//         strcat(datatype, start->val);
//         start = start->next;
//     }
//     if (strcmp(curs->next->val, "=") == 0 && curs->next != end)
//     {
//         curs = curs->next->next;

//         int catch = 0;
//         while ((strcmp(curs->val, ";") != 0 && catch == 0) || catch == 1 || catch == 2)
//         {
//             if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
//             {
//                 catch = (catch == 2) ? 0 : 2;
//             }
//             else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
//             {
//                 catch = (catch == 1) ? 0 : 1;
//             }
//             memset(name, 0, sizeof(name));
//             memset(value, 0, sizeof(value));
//             if ((strcmp(curs->next->val, ",") != 0 || strcmp(curs->next->val, ";") != 0) && catch == 0)
//             {
//                 strcat(name, start->val);
//                 start = start->next->next;
//                 while (start != end->next)
//                 {
//                     strcat(value, start->val);
//                     Property *prop = createProperty();
//                     strcpy(prop->datatype, datatype);
//                     strcpy(prop->name, name);
//                     strcpy(prop->value, value);
//                     update_hm(function->hmap, prop->name, (void *)prop->datatype);
//                     function->properties[function->propertiesCnt++] = prop;
//                     start = start->next;
//                 }
//             }
//             curs = curs->next;
//         }
//     }
//     else
//     {
//         Property *prop = createProperty();
//         strcpy(prop->datatype, datatype);
//         strcpy(prop->name, name);
//         strcpy(prop->value, "");
//         update_hm(function->hmap, prop->name, (void *)prop->datatype);
//         function->properties[function->propertiesCnt++] = prop;
//     }
// }

// void parseFunctionBody(Class *class, Function *function, Token *start, Token *end)
// {
//     function->bodyStart = start;
//     function->bodyEnd = end;
//     int catch = 0;
//     Token *curs = start;
//     while (curs != end)
//     {
//         if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
//         {
//             catch = (catch == 2) ? 0 : 2;
//         }
//         else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
//         {
//             catch = (catch == 1) ? 0 : 1;
//         }
//         if (strcmp(curs->val, ";") == 0 && catch == 0)
//         {
//             parseFunctionBodyStatement(class, function, start, curs);
//             curs = start = curs->next;
//         }
//         else if ((strcmp(curs->val, "for") == 0 || strcmp(curs->val, "while") == 0 || strcmp(curs->val, "if") == 0) && catch)
//         {
//             while ((strcmp(curs->val, "{") != 0 && strcmp(curs->val, "\n") != 0) || catch == 1 || catch == 2)
//             {
//                 if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
//                 {
//                     catch = (catch == 2) ? 0 : 2;
//                 }
//                 else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
//                 {
//                     catch = (catch == 1) ? 0 : 1;
//                 }
//                 if (strcmp(curs->val, "=") == 0 && strcmp(curs->next->val, "=") != 0 && isalpha(curs->prev->val[0]) != 0 && isalpha(curs->prev->prev->val[0]) != 0)
//                 {
//                     parseFunctionBodyStatement(class, function, start, curs);
//                     start = curs = curs->next;
//                     continue;
//                 }
//                 curs = curs->next;
//             }
//         }
//         curs = curs->next;
//     }
// }

void parseFunction(Class *class, Token *start, Token *end)
{
    char datatype[1000];
    memset(datatype, 0, sizeof(datatype));
    char name[1000];
    memset(name, 0, sizeof(name));
    char overloadedName[1000];
    memset(overloadedName, 0, sizeof(overloadedName));
    Function *function = createFunction();
    int encapsulation = -1;
    if (strcmp(start->val, "+") == 0)
    {
        encapsulation = PUBLIC_CLASS_FUNC;
        start = start->next;
    }
    else if (strcmp(start->val, "-") == 0)
    {
        encapsulation = PUBLIC_OBJECT_FUNC;
        start = start->next;
    }
    Token *curs = start;
    while (strcmp(curs->next->val, "(") != 0)
    {
        curs = curs->next;
    }
    while (start != curs)
    {
        if (strcmp(start->val, class->name) == 0)
        {
            strcat(datatype, "Object_");
        }
        strcat(datatype, start->val);
        start = start->next;
    }
    strcat(name, start->val);
    curs = start = start->next->next;
    if (strcmp(start->val, ")") == 0)
    {
        sprintf(overloadedName, "%s%svoid", overloadedName, name);
    }
    else
    {
        strcat(overloadedName, name);
        while (strcmp(curs->val, ")") != 0)
        {
            curs = curs->next;
        }
        Property *param = createProperty();
        while (start != curs)
        {
            if (strcmp(start->val, ",") == 0)
            {
                update_hm(function->hmap, param->name, (void *)param->datatype);
                function->params[function->paramsCnt++] = param;
                param = createProperty();
            }
            else
            {
                if (strcmp(start->next->val, ",") == 0 || strcmp(start->next->val, ")") == 0)
                {
                    if (strlen(param->datatype) == 0)
                    {
                        strcat(overloadedName, start->val);
                        strcat(param->datatype, start->val);
                    }
                    else
                    {
                        strcat(param->name, start->val);
                    }
                }
                else
                {
                    strcat(param->datatype, start->val);
                    strcat(param->datatype, " ");
                    strcat(overloadedName, start->val);
                }
            }
            start = start->next;
        }
        update_hm(function->hmap, (param->name), (void *)param->datatype);
        function->params[function->paramsCnt++] = param;
    }
    if (encapsulation == PUBLIC_CLASS_FUNC)
    {
        update_hm(class->class_hmap, overloadedName, (void *)datatype);
    }
    else if (encapsulation == PUBLIC_OBJECT_FUNC)
    {
        update_hm(class->object_hmap, overloadedName, (void *)datatype);
    }
    strcpy(function->overloadedName, overloadedName);
    strcpy(function->datatype, datatype);
    strcpy(function->name, name);
    //update_hm(class->return_hmap, function->overloadedName, (void *)function->datatype);
    //parseFunctionBody(class, function, curs->next->next, end);
    function->bodyStart = curs->next->next;
    function->bodyEnd = end;
    function->encapsulation = encapsulation;
    class->functions[class->functionsCnt++] = function;
}

Token *parseClass(NFile *file, Token *start, Token *end)
{
    Token *curs = start->next;
    Class *class = createClass();
    end = endScope(end);

    class->name = start->next->val;
    update_hm(classes_hmap, class->name, (void *)class);
    start = start->next->next;
    if (strcmp(start->val, ":") == 0)
    {
        start = start->next;
        Class *parent = NULL;
        if (isClassInstance(file, start->val))
        {
            for (int i = 0; i < file->inheritanceCnt; ++i)
            {
                for (int j = 0; j < file->inheritance[i]->classesCnt; ++j)
                {
                    if (strcmp(file->inheritance[i]->classes[j]->name, start->val) == 0)
                    {
                        parent = file->inheritance[i]->classes[j];
                        break;
                    }
                }
            }
        }
        class->parent = parent;
        start = start->next;
    }

    while (start != end)
    {
        if (strcmp(start->val, "+") == 0 || strcmp(start->val, "-") == 0)
        {
            int catch = 0;
            curs = start;
            while ((strcmp(curs->val, ";") != 0 && strcmp(curs->val, "{") != 0) || catch == 1 || catch == 2)
            {
                if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
                {
                    catch = (catch == 2) ? 0 : 2;
                }
                else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
                {
                    catch = (catch == 1) ? 0 : 1;
                }
                curs = curs->next;
            }
            if (strcmp(curs->val, ";") == 0)
            {

                parseStatement(class, start, curs);
            }
            else
            {
                curs = endScope(curs);
                parseFunction(class, start, curs);
                start = curs->next;
            }
        }
        start = start->next;
    }
    file->classes[file->classesCnt++] = class;
    return start;
}

void parseLevel0(NFile *file)
{
    Token *start = file->token;
    Token *eof = file->endToken;
    Token *curs = start;
    Token *end = start;

    while (curs != eof)
    {
        if (strcmp(curs->val, "#") == 0 && strcmp(curs->next->next->val, "<") != 0)
        {
            end = start = curs = writeInclude(file, curs, end);
        }
        else if (strcmp(curs->val, "#") == 0 && strcmp(curs->next->next->val, "<") == 0)
        {
            while (curs != NULL && strcmp(curs->val, "\n") != 0)
            {
                fwrite(curs->val, 1, strlen(curs->val), file->fileC);
                curs = curs->next;
            }
            fwrite("\n", 1, 1, file->fileC);
            end = start = curs->next;
        }
        else if (strcmp(curs->val, "class") == 0)
        {
            end = start = curs = parseClass(file, curs, end);
        }
        curs = curs->next;
    }
    char str[1000];
    memset(str, 0, sizeof(str));
    sprintf(str, "%s\n#include \"%s", str, file->filename);
    str[strlen(str) - 1] = 'h';
    strcat(str, "\"\n\n");
    fwrite(str, 1, strlen(str), file->fileC);
}

void constructClassStruct(NFile *file, Class *class)
{
    FILE *cFile = file->fileC;
    FILE *hFile = file->fileH;

    if (class->parent != NULL)
    {
        int jLen = class->functionsCnt;
        int len = class->parent->functionsCnt;
        for (int i = 0; i < len; ++i)
        {
            int ctc = 1;
            for (int j = 0; j < jLen; ++j)
            {
                if (strcmp(class->parent->functions[i]->name, class->functions[j]->name) == 0)
                {
                    ctc = 0;
                    break;
                }
            }
            if (strstr(class->parent->functions[i]->name, "init") == class->parent->functions[i]->name)
            {
                memset(class->parent->functions[i]->datatype, 0, sizeof(class->parent->functions[i]->datatype));
                sprintf(class->parent->functions[i]->datatype, "%sObject_%s *", class->parent->functions[i]->datatype, class->name);
                strcat(class->parent->functions[i]->name, class->parent->name);
                ctc = 1;
            }
            if (ctc == 1)
            {
                class->functions[class->functionsCnt++] = class->parent->functions[i];
            }
        }
        jLen = class->propertiesCnt;
        len = class->parent->propertiesCnt;
        for (int i = 0; i < len; ++i)
        {
            int ctc = 1;
            for (int j = 0; j < jLen; ++j)
            {
                if (strcmp(class->parent->properties[i]->name, class->properties[j]->name) == 0)
                {
                    ctc = 0;
                    break;
                }
            }
            if (ctc == 1)
            {
                class->properties[class->propertiesCnt++] = class->parent->properties[i];
            }
        }
    }

    char str[100000];
    memset(str, 0, sizeof(str));
    sprintf(str, "%stypedef struct Class_%s Class_%s;\n", str, class->name, class->name);
    sprintf(str, "%stypedef struct Object_%s Object_%s;\n", str, class->name, class->name);
    sprintf(str, "%sClass_%s *Class_%s_Instance;\n", str, class->name, class->name);
    fwrite(str, 1, strlen(str), hFile);

    memset(str, 0, sizeof(str));
    sprintf(str, "%sstruct Class_%s {\n", str, class->name);
    for (int i = 0; i < class->propertiesCnt; ++i)
    {
        if (class->properties[i]->encapsulation == PUBLIC_CLASS_VAR)
        {
            sprintf(str, "%s\t%s %s;\n", str, class->properties[i]->datatype, class->properties[i]->name);
        }
    }
    Class *parent = NULL;
    if (class->parent != NULL)
    {
        parent = class->parent;
    }
    for (int i = 0; i < class->functionsCnt; ++i)
    {
        if (class->functions[i]->encapsulation == PUBLIC_CLASS_FUNC && strcmp(class->functions[i]->name, "main") != 0)
        {
            if (parent != NULL && strstr(class->functions[i]->name, parent->name) != NULL)
            {
                sprintf(str, "%s\t%s(*%s%s)(Object_%s *this,", str, class->functions[i]->datatype, class->functions[i]->name, class->name, class->name);
            }
            else
            {
                sprintf(str, "%s\t%s(*%s%s)(", str, class->functions[i]->datatype, class->functions[i]->name, class->name);
            }
            for (int j = 0; j < class->functions[i]->paramsCnt; ++j)
            {
                if (class->functions[i]->params[j]->datatype != NULL)
                {
                    if (strcmp(class->functions[i]->params[j]->datatype, "void") == 0)
                    {
                        continue;
                    }
                    strcat(str, class->functions[i]->params[j]->datatype);
                    if (class->functions[i]->params[j]->name != NULL)
                    {
                        strcat(str, class->functions[i]->params[j]->name);
                        if (j + 1 < class->functions[i]->paramsCnt)
                        {
                            strcat(str, ",");
                        }
                    }
                }
            }
            if (str[strlen(str) - 1] == ',')
            {
                str[strlen(str) - 1] = '\0';
            }
            sprintf(str, "%s);\n", str);
        }
    }
    sprintf(str, "%s};\n", str);
    fwrite(str, 1, strlen(str), hFile);

    memset(str, 0, sizeof(str));
    sprintf(str, "%sstruct Object_%s {\n", str, class->name);
    for (int i = 0; i < class->propertiesCnt; ++i)
    {
        if (class->properties[i]->encapsulation == PUBLIC_OBJECT_VAR)
        {
            sprintf(str, "%s\t%s %s;\n", str, class->properties[i]->datatype, class->properties[i]->name);
        }
    }
    for (int i = 0; i < class->functionsCnt; ++i)
    {
        if (class->functions[i]->encapsulation == PUBLIC_OBJECT_FUNC && strcmp(class->functions[i]->name, "main") != 0)
        {
            sprintf(str, "%s\t%s(*%s%s)(Object_%s *this", str, class->functions[i]->datatype, class->functions[i]->name, class->name, class->name);
            for (int j = 0; j < class->functions[i]->paramsCnt; ++j)
            {
                if (class->functions[i]->params[j]->datatype != NULL)
                {
                    if (strcmp(class->functions[i]->params[j]->datatype, "void") == 0)
                    {
                        continue;
                    }
                    strcat(str, ", ");
                    strcat(str, class->functions[i]->params[j]->datatype);
                    if (class->functions[i]->params[j]->name != NULL)
                    {
                        strcat(str, class->functions[i]->params[j]->name);
                    }
                }
            }
            sprintf(str, "%s);\n", str);
        }
    }
    sprintf(str, "%s};\n", str);
    fwrite(str, 1, strlen(str), hFile);
}

char *preInitContent(NFile *file, Class *class, Function *function)
{
    char *str = (char *)malloc(1000000 * sizeof(char));
    memset(str, 0, sizeof(str));
    sprintf(str, "%s\tObject_%s *this = malloc(sizeof(Object_%s));\n", str, class->name, class->name);
    for (int i = 0; i < class->functionsCnt; ++i)
    {
        if (class->functions[i]->encapsulation == PUBLIC_OBJECT_FUNC && strcmp(class->functions[i]->name, "main") != 0)
        {
            sprintf(str, "%s\tthis->%s%s = &%s%s;\n", str, class->functions[i]->name, class->name, class->functions[i]->name, class->name);
        }
    }
    for (int i = 0; i < class->propertiesCnt; ++i)
    {
        if (class->properties[i]->encapsulation == PUBLIC_OBJECT_VAR)
        {
            if (strlen(class->properties[i]->value) != 0)
            {
                sprintf(str, "%s\tthis->%s = %s;\n", str, class->properties[i]->name, class->properties[i]->value);
            }
        }
    }
    return str;
}

char *convertToOverloaded(char *val)
{
    int len = strlen(val);
    char *newVal = malloc((strlen(val) + 1) * sizeof(char));
    memset(newVal, 0, sizeof(newVal));
    int j = 0;
    for (int i = 0; i < len; ++i)
    {
        if (isalnum(val[i]) != 0)
        {
            newVal[j++] = val[i];
        }
        else if (val[i] == '*')
        {
            newVal[j++] = '1';
        }
    }
    return newVal;
}

char *constructClassFunctionsBody(NFile *file, Class *class, Function *function, Token *start, Token *end)
{
    char *str = (char *)malloc(1000000 * sizeof(char));
    memset(str, 0, sizeof(str));
    while (start != end)
    {
        if (isalnum(start->val[0]) != 0 && strcmp(start->next->val, "-") == 0 && strcmp(start->next->next->val, ">") == 0 && isClassInstance(file, start->val))
        {
            if (strcmp(start->next->next->next->next->val, "(") == 0)
            {
                sprintf(str, "%sClass_%s_Instance->%s%s(", str, start->val, start->next->next->next->val, start->val);
                start = start->next->next->next->next;
            }
            else
            {
                sprintf(str, "%sClass_%s_Instance", str, start->val);
            }
            start = start->next;
            continue;
        }
        else if (isalnum(start->val[0]) != 0 && strcmp(start->next->val, "-") == 0 && strcmp(start->next->next->val, ">") == 0)
        {
            char *val = NULL;
            if (strcmp(start->val, "super") == 0)
            {
                sprintf(str, "%sClass_%s_Instance->%s%s%s(this,", str, class->name, function->name, class->parent->name, class->name);
                start = start->next->next->next->next->next;
                continue;
            }
            else if (strcmp(start->val, "this") != 0)
            {
                val = fetch_hm(function->hmap, start->val);
                if (val != NULL)
                {
                    if (isClassInstance(file, val))
                    {
                        sprintf(str, "%s%s->%s%s", str, start->val, start->next->next->next->val, val);
                        start = start->next->next->next->next;
                        continue;
                    }
                }
            }
            else if (strcmp(start->val, "this") == 0)
            {
                if (strcmp(start->next->next->next->next->val, "(") == 0)
                {
                    sprintf(str, "%s%s->%s%s(this", str, start->val, start->next->next->next->val, class->name);
                    for (int i = 0; i < class->functionsCnt; ++i)
                    {
                        if (strcmp(class->functions[i]->name, start->next->next->next->val) == 0)
                        {
                            if (class->functions[i]->paramsCnt > 0)
                            {
                                strcat(str, ",");
                            }
                            break;
                        }
                    }
                    start = start->next->next->next->next->next;
                    continue;
                }
            }
        }
        else if (isalnum(start->val[0]) != 0 && isClassInstance(file, start->val))
        {
            Token *tmp = start->next;
            while (isalnum(tmp->val[0]) == 0)
            {
                tmp = tmp->next;
            }
            update_hm(function->hmap, tmp->val, start->val);
            sprintf(str, "%sObject_%s", str, start->val);
            start = start->next;
            continue;
        }
        strcat(str, start->val);
        if (isalnum(start->val[0]) != 0 && isalnum(start->next->val[0]) != 0)
        {
            strcat(str, " ");
        }
        start = start->next;
    }
    return str;
}

void constructClassFunctions(NFile *file, Class *class)
{
    FILE *cFile = file->fileC;
    FILE *hFile = file->fileH;

    char str[1000000];

    for (int i = 0; i < class->functionsCnt; ++i)
    {
        memset(str, 0, sizeof(str));
        int parentInit = 0;
        if (class->parent != NULL)
        {
            if (strstr(class->functions[i]->name, "init") != NULL && strstr(class->functions[i]->name, class->parent->name) != NULL)
            {
                parentInit = 1;
            }
        }
        sprintf(str, "%s%s %s%s(", str, class->functions[i]->datatype, class->functions[i]->name, class->name);
        if (((class->functions[i]->encapsulation == PUBLIC_OBJECT_FUNC) && strcmp(class->functions[i]->name, "main") != 0) || parentInit)
        {
            sprintf(str, "%s Object_%s *this", str, class->name);
        }
        for (int j = 0; j < class->functions[i]->paramsCnt; ++j)
        {
            if (strcmp(class->functions[i]->params[j]->datatype, "void") == 0)
            {
                continue;
            }
            if (j + 1 == class->functions[i]->paramsCnt)
            {
                if (class->functions[i]->encapsulation == PUBLIC_CLASS_FUNC && !parentInit)
                {
                    sprintf(str, "%s %s %s", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
                else
                {
                    sprintf(str, "%s, %s %s", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
            }
            else
            {
                if (class->functions[i]->encapsulation == PUBLIC_CLASS_FUNC && !parentInit)
                {
                    sprintf(str, "%s %s %s,", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
                else
                {
                    sprintf(str, "%s, %s %s", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
            }
        }
        strcat(str, ") {\n");
        if (strcmp(class->functions[i]->name, "init") == 0)
        {
            char *pre = preInitContent(file, class, class->functions[i]);
            strcat(str, pre);
            free(pre);
            Token *start = class->functions[i]->bodyStart;
            Token *end = class->functions[i]->bodyEnd;

            strcat(str, constructClassFunctionsBody(file, class, class->functions[i], start, end));
        }
        else
        {
            Token *start = class->functions[i]->bodyStart;
            Token *end = class->functions[i]->bodyEnd;
            strcat(str, constructClassFunctionsBody(file, class, class->functions[i], start, end));
        }
        strcat(str, "}\n");
        fwrite(str, 1, strlen(str), cFile);
    }
}

void constructClassFunctionProtos(NFile *file, Class *class)
{
    FILE *cFile = file->fileC;
    FILE *hFile = file->fileH;

    char str[100000];

    for (int i = 0; i < class->functionsCnt; ++i)
    {
        memset(str, 0, sizeof(str));

        int parentInit = 0;
        if (class->parent != NULL)
        {
            if (strstr(class->functions[i]->name, "init") != NULL && strstr(class->functions[i]->name, class->parent->name) != NULL)
            {
                parentInit = 1;
            }
        }

        sprintf(str, "%s%s %s%s(", str, class->functions[i]->datatype, class->functions[i]->name, class->name);
        if ((class->functions[i]->encapsulation == PUBLIC_OBJECT_FUNC && strcmp(class->functions[i]->name, "main") != 0) || parentInit)
        {
            sprintf(str, "%s Object_%s *this", str, class->name);
        }
        for (int j = 0; j < class->functions[i]->paramsCnt; ++j)
        {
            if (strcmp(class->functions[i]->params[j]->datatype, "void") == 0)
            {
                continue;
            }
            if (j + 1 == class->functions[i]->paramsCnt)
            {
                if (class->functions[i]->encapsulation == PUBLIC_CLASS_FUNC && !parentInit)
                {
                    sprintf(str, "%s %s %s", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
                else
                {
                    sprintf(str, "%s, %s %s", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
            }
            else
            {
                if (class->functions[i]->encapsulation == PUBLIC_CLASS_FUNC && !parentInit)
                {
                    sprintf(str, "%s %s %s,", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
                else
                {
                    sprintf(str, "%s, %s %s", str, class->functions[i]->params[j]->datatype, class->functions[i]->params[j]->name);
                }
            }
        }
        strcat(str, ");\n");
        fwrite(str, 1, strlen(str), cFile);
    }
}

void constructInitFunction(NFile *file)
{
    char hStr[1000000];
    char str[1000000];
    memset(str, 0, sizeof(str));
    for (int i = 0; i < file->classesCnt; ++i)
    {
        Class *class = file->classes[i];
        memset(hStr, 0, sizeof(hStr));
        sprintf(hStr, "%svoid start%s();\n", hStr, class->name);
        fwrite(hStr, 1, strlen(hStr), file->fileH);

        sprintf(str, "%svoid start%s() {\n", str, class->name);
        sprintf(str, "%s\tif(Class_%s_Instance == NULL) {\n\t\tClass_%s_Instance = malloc(sizeof(Class_%s_Instance));\n", str, class->name, class->name, class->name);
        for (int i = 0; i < class->propertiesCnt; ++i)
        {
            if (class->properties[i]->encapsulation == PUBLIC_CLASS_VAR)
            {
                if (strlen(class->properties[i]->value) != 0)
                {
                    sprintf(str, "%s\t\tClass_%s_Instance->%s = %s;\n", str, class->name, class->properties[i]->name, class->properties[i]->value);
                }
            }
        }
        for (int i = 0; i < class->functionsCnt; ++i)
        {
            if (class->functions[i]->encapsulation == PUBLIC_CLASS_FUNC)
            {
                sprintf(str, "%s\t\tClass_%s_Instance->%s%s = &%s%s;\n", str, class->name, class->functions[i]->name, class->name, class->functions[i]->name, class->name);
            }
        }
        strcat(str, "\t}\n");
        if (i + 1 >= file->classesCnt)
        {
            for (int i = 0; i < file->inheritanceCnt; ++i)
            {
                for (int j = 0; j < file->inheritance[i]->classesCnt; ++j)
                {
                    sprintf(str, "%s\tstart%s();\n", str, file->inheritance[i]->classes[j]->name);
                }
            }
        }
        strcat(str, "}\n");
    }
    fwrite(str, 1, strlen(str), file->fileC);
}

int isClassInstance(NFile *file, char *name)
{
    for (int i = 0; i < file->classesCnt; ++i)
    {
        if (strcmp(name, file->classes[i]->name) == 0)
        {
            return 1;
        }
    }
    for (int i = 0; i < file->inheritanceCnt; ++i)
    {
        for (int j = 0; j < file->inheritance[i]->classesCnt; ++j)
        {
            if (strcmp(name, file->inheritance[i]->classes[j]->name) == 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

void constructEverythingElse(NFile *file, Token *sstart, Token *eend)
{
    char str[100000];
    memset(str, 0, sizeof(str));
    Token *eof = eend;
    Token *start = sstart;
    Token *curs = NULL;
    HMap **hmap = create_hm();
    int inString = 0;
    while (start != NULL && start != eof)
    {
        if (curs != NULL)
        {
            if (curs == start)
            {
                curs = NULL;
            }
        }
        memset(str, 0, sizeof(str));
        if (curs == NULL)
        {
            if (strcmp(start->val, "#") == 0 && strcmp(start->next->val, "include") == 0)
            {
                while (start != NULL && strcmp(start->val, "\n") != 0)
                {
                    start = start->next;
                }
                continue;
            }
            if (strcmp(start->val, "class") == 0)
            {
                start = endScope(start);
                start = start->next;
                continue;
            }
            if (strcmp(start->val, "-") == 0)
            {
                int catch = 0;
                curs = start;
                while ((strcmp(curs->val, ";") != 0 && strcmp(curs->val, "{") != 0) || catch == 1 || catch == 2)
                {
                    if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
                    {
                        catch = (catch == 2) ? 0 : 2;
                    }
                    else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
                    {
                        catch = (catch == 1) ? 0 : 1;
                    }
                    curs = curs->next;
                }
                if (strcmp(curs->val, "{") == 0)
                {
                    curs = endScope(curs);
                }
                start = start->next;
                continue;
            }
            if (strcmp(start->val, "+") == 0)
            {
                int catch = 0;
                Token *tmpStart;
                tmpStart = curs = start;
                tmpStart = tmpStart->next;
                while ((strcmp(curs->val, ";") != 0 && strcmp(curs->val, "{") != 0) || catch == 1 || catch == 2)
                {
                    if (strcmp(curs->val, "'") == 0 && (catch == 0 || catch == 2))
                    {
                        catch = (catch == 2) ? 0 : 2;
                    }
                    else if (strcmp(curs->val, "\"") == 0 && (catch == 0 || catch == 1))
                    {
                        catch = (catch == 1) ? 0 : 1;
                    }
                    curs = curs->next;
                }
                while (tmpStart != curs)
                {
                    if (isClassInstance(file, tmpStart->val))
                    {
                        sprintf(str, "%sClass_%s_Instance ", str, tmpStart->val);
                        fwrite(str, 1, strlen(str), file->fileH);
                    }
                    else
                    {
                        fwrite(tmpStart->val, 1, strlen(tmpStart->val), file->fileH);
                        if (isalnum(tmpStart->val[0]) != 0)
                        {
                            fwrite(" ", 1, 1, file->fileH);
                        }
                    }
                    tmpStart = tmpStart->next;
                }
                fwrite(";\n", 1, 2, file->fileH);
                if (strcmp(curs->val, "{") == 0)
                {
                    curs = endScope(curs);
                }
                start = start->next;
                continue;
            }
        }
        if (isalnum(start->val[0]) != 0 && strcmp(start->next->val, "-") == 0 && strcmp(start->next->next->val, ">") == 0 && isClassInstance(file, start->val))
        {
            if (strcmp(start->next->next->next->next->val, "(") == 0)
            {
                sprintf(str, "%sClass_%s_Instance->%s%s", str, start->val, start->next->next->next->val, start->val);
                fwrite(str, 1, strlen(str), file->fileC);
                start = start->next->next->next->next;
            }
            else
            {
                sprintf(str, "%sClass_%s_Instance", str, start->val);
                fwrite(str, 1, strlen(str), file->fileC);
                start = start->next;
            }
            continue;
        }
        else if (isalnum(start->val[0]) != 0 && strcmp(start->next->val, "-") == 0 && strcmp(start->next->next->val, ">") == 0)
        {
            char *val = fetch_hm(hmap, start->val);
            if (val != NULL)
            {
                if (isClassInstance(file, val))
                {
                    sprintf(str, "%s%s->%s%s(%s", str, start->val, start->next->next->next->val, val, start->val);
                    if (isalnum(start->next->next->next->next->next->val[0]) != 0)
                    {
                        strcat(str, ",");
                    }
                    fwrite(str, 1, strlen(str), file->fileC);
                    start = start->next->next->next->next->next;
                    continue;
                }
            }
        }
        else if (isalnum(start->val[0]) != 0 && isClassInstance(file, start->val))
        {
            Token *tmp = start->next;
            while (isalnum(tmp->val[0]) == 0)
            {
                tmp = tmp->next;
            }
            update_hm(hmap, tmp->val, start->val);
            sprintf(str, "%sObject_%s", str, start->val);
            fwrite(str, 1, strlen(str), file->fileC);
            start = start->next;
            continue;
        }
        else if (strcmp(start->val, "main") == 0 && strcmp(start->next->val, "(") == 0)
        {
            while (strcmp(start->val, "{") != 0)
            {
                fwrite(start->val, 1, strlen(start->val), file->fileC);
                if (isalnum(start->val[0]) != 0)
                {
                    fwrite(" ", 1, 1, file->fileC);
                }
                start = start->next;
            }
            strcat(str, "{\n");
            for (int i = 0; i < file->classesCnt; ++i)
            {
                sprintf(str, "%s\tstart%s();\n", str, file->classes[i]->name);
            }
            fwrite(str, 1, strlen(str), file->fileC);
            start = start->next;
            continue;
        }
        else
        {
            if (inString == 0 && (start->val[0] == '"' || start->val[0] == '\''))
            {
                inString = start->val[0];
            }
            else if ((inString == '"' && start->val[0] == '"') || (inString == '\'' && start->val[0] == '\''))
            {
                inString = 0;
            }
            fwrite(start->val, 1, strlen(start->val), file->fileC);
            if (isalnum(start->val[0]) != 0 && inString == 0)
            {
                fwrite(" ", 1, 1, file->fileC);
            }
        }
        start = start->next;
    }
}

void constructFile(NFile *file)
{
    for (int i = 0; i < file->classesCnt; ++i)
    {
        constructClassStruct(file, file->classes[i]);
        constructClassFunctionProtos(file, file->classes[i]);
        constructClassFunctions(file, file->classes[i]);
    }
    constructInitFunction(file);
    constructEverythingElse(file, file->token, file->endToken);
}

void tokenize(char *filename, NFile *currFile)
{
    for (int i = 0; i < fileListCnt; ++i)
    {
        if (strcmp(fileList[i]->filename, filename) == 0)
        {
            return;
        }
    }

    NFile *file = NULL;
    fileList[fileListCnt++] = file = createNFile(filename);

    if (currFile != NULL)
    {
        currFile->inheritance[currFile->inheritanceCnt++] = file;
    }

    FILE *f = file->fileN;
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0)
    {
        return;
    }
    char *buff = malloc(len + 1);
    memset(buff, 0, sizeof(buff));
    fread(buff, 1, len, f);

    int i = 0;
    int inComment = 0;
    int tmpI = 0;
    char tmp[1000];
    memset(tmp, 0, sizeof(tmp));
    while (i < len + 1)
    {
        if (buff[i] == '/' && buff[i + 1] == '/' && inComment == 0)
        {
            if (tmpI != 0)
            {
                appendToken(file, tmp, 'N');
                tmpI = 0;
                memset(tmp, 0, sizeof(tmp));
            }
            inComment = 1;
            ++i;
            ++i;
            continue;
        }
        if (buff[i] == '/' && buff[i + 1] == '*' && inComment == 0)
        {
            if (tmpI != 0)
            {
                appendToken(file, tmp, 'N');
                tmpI = 0;
                memset(tmp, 0, sizeof(tmp));
            }
            inComment = 2;
            ++i;
            ++i;
            continue;
        }
        if (inComment > 0)
        {
            if (buff[i] == '\n' && inComment == 1)
            {
                inComment = 0;
                continue;
            }
            else if (buff[i] == '*' && buff[i + 1] == '/' && inComment == 2)
            {
                inComment = 0;
                ++i;
                ++i;
                continue;
            }
            ++i;
            continue;
        }
        else
        {
            if (isalnum(buff[i]) > 0 || buff[i] == '_')
            {
                tmp[tmpI++] = buff[i];
            }
            else if (isspace(buff[i]) == 0 || buff[i] == '\n')
            {
                if (tmpI != 0)
                {
                    appendToken(file, tmp, 'N');
                    tmpI = 0;
                    memset(tmp, 0, sizeof(tmp));
                }
                tmp[tmpI++] = buff[i];
                appendToken(file, tmp, 'N');
                tmpI = 0;
                memset(tmp, 0, sizeof(tmp));
            }
            else
            {
                if (tmpI != 0)
                {
                    appendToken(file, tmp, 'N');
                    tmpI = 0;
                    memset(tmp, 0, sizeof(tmp));
                }
            }
        }
        ++i;
    }
    parseLevel0(file);
    constructFile(file);
}

int main(int argc, char **argv)
{
    memset(rootfilename, 0, sizeof(rootfilename));
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argv[i], "-r") == 0)
        {
            ++i;
            strcpy(rootfilename, argv[i]);
            continue;
        }
    }
    if (strlen(rootfilename) == 0)
    {
        printf("Please specify a root .n file\nExample: nymph -r main.n\n");
        return 0;
    }
    classes_hmap = create_hm();
    fileList = (NFile **)malloc(1000 * sizeof(NFile *));
    fileListCnt = 0;

    tokenize(rootfilename, NULL);

    return 0;
}