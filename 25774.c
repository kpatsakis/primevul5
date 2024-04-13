static void do_home_directory(HttpResponse res) {
        char buf[STRLEN];
        boolean_t on = true;
        boolean_t header = true;

        for (Service_T s = servicelist_conf; s; s = s->next_conf) {
                if (s->type != Service_Directory)
                        continue;
                if (header) {
                        StringBuffer_append(res->outputbuffer,
                                            "<table id='header-row'>"
                                            "<tr>"
                                            "<th class='left first'>Directory</th>"
                                            "<th class='left'>Status</th>"
                                            "<th class='right'>Permission</th>"
                                            "<th class='right'>UID</th>"
                                            "<th class='right'>GID</th>"
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
                if (! Util_hasServiceStatus(s) || s->inf.directory->mode < 0)
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                else
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%04o</td>", s->inf.directory->mode & 07777);
                if (! Util_hasServiceStatus(s) || s->inf.directory->uid < 0)
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                else
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%d</td>", s->inf.directory->uid);
                if (! Util_hasServiceStatus(s) || s->inf.directory->gid < 0)
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                else
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%d</td>", s->inf.directory->gid);
                StringBuffer_append(res->outputbuffer, "</tr>");
                on = ! on;
        }
        if (! header)
                StringBuffer_append(res->outputbuffer, "</table>");
}