};

static umode_t iscsi_flashnode_sess_attr_is_visible(struct kobject *kobj,
						    struct attribute *attr,
						    int i)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct iscsi_bus_flash_session *fnode_sess =
						iscsi_dev_to_flash_session(dev);
	struct iscsi_transport *t = fnode_sess->transport;
	int param;

	if (attr == &dev_attr_fnode_auto_snd_tgt_disable.attr) {
		param = ISCSI_FLASHNODE_AUTO_SND_TGT_DISABLE;
	} else if (attr == &dev_attr_fnode_discovery_session.attr) {
		param = ISCSI_FLASHNODE_DISCOVERY_SESS;
	} else if (attr == &dev_attr_fnode_portal_type.attr) {
		param = ISCSI_FLASHNODE_PORTAL_TYPE;
	} else if (attr == &dev_attr_fnode_entry_enable.attr) {
		param = ISCSI_FLASHNODE_ENTRY_EN;
	} else if (attr == &dev_attr_fnode_immediate_data.attr) {
		param = ISCSI_FLASHNODE_IMM_DATA_EN;
	} else if (attr == &dev_attr_fnode_initial_r2t.attr) {
		param = ISCSI_FLASHNODE_INITIAL_R2T_EN;
	} else if (attr == &dev_attr_fnode_data_seq_in_order.attr) {
		param = ISCSI_FLASHNODE_DATASEQ_INORDER;
	} else if (attr == &dev_attr_fnode_data_pdu_in_order.attr) {
		param = ISCSI_FLASHNODE_PDU_INORDER;
	} else if (attr == &dev_attr_fnode_chap_auth.attr) {
		param = ISCSI_FLASHNODE_CHAP_AUTH_EN;
	} else if (attr == &dev_attr_fnode_discovery_logout.attr) {
		param = ISCSI_FLASHNODE_DISCOVERY_LOGOUT_EN;
	} else if (attr == &dev_attr_fnode_bidi_chap.attr) {
		param = ISCSI_FLASHNODE_BIDI_CHAP_EN;
	} else if (attr == &dev_attr_fnode_discovery_auth_optional.attr) {
		param = ISCSI_FLASHNODE_DISCOVERY_AUTH_OPTIONAL;
	} else if (attr == &dev_attr_fnode_erl.attr) {
		param = ISCSI_FLASHNODE_ERL;
	} else if (attr == &dev_attr_fnode_first_burst_len.attr) {
		param = ISCSI_FLASHNODE_FIRST_BURST;
	} else if (attr == &dev_attr_fnode_def_time2wait.attr) {
		param = ISCSI_FLASHNODE_DEF_TIME2WAIT;
	} else if (attr == &dev_attr_fnode_def_time2retain.attr) {
		param = ISCSI_FLASHNODE_DEF_TIME2RETAIN;
	} else if (attr == &dev_attr_fnode_max_outstanding_r2t.attr) {
		param = ISCSI_FLASHNODE_MAX_R2T;
	} else if (attr == &dev_attr_fnode_isid.attr) {
		param = ISCSI_FLASHNODE_ISID;
	} else if (attr == &dev_attr_fnode_tsid.attr) {
		param = ISCSI_FLASHNODE_TSID;
	} else if (attr == &dev_attr_fnode_max_burst_len.attr) {
		param = ISCSI_FLASHNODE_MAX_BURST;
	} else if (attr == &dev_attr_fnode_def_taskmgmt_tmo.attr) {
		param = ISCSI_FLASHNODE_DEF_TASKMGMT_TMO;
	} else if (attr == &dev_attr_fnode_targetalias.attr) {
		param = ISCSI_FLASHNODE_ALIAS;
	} else if (attr == &dev_attr_fnode_targetname.attr) {
		param = ISCSI_FLASHNODE_NAME;
	} else if (attr == &dev_attr_fnode_tpgt.attr) {
		param = ISCSI_FLASHNODE_TPGT;
	} else if (attr == &dev_attr_fnode_discovery_parent_idx.attr) {
		param = ISCSI_FLASHNODE_DISCOVERY_PARENT_IDX;
	} else if (attr == &dev_attr_fnode_discovery_parent_type.attr) {
		param = ISCSI_FLASHNODE_DISCOVERY_PARENT_TYPE;
	} else if (attr == &dev_attr_fnode_chap_in_idx.attr) {
		param = ISCSI_FLASHNODE_CHAP_IN_IDX;
	} else if (attr == &dev_attr_fnode_chap_out_idx.attr) {
		param = ISCSI_FLASHNODE_CHAP_OUT_IDX;
	} else if (attr == &dev_attr_fnode_username.attr) {
		param = ISCSI_FLASHNODE_USERNAME;
	} else if (attr == &dev_attr_fnode_username_in.attr) {
		param = ISCSI_FLASHNODE_USERNAME_IN;
	} else if (attr == &dev_attr_fnode_password.attr) {
		param = ISCSI_FLASHNODE_PASSWORD;
	} else if (attr == &dev_attr_fnode_password_in.attr) {
		param = ISCSI_FLASHNODE_PASSWORD_IN;
	} else if (attr == &dev_attr_fnode_is_boot_target.attr) {
		param = ISCSI_FLASHNODE_IS_BOOT_TGT;
	} else {
		WARN_ONCE(1, "Invalid flashnode session attr");
		return 0;
	}
