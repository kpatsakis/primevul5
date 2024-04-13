static char *get_service_status(Output_Type type, Service_T s, char *buf, int buflen) {
        ASSERT(s);
        ASSERT(buf);
        if (s->monitor == Monitor_Not || s->monitor & Monitor_Init) {
                get_monitoring_status(type, s, buf, buflen);
        } else if (s->error == 0) {
                snprintf(buf, buflen, type == HTML ? "<span class='green-text'>OK</span>" : Color_lightGreen("OK"));
        } else {
                // In the case that the service has actualy some failure, the error bitmap will be non zero
                char *p = buf;
                EventTable_T *et = Event_Table;
                while ((*et).id) {
                        if (s->error & (*et).id) {
                                if (p > buf)
                                        p += snprintf(p, buflen - (p - buf), " | ");
                                if (s->error_hint & (*et).id) {
                                        if (type == HTML)
                                                p += snprintf(p, buflen - (p - buf), "<span class='orange-text'>%s</span>", (*et).description_changed);
                                        else
                                                p += snprintf(p, buflen - (p - buf), Color_lightYellow("%s", (*et).description_changed));
                                } else {
                                        if (type == HTML)
                                                p += snprintf(p, buflen - (p - buf), "<span class='red-text'>%s</span>", (*et).description_failed);
                                        else
                                                p += snprintf(p, buflen - (p - buf), Color_lightRed("%s", (*et).description_failed));
                                }
                        }
                        et++;
                }
        }
        if (s->doaction)
                snprintf(buf + strlen(buf), buflen - strlen(buf) - 1, " - %s pending", actionnames[s->doaction]);
        return buf;
}