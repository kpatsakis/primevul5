	NULL,
};

static umode_t iscsi_flashnode_conn_attr_is_visible(struct kobject *kobj,
						    struct attribute *attr,
						    int i)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct iscsi_bus_flash_conn *fnode_conn = iscsi_dev_to_flash_conn(dev);
	struct iscsi_transport *t = fnode_conn->transport;
	int param;

	if (attr == &dev_attr_fnode_is_fw_assigned_ipv6.attr) {
		param = ISCSI_FLASHNODE_IS_FW_ASSIGNED_IPV6;
	} else if (attr == &dev_attr_fnode_header_digest.attr) {
		param = ISCSI_FLASHNODE_HDR_DGST_EN;
	} else if (attr == &dev_attr_fnode_data_digest.attr) {
		param = ISCSI_FLASHNODE_DATA_DGST_EN;
	} else if (attr == &dev_attr_fnode_snack_req.attr) {
		param = ISCSI_FLASHNODE_SNACK_REQ_EN;
	} else if (attr == &dev_attr_fnode_tcp_timestamp_stat.attr) {
		param = ISCSI_FLASHNODE_TCP_TIMESTAMP_STAT;
	} else if (attr == &dev_attr_fnode_tcp_nagle_disable.attr) {
		param = ISCSI_FLASHNODE_TCP_NAGLE_DISABLE;
	} else if (attr == &dev_attr_fnode_tcp_wsf_disable.attr) {
		param = ISCSI_FLASHNODE_TCP_WSF_DISABLE;
	} else if (attr == &dev_attr_fnode_tcp_timer_scale.attr) {
		param = ISCSI_FLASHNODE_TCP_TIMER_SCALE;
	} else if (attr == &dev_attr_fnode_tcp_timestamp_enable.attr) {
		param = ISCSI_FLASHNODE_TCP_TIMESTAMP_EN;
	} else if (attr == &dev_attr_fnode_fragment_disable.attr) {
		param = ISCSI_FLASHNODE_IP_FRAG_DISABLE;
	} else if (attr == &dev_attr_fnode_max_recv_dlength.attr) {
		param = ISCSI_FLASHNODE_MAX_RECV_DLENGTH;
	} else if (attr == &dev_attr_fnode_max_xmit_dlength.attr) {
		param = ISCSI_FLASHNODE_MAX_XMIT_DLENGTH;
	} else if (attr == &dev_attr_fnode_keepalive_tmo.attr) {
		param = ISCSI_FLASHNODE_KEEPALIVE_TMO;
	} else if (attr == &dev_attr_fnode_port.attr) {
		param = ISCSI_FLASHNODE_PORT;
	} else if (attr == &dev_attr_fnode_ipaddress.attr) {
		param = ISCSI_FLASHNODE_IPADDR;
	} else if (attr == &dev_attr_fnode_redirect_ipaddr.attr) {
		param = ISCSI_FLASHNODE_REDIRECT_IPADDR;
	} else if (attr == &dev_attr_fnode_max_segment_size.attr) {
		param = ISCSI_FLASHNODE_MAX_SEGMENT_SIZE;
	} else if (attr == &dev_attr_fnode_local_port.attr) {
		param = ISCSI_FLASHNODE_LOCAL_PORT;
	} else if (attr == &dev_attr_fnode_ipv4_tos.attr) {
		param = ISCSI_FLASHNODE_IPV4_TOS;
	} else if (attr == &dev_attr_fnode_ipv6_traffic_class.attr) {
		param = ISCSI_FLASHNODE_IPV6_TC;
	} else if (attr == &dev_attr_fnode_ipv6_flow_label.attr) {
		param = ISCSI_FLASHNODE_IPV6_FLOW_LABEL;
	} else if (attr == &dev_attr_fnode_link_local_ipv6.attr) {
		param = ISCSI_FLASHNODE_LINK_LOCAL_IPV6;
	} else if (attr == &dev_attr_fnode_tcp_xmit_wsf.attr) {
		param = ISCSI_FLASHNODE_TCP_XMIT_WSF;
	} else if (attr == &dev_attr_fnode_tcp_recv_wsf.attr) {
		param = ISCSI_FLASHNODE_TCP_RECV_WSF;
	} else if (attr == &dev_attr_fnode_statsn.attr) {
		param = ISCSI_FLASHNODE_STATSN;
	} else if (attr == &dev_attr_fnode_exp_statsn.attr) {
		param = ISCSI_FLASHNODE_EXP_STATSN;
	} else {
		WARN_ONCE(1, "Invalid flashnode connection attr");
		return 0;
	}