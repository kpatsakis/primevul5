dnp3_udp_check_header(packet_info *pinfo _U_, tvbuff_t *tvb, int offset _U_, void *data _U_)
{
    return check_dnp3_header(tvb, FALSE);
}