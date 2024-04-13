static NTSTATUS smbXcli_negprot_dispatch_incoming(struct smbXcli_conn *conn,
						  TALLOC_CTX *tmp_mem,
						  uint8_t *inbuf)
{
	size_t num_pending = talloc_array_length(conn->pending);
	struct tevent_req *subreq;
	struct smbXcli_req_state *substate;
	struct tevent_req *req;
	uint32_t protocol_magic;
	size_t inbuf_len = smb_len_nbt(inbuf);

	if (num_pending != 1) {
		return NT_STATUS_INTERNAL_ERROR;
	}

	if (inbuf_len < 4) {
		return NT_STATUS_INVALID_NETWORK_RESPONSE;
	}

	subreq = conn->pending[0];
	substate = tevent_req_data(subreq, struct smbXcli_req_state);
	req = tevent_req_callback_data(subreq, struct tevent_req);

	protocol_magic = IVAL(inbuf, 4);

	switch (protocol_magic) {
	case SMB_MAGIC:
		tevent_req_set_callback(subreq, smbXcli_negprot_smb1_done, req);
		conn->dispatch_incoming = smb1cli_conn_dispatch_incoming;
		return smb1cli_conn_dispatch_incoming(conn, tmp_mem, inbuf);

	case SMB2_MAGIC:
		if (substate->smb2.recv_iov == NULL) {
			/*
			 * For the SMB1 negprot we have move it.
			 */
			substate->smb2.recv_iov = substate->smb1.recv_iov;
			substate->smb1.recv_iov = NULL;
		}

		/*
		 * we got an SMB2 answer, which consumed sequence number 0
		 * so we need to use 1 as the next one.
		 *
		 * we also need to set the current credits to 0
		 * as we consumed the initial one. The SMB2 answer
		 * hopefully grant us a new credit.
		 */
		conn->smb2.mid = 1;
		conn->smb2.cur_credits = 0;
		tevent_req_set_callback(subreq, smbXcli_negprot_smb2_done, req);
		conn->dispatch_incoming = smb2cli_conn_dispatch_incoming;
		return smb2cli_conn_dispatch_incoming(conn, tmp_mem, inbuf);
	}

	DEBUG(10, ("Got non-SMB PDU\n"));
	return NT_STATUS_INVALID_NETWORK_RESPONSE;
}
