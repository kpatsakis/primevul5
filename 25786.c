static void print_service_rules_port(HttpResponse res, Service_T s) {
        for (Port_T p = s->portlist; p; p = p->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Port</td><td>");
                StringBuffer_T buf = StringBuffer_create(64);
                StringBuffer_append(buf, "If failed [%s]:%d%s",
                        p->hostname, p->target.net.port, Util_portRequestDescription(p));
                if (p->outgoing.ip)
                        StringBuffer_append(buf, " via address %s", p->outgoing.ip);
                StringBuffer_append(buf, " type %s/%s protocol %s with timeout %s",
                        Util_portTypeDescription(p), Util_portIpDescription(p), p->protocol->name, Fmt_time2str(p->timeout, (char[11]){}));
                if (p->retry > 1)
                        StringBuffer_append(buf, " and retry %d times", p->retry);
#ifdef HAVE_OPENSSL
                if (p->target.net.ssl.options.flags) {
                        StringBuffer_append(buf, " using TLS");
                        const char *options = Ssl_printOptions(&p->target.net.ssl.options, (char[STRLEN]){}, STRLEN);
                        if (options && *options)
                                StringBuffer_append(buf, " with options {%s}", options);
                        if (p->target.net.ssl.certificate.minimumDays > 0)
                                StringBuffer_append(buf, " and certificate valid for at least %d days", p->target.net.ssl.certificate.minimumDays);
                        if (p->target.net.ssl.options.checksum)
                                StringBuffer_append(buf, " and certificate checksum %s equal to '%s'", checksumnames[p->target.net.ssl.options.checksumType], p->target.net.ssl.options.checksum);
                }
#endif
                Util_printRule(res->outputbuffer, p->action, "%s", StringBuffer_toString(buf));
                StringBuffer_free(&buf);
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}