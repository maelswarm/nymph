typedef struct Class_String Class_String;
typedef struct Object_String Object_String;
Class_String *Class_String_Instance;
struct Class_String {
	Object_String*(*initString)(char * text);
};
struct Object_String {
	char* content;
	int length;
	int size;
	void(*reallocString)(Object_String *this, int size);
	void(*valueString)(Object_String *this, char * text);
	void(*printlnString)(Object_String *this);
	int(*indexOfString)(Object_String *this, char * text);
	int(*lastIndexOfString)(Object_String *this, char * text);
};
void startString();
