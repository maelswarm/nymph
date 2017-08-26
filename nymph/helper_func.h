//
//  helper_func.h
//  
//
//  Created by fairy-slipper on 8/26/17.
//
//

#ifndef helper_func_h
#define helper_func_h

#include <stdio.h>

char *getToken(char *str);
int containsDataType(char *str);
int isDataType(char *str);
int isStorageC(char *str);
int isFuncCall(char *str);
int isVar(char *str);
int isVal(char *str);
int isReturn(char *str);
int isOper(char *str);

int numberOfcharInString(char* str, char c);
char *postLastBracket(char *str);
int determinePointer(char* str, char c);
char *str_replace(char *orig, char *rep, char *with);
void trimAllButLetter(char* str);
void trimAllButLetterAndStar(char* str);
void trimAllButAlphaAndStar(char* str);
void trimAllButAlpha(char* str);
void trimAllButNumbers(char* str);
void trim(char* str);
char *nSubString(char *string, char *end);
char *nPostSubString(char *string, char *end);
char *subStringPostLastOccurance(char *string, char end);
char *subStringLastOccurance(char *string, char end);
char *lastPtheses(char *str);
char *postLastPtheses(char *str);
char *lastBracket(char *str);

char *loadFile(FILE *fp);

#endif /* helper_func_h */
