int iwl_fw_dbg_ini_collect(struct iwl_fw_runtime *fwrt, u32 legacy_trigger_id)
{
	int id;

	switch (legacy_trigger_id) {
	case FW_DBG_TRIGGER_FW_ASSERT:
	case FW_DBG_TRIGGER_ALIVE_TIMEOUT:
	case FW_DBG_TRIGGER_DRIVER:
		id = IWL_FW_TRIGGER_ID_FW_ASSERT;
		break;
	case FW_DBG_TRIGGER_USER:
		id = IWL_FW_TRIGGER_ID_USER_TRIGGER;
		break;
	default:
		return -EIO;
	}

	return _iwl_fw_dbg_ini_collect(fwrt, id);
}