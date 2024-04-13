static void do_home_host(HttpResponse res) {
        char buf[STRLEN];
        boolean_t on = true;
        boolean_t header = true;

        for (Service_T s = servicelist_conf; s; s = s->next_conf) {
                if (s->type != Service_Host)
                        continue;
                if (header) {
                        StringBuffer_append(res->outputbuffer,
                                            "<table id='header-row'>"
                                            "<tr>"
                                            "<th class='left first'>Host</th>"
                                            "<th class='left'>Status</th>"
                                            "<th class='right'>Protocol(s)</th>"
                                            "</tr>");
                        header = false;
                }
                StringBuffer_append(res->outputbuffer,
                                    "<tr %s>"
                                    "<td class='left'><a href='%s'>%s</a></td>"
                                    "<td class='left'>%s</td>",
                                    on ? "class='stripe'" : "",
                                    s->name, s->name,
                                    get_service_status(HTML, s, buf, sizeof(buf)));
                if (! Util_hasServiceStatus(s)) {
                        StringBuffer_append(res->outputbuffer,
                                            "<td class='right'>-</td>");
                } else {
                        StringBuffer_append(res->outputbuffer,
                                            "<td class='right'>");
                        for (Icmp_T icmp = s->icmplist; icmp; icmp = icmp->next) {
                                if (icmp != s->icmplist)
                                        StringBuffer_append(res->outputbuffer, "&nbsp;&nbsp;<b>|</b>&nbsp;&nbsp;");
                                switch (icmp->is_available) {
                                        case Connection_Init:
                                                StringBuffer_append(res->outputbuffer, "<span class='gray-text'>[Ping]</span>");
                                                break;
                                        case Connection_Failed:
                                                StringBuffer_append(res->outputbuffer, "<span class='red-text'>[Ping]</span>");
                                                break;
                                        default:
                                                StringBuffer_append(res->outputbuffer, "<span>[Ping]</span>");
                                                break;
                                }
                        }
                        if (s->icmplist && s->portlist)
                                StringBuffer_append(res->outputbuffer, "&nbsp;&nbsp;<b>|</b>&nbsp;&nbsp;");
                        for (Port_T port = s->portlist; port; port = port->next) {
                                if (port != s->portlist)
                                        StringBuffer_append(res->outputbuffer, "&nbsp;&nbsp;<b>|</b>&nbsp;&nbsp;");
                                switch (port->is_available) {
                                        case Connection_Init:
                                                StringBuffer_append(res->outputbuffer, "<span class='gray-text'>[%s] at port %d</span>", port->protocol->name, port->target.net.port);
                                                break;
                                        case Connection_Failed:
                                                StringBuffer_append(res->outputbuffer, "<span class='red-text'>[%s] at port %d</span>", port->protocol->name, port->target.net.port);
                                                break;
                                        default:
                                                if (port->target.net.ssl.options.flags && port->target.net.ssl.certificate.validDays < port->target.net.ssl.certificate.minimumDays)
                                                        StringBuffer_append(res->outputbuffer, "<span class='red-text'>[%s] at port %d</span>", port->protocol->name, port->target.net.port);
                                                else
                                                        StringBuffer_append(res->outputbuffer, "<span>[%s] at port %d</span>", port->protocol->name, port->target.net.port);
                                                break;
                                }
                        }
                        StringBuffer_append(res->outputbuffer, "</td>");
                }
                StringBuffer_append(res->outputbuffer, "</tr>");
                on = ! on;
        }
        if (! header)
                StringBuffer_append(res->outputbuffer, "</table>");
}