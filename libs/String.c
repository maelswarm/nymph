#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<ctype.h>

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
char* appendString( Object_String *this, char *  text);
char* prependString( Object_String *this, char *  text);
void trimString( Object_String *this);
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
	this->appendString = &appendString;
	this->prependString = &prependString;
	this->trimString = &trimString;
	this->value = NULL;
	this->length = 0;

this->value=(char*)malloc((sizeof(text)+1)*sizeof(char));
memset(this->value,0,sizeof(this->value));
this->length=sizeof(text);
this->valueString(this,text);
return this;
}
void reallocString( Object_String *this, int  size) {

char*new=(char*)malloc(size);
memset(new,0,sizeof(new));
for(int i=0;i<this->length;++i){
new[i]=this->value[i];
}
this->length=size;
free(this->value);
this->value=new;
}
void valueString( Object_String *this, char *  text) {

if(strlen(text)>=this->length){
this->reallocString(this,strlen(text));
}
strncpy(this->value,text,this->length);
}
void printlnString( Object_String *this) {

int len=strlen(this->value);
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
char* appendString( Object_String *this, char *  text) {

int start=strlen(this->value);
int len=strlen(text);
this->reallocString(this,len+this->length);
for(int i=0;i<len;++i){
this->value[start+i]=text[i];
}
}
char* prependString( Object_String *this, char *  text) {

int len=strlen(text);
char*new=(char*)malloc(len+this->length);
memset(new,0,sizeof(new));
for(int i=0;i<len;++i){
new[i]=text[i];
}
for(int i=0;i<strlen(this->value);++i){
new[i+len]=this->value[i];
}
free(this->value);
this->value=new;
}
void trimString( Object_String *this) {

char*tmp1=this->value;
while(isspace(tmp1[0])!=0&&tmp1[0]!=0){
++tmp1;
}
char*tmp2=tmp1+1;
while(isspace(tmp2[0])==0||tmp2[0]==0){
++tmp2;
}
if(tmp2-tmp1<=0){
return;
}
char*new=(char*)malloc(tmp2-tmp1+1);
strncpy(new,tmp1,(int)(tmp2-tmp1));
free(this->value);
this->value=new;
}
void startString() {
	if(Class_String_Instance == NULL) {
		Class_String_Instance = malloc(sizeof(Class_String_Instance));
		Class_String_Instance->initString = &initString;
	}
}








int main (void ){
	startString();


Object_String*helloWorld =Class_String_Instance->initString("Hello World");
helloWorld->printlnString(helloWorld);
helloWorld->valueString(helloWorld,"This is me!");
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
helloWorld->appendString(helloWorld,"blahblahblah    b");
helloWorld->printlnString(helloWorld);
helloWorld->prependString(helloWorld,"    blahblahblah ");
helloWorld->printlnString(helloWorld);
helloWorld->trimString(helloWorld);
helloWorld->printlnString(helloWorld);

return 0 ;
}