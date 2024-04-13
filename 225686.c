verify_scps(packet_info *pinfo,  proto_item *tf_syn, struct tcp_analysis *tcpd)
{
    tf_syn = 0x0;

    if(tcpd) {
        if ((!(tcpd->flow1.scps_capable)) || (!(tcpd->flow2.scps_capable))) {
            tcpd->flow1.scps_capable = 0;
            tcpd->flow2.scps_capable = 0;
        } else {
            expert_add_info(pinfo, tf_syn, &ei_tcp_scps_capable);
        }
    }
}