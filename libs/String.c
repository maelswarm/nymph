#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

#include "String.h"

Object_String* initString( char *  text);
void reallocString( Object_String *this, int  size);
void valueString( Object_String *this, char *  text);
void printlnString( Object_String *this);
int indexOfString( Object_String *this, char *  text);
int lastIndexOfString( Object_String *this, char *  text);
char* sliceString( Object_String *this, int  start, int  end);
Object_String* initString( char *  text) {
	Object_String *this = malloc(sizeof(Object_String));
	this->reallocString = &reallocString;
	this->valueString = &valueString;
	this->printlnString = &printlnString;
	this->indexOfString = &indexOfString;
	this->lastIndexOfString = &lastIndexOfString;
	this->sliceString = &sliceString;
	this->content = NULL;
	this->length = 0;
	this->size = 0;

this->content=(char*)malloc(this->size*sizeof(char));
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
printf("%c",this->content[i]);
}
printf("\n");
}
int indexOfString( Object_String *this, char *  text) {

void*p=strstr(this->content,text);
return(int)(p-(void*)this->content);
}
int lastIndexOfString( Object_String *this, char *  text) {

char*tmp=(void*)this->content,*p=0;
while((tmp=strstr(tmp,text))!=NULL){
p=tmp;
++tmp;
}
return(int)(p-this->content);
}
char* sliceString( Object_String *this, int  start, int  end) {

int len=end-start;
char*r=(char*)malloc((len+1)*sizeof(char));
memset(r,0,sizeof(r));
for(int i=0;i<len;++i){
r[i]=this->content[start+i];
}
return r;
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
printf ("%i\n",helloWorld->indexOfString(helloWorld,"is"));
printf ("%i\n",helloWorld->lastIndexOfString(helloWorld,"is"));
printf ("%s\n",helloWorld->sliceString(helloWorld,2 ,6 ));

return 0 ;
}