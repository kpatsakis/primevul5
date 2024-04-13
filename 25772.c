static void do_home_file(HttpResponse res) {
        char buf[STRLEN];
        boolean_t on = true;
        boolean_t header = true;

        for (Service_T s = servicelist_conf; s; s = s->next_conf) {
                if (s->type != Service_File)
                        continue;
                if (header) {
                        StringBuffer_append(res->outputbuffer,
                                            "<table id='header-row'>"
                                            "<tr>"
                                            "<th class='left first'>File</th>"
                                            "<th class='left'>Status</th>"
                                            "<th class='right'>Size</th>"
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
                if (! Util_hasServiceStatus(s) || s->inf.file->size < 0)
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                else
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%s</td>", Fmt_bytes2str(s->inf.file->size, (char[10]){}));
                if (! Util_hasServiceStatus(s) || s->inf.file->mode < 0)
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                else
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%04o</td>", s->inf.file->mode & 07777);
                if (! Util_hasServiceStatus(s) || s->inf.file->uid < 0)
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                else
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%d</td>", s->inf.file->uid);
                if (! Util_hasServiceStatus(s) || s->inf.file->gid < 0)
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                else
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%d</td>", s->inf.file->gid);
                StringBuffer_append(res->outputbuffer, "</tr>");
                on = ! on;
        }
        if (! header)
                StringBuffer_append(res->outputbuffer, "</table>");
}