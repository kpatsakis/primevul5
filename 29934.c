static NTSTATUS smb2cli_inbuf_parse_compound(struct smbXcli_conn *conn,
					     uint8_t *buf,
					     size_t buflen,
					     TALLOC_CTX *mem_ctx,
					     struct iovec **piov, int *pnum_iov)
{
	struct iovec *iov;
	int num_iov = 0;
	size_t taken = 0;
	uint8_t *first_hdr = buf;
	size_t verified_buflen = 0;
	uint8_t *tf = NULL;
	size_t tf_len = 0;

	iov = talloc_array(mem_ctx, struct iovec, num_iov);
	if (iov == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	while (taken < buflen) {
		size_t len = buflen - taken;
		uint8_t *hdr = first_hdr + taken;
		struct iovec *cur;
		size_t full_size;
		size_t next_command_ofs;
		uint16_t body_size;
		struct iovec *iov_tmp;

		if (verified_buflen > taken) {
			len = verified_buflen - taken;
		} else {
			tf = NULL;
			tf_len = 0;
		}

		if (len < 4) {
			DEBUG(10, ("%d bytes left, expected at least %d\n",
				   (int)len, 4));
			goto inval;
		}
		if (IVAL(hdr, 0) == SMB2_TF_MAGIC) {
			struct smbXcli_session *s;
			uint64_t uid;
			struct iovec tf_iov[2];
			size_t enc_len;
			NTSTATUS status;

			if (len < SMB2_TF_HDR_SIZE) {
				DEBUG(10, ("%d bytes left, expected at least %d\n",
					   (int)len, SMB2_TF_HDR_SIZE));
				goto inval;
			}
			tf = hdr;
			tf_len = SMB2_TF_HDR_SIZE;
			taken += tf_len;

			hdr = first_hdr + taken;
			enc_len = IVAL(tf, SMB2_TF_MSG_SIZE);
			uid = BVAL(tf, SMB2_TF_SESSION_ID);

			if (len < SMB2_TF_HDR_SIZE + enc_len) {
				DEBUG(10, ("%d bytes left, expected at least %d\n",
					   (int)len,
					   (int)(SMB2_TF_HDR_SIZE + enc_len)));
				goto inval;
			}

			s = conn->sessions;
			for (; s; s = s->next) {
				if (s->smb2->session_id != uid) {
					continue;
				}
				break;
			}

			if (s == NULL) {
				DEBUG(10, ("unknown session_id %llu\n",
					   (unsigned long long)uid));
				goto inval;
			}

			tf_iov[0].iov_base = (void *)tf;
			tf_iov[0].iov_len = tf_len;
			tf_iov[1].iov_base = (void *)hdr;
			tf_iov[1].iov_len = enc_len;

			status = smb2_signing_decrypt_pdu(s->smb2->decryption_key,
							  conn->smb2.server.cipher,
							  tf_iov, 2);
			if (!NT_STATUS_IS_OK(status)) {
				TALLOC_FREE(iov);
				return status;
			}

			verified_buflen = taken + enc_len;
			len = enc_len;
		}

		/*
		 * We need the header plus the body length field
		 */

		if (len < SMB2_HDR_BODY + 2) {
			DEBUG(10, ("%d bytes left, expected at least %d\n",
				   (int)len, SMB2_HDR_BODY));
			goto inval;
		}
		if (IVAL(hdr, 0) != SMB2_MAGIC) {
			DEBUG(10, ("Got non-SMB2 PDU: %x\n",
				   IVAL(hdr, 0)));
			goto inval;
		}
		if (SVAL(hdr, 4) != SMB2_HDR_BODY) {
			DEBUG(10, ("Got HDR len %d, expected %d\n",
				   SVAL(hdr, 4), SMB2_HDR_BODY));
			goto inval;
		}

		full_size = len;
		next_command_ofs = IVAL(hdr, SMB2_HDR_NEXT_COMMAND);
		body_size = SVAL(hdr, SMB2_HDR_BODY);

		if (next_command_ofs != 0) {
			if (next_command_ofs < (SMB2_HDR_BODY + 2)) {
				goto inval;
			}
			if (next_command_ofs > full_size) {
				goto inval;
			}
			full_size = next_command_ofs;
		}
		if (body_size < 2) {
			goto inval;
		}
		body_size &= 0xfffe;

		if (body_size > (full_size - SMB2_HDR_BODY)) {
			goto inval;
		}

		iov_tmp = talloc_realloc(mem_ctx, iov, struct iovec,
					 num_iov + 4);
		if (iov_tmp == NULL) {
			TALLOC_FREE(iov);
			return NT_STATUS_NO_MEMORY;
		}
		iov = iov_tmp;
		cur = &iov[num_iov];
		num_iov += 4;

		cur[0].iov_base = tf;
		cur[0].iov_len  = tf_len;
		cur[1].iov_base = hdr;
		cur[1].iov_len  = SMB2_HDR_BODY;
		cur[2].iov_base = hdr + SMB2_HDR_BODY;
		cur[2].iov_len  = body_size;
		cur[3].iov_base = hdr + SMB2_HDR_BODY + body_size;
		cur[3].iov_len  = full_size - (SMB2_HDR_BODY + body_size);

		taken += full_size;
	}

	*piov = iov;
	*pnum_iov = num_iov;
	return NT_STATUS_OK;

inval:
	TALLOC_FREE(iov);
	return NT_STATUS_INVALID_NETWORK_RESPONSE;
}
