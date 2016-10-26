#include "net-snmp.h"

void net_snmp_init(net_snmp_t *net_snmp, char *agent_name, char *community_string) {
    init_snmp("net-snmp");

    snmp_sess_init(&net_snmp->session);         
    net_snmp->session.peername = strdup(agent_name);
    net_snmp->session.version = SNMP_VERSION_2c;
    net_snmp->session.community = community_string;
    net_snmp->session.community_len = strlen(net_snmp->session.community);

    net_snmp->response = NULL;
}

void net_snmp_open(net_snmp_t *net_snmp) {
    SOCK_STARTUP;                                                     /* For win32 */
    net_snmp->ss = snmp_open(&net_snmp->session);                     /* establish the session */
    if (!net_snmp->ss) {
        snmp_sess_perror("ack", &net_snmp->session);
        SOCK_CLEANUP;
        exit(1);
    }
}
static void net_snmp_message(net_snmp_t *net_snmp, int pdu_type, char *oid_value, char *value, u_char type) {
    netsnmp_pdu *pdu;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
    char *OID;
    int status;

    if (oid_value == NULL || net_snmp == NULL) {
        fprintf(stdout, "net_snmp_message(): Invalid Agrument\n");
        fflush(stdout);
        exit(1);
    }
    else {
        OID = oid_value;
        /* Protocol data units (PDUs) */
        pdu = snmp_pdu_create(pdu_type);
        anOID_len = MAX_OID_LEN;

        if (!snmp_parse_oid(OID, anOID, &anOID_len)) {
            snmp_perror(OID);
            SOCK_CLEANUP;
            exit(1);
        }

        switch(pdu_type) {
            case SNMP_MSG_SET:
                if (value != NULL) {
                    if (!snmp_pdu_add_variable(pdu, anOID, anOID_len, type, value, strlen(value))) {
                        snmp_perror(OID);
                        SOCK_CLEANUP;
                        exit(1);
                    }
                }
                break;
            case SNMP_MSG_GETBULK:
                pdu->non_repeaters = 0; 
                pdu->max_repetitions = GET_BULK_SIZE;
            case SNMP_MSG_GET:
            case SNMP_MSG_GETNEXT:
                snmp_add_null_var(pdu, anOID, anOID_len);
                break;
        }

        status = snmp_synch_response(net_snmp->ss, pdu, &net_snmp->response);

        if (status == STAT_SUCCESS && net_snmp->response->errstat == SNMP_ERR_NOERROR) {
            /* SUCCESS: Print the result variables */
            if (net_snmp->on_response != NULL) {
                net_snmp->on_response(net_snmp);
            }
            else {
                fprintf(stdout, "net_snmp->on_response NULL");
                fflush(stdout);
            }
        } else {
            /* FAILURE: print what went wrong! */
            if (status == STAT_SUCCESS)
                fprintf(stderr, "Error in packet\nReason: %s\n", snmp_errstring(net_snmp->response->errstat));
            else if (status == STAT_TIMEOUT)
                fprintf(stderr, "Timeout: No response from %s.\n", net_snmp->session.peername);
            else
                snmp_sess_perror("net-snmp", net_snmp->ss);
        }
    }
}

void net_snmp_get(net_snmp_t *net_snmp, char *oid_value) {
    net_snmp_message(net_snmp, SNMP_MSG_GET, oid_value, NULL, ASN_NULL);
}
void net_snmp_get_next(net_snmp_t *net_snmp, char *oid_value) {
    net_snmp_message(net_snmp, SNMP_MSG_GETNEXT, oid_value, NULL, ASN_NULL);
}
void net_snmp_get_bulk(net_snmp_t *net_snmp, char *oid_value) {
    net_snmp_message(net_snmp, SNMP_MSG_GETBULK, oid_value, NULL, ASN_NULL);
}
void net_snmp_set(net_snmp_t *net_snmp, char *oid_value, char *value, char *type) {
    u_char msg_type;

    switch(type[0]) {
        case 's':
            msg_type = ASN_OCTET_STR;
            break;
        case 'b':
            msg_type = ASN_BIT_STR;
            break;
        case 'i':
            msg_type = ASN_INTEGER;
            break;
        case 'u':
            msg_type = ASN_UNSIGNED;
            break;
        case 't':
            msg_type = ASN_TIMETICKS;
            break;
        case 'a':
            msg_type = ASN_IPADDRESS;
            break;
        case 'o':
            msg_type = ASN_OBJECT_ID;
            break;
        case 'c':
            msg_type = ASN_COUNTER;
            break;
        case 'C':
            msg_type = ASN_COUNTER64;
            break;
        default:
            msg_type = ASN_NULL;
            break;

    }
    net_snmp_message(net_snmp, SNMP_MSG_SET, oid_value, value, msg_type);
}
void net_snmp_close(net_snmp_t *net_snmp) {
    if (net_snmp->response) {
        snmp_free_pdu(net_snmp->response);
    }
    snmp_close(net_snmp->ss);
    SOCK_CLEANUP;
}

