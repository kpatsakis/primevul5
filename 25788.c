static void print_service_rules_program(HttpResponse res, Service_T s) {
        if (s->type == Service_Program) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Program timeout</td><td>Terminate the program if not finished within %s</td></tr>", Fmt_time2str(s->program->timeout, (char[11]){}));
                for (Status_T status = s->statuslist; status; status = status->next) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Test Exit value</td><td>");
                        if (status->operator == Operator_Changed)
                                Util_printRule(res->outputbuffer, status->action, "If exit value changed");
                        else
                                Util_printRule(res->outputbuffer, status->action, "If exit value %s %d", operatorshortnames[status->operator], status->return_value);
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                }
        }
}