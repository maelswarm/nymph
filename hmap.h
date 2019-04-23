typedef struct HMap HMap;

HMap **create_hm(void);

void update_hm(HMap **, char *, void *);

void *fetch_hm(HMap **, char *);

void remove_hm(HMap **, char *);

void free_hm(HMap **);
