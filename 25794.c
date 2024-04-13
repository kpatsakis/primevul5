static void do_home_filesystem(HttpResponse res) {
        char buf[STRLEN];
        boolean_t on = true;
        boolean_t header = true;

        for (Service_T s = servicelist_conf; s; s = s->next_conf) {
                if (s->type != Service_Filesystem)
                        continue;
                if (header) {
                        StringBuffer_append(res->outputbuffer,
                                            "<table id='header-row'>"
                                            "<tr>"
                                            "<th class='left first'>Filesystem</th>"
                                            "<th class='left'>Status</th>"
                                            "<th class='right'>Space usage</th>"
                                            "<th class='right'>Inodes usage</th>"
                                            "<th class='right column'>Read</th>"
                                            "<th class='right column'>Write</th>"
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
                        StringBuffer_append(res->outputbuffer,
                                            "<td class='right'>- [-]</td>"
                                            "<td class='right'>- [-]</td>"
                                            "<td class='right column'>- [-]</td>"
                                            "<td class='right column'>- [-]</td>");
                } else {
                        StringBuffer_append(res->outputbuffer,
                                            "<td class='right column%s'>%.1f%% [%s]</td>",
                                            (s->error & Event_Resource) ? " red-text" : "",
                                            s->inf.filesystem->space_percent,
                                            s->inf.filesystem->f_bsize > 0 ? Fmt_bytes2str(s->inf.filesystem->f_blocksused * s->inf.filesystem->f_bsize, buf) : "0 MB");
                        if (s->inf.filesystem->f_files > 0) {
                                StringBuffer_append(res->outputbuffer,
                                                    "<td class='right column%s'>%.1f%% [%lld objects]</td>",
                                                    (s->error & Event_Resource) ? " red-text" : "",
                                                    s->inf.filesystem->inode_percent,
                                                    s->inf.filesystem->f_filesused);
                        } else {
                                StringBuffer_append(res->outputbuffer,
                                                    "<td class='right column'>not supported by filesystem</td>");
                        }
                        StringBuffer_append(res->outputbuffer,
                                            "<td class='right column%s'>%s/s</td>"
                                            "<td class='right column%s'>%s/s</td>",
                                            (s->error & Event_Resource) ? " red-text" : "",
                                            Fmt_bytes2str(Statistics_deltaNormalize(&(s->inf.filesystem->read.bytes)), (char[10]){}),
                                            (s->error & Event_Resource) ? " red-text" : "",
                                            Fmt_bytes2str(Statistics_deltaNormalize(&(s->inf.filesystem->write.bytes)), (char[10]){}));
                }
                StringBuffer_append(res->outputbuffer, "</tr>");
                on = ! on;
        }
        if (! header)
                StringBuffer_append(res->outputbuffer, "</table>");
}