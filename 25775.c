static void do_service(HttpRequest req, HttpResponse res, Service_T s) {
        char buf[STRLEN];

        ASSERT(s);

        do_head(res, s->name, s->name, Run.polltime);
        StringBuffer_append(res->outputbuffer,
                            "<h2>%s status</h2>"
                            "<table id='status-table'>"
                            "<tr>"
                            "<th width='30%%'>Parameter</th>"
                            "<th width='70%%'>Value</th>"
                            "</tr>"
                            "<tr>"
                            "<td>Name</td>"
                            "<td>%s</td>"
                            "</tr>",
                            servicetypes[s->type],
                            s->name);
        if (s->type == Service_Process)
                StringBuffer_append(res->outputbuffer, "<tr><td>%s</td><td>%s</td></tr>", s->matchlist ? "Match" : "Pid file", s->path);
        else if (s->type == Service_Host)
                StringBuffer_append(res->outputbuffer, "<tr><td>Address</td><td>%s</td></tr>", s->path);
        else if (s->type == Service_Net)
                StringBuffer_append(res->outputbuffer, "<tr><td>Interface</td><td>%s</td></tr>", s->path);
        else if (s->type != Service_System)
                StringBuffer_append(res->outputbuffer, "<tr><td>Path</td><td>%s</td></tr>", s->path);
        StringBuffer_append(res->outputbuffer, "<tr><td>Status</td><td>%s</td></tr>", get_service_status(HTML, s, buf, sizeof(buf)));
        for (ServiceGroup_T sg = servicegrouplist; sg; sg = sg->next)
                for (list_t m = sg->members->head; m; m = m->next)
                        if (m->e == s)
                                StringBuffer_append(res->outputbuffer, "<tr><td>Group</td><td class='blue-text'>%s</td></tr>", sg->name);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Monitoring status</td><td>%s</td></tr>", get_monitoring_status(HTML, s, buf, sizeof(buf)));
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Monitoring mode</td><td>%s</td></tr>", modenames[s->mode]);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>On reboot</td><td>%s</td></tr>", onrebootnames[s->onreboot]);
        for (Dependant_T d = s->dependantlist; d; d = d->next) {
                if (d->dependant != NULL) {
                        StringBuffer_append(res->outputbuffer,
                                            "<tr><td>Depends on service </td><td> <a href=%s> %s </a></td></tr>",
                                            d->dependant, d->dependant);
                }
        }
        if (s->start) {
                StringBuffer_append(res->outputbuffer, "<tr><td>Start program</td><td>'%s'", Util_commandDescription(s->start, (char[STRLEN]){}));
                if (s->start->has_uid)
                        StringBuffer_append(res->outputbuffer, " as uid %d", s->start->uid);
                if (s->start->has_gid)
                        StringBuffer_append(res->outputbuffer, " as gid %d", s->start->gid);
                StringBuffer_append(res->outputbuffer, " timeout %s", Fmt_time2str(s->start->timeout, (char[11]){}));
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        if (s->stop) {
                StringBuffer_append(res->outputbuffer, "<tr><td>Stop program</td><td>'%s'", Util_commandDescription(s->stop, (char[STRLEN]){}));
                if (s->stop->has_uid)
                        StringBuffer_append(res->outputbuffer, " as uid %d", s->stop->uid);
                if (s->stop->has_gid)
                        StringBuffer_append(res->outputbuffer, " as gid %d", s->stop->gid);
                StringBuffer_append(res->outputbuffer, " timeout %s", Fmt_time2str(s->stop->timeout, (char[11]){}));
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        if (s->restart) {
                StringBuffer_append(res->outputbuffer, "<tr><td>Restart program</td><td>'%s'", Util_commandDescription(s->restart, (char[STRLEN]){}));
                if (s->restart->has_uid)
                        StringBuffer_append(res->outputbuffer, " as uid %d", s->restart->uid);
                if (s->restart->has_gid)
                        StringBuffer_append(res->outputbuffer, " as gid %d", s->restart->gid);
                StringBuffer_append(res->outputbuffer, " timeout %s", Fmt_time2str(s->restart->timeout, (char[11]){}));
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        if (s->every.type != Every_Cycle) {
                StringBuffer_append(res->outputbuffer, "<tr><td>Check service</td><td>");
                if (s->every.type == Every_SkipCycles)
                        StringBuffer_append(res->outputbuffer, "every %d cycle", s->every.spec.cycle.number);
                else if (s->every.type == Every_Cron)
                        StringBuffer_append(res->outputbuffer, "every <code>\"%s\"</code>", s->every.spec.cron);
                else if (s->every.type == Every_NotInCron)
                        StringBuffer_append(res->outputbuffer, "not every <code>\"%s\"</code>", s->every.spec.cron);
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        _printStatus(HTML, res, s);
        // Rules
        print_service_rules_timeout(res, s);
        print_service_rules_nonexistence(res, s);
        print_service_rules_existence(res, s);
        print_service_rules_icmp(res, s);
        print_service_rules_port(res, s);
        print_service_rules_socket(res, s);
        print_service_rules_perm(res, s);
        print_service_rules_uid(res, s);
        print_service_rules_euid(res, s);
        print_service_rules_secattr(res, s);
        print_service_rules_gid(res, s);
        print_service_rules_timestamp(res, s);
        print_service_rules_fsflags(res, s);
        print_service_rules_filesystem(res, s);
        print_service_rules_size(res, s);
        print_service_rules_linkstatus(res, s);
        print_service_rules_linkspeed(res, s);
        print_service_rules_linksaturation(res, s);
        print_service_rules_uploadbytes(res, s);
        print_service_rules_uploadpackets(res, s);
        print_service_rules_downloadbytes(res, s);
        print_service_rules_downloadpackets(res, s);
        print_service_rules_uptime(res, s);
        print_service_rules_content(res, s);
        print_service_rules_checksum(res, s);
        print_service_rules_pid(res, s);
        print_service_rules_ppid(res, s);
        print_service_rules_program(res, s);
        print_service_rules_resource(res, s);
        print_alerts(res, s->maillist);
        StringBuffer_append(res->outputbuffer, "</table>");
        print_buttons(req, res, s);
        do_foot(res);
}