static void print_service_rules_resource(HttpResponse res, Service_T s) {
        char buf[STRLEN];
        for (Resource_T q = s->resourcelist; q; q = q->next) {
                StringBuffer_append(res->outputbuffer, "<tr class='rule'><td>");
                switch (q->resource_id) {
                        case Resource_CpuPercent:
                                StringBuffer_append(res->outputbuffer, "CPU usage limit");
                                break;

                        case Resource_CpuPercentTotal:
                                StringBuffer_append(res->outputbuffer, "CPU usage limit (incl. children)");
                                break;

                        case Resource_CpuUser:
                                StringBuffer_append(res->outputbuffer, "CPU user limit");
                                break;

                        case Resource_CpuSystem:
                                StringBuffer_append(res->outputbuffer, "CPU system limit");
                                break;

                        case Resource_CpuWait:
                                StringBuffer_append(res->outputbuffer, "CPU wait limit");
                                break;

                        case Resource_MemoryPercent:
                                StringBuffer_append(res->outputbuffer, "Memory usage limit");
                                break;

                        case Resource_MemoryKbyte:
                                StringBuffer_append(res->outputbuffer, "Memory amount limit");
                                break;

                        case Resource_SwapPercent:
                                StringBuffer_append(res->outputbuffer, "Swap usage limit");
                                break;

                        case Resource_SwapKbyte:
                                StringBuffer_append(res->outputbuffer, "Swap amount limit");
                                break;

                        case Resource_LoadAverage1m:
                                StringBuffer_append(res->outputbuffer, "Load average (1min)");
                                break;

                        case Resource_LoadAverage5m:
                                StringBuffer_append(res->outputbuffer, "Load average (5min)");
                                break;

                        case Resource_LoadAverage15m:
                                StringBuffer_append(res->outputbuffer, "Load average (15min)");
                                break;

                        case Resource_Threads:
                                StringBuffer_append(res->outputbuffer, "Threads");
                                break;

                        case Resource_Children:
                                StringBuffer_append(res->outputbuffer, "Children");
                                break;

                        case Resource_MemoryKbyteTotal:
                                StringBuffer_append(res->outputbuffer, "Memory amount limit (incl. children)");
                                break;

                        case Resource_MemoryPercentTotal:
                                StringBuffer_append(res->outputbuffer, "Memory usage limit (incl. children)");
                                break;

                        case Resource_ReadBytes:
                                StringBuffer_append(res->outputbuffer, "Disk read limit");
                                break;

                        case Resource_ReadOperations:
                                StringBuffer_append(res->outputbuffer, "Disk read limit");
                                break;

                        case Resource_WriteBytes:
                                StringBuffer_append(res->outputbuffer, "Disk write limit");
                                break;

                        case Resource_WriteOperations:
                                StringBuffer_append(res->outputbuffer, "Disk write limit");
                                break;

                        default:
                                break;
                }
                StringBuffer_append(res->outputbuffer, "</td><td>");
                switch (q->resource_id) {
                        case Resource_CpuPercent:
                        case Resource_CpuPercentTotal:
                        case Resource_MemoryPercentTotal:
                        case Resource_CpuUser:
                        case Resource_CpuSystem:
                        case Resource_CpuWait:
                        case Resource_MemoryPercent:
                        case Resource_SwapPercent:
                                Util_printRule(res->outputbuffer, q->action, "If %s %.1f%%", operatornames[q->operator], q->limit);
                                break;

                        case Resource_MemoryKbyte:
                        case Resource_SwapKbyte:
                        case Resource_MemoryKbyteTotal:
                                Util_printRule(res->outputbuffer, q->action, "If %s %s", operatornames[q->operator], Fmt_bytes2str(q->limit, buf));
                                break;

                        case Resource_LoadAverage1m:
                        case Resource_LoadAverage5m:
                        case Resource_LoadAverage15m:
                                Util_printRule(res->outputbuffer, q->action, "If %s %.1f", operatornames[q->operator], q->limit);
                                break;

                        case Resource_Threads:
                        case Resource_Children:
                                Util_printRule(res->outputbuffer, q->action, "If %s %.0f", operatornames[q->operator], q->limit);
                                break;

                        case Resource_ReadBytes:
                        case Resource_WriteBytes:
                                Util_printRule(res->outputbuffer, q->action, "if %s %s", operatornames[q->operator], Fmt_bytes2str(q->limit, (char[10]){}));
                                break;

                        case Resource_ReadOperations:
                        case Resource_WriteOperations:
                                Util_printRule(res->outputbuffer, q->action, "if %s %.0f operations/s", operatornames[q->operator], q->limit);
                                break;

                        default:
                                break;
                }
                StringBuffer_append(res->outputbuffer, "</td></tr>");
        }
}