#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<ctype.h>

#include "JSON.h"

Object_JSON_Entity* initJSON_Entity();
Object_JSON_Entity* initJSON_Entity() {
	Object_JSON_Entity *this = malloc(sizeof(Object_JSON_Entity));

this->parent=NULL;
this->keys=NULL;
this->parent=NULL;
return this;
}
Object_JSON_Entity* _create_entityJSON( void *  datatype, void *  value);
void parseJSON( char *  str);
char** _tokenizeJSON( char *  str);
Object_JSON_Entity* _create_entityJSON( void *  datatype, void *  value) {

Object_JSON_Entity*entity=Class_JSON_Entity_Instance->initJSON_Entity();
entity->parent=NULL;
entity->keys=NULL;
entity->values=value;
return entity;
}
void parseJSON( char *  str) {

printf("fff");
}
char** _tokenizeJSON( char *  str) {

char buff[100000];
int buffI=0;
memset(buff,0,sizeof(buff));
char**tokens=(char**)malloc((strlen(str)+1)*sizeof(char*));
for(int i=0;i<strlen(str)+1;++i){
tokens[i]=NULL;
}
int i=0;
for(int idx=0;idx<strlen(str);++i){
if(str[idx]==','||str[idx]=='{'||str[idx]=='}'||str[idx]=='['||str[idx]==']'||str[idx]==':'){
tokens[i]=(char*)malloc(strlen(buff)+1);
memset(tokens[i],0,sizeof(tokens[i]));
strcpy(tokens[i],buff);
memset(buff,0,sizeof(buff));
buffI=0;
++i;

tokens[i]=malloc(2);
memset(tokens[i],0,sizeof(tokens[i]));
tokens[i][0]=str[idx];
buffI=0;
++i;
}else{
buff[buffI]=str[idx];
}
}
if(strlen(buff)>0){
tokens[i]=(char*)malloc(strlen(buff)+1);
memset(tokens[i],0,sizeof(tokens[i]));
strcpy(tokens[i],buff);
memset(buff,0,sizeof(buff));
buffI=0;
++i;
}
return tokens;
}
void startJSON_Entity() {
	if(Class_JSON_Entity_Instance == NULL) {
		Class_JSON_Entity_Instance = malloc(sizeof(Class_JSON_Entity_Instance));
		Class_JSON_Entity_Instance->initJSON_Entity = &initJSON_Entity;
	}
}
void startJSON() {
	if(Class_JSON_Instance == NULL) {
		Class_JSON_Instance = malloc(sizeof(Class_JSON_Instance));
		Class_JSON_Instance->_create_entityJSON = &_create_entityJSON;
		Class_JSON_Instance->parseJSON = &parseJSON;
		Class_JSON_Instance->_tokenizeJSON = &_tokenizeJSON;
	}
}













int main (void ){
	startJSON_Entity();
	startJSON();

char **tokens =Class_JSON_Instance->_tokenizeJSON("{ aaa:[1,2,3,4], bbb: null }");
int i =0 ;
while (tokens [i ]!=NULL ){
printf ("%s\n",tokens [i ]);
++i ;
}
return 0 ;
}