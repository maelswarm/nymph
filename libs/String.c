#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

#include "String.h"

Object_String* initString( char *  text);
void setContentString( Object_String *this, char *  text);
void printlnString( Object_String *this);
Object_String* initString( char *  text) {
	Object_String *this = malloc(sizeof(Object_String));
	this->setContentString = &setContentString;
	this->printlnString = &printlnString;
	this->content = NULL;
	this->length = 0;
	this->size = 0;

this->length=strlen(text);
this->size=this->length+1;
this->content=(uint16_t*)malloc(this->size*sizeof(uint16_t));
memset(this->content,0,sizeof(this->content));
this->setContentString(this,text);
this->printlnString(this);
return this;
}
void setContentString( Object_String *this, char *  text) {

int len=this->length;
for(int i=0;i<len;++i){
this->content[i]=text[i];
}
}
void printlnString( Object_String *this) {

int len=this->length;
for(int i=0;i<len;++i){
printf("%d",this->content[i]);
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

return 0 ;
}