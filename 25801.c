static void do_runtime(HttpRequest req, HttpResponse res) {
        int pid = exist_daemon();
        char buf[STRLEN];

        do_head(res, "_runtime", "Runtime", 1000);
        StringBuffer_append(res->outputbuffer,
                            "<h2>Monit runtime status</h2>");
        StringBuffer_append(res->outputbuffer, "<table id='status-table'><tr>"
                            "<th width='40%%'>Parameter</th>"
                            "<th width='60%%'>Value</th></tr>");
        StringBuffer_append(res->outputbuffer, "<tr><td>Monit ID</td><td>%s</td></tr>", Run.id);
        StringBuffer_append(res->outputbuffer, "<tr><td>Host</td><td>%s</td></tr>",  Run.system->name);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Process id</td><td>%d</td></tr>", pid);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Effective user running Monit</td>"
                            "<td>%s</td></tr>", Run.Env.user);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Controlfile</td><td>%s</td></tr>", Run.files.control);
        if (Run.files.log)
                StringBuffer_append(res->outputbuffer,
                                    "<tr><td>Logfile</td><td>%s</td></tr>", Run.files.log);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Pidfile</td><td>%s</td></tr>", Run.files.pid);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>State file</td><td>%s</td></tr>", Run.files.state);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Debug</td><td>%s</td></tr>",
                            Run.debug ? "True" : "False");
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Log</td><td>%s</td></tr>", (Run.flags & Run_Log) ? "True" : "False");
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Use syslog</td><td>%s</td></tr>",
                            (Run.flags & Run_UseSyslog) ? "True" : "False");
        if (Run.eventlist_dir) {
                if (Run.eventlist_slots < 0)
                        snprintf(buf, STRLEN, "unlimited");
                else
                        snprintf(buf, STRLEN, "%d", Run.eventlist_slots);
                StringBuffer_append(res->outputbuffer,
                                    "<tr><td>Event queue</td>"
                                    "<td>base directory %s with %d slots</td></tr>",
                                    Run.eventlist_dir, Run.eventlist_slots);
        }
#ifdef HAVE_OPENSSL
        {
                const char *options = Ssl_printOptions(&(Run.ssl), (char[STRLEN]){}, STRLEN);
                if (options && *options)
                        StringBuffer_append(res->outputbuffer,
                                    "<tr><td>SSL options</td><td>%s</td></tr>", options);
        }
