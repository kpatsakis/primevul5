static void do_home_system(HttpResponse res) {
        Service_T s = Run.system;
        char buf[STRLEN];

        StringBuffer_append(res->outputbuffer,
                            "<table id='header-row'>"
                            "<tr>"
                            "<th class='left first'>System</th>"
                            "<th class='left'>Status</th>"
                            "<th class='right column'>Load</th>"
                            "<th class='right column'>CPU</th>"
                            "<th class='right column'>Memory</th>"
                            "<th class='right column'>Swap</th>"
                            "</tr>"
                            "<tr class='stripe'>"
                            "<td class='left'><a href='%s'>%s</a></td>"
                            "<td class='left'>%s</td>"
                            "<td class='right column'>[%.2f]&nbsp;[%.2f]&nbsp;[%.2f]</td>"
                            "<td class='right column'>"
                            "%.1f%%us,&nbsp;%.1f%%sy"
#ifdef HAVE_CPU_WAIT
                            ",&nbsp;%.1f%%wa"
#endif
                            "</td>",
                            s->name, s->name,
                            get_service_status(HTML, s, buf, sizeof(buf)),
                            systeminfo.loadavg[0], systeminfo.loadavg[1], systeminfo.loadavg[2],
                            systeminfo.cpu.usage.user > 0. ? systeminfo.cpu.usage.user : 0.,
                            systeminfo.cpu.usage.system > 0. ? systeminfo.cpu.usage.system : 0.
#ifdef HAVE_CPU_WAIT
                            , systeminfo.cpu.usage.wait > 0. ? systeminfo.cpu.usage.wait : 0.
#endif
                            );
        StringBuffer_append(res->outputbuffer,
                            "<td class='right column'>%.1f%% [%s]</td>",
                            systeminfo.memory.usage.percent, Fmt_bytes2str(systeminfo.memory.usage.bytes, buf));
        StringBuffer_append(res->outputbuffer,
                            "<td class='right column'>%.1f%% [%s]</td>",
                            systeminfo.swap.usage.percent, Fmt_bytes2str(systeminfo.swap.usage.bytes, buf));
        StringBuffer_append(res->outputbuffer,
                            "</tr>"
                            "</table>");
}