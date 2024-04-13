static NTSTATUS smb1cli_pull_raw_error(const uint8_t *hdr)
{
	uint32_t flags2 = SVAL(hdr, HDR_FLG2);
	NTSTATUS status = NT_STATUS(IVAL(hdr, HDR_RCLS));

	if (NT_STATUS_IS_OK(status)) {
		return NT_STATUS_OK;
	}

	if (flags2 & FLAGS2_32_BIT_ERROR_CODES) {
		return status;
	}

	return NT_STATUS_DOS(CVAL(hdr, HDR_RCLS), SVAL(hdr, HDR_ERR));
}
