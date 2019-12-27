typedef struct Class_JSON_Entity Class_JSON_Entity;
typedef struct Object_JSON_Entity Object_JSON_Entity;
Class_JSON_Entity *Class_JSON_Entity_Instance;
struct Class_JSON_Entity {
	Object_JSON_Entity*(*initJSON_Entity)();
};
struct Object_JSON_Entity {
	void* parent;
	void* keys;
	void* values;
};
typedef struct Class_JSON Class_JSON;
typedef struct Object_JSON Object_JSON;
Class_JSON *Class_JSON_Instance;
struct Class_JSON {
	Object_JSON_Entity*(*_create_entityJSON)(void * datatype,void * value);
	void(*parseJSON)(char * str);
	char**(*_tokenizeJSON)(char * str);
};
struct Object_JSON {
};
void startJSON_Entity();
void startJSON();
