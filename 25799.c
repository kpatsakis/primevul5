static void print_service_rules_existence(HttpResponse res, Service_T s) {
        for (Exist_T l = s->existlist; l; l = l->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Non-Existence</td><td>");
                Util_printRule(res->outputbuffer, l->action, "If exist");
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}