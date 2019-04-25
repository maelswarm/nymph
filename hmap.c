#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "hmap.h"

struct HMap
{
    char *key;
    void *value;
    HMap *next;
};

int hash(char *key)
{
    int hash = 0;
    int len = strlen(key);
    for (int i = 0; i < len; i++)
    {
        hash += ((unsigned int)key[i] * 11);
    }
    return hash %= 10000;
}

HMap **create_hm()
{
    HMap **hashmap = malloc(10000 * sizeof(HMap *));
    for (int i = 0; i < 10000; i++)
    {
        hashmap[i] = NULL;
    }
    return hashmap;
}

void update_hm(HMap **map, char *key, void *value)
{
    int idx = hash(key);
    HMap *row = map[idx];

    if (row == NULL)
    {
        row = malloc(sizeof(HMap));
        row->key = malloc(1000 * sizeof(char));
        row->value = NULL;
        row->next = NULL;
        strcpy(row->key, key);
        row->value = value;
        map[idx] = row;
        return;
    }

    while (row->next != NULL && (strcmp(row->key, key) != 0))
    {
        row = row->next;
    }
    if (strcmp(row->key, key) == 0)
    {
        strcpy(row->key, key);
        row->value = value;
    }
    else
    {
        HMap *node = malloc(sizeof(HMap));
        node->key = malloc(1000 * sizeof(char));
        node->next = NULL;
        strcpy(node->key, key);
        node->value = value;
        row->next = node;
    }
}

void *fetch_hm(HMap **map, char *key)
{
    int idx = hash(key);
    HMap *row = map[idx];
    if (row == NULL)
    {
        return NULL;
    }
    while (row->next != NULL && (strcmp(row->key, key) != 0))
    {
        row = row->next;
    }
    if (strcmp(row->key, key) == 0)
    {
        return row->value;
    }
    return NULL;
}

void remove_hm(HMap **map, char *key)
{
    int idx = hash(key);
    HMap *row = map[idx];
    HMap *prev = NULL;
    while (row->next != NULL && (strcmp(row->key, key) != 0))
    {
        prev = row;
        row = row->next;
    }
    if (prev == NULL)
    {
        HMap *tmp = row->next;
        free(row->key);
        free(row);
        map[idx] = tmp;
    }
    else
    {
        prev->next = row->next;
        free(row->key);
        free(row);
    }
}

void free_hm(HMap **map)
{
    for (int i = 0; i < 10000; i++)
    {
        HMap *row = map[i];
        while (row != NULL)
        {
            if (row->key != NULL)
            {
                free(row->key);
            }
            HMap *tmp = row->next;

            if (row != NULL)
            {
                free(row);
            }
            row = tmp;
        }
    }
    free(map);
}