#endif
        if (Run.mmonits) {
                StringBuffer_append(res->outputbuffer, "<tr><td>M/Monit server(s)</td><td>");
                for (Mmonit_T c = Run.mmonits; c; c = c->next)
                {
                        StringBuffer_append(res->outputbuffer, "%s with timeout %s", c->url->url, Fmt_time2str(c->timeout, (char[11]){}));
#ifdef HAVE_OPENSSL
                        if (c->ssl.flags) {
                                StringBuffer_append(res->outputbuffer, " using TLS");
                                const char *options = Ssl_printOptions(&c->ssl, (char[STRLEN]){}, STRLEN);
                                if (options && *options)
                                        StringBuffer_append(res->outputbuffer, " with options {%s}", options);
                                if (c->ssl.checksum)
                                        StringBuffer_append(res->outputbuffer, " and certificate checksum %s equal to '%s'", checksumnames[c->ssl.checksumType], c->ssl.checksum);
                        }
#endif
                        if (Run.flags & Run_MmonitCredentials && c->url->user)
                                StringBuffer_append(res->outputbuffer, " with credentials");
                        if (c->next)
                                StringBuffer_append(res->outputbuffer, "</td></tr><tr><td>&nbsp;</td><td>");
                }
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        if (Run.mailservers) {
                StringBuffer_append(res->outputbuffer, "<tr><td>Mail server(s)</td><td>");
                for (MailServer_T mta = Run.mailservers; mta; mta = mta->next) {
                        StringBuffer_append(res->outputbuffer, "%s:%d", mta->host, mta->port);
#ifdef HAVE_OPENSSL
                        if (mta->ssl.flags) {
                                StringBuffer_append(res->outputbuffer, " using TLS");
                                const char *options = Ssl_printOptions(&mta->ssl, (char[STRLEN]){}, STRLEN);
                                if (options && *options)
                                        StringBuffer_append(res->outputbuffer, " with options {%s}", options);
                                if (mta->ssl.checksum)
                                        StringBuffer_append(res->outputbuffer, " and certificate checksum %s equal to '%s'", checksumnames[mta->ssl.checksumType], mta->ssl.checksum);
                        }
#endif
                        if (mta->next)
                                StringBuffer_append(res->outputbuffer, "</td></tr><tr><td>&nbsp;</td><td>");
                }
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        if (Run.MailFormat.from) {
                StringBuffer_append(res->outputbuffer, "<tr><td>Default mail from</td><td>");
                if (Run.MailFormat.from->name)
                        StringBuffer_append(res->outputbuffer, "%s &lt;%s&gt;", Run.MailFormat.from->name, Run.MailFormat.from->address);
                else
                        StringBuffer_append(res->outputbuffer, "%s", Run.MailFormat.from->address);
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        if (Run.MailFormat.replyto) {
                StringBuffer_append(res->outputbuffer, "<tr><td>Default mail reply to</td><td>");
                if (Run.MailFormat.replyto->name)
                        StringBuffer_append(res->outputbuffer, "%s &lt;%s&gt;", Run.MailFormat.replyto->name, Run.MailFormat.replyto->address);
                else
                        StringBuffer_append(res->outputbuffer, "%s", Run.MailFormat.replyto->address);
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
        if (Run.MailFormat.subject)
                StringBuffer_append(res->outputbuffer, "<tr><td>Default mail subject</td><td>%s</td></tr>", Run.MailFormat.subject);
        if (Run.MailFormat.message)
                StringBuffer_append(res->outputbuffer, "<tr><td>Default mail message</td><td>%s</td></tr>", Run.MailFormat.message);
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for Send/Expect buffer</td><td>%s</td></tr>", Fmt_bytes2str(Run.limits.sendExpectBuffer, buf));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for file content buffer</td><td>%s</td></tr>", Fmt_bytes2str(Run.limits.fileContentBuffer, buf));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for HTTP content buffer</td><td>%s</td></tr>", Fmt_bytes2str(Run.limits.httpContentBuffer, buf));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for program output</td><td>%s</td></tr>", Fmt_bytes2str(Run.limits.programOutput, buf));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for network timeout</td><td>%s</td></tr>", Fmt_time2str(Run.limits.networkTimeout, (char[11]){}));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for check program timeout</td><td>%s</td></tr>", Fmt_time2str(Run.limits.programTimeout, (char[11]){}));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for service stop timeout</td><td>%s</td></tr>", Fmt_time2str(Run.limits.stopTimeout, (char[11]){}));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for service start timeout</td><td>%s</td></tr>", Fmt_time2str(Run.limits.startTimeout, (char[11]){}));
        StringBuffer_append(res->outputbuffer, "<tr><td>Limit for service restart timeout</td><td>%s</td></tr>", Fmt_time2str(Run.limits.restartTimeout, (char[11]){}));
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>On reboot</td><td>%s</td></tr>", onrebootnames[Run.onreboot]);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>Poll time</td><td>%d seconds with start delay %d seconds</td></tr>",
                            Run.polltime, Run.startdelay);
        if (Run.httpd.flags & Httpd_Net) {
                StringBuffer_append(res->outputbuffer,
                                    "<tr><td>httpd bind address</td><td>%s</td></tr>",
                                    Run.httpd.socket.net.address ? Run.httpd.socket.net.address : "Any/All");
                StringBuffer_append(res->outputbuffer,
                                    "<tr><td>httpd portnumber</td><td>%d</td></tr>", Run.httpd.socket.net.port);
#ifdef HAVE_OPENSSL
                const char *options = Ssl_printOptions(&(Run.httpd.socket.net.ssl), (char[STRLEN]){}, STRLEN);
                if (options && *options)
                        StringBuffer_append(res->outputbuffer,
                                    "<tr><td>httpd encryption</td><td>%s</td></tr>", options);
#endif
        }
        if (Run.httpd.flags & Httpd_Unix)
                StringBuffer_append(res->outputbuffer,
                                    "<tr><td>httpd unix socket</td><td>%s</td></tr>",
                                    Run.httpd.socket.unix.path);
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>httpd signature</td><td>%s</td></tr>",
                            Run.httpd.flags & Httpd_Signature ? "True" : "False");
        StringBuffer_append(res->outputbuffer,
                            "<tr><td>httpd auth. style</td><td>%s</td></tr>",
                            Run.httpd.credentials && Engine_hasAllow() ? "Basic Authentication and Host/Net allow list" : Run.httpd.credentials ? "Basic Authentication" : Engine_hasAllow() ? "Host/Net allow list" : "No authentication");
        print_alerts(res, Run.maillist);
        StringBuffer_append(res->outputbuffer, "</table>");
        if (! is_readonly(req)) {
                StringBuffer_append(res->outputbuffer,
                                    "<table id='buttons'><tr>");
                StringBuffer_append(res->outputbuffer,
                                    "<td style='color:red;'>"
                                    "<form method=POST action='_runtime'>Stop Monit http server? "
                                    "<input type=hidden name='securitytoken' value='%s'>"
                                    "<input type=hidden name='action' value='stop'>"
                                    "<input type=submit value='Go'>"
                                    "</form>"
                                    "</td>",
                                    res->token);
                StringBuffer_append(res->outputbuffer,
                                    "<td>"
                                    "<form method=POST action='_runtime'>Force validate now? "
                                    "<input type=hidden name='securitytoken' value='%s'>"
                                    "<input type=hidden name='action' value='validate'>"
                                    "<input type=submit value='Go'>"
                                    "</form>"
                                    "</td>",
                                    res->token);

                if ((Run.flags & Run_Log) && ! (Run.flags & Run_UseSyslog)) {
                        StringBuffer_append(res->outputbuffer,
                                            "<td>"
                                            "<form method=POST action='_viewlog'>View Monit logfile? "
                                            "<input type=hidden name='securitytoken' value='%s'>"
                                            "<input type=submit value='Go'>"
                                            "</form>"
                                            "</td>",
                                            res->token);
                }
                StringBuffer_append(res->outputbuffer,
                                    "</tr></table>");
        }
        do_foot(res);
}