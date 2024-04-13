void iwl_fw_error_print_fseq_regs(struct iwl_fw_runtime *fwrt)
{
	struct iwl_trans *trans = fwrt->trans;
	unsigned long flags;
	int i;
	struct {
		u32 addr;
		const char *str;
	} fseq_regs[] = {
		FSEQ_REG(FSEQ_ERROR_CODE),
		FSEQ_REG(FSEQ_TOP_INIT_VERSION),
		FSEQ_REG(FSEQ_CNVIO_INIT_VERSION),
		FSEQ_REG(FSEQ_OTP_VERSION),
		FSEQ_REG(FSEQ_TOP_CONTENT_VERSION),
		FSEQ_REG(FSEQ_ALIVE_TOKEN),
		FSEQ_REG(FSEQ_CNVI_ID),
		FSEQ_REG(FSEQ_CNVR_ID),
		FSEQ_REG(CNVI_AUX_MISC_CHIP),
		FSEQ_REG(CNVR_AUX_MISC_CHIP),
		FSEQ_REG(CNVR_SCU_SD_REGS_SD_REG_DIG_DCDC_VTRIM),
		FSEQ_REG(CNVR_SCU_SD_REGS_SD_REG_ACTIVE_VDIG_MIRROR),
	};

	if (!iwl_trans_grab_nic_access(trans, &flags))
		return;

	IWL_ERR(fwrt, "Fseq Registers:\n");

	for (i = 0; i < ARRAY_SIZE(fseq_regs); i++)
		IWL_ERR(fwrt, "0x%08X | %s\n",
			iwl_read_prph_no_grab(trans, fseq_regs[i].addr),
			fseq_regs[i].str);

	iwl_trans_release_nic_access(trans, &flags);
}