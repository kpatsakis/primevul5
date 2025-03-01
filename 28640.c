void net_checksum_calculate(uint8_t *data, int length)
{
    int hlen, plen, proto, csum_offset;
    uint16_t csum;

    if ((data[14] & 0xf0) != 0x40)
	return; /* not IPv4 */
    hlen  = (data[14] & 0x0f) * 4;
    plen  = (data[16] << 8 | data[17]) - hlen;
    proto = data[23];

    switch (proto) {
    case PROTO_TCP:
	csum_offset = 16;
	break;
    case PROTO_UDP:
	csum_offset = 6;
	break;
    default:
	return;
    }

    if (plen < csum_offset+2)
	return;

    data[14+hlen+csum_offset]   = 0;
    data[14+hlen+csum_offset+1] = 0;
    csum = net_checksum_tcpudp(plen, proto, data+14+12, data+14+hlen);
    data[14+hlen+csum_offset]   = csum >> 8;
    data[14+hlen+csum_offset+1] = csum & 0xff;
}