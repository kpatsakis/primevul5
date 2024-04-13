static void print_service_rules_uploadbytes(HttpResponse res, Service_T s) {
        for (Bandwidth_T bl = s->uploadbyteslist; bl; bl = bl->next) {
                if (bl->range == Time_Second) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Upload bytes</td><td>");
                        Util_printRule(res->outputbuffer, bl->action, "If %s %s/s", operatornames[bl->operator], Fmt_bytes2str(bl->limit, (char[10]){}));
                } else {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Total upload bytes</td><td>");
                        Util_printRule(res->outputbuffer, bl->action, "If %s %s in last %d %s(s)", operatornames[bl->operator], Fmt_bytes2str(bl->limit, (char[10]){}), bl->rangecount, Util_timestr(bl->range));
                }
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}