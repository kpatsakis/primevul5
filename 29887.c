bool smb1cli_conn_check_signing(struct smbXcli_conn *conn,
				const uint8_t *buf, uint32_t seqnum)
{
	const uint8_t *hdr = buf + NBT_HDR_SIZE;
	size_t len = smb_len_nbt(buf);

	return smb_signing_check_pdu(conn->smb1.signing, hdr, len, seqnum);
}
