#ifndef ACE_FIREWALL_H
#define ACE_FIREWALL_H

typedef enum FW_RULE_CMD_T
{
    CONNECT,
    BIND,
    UDP_ASSOCIATE
} fw_rule_cmd_t;

typedef struct FW_RULE_T
{
    fw_rule_cmd_t cmd;
    char* host;
} fw_rule_t;

#endif 
