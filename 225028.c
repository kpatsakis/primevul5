	NULL,
};

static umode_t iscsi_session_attr_is_visible(struct kobject *kobj,
					    struct attribute *attr, int i)
{
	struct device *cdev = container_of(kobj, struct device, kobj);
	struct iscsi_cls_session *session = transport_class_to_session(cdev);
	struct iscsi_transport *t = session->transport;
	int param;

	if (attr == &dev_attr_sess_initial_r2t.attr)
		param = ISCSI_PARAM_INITIAL_R2T_EN;
	else if (attr == &dev_attr_sess_max_outstanding_r2t.attr)
		param = ISCSI_PARAM_MAX_R2T;
	else if (attr == &dev_attr_sess_immediate_data.attr)
		param = ISCSI_PARAM_IMM_DATA_EN;
	else if (attr == &dev_attr_sess_first_burst_len.attr)
		param = ISCSI_PARAM_FIRST_BURST;
	else if (attr == &dev_attr_sess_max_burst_len.attr)
		param = ISCSI_PARAM_MAX_BURST;
	else if (attr == &dev_attr_sess_data_pdu_in_order.attr)
		param = ISCSI_PARAM_PDU_INORDER_EN;
	else if (attr == &dev_attr_sess_data_seq_in_order.attr)
		param = ISCSI_PARAM_DATASEQ_INORDER_EN;
	else if (attr == &dev_attr_sess_erl.attr)
		param = ISCSI_PARAM_ERL;
	else if (attr == &dev_attr_sess_targetname.attr)
		param = ISCSI_PARAM_TARGET_NAME;
	else if (attr == &dev_attr_sess_tpgt.attr)
		param = ISCSI_PARAM_TPGT;
	else if (attr == &dev_attr_sess_chap_in_idx.attr)
		param = ISCSI_PARAM_CHAP_IN_IDX;
	else if (attr == &dev_attr_sess_chap_out_idx.attr)
		param = ISCSI_PARAM_CHAP_OUT_IDX;
	else if (attr == &dev_attr_sess_password.attr)
		param = ISCSI_PARAM_USERNAME;
	else if (attr == &dev_attr_sess_password_in.attr)
		param = ISCSI_PARAM_USERNAME_IN;
	else if (attr == &dev_attr_sess_username.attr)
		param = ISCSI_PARAM_PASSWORD;
	else if (attr == &dev_attr_sess_username_in.attr)
		param = ISCSI_PARAM_PASSWORD_IN;
	else if (attr == &dev_attr_sess_fast_abort.attr)
		param = ISCSI_PARAM_FAST_ABORT;
	else if (attr == &dev_attr_sess_abort_tmo.attr)
		param = ISCSI_PARAM_ABORT_TMO;
	else if (attr == &dev_attr_sess_lu_reset_tmo.attr)
		param = ISCSI_PARAM_LU_RESET_TMO;
	else if (attr == &dev_attr_sess_tgt_reset_tmo.attr)
		param = ISCSI_PARAM_TGT_RESET_TMO;
	else if (attr == &dev_attr_sess_ifacename.attr)
		param = ISCSI_PARAM_IFACE_NAME;
	else if (attr == &dev_attr_sess_initiatorname.attr)
		param = ISCSI_PARAM_INITIATOR_NAME;
	else if (attr == &dev_attr_sess_targetalias.attr)
		param = ISCSI_PARAM_TARGET_ALIAS;
	else if (attr == &dev_attr_sess_boot_root.attr)
		param = ISCSI_PARAM_BOOT_ROOT;
	else if (attr == &dev_attr_sess_boot_nic.attr)
		param = ISCSI_PARAM_BOOT_NIC;
	else if (attr == &dev_attr_sess_boot_target.attr)
		param = ISCSI_PARAM_BOOT_TARGET;
	else if (attr == &dev_attr_sess_auto_snd_tgt_disable.attr)
		param = ISCSI_PARAM_AUTO_SND_TGT_DISABLE;
	else if (attr == &dev_attr_sess_discovery_session.attr)
		param = ISCSI_PARAM_DISCOVERY_SESS;
	else if (attr == &dev_attr_sess_portal_type.attr)
		param = ISCSI_PARAM_PORTAL_TYPE;
	else if (attr == &dev_attr_sess_chap_auth.attr)
		param = ISCSI_PARAM_CHAP_AUTH_EN;
	else if (attr == &dev_attr_sess_discovery_logout.attr)
		param = ISCSI_PARAM_DISCOVERY_LOGOUT_EN;
	else if (attr == &dev_attr_sess_bidi_chap.attr)
		param = ISCSI_PARAM_BIDI_CHAP_EN;
	else if (attr == &dev_attr_sess_discovery_auth_optional.attr)
		param = ISCSI_PARAM_DISCOVERY_AUTH_OPTIONAL;
	else if (attr == &dev_attr_sess_def_time2wait.attr)
		param = ISCSI_PARAM_DEF_TIME2WAIT;
	else if (attr == &dev_attr_sess_def_time2retain.attr)
		param = ISCSI_PARAM_DEF_TIME2RETAIN;
	else if (attr == &dev_attr_sess_isid.attr)
		param = ISCSI_PARAM_ISID;
	else if (attr == &dev_attr_sess_tsid.attr)
		param = ISCSI_PARAM_TSID;
	else if (attr == &dev_attr_sess_def_taskmgmt_tmo.attr)
		param = ISCSI_PARAM_DEF_TASKMGMT_TMO;
	else if (attr == &dev_attr_sess_discovery_parent_idx.attr)
		param = ISCSI_PARAM_DISCOVERY_PARENT_IDX;
	else if (attr == &dev_attr_sess_discovery_parent_type.attr)
		param = ISCSI_PARAM_DISCOVERY_PARENT_TYPE;
	else if (attr == &dev_attr_priv_sess_recovery_tmo.attr)
		return S_IRUGO | S_IWUSR;
	else if (attr == &dev_attr_priv_sess_state.attr)
		return S_IRUGO;
	else if (attr == &dev_attr_priv_sess_creator.attr)
		return S_IRUGO;
	else if (attr == &dev_attr_priv_sess_target_id.attr)
		return S_IRUGO;
	else {
		WARN_ONCE(1, "Invalid session attr");
		return 0;
	}