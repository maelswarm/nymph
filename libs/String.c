#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

#include "String.h"

Object_String* initString( char *  text);
void reallocString( Object_String *this, int  size);
void valueString( Object_String *this, char *  text);
void printlnString( Object_String *this);
Object_String* initString( char *  text) {
	Object_String *this = malloc(sizeof(Object_String));
	this->reallocString = &reallocString;
	this->valueString = &valueString;
	this->printlnString = &printlnString;
	this->content = NULL;
	this->length = 0;
	this->size = 0;

this->content=(uint16_t*)malloc(this->size*sizeof(uint16_t));
memset(this->content,0,sizeof(this->content));
this->valueString(this,text);
return this;
}
void reallocString( Object_String *this, int  size) {

this->content=realloc(this->content,size);
this->size=size;
}
void valueString( Object_String *this, char *  text) {

this->length=strlen(text);
if(strlen(text)>=this->size){
this->reallocString(this,this->length+1);
}
int len=this->length;
for(int i=0;i<len;++i){
this->content[i]=text[i];
}
}
void printlnString( Object_String *this) {

int len=this->length;
for(int i=0;i<len;++i){
printf("%c",(char)this->content[i]);
}
printf("\n");
}
void startString() {
	if(Class_String_Instance == NULL) {
		Class_String_Instance = malloc(sizeof(Class_String_Instance));
		Class_String_Instance->initString = &initString;
	}
}







int main (void ){
	startString();


Object_String*helloWorld =Class_String_Instance->initString("HelloWorld");
helloWorld->printlnString(helloWorld);
helloWorld->valueString(helloWorld,"Thisisme!");
helloWorld->printlnString(helloWorld);

return 0 ;
}