
#ifndef ACE_SERVER_H
#define ACE_SERVER_H

#include <stddef.h>
#include "protocol/methods/userpass.h"
#include "protocol/method_exchange.h"
#include "misc/defs.h"

/* Main function that launches the server */
void serve();

/* Handles a client connection */
/* Used in separate pthread */
static void* handle_client(void* client);

/* Execute method-specific function depending on method */
static bool handle_method(fd_t client);

size_t get_total_connnections();
void inc_total_connections();
void dec_total_connections();



#endif 
