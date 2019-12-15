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
void replaceString( Object_String *this, char *  target, char *  text);
void toLowerCaseString( Object_String *this);
void toUpperCaseString( Object_String *this);
Object_String* initString( char *  text) {
	Object_String *this = malloc(sizeof(Object_String));
	this->reallocString = &reallocString;
	this->valueString = &valueString;
	this->printlnString = &printlnString;
	this->indexOfString = &indexOfString;
	this->lastIndexOfString = &lastIndexOfString;
	this->sliceString = &sliceString;
	this->replaceString = &replaceString;
	this->toLowerCaseString = &toLowerCaseString;
	this->toUpperCaseString = &toUpperCaseString;
	this->value = NULL;
	this->length = 0;

this->value=(char*)malloc((sizeof(text)+1)*sizeof(char));
memset(this->value,0,sizeof(this->value));
this->length=sizeof(text);
this->valueString(this,text);
return this;
}
void reallocString( Object_String *this, int  size) {

this->value=realloc(this->value,size);
this->length=size;
}
void valueString( Object_String *this, char *  text) {

if(strlen(text)>=this->length-1){
this->reallocString(this,strlen(text)+1);
}
int len=this->length;
for(int i=0;i<len;++i){
this->value[i]=text[i];
}
this->value[len]=0;
}
void printlnString( Object_String *this) {

int len=this->length;
for(int i=0;i<len;++i){
printf("%c",this->value[i]);
}
printf("\n");
}
int indexOfString( Object_String *this, char *  text) {

char*p=strstr(this->value,text);
return p<0?-1:(int)(p-this->value);
}
int lastIndexOfString( Object_String *this, char *  text) {

char*tmp=(void*)this->value,*p=0;
while((tmp=strstr(tmp,text))!=NULL){
p=tmp;
++tmp;
}
return p<0?-1:(int)(p-this->value);
}
char* sliceString( Object_String *this, int  start, int  end) {

int len=end-start;
char*r=(char*)malloc((len+1)*sizeof(char));
memset(r,0,sizeof(r));
for(int i=0;i<len;++i){
r[i]=this->value[start+i];
}
return r;
}
void replaceString( Object_String *this, char *  target, char *  text) {

int p=this->indexOfString(this,target);
if(p<0){
return;
}
if(strlen(target)<strlen(text)){
this->reallocString(this,strlen(text)-strlen(target)+this->length);
}
int len=strlen(text);
for(int i=0;i<len;++i){
this->value[p+i]=text[i];
}
this->value[this->length-1]=0;
}
void toLowerCaseString( Object_String *this) {

for(int i=0;i<this->length;++i){
int val=this->value[i];
if(val>64&&val<91){
this->value[i]=val+32;
}
}
}
void toUpperCaseString( Object_String *this) {

for(int i=0;i<this->length;++i){
int val=this->value[i];
if(val>96&&val<123){
this->value[i]=val-32;
}
}
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
helloWorld->replaceString(helloWorld,"me","bladerunner");
helloWorld->printlnString(helloWorld);
helloWorld->toUpperCaseString(helloWorld);
helloWorld->printlnString(helloWorld);
helloWorld->toLowerCaseString(helloWorld);
helloWorld->printlnString(helloWorld);

return 0 ;
}