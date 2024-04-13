static void print_service_rules_secattr(HttpResponse res, Service_T s) {
        for (SecurityAttribute_T a = s->secattrlist; a; a = a->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Security attribute</td><td>");
                Util_printRule(res->outputbuffer, a->action, "If failed %s", a->attribute);
                StringBuffer_append(res->outputbuffer, "</td></tr>");
         }
}