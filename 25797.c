static void print_service_rules_fsflags(HttpResponse res, Service_T s) {
        for (FsFlag_T l = s->fsflaglist; l; l = l->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Filesystem flags</td><td>");
                Util_printRule(res->outputbuffer, l->action, "If changed");
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}