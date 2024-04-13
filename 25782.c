static void print_alerts(HttpResponse res, Mail_T s) {
        for (Mail_T r = s; r; r = r->next) {
                StringBuffer_append(res->outputbuffer,
                                    "<tr class='stripe'><td>Alert mail to</td>"
                                    "<td>%s</td></tr>", r->to ? r->to : "");
                StringBuffer_append(res->outputbuffer, "<tr><td>Alert on</td><td>");
                if (r->events == Event_Null) {
                        StringBuffer_append(res->outputbuffer, "No events");
                } else if (r->events == Event_All) {
                        StringBuffer_append(res->outputbuffer, "All events");
                } else {
                        if (IS_EVENT_SET(r->events, Event_Action))
                                StringBuffer_append(res->outputbuffer, "Action ");
                        if (IS_EVENT_SET(r->events, Event_ByteIn))
                                StringBuffer_append(res->outputbuffer, "ByteIn ");
                        if (IS_EVENT_SET(r->events, Event_ByteOut))
                                StringBuffer_append(res->outputbuffer, "ByteOut ");
                        if (IS_EVENT_SET(r->events, Event_Checksum))
                                StringBuffer_append(res->outputbuffer, "Checksum ");
                        if (IS_EVENT_SET(r->events, Event_Connection))
                                StringBuffer_append(res->outputbuffer, "Connection ");
                        if (IS_EVENT_SET(r->events, Event_Content))
                                StringBuffer_append(res->outputbuffer, "Content ");
                        if (IS_EVENT_SET(r->events, Event_Data))
                                StringBuffer_append(res->outputbuffer, "Data ");
                        if (IS_EVENT_SET(r->events, Event_Exec))
                                StringBuffer_append(res->outputbuffer, "Exec ");
                        if (IS_EVENT_SET(r->events, Event_Exist))
                                StringBuffer_append(res->outputbuffer, "Exist ");
                        if (IS_EVENT_SET(r->events, Event_FsFlag))
                                StringBuffer_append(res->outputbuffer, "Fsflags ");
                        if (IS_EVENT_SET(r->events, Event_Gid))
                                StringBuffer_append(res->outputbuffer, "Gid ");
                        if (IS_EVENT_SET(r->events, Event_Instance))
                                StringBuffer_append(res->outputbuffer, "Instance ");
                        if (IS_EVENT_SET(r->events, Event_Invalid))
                                StringBuffer_append(res->outputbuffer, "Invalid ");
                        if (IS_EVENT_SET(r->events, Event_Link))
                                StringBuffer_append(res->outputbuffer, "Link ");
                        if (IS_EVENT_SET(r->events, Event_NonExist))
                                StringBuffer_append(res->outputbuffer, "Nonexist ");
                        if (IS_EVENT_SET(r->events, Event_Permission))
                                StringBuffer_append(res->outputbuffer, "Permission ");
                        if (IS_EVENT_SET(r->events, Event_PacketIn))
                                StringBuffer_append(res->outputbuffer, "PacketIn ");
                        if (IS_EVENT_SET(r->events, Event_PacketOut))
                                StringBuffer_append(res->outputbuffer, "PacketOut ");
                        if (IS_EVENT_SET(r->events, Event_Pid))
                                StringBuffer_append(res->outputbuffer, "PID ");
                        if (IS_EVENT_SET(r->events, Event_Icmp))
                                StringBuffer_append(res->outputbuffer, "Ping ");
                        if (IS_EVENT_SET(r->events, Event_PPid))
                                StringBuffer_append(res->outputbuffer, "PPID ");
                        if (IS_EVENT_SET(r->events, Event_Resource))
                                StringBuffer_append(res->outputbuffer, "Resource ");
                        if (IS_EVENT_SET(r->events, Event_Saturation))
                                StringBuffer_append(res->outputbuffer, "Saturation ");
                        if (IS_EVENT_SET(r->events, Event_Size))
                                StringBuffer_append(res->outputbuffer, "Size ");
                        if (IS_EVENT_SET(r->events, Event_Speed))
                                StringBuffer_append(res->outputbuffer, "Speed ");
                        if (IS_EVENT_SET(r->events, Event_Status))
                                StringBuffer_append(res->outputbuffer, "Status ");
                        if (IS_EVENT_SET(r->events, Event_Timeout))
                                StringBuffer_append(res->outputbuffer, "Timeout ");
                        if (IS_EVENT_SET(r->events, Event_Timestamp))
                                StringBuffer_append(res->outputbuffer, "Timestamp ");
                        if (IS_EVENT_SET(r->events, Event_Uid))
                                StringBuffer_append(res->outputbuffer, "Uid ");
                        if (IS_EVENT_SET(r->events, Event_Uptime))
                                StringBuffer_append(res->outputbuffer, "Uptime ");
                }
                StringBuffer_append(res->outputbuffer, "</td></tr>");
                if (r->reminder) {
                        StringBuffer_append(res->outputbuffer,
                                            "<tr><td>Alert reminder</td><td>%u cycles</td></tr>",
                                            r->reminder);
                }
        }
}