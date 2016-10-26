#ifndef __NET_SNMP__
#define __NEY_SNMP__

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#define GET_BULK_SIZE 10

typedef struct net_snmp_s net_snmp_t;

struct net_snmp_s {
    netsnmp_session session, *ss;
    netsnmp_pdu *response;
    void (*on_response)(net_snmp_t *net_snmp);
};

void net_snmp_init(net_snmp_t *net_snmp, char *agent_name, char *community_string);
void net_snmp_open(net_snmp_t *net_snmp);
void net_snmp_get(net_snmp_t *net_snmp, char *oid);
void net_snmp_get_next(net_snmp_t *net_snmp, char *oid_value);
void net_snmp_get_bulk(net_snmp_t *net_snmp, char *oid_value);
void net_snmp_set(net_snmp_t *net_snmp, char *oid_value, char *value, char *type);
void net_snmp_close(net_snmp_t *net_snmp);
#endif
