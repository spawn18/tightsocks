#ifndef ACE_FIREWALL_H
#define ACE_FIREWALL_H

#include "misc/defs.h"
#include "protocol/request.h"

#define FW_CMD_LEN 14
#define FW_HOST_LEN 255


typedef struct FW_RULE_T
{
    cmd_t cmd;
    char host[FW_HOST_LEN + 1];
} fw_rule_t;

bool fw_init();
void fw_add(const fw_rule_t *);
bool fw_find(const fw_rule_t *);

#endif 
