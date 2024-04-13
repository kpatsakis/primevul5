lyxml_dump_text(struct lyout *out, const char *text, LYXML_DATA_TYPE type)
{
    unsigned int i, n;

    if (!text) {
        return 0;
    }

    for (i = n = 0; text[i]; i++) {
        switch (text[i]) {
        case '&':
            n += ly_print(out, "&amp;");
            break;
        case '<':
            n += ly_print(out, "&lt;");
            break;
        case '>':
            /* not needed, just for readability */
            n += ly_print(out, "&gt;");
            break;
        case '"':
            if (type == LYXML_DATA_ATTR) {
                n += ly_print(out, "&quot;");
                break;
            }
            /* falls through */
        default:
            ly_write(out, &text[i], 1);
            n++;
        }
    }

    return n;
}