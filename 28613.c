static bool vmxnet_tx_pkt_parse_headers(struct VmxnetTxPkt *pkt)
{
    struct iovec *l2_hdr, *l3_hdr;
    size_t bytes_read;
    size_t full_ip6hdr_len;
    uint16_t l3_proto;

    assert(pkt);

    l2_hdr = &pkt->vec[VMXNET_TX_PKT_L2HDR_FRAG];
    l3_hdr = &pkt->vec[VMXNET_TX_PKT_L3HDR_FRAG];

    bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags, 0, l2_hdr->iov_base,
                            ETH_MAX_L2_HDR_LEN);
    if (bytes_read < sizeof(struct eth_header)) {
        l2_hdr->iov_len = 0;
        return false;
    }

    l2_hdr->iov_len = sizeof(struct eth_header);
    switch (be16_to_cpu(PKT_GET_ETH_HDR(l2_hdr->iov_base)->h_proto)) {
    case ETH_P_VLAN:
        l2_hdr->iov_len += sizeof(struct vlan_header);
        break;
    case ETH_P_DVLAN:
        l2_hdr->iov_len += 2 * sizeof(struct vlan_header);
        break;
    }

    if (bytes_read < l2_hdr->iov_len) {
        l2_hdr->iov_len = 0;
        return false;
    }

    l3_proto = eth_get_l3_proto(l2_hdr->iov_base, l2_hdr->iov_len);

    switch (l3_proto) {
    case ETH_P_IP:
        l3_hdr->iov_base = g_malloc(ETH_MAX_IP4_HDR_LEN);

        bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags, l2_hdr->iov_len,
                                l3_hdr->iov_base, sizeof(struct ip_header));

        if (bytes_read < sizeof(struct ip_header)) {
            l3_hdr->iov_len = 0;
            return false;
        }

        l3_hdr->iov_len = IP_HDR_GET_LEN(l3_hdr->iov_base);
        pkt->l4proto = ((struct ip_header *) l3_hdr->iov_base)->ip_p;

        /* copy optional IPv4 header data */
        bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags,
                                l2_hdr->iov_len + sizeof(struct ip_header),
                                l3_hdr->iov_base + sizeof(struct ip_header),
                                l3_hdr->iov_len - sizeof(struct ip_header));
        if (bytes_read < l3_hdr->iov_len - sizeof(struct ip_header)) {
            l3_hdr->iov_len = 0;
            return false;
        }
        break;

    case ETH_P_IPV6:
        if (!eth_parse_ipv6_hdr(pkt->raw, pkt->raw_frags, l2_hdr->iov_len,
                               &pkt->l4proto, &full_ip6hdr_len)) {
            l3_hdr->iov_len = 0;
            return false;
        }

        l3_hdr->iov_base = g_malloc(full_ip6hdr_len);

        bytes_read = iov_to_buf(pkt->raw, pkt->raw_frags, l2_hdr->iov_len,
                                l3_hdr->iov_base, full_ip6hdr_len);

        if (bytes_read < full_ip6hdr_len) {
            l3_hdr->iov_len = 0;
            return false;
        } else {
            l3_hdr->iov_len = full_ip6hdr_len;
        }
        break;

    default:
        l3_hdr->iov_len = 0;
        break;
    }

    vmxnet_tx_pkt_calculate_hdr_len(pkt);
    pkt->packet_type = get_eth_packet_type(l2_hdr->iov_base);
    return true;
}