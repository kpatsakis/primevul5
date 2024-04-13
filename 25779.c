static void print_service_rules_socket(HttpResponse res, Service_T s) {
        for (Port_T p = s->socketlist; p; p = p->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Unix Socket</td><td>");
                if (p->retry > 1)
                        Util_printRule(res->outputbuffer, p->action, "If failed %s type %s protocol %s with timeout %s and retry %d time(s)", p->target.unix.pathname, Util_portTypeDescription(p), p->protocol->name, Fmt_time2str(p->timeout, (char[11]){}), p->retry);
                else
                        Util_printRule(res->outputbuffer, p->action, "If failed %s type %s protocol %s with timeout %s", p->target.unix.pathname, Util_portTypeDescription(p), p->protocol->name, Fmt_time2str(p->timeout, (char[11]){}));
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}