tcp_flags_to_str(wmem_allocator_t *scope, const struct tcpheader *tcph)
{
    static const char flags[][4] = { "FIN", "SYN", "RST", "PSH", "ACK", "URG", "ECN", "CWR", "NS" };
    const int maxlength = 64; /* upper bounds, max 53B: 8 * 3 + 2 + strlen("Reserved") + 9 * 2 + 1 */

    char *pbuf;
    const char *buf;

    int i;

    buf = pbuf = (char *) wmem_alloc(scope, maxlength);
    *pbuf = '\0';

    for (i = 0; i < 9; i++) {
        if (tcph->th_flags & (1 << i)) {
            if (buf[0])
                pbuf = g_stpcpy(pbuf, ", ");
            pbuf = g_stpcpy(pbuf, flags[i]);
        }
    }

    if (tcph->th_flags & TH_RES) {
        if (buf[0])
            pbuf = g_stpcpy(pbuf, ", ");
        g_stpcpy(pbuf, "Reserved");
    }

    if (buf[0] == '\0')
        buf = "<None>";

    return buf;
}