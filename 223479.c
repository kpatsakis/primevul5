parse_text(struct ly_ctx *ctx, const char *data, char delim, unsigned int *len)
{
#define BUFSIZE 1024

    char buf[BUFSIZE];
    char *result = NULL;
    unsigned int r;
    int o, size = 0;
    int cdsect = 0;
    int32_t n;

    for (*len = o = 0; cdsect || data[*len] != delim; o++) {
        if (!data[*len] || (!cdsect && !strncmp(&data[*len], "]]>", 3))) {
            LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "element content, \"]]>\" found");
            goto error;
        }

loop:

        if (o > BUFSIZE - 4) {
            /* add buffer into the result */
            if (result) {
                size = size + o;
                result = ly_realloc(result, size + 1);
            } else {
                size = o;
                result = malloc((size + 1) * sizeof *result);
            }
            LY_CHECK_ERR_RETURN(!result, LOGMEM(ctx), NULL);
            memcpy(&result[size - o], buf, o);

            /* write again into the beginning of the buffer */
            o = 0;
        }

        if (cdsect || !strncmp(&data[*len], "<![CDATA[", 9)) {
            /* CDSect */
            if (!cdsect) {
                cdsect = 1;
                *len += 9;
            }
            if (data[*len] && !strncmp(&data[*len], "]]>", 3)) {
                *len += 3;
                cdsect = 0;
                o--;            /* we don't write any data in this iteration */
            } else {
                buf[o] = data[*len];
                (*len)++;
            }
        } else if (data[*len] == '&') {
            (*len)++;
            if (data[*len] != '#') {
                /* entity reference - only predefined refs are supported */
                if (!strncmp(&data[*len], "lt;", 3)) {
                    buf[o] = '<';
                    *len += 3;
                } else if (!strncmp(&data[*len], "gt;", 3)) {
                    buf[o] = '>';
                    *len += 3;
                } else if (!strncmp(&data[*len], "amp;", 4)) {
                    buf[o] = '&';
                    *len += 4;
                } else if (!strncmp(&data[*len], "apos;", 5)) {
                    buf[o] = '\'';
                    *len += 5;
                } else if (!strncmp(&data[*len], "quot;", 5)) {
                    buf[o] = '\"';
                    *len += 5;
                } else {
                    LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "entity reference (only predefined references are supported)");
                    goto error;
                }
            } else {
                /* character reference */
                (*len)++;
                if (isdigit(data[*len])) {
                    for (n = 0; isdigit(data[*len]); (*len)++) {
                        n = (10 * n) + (data[*len] - '0');
                    }
                    if (data[*len] != ';') {
                        LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "character reference, missing semicolon");
                        goto error;
                    }
                } else if (data[(*len)++] == 'x' && isxdigit(data[*len])) {
                    for (n = 0; isxdigit(data[*len]); (*len)++) {
                        if (isdigit(data[*len])) {
                            r = (data[*len] - '0');
                        } else if (data[*len] > 'F') {
                            r = 10 + (data[*len] - 'a');
                        } else {
                            r = 10 + (data[*len] - 'A');
                        }
                        n = (16 * n) + r;
                    }
                } else {
                    LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "character reference");
                    goto error;

                }
                r = pututf8(ctx, &buf[o], n);
                if (!r) {
                    LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "character reference value");
                    goto error;
                }
                o += r - 1;     /* o is ++ in for loop */
                (*len)++;
            }
        } else {
            r = copyutf8(ctx, &buf[o], &data[*len]);
            if (!r) {
                goto error;
            }

            o += r - 1;     /* o is ++ in for loop */
            (*len) = (*len) + r;
        }
    }

    if (delim == '<' && !strncmp(&data[*len], "<![CDATA[", 9)) {
        /* ignore loop's end condition on beginning of CDSect */
        goto loop;
    }
#undef BUFSIZE

    if (o) {
        if (result) {
            size = size + o;
            result = ly_realloc(result, size + 1);
        } else {
            size = o;
            result = malloc((size + 1) * sizeof *result);
        }
        LY_CHECK_ERR_RETURN(!result, LOGMEM(ctx), NULL);
        memcpy(&result[size - o], buf, o);
    }
    if (result) {
        result[size] = '\0';
    } else {
        size = 0;
        result = strdup("");
        LY_CHECK_ERR_RETURN(!result, LOGMEM(ctx), NULL)
    }

    return result;

error:
    *len = 0;
    free(result);
    return NULL;
}