static int hci_outgoing_auth_needed(struct hci_dev *hdev,
				    struct hci_conn *conn)
{
	if (conn->state != BT_CONFIG || !conn->out)
		return 0;

	if (conn->pending_sec_level == BT_SECURITY_SDP)
		return 0;

	/* Only request authentication for SSP connections or non-SSP
	 * devices with sec_level MEDIUM or HIGH or if MITM protection
	 * is requested.
	 */
	if (!hci_conn_ssp_enabled(conn) && !(conn->auth_type & 0x01) &&
	    conn->pending_sec_level != BT_SECURITY_FIPS &&
	    conn->pending_sec_level != BT_SECURITY_HIGH &&
	    conn->pending_sec_level != BT_SECURITY_MEDIUM)
		return 0;

	return 1;
}