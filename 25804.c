static void do_home_program(HttpResponse res) {
        char buf[STRLEN];
        boolean_t on = true;
        boolean_t header = true;

        for (Service_T s = servicelist_conf; s; s = s->next_conf) {
                if (s->type != Service_Program)
                        continue;
                if (header) {
                        StringBuffer_append(res->outputbuffer,
                                            "<table id='header-row'>"
                                            "<tr>"
                                            "<th class='left' class='first'>Program</th>"
                                            "<th class='left'>Status</th>"
                                            "<th class='left'>Output</th>"
                                            "<th class='right'>Last started</th>"
                                            "<th class='right'>Exit value</th>"
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
                        StringBuffer_append(res->outputbuffer, "<td class='left'>-</td>");
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                } else {
                        if (s->program->started) {
                                StringBuffer_append(res->outputbuffer, "<td class='left short'>");
                                if (StringBuffer_length(s->program->lastOutput)) {
                                        // Print first line only (escape HTML characters if any)
                                        const char *output = StringBuffer_toString(s->program->lastOutput);
                                        for (int i = 0; output[i]; i++) {
                                                if (output[i] == '<')
                                                        StringBuffer_append(res->outputbuffer, "&lt;");
                                                else if (output[i] == '>')
                                                        StringBuffer_append(res->outputbuffer, "&gt;");
                                                else if (output[i] == '&')
                                                        StringBuffer_append(res->outputbuffer, "&amp;");
                                                else if (output[i] == '\r' || output[i] == '\n')
                                                        break;
                                                else
                                                        StringBuffer_append(res->outputbuffer, "%c", output[i]);
                                        }
                                } else {
                                        StringBuffer_append(res->outputbuffer, "no output");
                                }
                                StringBuffer_append(res->outputbuffer, "</td>");
                                StringBuffer_append(res->outputbuffer, "<td class='right'>%s</td>", Time_fmt((char[32]){}, 32, "%d %b %Y %H:%M:%S", s->program->started));
                                StringBuffer_append(res->outputbuffer, "<td class='right'>%d</td>", s->program->exitStatus);
                        } else {
                                StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                                StringBuffer_append(res->outputbuffer, "<td class='right'>Not yet started</td>");
                                StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                        }
                }
                StringBuffer_append(res->outputbuffer, "</tr>");
                on = ! on;
        }
        if (! header)
                StringBuffer_append(res->outputbuffer, "</table>");

}