#include "firewall.h"

#include <string.h>


fw_rule_t fw_rules[100];

void fw_add(const fw_rule_t* rule)
{
    static int i = 0;
    if(i == 100) return;

    fw_rules[i] = *rule;
    i++;
}

bool fw_find(const fw_rule_t* rule)
{
    for(int i = 0; i < 100; i++)
    {
        if(strcmp(rule->host, fw_rules[i].host) == 0 && strcmp(rule->host, fw_rules[i].port) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}