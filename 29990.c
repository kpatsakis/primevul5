NTSTATUS cli_dfs_get_referral(TALLOC_CTX *ctx,
			struct cli_state *cli,
			const char *path,
			struct client_dfs_referral **refs,
			size_t *num_refs,
			size_t *consumed)
{
	unsigned int param_len = 0;
	uint16_t recv_flags2;
	uint8_t *param = NULL;
	uint8_t *rdata = NULL;
	char *p;
	char *endp;
	smb_ucs2_t *path_ucs;
	char *consumed_path = NULL;
	uint16_t consumed_ucs;
	uint16_t num_referrals;
	struct client_dfs_referral *referrals = NULL;
	NTSTATUS status;
	TALLOC_CTX *frame = talloc_stackframe();

	*num_refs = 0;
	*refs = NULL;

	param = talloc_array(talloc_tos(), uint8_t, 2);
	if (!param) {
		status = NT_STATUS_NO_MEMORY;
		goto out;
	}
	SSVAL(param, 0, 0x03);	/* max referral level */

	param = trans2_bytes_push_str(param, smbXcli_conn_use_unicode(cli->conn),
				      path, strlen(path)+1,
				      NULL);
	if (!param) {
		status = NT_STATUS_NO_MEMORY;
		goto out;
	}
	param_len = talloc_get_size(param);
	path_ucs = (smb_ucs2_t *)&param[2];

	if (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_SMB2_02) {
		DATA_BLOB in_input_buffer;
		DATA_BLOB in_output_buffer = data_blob_null;
		DATA_BLOB out_input_buffer = data_blob_null;
		DATA_BLOB out_output_buffer = data_blob_null;

		in_input_buffer.data = param;
		in_input_buffer.length = param_len;

		status = smb2cli_ioctl(cli->conn,
				       cli->timeout,
				       cli->smb2.session,
				       cli->smb2.tcon,
				       UINT64_MAX, /* in_fid_persistent */
				       UINT64_MAX, /* in_fid_volatile */
				       FSCTL_DFS_GET_REFERRALS,
				       0, /* in_max_input_length */
				       &in_input_buffer,
				       CLI_BUFFER_SIZE, /* in_max_output_length */
				       &in_output_buffer,
				       SMB2_IOCTL_FLAG_IS_FSCTL,
				       talloc_tos(),
				       &out_input_buffer,
				       &out_output_buffer);
		if (!NT_STATUS_IS_OK(status)) {
			goto out;
		}

		if (out_output_buffer.length < 4) {
			status = NT_STATUS_INVALID_NETWORK_RESPONSE;
			goto out;
		}

		recv_flags2 = FLAGS2_UNICODE_STRINGS;
		rdata = out_output_buffer.data;
		endp = (char *)rdata + out_output_buffer.length;
	} else {
		unsigned int data_len = 0;
		uint16_t setup[1];

		SSVAL(setup, 0, TRANSACT2_GET_DFS_REFERRAL);

		status = cli_trans(talloc_tos(), cli, SMBtrans2,
				   NULL, 0xffff, 0, 0,
				   setup, 1, 0,
				   param, param_len, 2,
				   NULL, 0, CLI_BUFFER_SIZE,
				   &recv_flags2,
				   NULL, 0, NULL, /* rsetup */
				   NULL, 0, NULL,
				   &rdata, 4, &data_len);
		if (!NT_STATUS_IS_OK(status)) {
			goto out;
		}

		endp = (char *)rdata + data_len;
	}

	consumed_ucs  = SVAL(rdata, 0);
	num_referrals = SVAL(rdata, 2);

	/* consumed_ucs is the number of bytes
	 * of the UCS2 path consumed not counting any
	 * terminating null. We need to convert
	 * back to unix charset and count again
	 * to get the number of bytes consumed from
	 * the incoming path. */

	errno = 0;
	if (pull_string_talloc(talloc_tos(),
			NULL,
			0,
			&consumed_path,
			path_ucs,
			consumed_ucs,
			STR_UNICODE) == 0) {
		if (errno != 0) {
			status = map_nt_error_from_unix(errno);
		} else {
			status = NT_STATUS_INVALID_NETWORK_RESPONSE;
		}
		goto out;
	}
	if (consumed_path == NULL) {
		status = map_nt_error_from_unix(errno);
		goto out;
	}
	*consumed = strlen(consumed_path);

	if (num_referrals != 0) {
		uint16_t ref_version;
		uint16_t ref_size;
		int i;
		uint16_t node_offset;

		referrals = talloc_array(ctx, struct client_dfs_referral,
					 num_referrals);

		if (!referrals) {
			status = NT_STATUS_NO_MEMORY;
			goto out;
		}
		/* start at the referrals array */

		p = (char *)rdata+8;
		for (i=0; i<num_referrals && p < endp; i++) {
			if (p + 18 > endp) {
				goto out;
			}
			ref_version = SVAL(p, 0);
			ref_size    = SVAL(p, 2);
			node_offset = SVAL(p, 16);

			if (ref_version != 3) {
				p += ref_size;
				continue;
			}

			referrals[i].proximity = SVAL(p, 8);
			referrals[i].ttl       = SVAL(p, 10);

			if (p + node_offset > endp) {
				status = NT_STATUS_INVALID_NETWORK_RESPONSE;
				goto out;
			}
			clistr_pull_talloc(referrals,
					   (const char *)rdata,
					   recv_flags2,
					   &referrals[i].dfspath,
					   p+node_offset,
					   PTR_DIFF(endp, p+node_offset),
					   STR_TERMINATE|STR_UNICODE);

			if (!referrals[i].dfspath) {
				status = map_nt_error_from_unix(errno);
				goto out;
			}
			p += ref_size;
		}
		if (i < num_referrals) {
			status = NT_STATUS_INVALID_NETWORK_RESPONSE;
			goto out;
		}
	}

	*num_refs = num_referrals;
	*refs = referrals;

  out:

	TALLOC_FREE(frame);
	return status;
}
