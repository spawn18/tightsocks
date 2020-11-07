#include "firewall.h"

#include <string.h>

static int i = 0;
fw_rule_t fw_rules[100];

void fw_add(const fw_rule_t* rule)
{
    if(i == 100) return;

    fw_rules[i] = *rule;
    i++;
}

bool fw_find(const fw_rule_t* rule)
{
    for(int j = 0; j < 100; j++)
    {
        if(strcmp(rule->host, fw_rules[i].host) == 0 && strcmp(rule->host, fw_rules[i].port) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}