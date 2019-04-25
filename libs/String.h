typedef struct Class_String Class_String;
typedef struct Object_String Object_String;
Class_String *Class_String_Instance;
struct Class_String {
	Object_String*(*initString)(char * text);
};
struct Object_String {
	uint16_t* content;
	int length;
	int size;
	void(*setContentString)(Object_String *this, char * text);
	void(*printlnString)(Object_String *this);
};
void startString();
