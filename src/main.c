#include "net-snmp.h"

void on_response(net_snmp_t *net_snmp) {
    netsnmp_variable_list *vars;
    int count;

    for(vars = net_snmp->response->variables; vars; vars = vars->next_variable)
        print_variable(vars->name, vars->name_length, vars);
}

int main() {
    net_snmp_t snmp_get, snmp_set;

    snmp_get.on_response = &on_response;
    snmp_set.on_response = &on_response;

    net_snmp_init(&snmp_get, "localhost", "public");
    net_snmp_init(&snmp_set, "localhost", "private");

    net_snmp_open(&snmp_get);
    net_snmp_open(&snmp_set);

    net_snmp_get(&snmp_get, "SNMPv2-MIB::sysName.0");
    net_snmp_set(&snmp_set, "SNMPv2-MIB::sysName.0", "ubuntu5", "s");
    net_snmp_get(&snmp_get, "SNMPv2-MIB::sysName.0");
    net_snmp_get_bulk(&snmp_get, "IF-MIB::ifPhysAddress");

    net_snmp_close(&snmp_get);
    net_snmp_close(&snmp_set);
}
