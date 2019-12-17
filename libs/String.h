typedef struct Class_String Class_String;
typedef struct Object_String Object_String;
Class_String *Class_String_Instance;
struct Class_String {
	Object_String*(*initString)(char * text);
};
struct Object_String {
	char* value;
	int size;
	void(*reallocString)(Object_String *this, int size);
	void(*valueString)(Object_String *this, char * text);
	void(*printlnString)(Object_String *this);
	int(*indexOfString)(Object_String *this, char * text);
	int(*lastIndexOfString)(Object_String *this, char * text);
	char*(*sliceString)(Object_String *this, int start, int end);
	void(*replaceString)(Object_String *this, char * target, char * text);
	void(*toLowerCaseString)(Object_String *this);
	void(*toUpperCaseString)(Object_String *this);
	char*(*appendString)(Object_String *this, char * text);
	char*(*prependString)(Object_String *this, char * text);
	void(*trimString)(Object_String *this);
};
void startString();
