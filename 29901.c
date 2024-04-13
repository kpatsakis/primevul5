static NTSTATUS smb1cli_conn_signv(struct smbXcli_conn *conn,
				   struct iovec *iov, int iov_count,
				   uint32_t *seqnum,
				   bool one_way_seqnum)
{
	TALLOC_CTX *frame = NULL;
	uint8_t *buf;

	/*
	 * Obvious optimization: Make cli_calculate_sign_mac work with struct
	 * iovec directly. MD5Update would do that just fine.
	 */

	if (iov_count < 4) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[0].iov_len != NBT_HDR_SIZE) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[1].iov_len != (MIN_SMB_SIZE-sizeof(uint16_t))) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[2].iov_len > (0xFF * sizeof(uint16_t))) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}
	if (iov[3].iov_len != sizeof(uint16_t)) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}

	frame = talloc_stackframe();

	buf = smbXcli_iov_concat(frame, &iov[1], iov_count - 1);
	if (buf == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	*seqnum = smb_signing_next_seqnum(conn->smb1.signing,
					  one_way_seqnum);
	smb_signing_sign_pdu(conn->smb1.signing,
			     buf, talloc_get_size(buf),
			     *seqnum);
	memcpy(iov[1].iov_base, buf, iov[1].iov_len);

	TALLOC_FREE(frame);
	return NT_STATUS_OK;
}
