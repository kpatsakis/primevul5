static void print_service_rules_timestamp(HttpResponse res, Service_T s) {
        for (Timestamp_T t = s->timestamplist; t; t = t->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>%c%s</td><td>", toupper(timestampnames[t->type][0]), timestampnames[t->type] + 1);
                if (t->test_changes)
                        Util_printRule(res->outputbuffer, t->action, "If changed");
                else
                        Util_printRule(res->outputbuffer, t->action, "If %s %s", operatornames[t->operator], Fmt_time2str(t->time * 1000., (char[11]){}));
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}