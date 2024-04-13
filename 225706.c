pdu_store_window_scale_option(guint8 ws, struct tcp_analysis *tcpd)
{
    if (tcpd)
        tcpd->fwd->win_scale=ws;
}