static void _formatStatus(const char *name, Event_Type errorType, Output_Type type, HttpResponse res, Service_T s, boolean_t validValue, const char *value, ...) {
        if (type == HTML) {
                StringBuffer_append(res->outputbuffer, "<tr><td>%c%s</td>", toupper(name[0]), name + 1);
        } else {
                StringBuffer_append(res->outputbuffer, "  %-28s ", name);
        }
        if (! validValue) {
                StringBuffer_append(res->outputbuffer, type == HTML ? "<td class='gray-text'>-</td>" : COLOR_DARKGRAY "-" COLOR_RESET);
        } else {
                va_list ap;
                va_start(ap, value);
                char *_value = Str_vcat(value, ap);
                va_end(ap);
                if (errorType != Event_Null && s->error & errorType)
                        StringBuffer_append(res->outputbuffer, type == HTML ? "<td class='red-text'>" : COLOR_LIGHTRED);
                else
                        StringBuffer_append(res->outputbuffer, type == HTML ? "<td>" : COLOR_DEFAULT);
                if (type == HTML) {
                        // If the output contains multiple line, wrap use <pre>, otherwise keep as is
                        boolean_t multiline = strrchr(_value, '\n') ? true : false;
                        if (multiline)
                                StringBuffer_append(res->outputbuffer, "<pre>");
                        escapeHTML(res->outputbuffer, _value);
                        StringBuffer_append(res->outputbuffer, "%s</td>", multiline ? "</pre>" : "");
                } else {
                        int column = 0;
                        for (int i = 0; _value[i]; i++) {
                                if (_value[i] == '\r') {
                                        // Discard CR
                                        continue;
                                } else if (_value[i] == '\n') {
                                        // Indent 2nd+ line
                                        if (_value[i + 1])
                                        StringBuffer_append(res->outputbuffer, "\n                               ");
                                        column = 0;
                                        continue;
                                } else if (column <= 200) {
                                        StringBuffer_append(res->outputbuffer, "%c", _value[i]);
                                        column++;
                                }
                        }
                        StringBuffer_append(res->outputbuffer, COLOR_RESET);
                }
                FREE(_value);
        }
        StringBuffer_append(res->outputbuffer, type == HTML ? "</tr>" : "\n");
}