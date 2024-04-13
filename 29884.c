static uint16_t smb1cli_alloc_mid(struct smbXcli_conn *conn)
{
	size_t num_pending = talloc_array_length(conn->pending);
	uint16_t result;

	if (conn->protocol == PROTOCOL_NONE) {
		/*
		 * This is what windows sends on the SMB1 Negprot request
		 * and some vendors reuse the SMB1 MID as SMB2 sequence number.
		 */
		return 0;
	}

	while (true) {
		size_t i;

		result = conn->smb1.mid++;
		if ((result == 0) || (result == 0xffff)) {
			continue;
		}

		for (i=0; i<num_pending; i++) {
			if (result == smb1cli_req_mid(conn->pending[i])) {
				break;
			}
		}

		if (i == num_pending) {
			return result;
		}
	}
}
