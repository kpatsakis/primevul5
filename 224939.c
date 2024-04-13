
static umode_t iscsi_iface_attr_is_visible(struct kobject *kobj,
					  struct attribute *attr, int i)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct iscsi_iface *iface = iscsi_dev_to_iface(dev);
	struct iscsi_transport *t = iface->transport;
	int param;
	int param_type;

	if (attr == &dev_attr_iface_enabled.attr)
		param = ISCSI_NET_PARAM_IFACE_ENABLE;
	else if (attr == &dev_attr_iface_vlan_id.attr)
		param = ISCSI_NET_PARAM_VLAN_ID;
	else if (attr == &dev_attr_iface_vlan_priority.attr)
		param = ISCSI_NET_PARAM_VLAN_PRIORITY;
	else if (attr == &dev_attr_iface_vlan_enabled.attr)
		param = ISCSI_NET_PARAM_VLAN_ENABLED;
	else if (attr == &dev_attr_iface_mtu.attr)
		param = ISCSI_NET_PARAM_MTU;
	else if (attr == &dev_attr_iface_port.attr)
		param = ISCSI_NET_PARAM_PORT;
	else if (attr == &dev_attr_iface_ipaddress_state.attr)
		param = ISCSI_NET_PARAM_IPADDR_STATE;
	else if (attr == &dev_attr_iface_delayed_ack_en.attr)
		param = ISCSI_NET_PARAM_DELAYED_ACK_EN;
	else if (attr == &dev_attr_iface_tcp_nagle_disable.attr)
		param = ISCSI_NET_PARAM_TCP_NAGLE_DISABLE;
	else if (attr == &dev_attr_iface_tcp_wsf_disable.attr)
		param = ISCSI_NET_PARAM_TCP_WSF_DISABLE;
	else if (attr == &dev_attr_iface_tcp_wsf.attr)
		param = ISCSI_NET_PARAM_TCP_WSF;
	else if (attr == &dev_attr_iface_tcp_timer_scale.attr)
		param = ISCSI_NET_PARAM_TCP_TIMER_SCALE;
	else if (attr == &dev_attr_iface_tcp_timestamp_en.attr)
		param = ISCSI_NET_PARAM_TCP_TIMESTAMP_EN;
	else if (attr == &dev_attr_iface_cache_id.attr)
		param = ISCSI_NET_PARAM_CACHE_ID;
	else if (attr == &dev_attr_iface_redirect_en.attr)
		param = ISCSI_NET_PARAM_REDIRECT_EN;
	else if (attr == &dev_attr_iface_def_taskmgmt_tmo.attr)
		param = ISCSI_IFACE_PARAM_DEF_TASKMGMT_TMO;
	else if (attr == &dev_attr_iface_header_digest.attr)
		param = ISCSI_IFACE_PARAM_HDRDGST_EN;
	else if (attr == &dev_attr_iface_data_digest.attr)
		param = ISCSI_IFACE_PARAM_DATADGST_EN;
	else if (attr == &dev_attr_iface_immediate_data.attr)
		param = ISCSI_IFACE_PARAM_IMM_DATA_EN;
	else if (attr == &dev_attr_iface_initial_r2t.attr)
		param = ISCSI_IFACE_PARAM_INITIAL_R2T_EN;
	else if (attr == &dev_attr_iface_data_seq_in_order.attr)
		param = ISCSI_IFACE_PARAM_DATASEQ_INORDER_EN;
	else if (attr == &dev_attr_iface_data_pdu_in_order.attr)
		param = ISCSI_IFACE_PARAM_PDU_INORDER_EN;
	else if (attr == &dev_attr_iface_erl.attr)
		param = ISCSI_IFACE_PARAM_ERL;
	else if (attr == &dev_attr_iface_max_recv_dlength.attr)
		param = ISCSI_IFACE_PARAM_MAX_RECV_DLENGTH;
	else if (attr == &dev_attr_iface_first_burst_len.attr)
		param = ISCSI_IFACE_PARAM_FIRST_BURST;
	else if (attr == &dev_attr_iface_max_outstanding_r2t.attr)
		param = ISCSI_IFACE_PARAM_MAX_R2T;
	else if (attr == &dev_attr_iface_max_burst_len.attr)
		param = ISCSI_IFACE_PARAM_MAX_BURST;
	else if (attr == &dev_attr_iface_chap_auth.attr)
		param = ISCSI_IFACE_PARAM_CHAP_AUTH_EN;
	else if (attr == &dev_attr_iface_bidi_chap.attr)
		param = ISCSI_IFACE_PARAM_BIDI_CHAP_EN;
	else if (attr == &dev_attr_iface_discovery_auth_optional.attr)
		param = ISCSI_IFACE_PARAM_DISCOVERY_AUTH_OPTIONAL;
	else if (attr == &dev_attr_iface_discovery_logout.attr)
		param = ISCSI_IFACE_PARAM_DISCOVERY_LOGOUT_EN;
	else if (attr == &dev_attr_iface_strict_login_comp_en.attr)
		param = ISCSI_IFACE_PARAM_STRICT_LOGIN_COMP_EN;
	else if (attr == &dev_attr_iface_initiator_name.attr)
		param = ISCSI_IFACE_PARAM_INITIATOR_NAME;
	else if (iface->iface_type == ISCSI_IFACE_TYPE_IPV4) {
		if (attr == &dev_attr_ipv4_iface_ipaddress.attr)
			param = ISCSI_NET_PARAM_IPV4_ADDR;
		else if (attr == &dev_attr_ipv4_iface_gateway.attr)
			param = ISCSI_NET_PARAM_IPV4_GW;
		else if (attr == &dev_attr_ipv4_iface_subnet.attr)
			param = ISCSI_NET_PARAM_IPV4_SUBNET;
		else if (attr == &dev_attr_ipv4_iface_bootproto.attr)
			param = ISCSI_NET_PARAM_IPV4_BOOTPROTO;
		else if (attr ==
			 &dev_attr_ipv4_iface_dhcp_dns_address_en.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_DNS_ADDR_EN;
		else if (attr ==
			 &dev_attr_ipv4_iface_dhcp_slp_da_info_en.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_SLP_DA_EN;
		else if (attr == &dev_attr_ipv4_iface_tos_en.attr)
			param = ISCSI_NET_PARAM_IPV4_TOS_EN;
		else if (attr == &dev_attr_ipv4_iface_tos.attr)
			param = ISCSI_NET_PARAM_IPV4_TOS;
		else if (attr == &dev_attr_ipv4_iface_grat_arp_en.attr)
			param = ISCSI_NET_PARAM_IPV4_GRAT_ARP_EN;
		else if (attr ==
			 &dev_attr_ipv4_iface_dhcp_alt_client_id_en.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_ALT_CLIENT_ID_EN;
		else if (attr == &dev_attr_ipv4_iface_dhcp_alt_client_id.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_ALT_CLIENT_ID;
		else if (attr ==
			 &dev_attr_ipv4_iface_dhcp_req_vendor_id_en.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_REQ_VENDOR_ID_EN;
		else if (attr ==
			 &dev_attr_ipv4_iface_dhcp_use_vendor_id_en.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_USE_VENDOR_ID_EN;
		else if (attr == &dev_attr_ipv4_iface_dhcp_vendor_id.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_VENDOR_ID;
		else if (attr ==
			 &dev_attr_ipv4_iface_dhcp_learn_iqn_en.attr)
			param = ISCSI_NET_PARAM_IPV4_DHCP_LEARN_IQN_EN;
		else if (attr ==
			 &dev_attr_ipv4_iface_fragment_disable.attr)
			param = ISCSI_NET_PARAM_IPV4_FRAGMENT_DISABLE;
		else if (attr ==
			 &dev_attr_ipv4_iface_incoming_forwarding_en.attr)
			param = ISCSI_NET_PARAM_IPV4_IN_FORWARD_EN;
		else if (attr == &dev_attr_ipv4_iface_ttl.attr)
			param = ISCSI_NET_PARAM_IPV4_TTL;
		else
			return 0;
	} else if (iface->iface_type == ISCSI_IFACE_TYPE_IPV6) {
		if (attr == &dev_attr_ipv6_iface_ipaddress.attr)
			param = ISCSI_NET_PARAM_IPV6_ADDR;
		else if (attr == &dev_attr_ipv6_iface_link_local_addr.attr)
			param = ISCSI_NET_PARAM_IPV6_LINKLOCAL;
		else if (attr == &dev_attr_ipv6_iface_router_addr.attr)
			param = ISCSI_NET_PARAM_IPV6_ROUTER;
		else if (attr == &dev_attr_ipv6_iface_ipaddr_autocfg.attr)
			param = ISCSI_NET_PARAM_IPV6_ADDR_AUTOCFG;
		else if (attr == &dev_attr_ipv6_iface_link_local_autocfg.attr)
			param = ISCSI_NET_PARAM_IPV6_LINKLOCAL_AUTOCFG;
		else if (attr == &dev_attr_ipv6_iface_link_local_state.attr)
			param = ISCSI_NET_PARAM_IPV6_LINKLOCAL_STATE;
		else if (attr == &dev_attr_ipv6_iface_router_state.attr)
			param = ISCSI_NET_PARAM_IPV6_ROUTER_STATE;
		else if (attr ==
			 &dev_attr_ipv6_iface_grat_neighbor_adv_en.attr)
			param = ISCSI_NET_PARAM_IPV6_GRAT_NEIGHBOR_ADV_EN;
		else if (attr == &dev_attr_ipv6_iface_mld_en.attr)
			param = ISCSI_NET_PARAM_IPV6_MLD_EN;
		else if (attr == &dev_attr_ipv6_iface_flow_label.attr)
			param = ISCSI_NET_PARAM_IPV6_FLOW_LABEL;
		else if (attr == &dev_attr_ipv6_iface_traffic_class.attr)
			param = ISCSI_NET_PARAM_IPV6_TRAFFIC_CLASS;
		else if (attr == &dev_attr_ipv6_iface_hop_limit.attr)
			param = ISCSI_NET_PARAM_IPV6_HOP_LIMIT;
		else if (attr == &dev_attr_ipv6_iface_nd_reachable_tmo.attr)
			param = ISCSI_NET_PARAM_IPV6_ND_REACHABLE_TMO;
		else if (attr == &dev_attr_ipv6_iface_nd_rexmit_time.attr)
			param = ISCSI_NET_PARAM_IPV6_ND_REXMIT_TIME;
		else if (attr == &dev_attr_ipv6_iface_nd_stale_tmo.attr)
			param = ISCSI_NET_PARAM_IPV6_ND_STALE_TMO;
		else if (attr == &dev_attr_ipv6_iface_dup_addr_detect_cnt.attr)
			param = ISCSI_NET_PARAM_IPV6_DUP_ADDR_DETECT_CNT;
		else if (attr == &dev_attr_ipv6_iface_router_adv_link_mtu.attr)
			param = ISCSI_NET_PARAM_IPV6_RTR_ADV_LINK_MTU;
		else
			return 0;
	} else {
		WARN_ONCE(1, "Invalid iface attr");
		return 0;
	}

	switch (param) {
	case ISCSI_IFACE_PARAM_DEF_TASKMGMT_TMO:
	case ISCSI_IFACE_PARAM_HDRDGST_EN:
	case ISCSI_IFACE_PARAM_DATADGST_EN:
	case ISCSI_IFACE_PARAM_IMM_DATA_EN:
	case ISCSI_IFACE_PARAM_INITIAL_R2T_EN:
	case ISCSI_IFACE_PARAM_DATASEQ_INORDER_EN:
	case ISCSI_IFACE_PARAM_PDU_INORDER_EN:
	case ISCSI_IFACE_PARAM_ERL:
	case ISCSI_IFACE_PARAM_MAX_RECV_DLENGTH:
	case ISCSI_IFACE_PARAM_FIRST_BURST:
	case ISCSI_IFACE_PARAM_MAX_R2T:
	case ISCSI_IFACE_PARAM_MAX_BURST:
	case ISCSI_IFACE_PARAM_CHAP_AUTH_EN:
	case ISCSI_IFACE_PARAM_BIDI_CHAP_EN:
	case ISCSI_IFACE_PARAM_DISCOVERY_AUTH_OPTIONAL:
	case ISCSI_IFACE_PARAM_DISCOVERY_LOGOUT_EN:
	case ISCSI_IFACE_PARAM_STRICT_LOGIN_COMP_EN:
	case ISCSI_IFACE_PARAM_INITIATOR_NAME:
		param_type = ISCSI_IFACE_PARAM;
		break;
	default:
		param_type = ISCSI_NET_PARAM;
	}

	return t->attr_is_visible(param_type, param);