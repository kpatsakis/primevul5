static void print_service_rules_icmp(HttpResponse res, Service_T s) {
        for (Icmp_T i = s->icmplist; i; i = i->next) {
                switch (i->family) {
                        case Socket_Ip4:
                                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Ping4</td><td>");
                                break;
                        case Socket_Ip6:
                                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Ping6</td><td>");
                                break;
                        default:
                                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Ping</td><td>");
                                break;
                }
                Util_printRule(res->outputbuffer, i->action, "If failed [count %d size %d with timeout %s%s%s]", i->count, i->size, Fmt_time2str(i->timeout, (char[11]){}), i->outgoing.ip ? " via address " : "", i->outgoing.ip ? i->outgoing.ip : "");
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}