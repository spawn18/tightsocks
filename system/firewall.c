#include "firewall.h"

#include "misc/defs.h"

#include <stdlib.h>
#include <string.h>

static int rulesLen;
static fw_rule_t* fw_rules = NULL;


bool fw_init()
{
    if(fw_rules == NULL)
    {
        rulesLen = 10;
        fw_rules = malloc(sizeof(fw_rule_t) * rulesLen);
        return TRUE;
    }

    return FALSE;
}


void fw_add(const fw_rule_t *rule)
{
    static int i = 0;

    if(i == rulesLen)
    {
        rulesLen *= 2;
        fw_rules = realloc(fw_rules, rulesLen);
    }

    fw_rules[i] = *rule;
    i++;
}

bool fw_find(const fw_rule_t *rule)
{
    for(int i = 0; i < rulesLen; i++)
    {
        if( (strcmp(fw_rules[i].host, rule->host) == 0 || strcmp(fw_rules[i].host, "*") == 0) &&
            (strcmp(fw_rules[i].port, rule->port) == 0 || strcmp(fw_rules[i].port, "*") == 0))
        {
            return TRUE;
        }
    }

    return FALSE;
}
