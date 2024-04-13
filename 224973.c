	NULL,
};

static umode_t iscsi_conn_attr_is_visible(struct kobject *kobj,
					 struct attribute *attr, int i)
{
	struct device *cdev = container_of(kobj, struct device, kobj);
	struct iscsi_cls_conn *conn = transport_class_to_conn(cdev);
	struct iscsi_transport *t = conn->transport;
	int param;

	if (attr == &dev_attr_conn_max_recv_dlength.attr)
		param = ISCSI_PARAM_MAX_RECV_DLENGTH;
	else if (attr == &dev_attr_conn_max_xmit_dlength.attr)
		param = ISCSI_PARAM_MAX_XMIT_DLENGTH;
	else if (attr == &dev_attr_conn_header_digest.attr)
		param = ISCSI_PARAM_HDRDGST_EN;
	else if (attr == &dev_attr_conn_data_digest.attr)
		param = ISCSI_PARAM_DATADGST_EN;
	else if (attr == &dev_attr_conn_ifmarker.attr)
		param = ISCSI_PARAM_IFMARKER_EN;
	else if (attr == &dev_attr_conn_ofmarker.attr)
		param = ISCSI_PARAM_OFMARKER_EN;
	else if (attr == &dev_attr_conn_address.attr)
		param = ISCSI_PARAM_CONN_ADDRESS;
	else if (attr == &dev_attr_conn_port.attr)
		param = ISCSI_PARAM_CONN_PORT;
	else if (attr == &dev_attr_conn_exp_statsn.attr)
		param = ISCSI_PARAM_EXP_STATSN;
	else if (attr == &dev_attr_conn_persistent_address.attr)
		param = ISCSI_PARAM_PERSISTENT_ADDRESS;
	else if (attr == &dev_attr_conn_persistent_port.attr)
		param = ISCSI_PARAM_PERSISTENT_PORT;
	else if (attr == &dev_attr_conn_ping_tmo.attr)
		param = ISCSI_PARAM_PING_TMO;
	else if (attr == &dev_attr_conn_recv_tmo.attr)
		param = ISCSI_PARAM_RECV_TMO;
	else if (attr == &dev_attr_conn_local_port.attr)
		param = ISCSI_PARAM_LOCAL_PORT;
	else if (attr == &dev_attr_conn_statsn.attr)
		param = ISCSI_PARAM_STATSN;
	else if (attr == &dev_attr_conn_keepalive_tmo.attr)
		param = ISCSI_PARAM_KEEPALIVE_TMO;
	else if (attr == &dev_attr_conn_max_segment_size.attr)
		param = ISCSI_PARAM_MAX_SEGMENT_SIZE;
	else if (attr == &dev_attr_conn_tcp_timestamp_stat.attr)
		param = ISCSI_PARAM_TCP_TIMESTAMP_STAT;
	else if (attr == &dev_attr_conn_tcp_wsf_disable.attr)
		param = ISCSI_PARAM_TCP_WSF_DISABLE;
	else if (attr == &dev_attr_conn_tcp_nagle_disable.attr)
		param = ISCSI_PARAM_TCP_NAGLE_DISABLE;
	else if (attr == &dev_attr_conn_tcp_timer_scale.attr)
		param = ISCSI_PARAM_TCP_TIMER_SCALE;
	else if (attr == &dev_attr_conn_tcp_timestamp_enable.attr)
		param = ISCSI_PARAM_TCP_TIMESTAMP_EN;
	else if (attr == &dev_attr_conn_fragment_disable.attr)
		param = ISCSI_PARAM_IP_FRAGMENT_DISABLE;
	else if (attr == &dev_attr_conn_ipv4_tos.attr)
		param = ISCSI_PARAM_IPV4_TOS;
	else if (attr == &dev_attr_conn_ipv6_traffic_class.attr)
		param = ISCSI_PARAM_IPV6_TC;
	else if (attr == &dev_attr_conn_ipv6_flow_label.attr)
		param = ISCSI_PARAM_IPV6_FLOW_LABEL;
	else if (attr == &dev_attr_conn_is_fw_assigned_ipv6.attr)
		param = ISCSI_PARAM_IS_FW_ASSIGNED_IPV6;
	else if (attr == &dev_attr_conn_tcp_xmit_wsf.attr)
		param = ISCSI_PARAM_TCP_XMIT_WSF;
	else if (attr == &dev_attr_conn_tcp_recv_wsf.attr)
		param = ISCSI_PARAM_TCP_RECV_WSF;
	else if (attr == &dev_attr_conn_local_ipaddr.attr)
		param = ISCSI_PARAM_LOCAL_IPADDR;
	else if (attr == &dev_attr_conn_state.attr)
		return S_IRUGO;
	else {
		WARN_ONCE(1, "Invalid conn attr");
		return 0;
	}