static void do_home_net(HttpResponse res) {
        char buf[STRLEN];
        boolean_t on = true;
        boolean_t header = true;

        for (Service_T s = servicelist_conf; s; s = s->next_conf) {
                if (s->type != Service_Net)
                        continue;
                if (header) {
                        StringBuffer_append(res->outputbuffer,
                                            "<table id='header-row'>"
                                            "<tr>"
                                            "<th class='left first'>Net</th>"
                                            "<th class='left'>Status</th>"
                                            "<th class='right'>Upload</th>"
                                            "<th class='right'>Download</th>"
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
                if (! Util_hasServiceStatus(s) || Link_getState(s->inf.net->stats) != 1) {
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                } else {
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%s&#47;s</td>", Fmt_bytes2str(Link_getBytesOutPerSecond(s->inf.net->stats), buf));
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%s&#47;s</td>", Fmt_bytes2str(Link_getBytesInPerSecond(s->inf.net->stats), buf));
                }
                StringBuffer_append(res->outputbuffer, "</tr>");
                on = ! on;
        }
        if (! header)
                StringBuffer_append(res->outputbuffer, "</table>");
}