#ifndef ACE_FIREWALL_H
#define ACE_FIREWALL_H

#include "misc/defs.h"
#include "protocol/request.h"

typedef struct FW_RULE_T
{
    char host[255 + 1];
    char port[2 + 1];
} fw_rule_t;

bool fw_init();
void fw_add(const fw_rule_t *);
bool fw_find(const fw_rule_t *);

#endif 
