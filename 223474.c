lyxml_getutf8(struct ly_ctx *ctx, const char *buf, unsigned int *read)
{
    int c, aux;
    int i;

    c = buf[0];
    *read = 0;

    /* buf is NULL terminated string, so 0 means EOF */
    if (!c) {
        LOGVAL(ctx, LYE_EOF, LY_VLOG_NONE, NULL);
        return 0;
    }
    *read = 1;

    /* process character byte(s) */
    if ((c & 0xf8) == 0xf0) {
        /* four bytes character */
        *read = 4;

        c &= 0x07;
        for (i = 1; i <= 3; i++) {
            aux = buf[i];
            if ((aux & 0xc0) != 0x80) {
                LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
                return 0;
            }

            c = (c << 6) | (aux & 0x3f);
        }

        if (c < 0x1000 || c > 0x10ffff) {
            LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
            return 0;
        }
    } else if ((c & 0xf0) == 0xe0) {
        /* three bytes character */
        *read = 3;

        c &= 0x0f;
        for (i = 1; i <= 2; i++) {
            aux = buf[i];
            if ((aux & 0xc0) != 0x80) {
                LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
                return 0;
            }

            c = (c << 6) | (aux & 0x3f);
        }

        if (c < 0x800 || (c > 0xd7ff && c < 0xe000) || c > 0xfffd) {
            LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
            return 0;
        }
    } else if ((c & 0xe0) == 0xc0) {
        /* two bytes character */
        *read = 2;

        aux = buf[1];
        if ((aux & 0xc0) != 0x80) {
            LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
            return 0;
        }
        c = ((c & 0x1f) << 6) | (aux & 0x3f);

        if (c < 0x80) {
            LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
            return 0;
        }
    } else if (!(c & 0x80)) {
        /* one byte character */
        if (c < 0x20 && c != 0x9 && c != 0xa && c != 0xd) {
            /* invalid character */
            LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
            return 0;
        }
    } else {
        /* invalid character */
        LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "input character");
        return 0;
    }

    return c;
}