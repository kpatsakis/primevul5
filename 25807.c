static void print_service_rules_filesystem(HttpResponse res, Service_T s) {
        for (FileSystem_T dl = s->filesystemlist; dl; dl = dl->next) {
                if (dl->resource == Resource_Inode) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Inodes usage limit</td><td>");
                        if (dl->limit_absolute > -1)
                                Util_printRule(res->outputbuffer, dl->action, "If %s %lld", operatornames[dl->operator], dl->limit_absolute);
                        else
                                Util_printRule(res->outputbuffer, dl->action, "If %s %.1f%%", operatornames[dl->operator], dl->limit_percent);
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_InodeFree) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Inodes free limit</td><td>");
                        if (dl->limit_absolute > -1)
                                Util_printRule(res->outputbuffer, dl->action, "If %s %lld", operatornames[dl->operator], dl->limit_absolute);
                        else
                                Util_printRule(res->outputbuffer, dl->action, "If %s %.1f%%", operatornames[dl->operator], dl->limit_percent);
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_Space) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Space usage limit</td><td>");
                        if (dl->limit_absolute > -1) {
                                Util_printRule(res->outputbuffer, dl->action, "If %s %s", operatornames[dl->operator], Fmt_bytes2str(dl->limit_absolute, (char[10]){}));
                        } else {
                                Util_printRule(res->outputbuffer, dl->action, "If %s %.1f%%", operatornames[dl->operator], dl->limit_percent);
                        }
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_SpaceFree) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Space free limit</td><td>");
                        if (dl->limit_absolute > -1) {
                                Util_printRule(res->outputbuffer, dl->action, "If %s %s", operatornames[dl->operator], Fmt_bytes2str(dl->limit_absolute, (char[10]){}));
                        } else {
                                Util_printRule(res->outputbuffer, dl->action, "If %s %.1f%%", operatornames[dl->operator], dl->limit_percent);
                        }
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_ReadBytes) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Read limit</td><td>");
                        Util_printRule(res->outputbuffer, dl->action, "If read %s %s/s", operatornames[dl->operator], Fmt_bytes2str(dl->limit_absolute, (char[10]){}));
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_ReadOperations) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Read limit</td><td>");
                        Util_printRule(res->outputbuffer, dl->action, "If read %s %llu operations/s", operatornames[dl->operator], dl->limit_absolute);
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_WriteBytes) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Write limit</td><td>");
                        Util_printRule(res->outputbuffer, dl->action, "If write %s %s/s", operatornames[dl->operator], Fmt_bytes2str(dl->limit_absolute, (char[10]){}));
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_WriteOperations) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Write limit</td><td>");
                        Util_printRule(res->outputbuffer, dl->action, "If write %s %llu operations/s", operatornames[dl->operator], dl->limit_absolute);
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                } else if (dl->resource == Resource_ServiceTime) {
                        StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>Service time limit</td><td>");
                        Util_printRule(res->outputbuffer, dl->action, "If service time %s %s/operation", operatornames[dl->operator], Fmt_time2str(dl->limit_absolute, (char[11]){}));
                        StringBuffer_append(res->outputbuffer, "</td></tr>");
                }
        }
}