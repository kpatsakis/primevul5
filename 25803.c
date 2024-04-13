static void print_service_rules_nonexistence(HttpResponse res, Service_T s) {
        for (NonExist_T l = s->nonexistlist; l; l = l->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Existence</td><td>");
                Util_printRule(res->outputbuffer, l->action, "If doesn't exist");
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}