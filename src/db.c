#include <stdlib.h>

#include "../include/db.h"
#include "../include/types.h"

/* PRIVATE */
static socks_client* db;

int db_sort_compare(const void* a, const void* b)
{
    socks_client sa = *(socks_client*)a;
    socks_client sb = *(socks_client*)b;

    if      (sa.client < sb.client)  return -1;
    else if (sa.client > sb.client)  return 1;
    else                             return 0;
}

static void db_sort()
{
    qsort(db, MAX_CLIENTS, sizeof(socks_client), db_sort_compare);
}


/* PUBLIC */
void db_init()
{
    db = malloc(sizeof(socks_client)*MAX_CLIENTS);
}

void db_delete()
{
    free(db);
}

void db_add(socks_client client)
{
    db_sort();
}

void db_remove(fd_t client)
{

}

void db_get(fd_t client)
{

}


