BOOL license_send_valid_client_error_packet(rdpRdp* rdp)
{
	rdpLicense* license = rdp->license;
	wStream* s = license_send_stream_init(license);
	if (!s)
		return FALSE;

	DEBUG_LICENSE("Sending Error Alert Packet");
	Stream_Write_UINT32(s, STATUS_VALID_CLIENT); /* dwErrorCode */
	Stream_Write_UINT32(s, ST_NO_TRANSITION);    /* dwStateTransition */

	if (license_write_binary_blob(s, license->ErrorInfo))
		return license_send(license, s, ERROR_ALERT);

	Stream_Release(s);
	return FALSE;
}