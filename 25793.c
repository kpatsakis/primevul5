static void do_home_process(HttpResponse res) {
        char      buf[STRLEN];
        boolean_t on = true;
        boolean_t header = true;

        for (Service_T s = servicelist_conf; s; s = s->next_conf) {
                if (s->type != Service_Process)
                        continue;
                if (header) {
                        StringBuffer_append(res->outputbuffer,
                                            "<table id='header-row'>"
                                            "<tr>"
                                            "<th class='left' class='first'>Process</th>"
                                            "<th class='left'>Status</th>"
                                            "<th class='right'>Uptime</th>"
                                            "<th class='right'>CPU Total</b></th>"
                                            "<th class='right'>Memory Total</th>"
                                            "<th class='right column'>Read</th>"
                                            "<th class='right column'>Write</th>"
                                            "</tr>");
                        header = false;
                }
                StringBuffer_append(res->outputbuffer,
                                    "<tr%s>"
                                    "<td class='left'><a href='%s'>%s</a></td>"
                                    "<td class='left'>%s</td>",
                                    on ? " class='stripe'" : "",
                                    s->name, s->name,
                                    get_service_status(HTML, s, buf, sizeof(buf)));
                if (! (Run.flags & Run_ProcessEngineEnabled) || ! Util_hasServiceStatus(s) || s->inf.process->uptime < 0) {
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                } else {
                        StringBuffer_append(res->outputbuffer, "<td class='right'>%s</td>", _getUptime(s->inf.process->uptime, (char[256]){}));
                }
                if (! (Run.flags & Run_ProcessEngineEnabled) || ! Util_hasServiceStatus(s) || s->inf.process->total_cpu_percent < 0) {
                                StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                } else {
                        StringBuffer_append(res->outputbuffer, "<td class='right%s'>%.1f%%</td>", (s->error & Event_Resource) ? " red-text" : "", s->inf.process->total_cpu_percent);
                }
                if (! (Run.flags & Run_ProcessEngineEnabled) || ! Util_hasServiceStatus(s) || s->inf.process->total_mem_percent < 0) {
                        StringBuffer_append(res->outputbuffer, "<td class='right'>-</td>");
                } else {
                        StringBuffer_append(res->outputbuffer, "<td class='right%s'>%.1f%% [%s]</td>", (s->error & Event_Resource) ? " red-text" : "", s->inf.process->total_mem_percent, Fmt_bytes2str(s->inf.process->total_mem, buf));
                }
                boolean_t hasReadBytes = Statistics_initialized(&(s->inf.process->read.bytes));
                boolean_t hasReadOperations = Statistics_initialized(&(s->inf.process->read.operations));
                if (! (Run.flags & Run_ProcessEngineEnabled) || ! Util_hasServiceStatus(s) || (! hasReadBytes && ! hasReadOperations)) {
                        StringBuffer_append(res->outputbuffer, "<td class='right column'>-</td>");
                } else if (hasReadBytes) {
                        StringBuffer_append(res->outputbuffer, "<td class='right column%s'>%s/s</td>", (s->error & Event_Resource) ? " red-text" : "", Fmt_bytes2str(Statistics_deltaNormalize(&(s->inf.process->read.bytes)), (char[10]){}));
                } else if (hasReadOperations) {
                        StringBuffer_append(res->outputbuffer, "<td class='right column%s'>%.1f/s</td>", (s->error & Event_Resource) ? " red-text" : "", Statistics_deltaNormalize(&(s->inf.process->read.operations)));
                }
                boolean_t hasWriteBytes = Statistics_initialized(&(s->inf.process->write.bytes));
                boolean_t hasWriteOperations = Statistics_initialized(&(s->inf.process->write.operations));
                if (! (Run.flags & Run_ProcessEngineEnabled) || ! Util_hasServiceStatus(s) || (! hasWriteBytes && ! hasWriteOperations)) {
                        StringBuffer_append(res->outputbuffer, "<td class='right column'>-</td>");
                } else if (hasWriteBytes) {
                        StringBuffer_append(res->outputbuffer, "<td class='right column%s'>%s/s</td>", (s->error & Event_Resource) ? " red-text" : "", Fmt_bytes2str(Statistics_deltaNormalize(&(s->inf.process->write.bytes)), (char[10]){}));
                } else if (hasWriteOperations) {
                        StringBuffer_append(res->outputbuffer, "<td class='right column%s'>%.1f/s</td>", (s->error & Event_Resource) ? " red-text" : "", Statistics_deltaNormalize(&(s->inf.process->write.operations)));
                }
                StringBuffer_append(res->outputbuffer, "</tr>");
                on = ! on;
        }
        if (! header)
                StringBuffer_append(res->outputbuffer, "</table>");
}