static NTSTATUS smb1cli_inbuf_parse_chain(uint8_t *buf, TALLOC_CTX *mem_ctx,
					  struct iovec **piov, int *pnum_iov)
{
	struct iovec *iov;
	int num_iov;
	size_t buflen;
	size_t taken;
	size_t remaining;
	uint8_t *hdr;
	uint8_t cmd;
	uint32_t wct_ofs;
	NTSTATUS status;
	size_t min_size = MIN_SMB_SIZE;

	buflen = smb_len_tcp(buf);
	taken = 0;

	hdr = buf + NBT_HDR_SIZE;

	status = smb1cli_pull_raw_error(hdr);
	if (NT_STATUS_IS_ERR(status)) {
		/*
		 * This is an ugly hack to support OS/2
		 * which skips the byte_count in the DATA block
		 * on some error responses.
		 *
		 * See bug #9096
		 */
		min_size -= sizeof(uint16_t);
	}

	if (buflen < min_size) {
		return NT_STATUS_INVALID_NETWORK_RESPONSE;
	}

	/*
	 * This returns iovec elements in the following order:
	 *
	 * - SMB header
	 *
	 * - Parameter Block
	 * - Data Block
	 *
	 * - Parameter Block
	 * - Data Block
	 *
	 * - Parameter Block
	 * - Data Block
	 */
	num_iov = 1;

	iov = talloc_array(mem_ctx, struct iovec, num_iov);
	if (iov == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	iov[0].iov_base = hdr;
	iov[0].iov_len = HDR_WCT;
	taken += HDR_WCT;

	cmd = CVAL(hdr, HDR_COM);
	wct_ofs = HDR_WCT;

	while (true) {
		size_t len = buflen - taken;
		struct iovec *cur;
		struct iovec *iov_tmp;
		uint8_t wct;
		uint32_t bcc_ofs;
		uint16_t bcc;
		size_t needed;

		/*
		 * we need at least WCT
		 */
		needed = sizeof(uint8_t);
		if (len < needed) {
			DEBUG(10, ("%s: %d bytes left, expected at least %d\n",
				   __location__, (int)len, (int)needed));
			goto inval;
		}

		/*
		 * Now we check if the specified words are there
		 */
		wct = CVAL(hdr, wct_ofs);
		needed += wct * sizeof(uint16_t);
		if (len < needed) {
			DEBUG(10, ("%s: %d bytes left, expected at least %d\n",
				   __location__, (int)len, (int)needed));
			goto inval;
		}

		if ((num_iov == 1) &&
		    (len == needed) &&
		    NT_STATUS_IS_ERR(status))
		{
			/*
			 * This is an ugly hack to support OS/2
			 * which skips the byte_count in the DATA block
			 * on some error responses.
			 *
			 * See bug #9096
			 */
			iov_tmp = talloc_realloc(mem_ctx, iov, struct iovec,
						 num_iov + 2);
			if (iov_tmp == NULL) {
				TALLOC_FREE(iov);
				return NT_STATUS_NO_MEMORY;
			}
			iov = iov_tmp;
			cur = &iov[num_iov];
			num_iov += 2;

			cur[0].iov_len = 0;
			cur[0].iov_base = hdr + (wct_ofs + sizeof(uint8_t));
			cur[1].iov_len = 0;
			cur[1].iov_base = cur[0].iov_base;

			taken += needed;
			break;
		}

		/*
		 * we need at least BCC
		 */
		needed += sizeof(uint16_t);
		if (len < needed) {
			DEBUG(10, ("%s: %d bytes left, expected at least %d\n",
				   __location__, (int)len, (int)needed));
			goto inval;
		}

		/*
		 * Now we check if the specified bytes are there
		 */
		bcc_ofs = wct_ofs + sizeof(uint8_t) + wct * sizeof(uint16_t);
		bcc = SVAL(hdr, bcc_ofs);
		needed += bcc * sizeof(uint8_t);
		if (len < needed) {
			DEBUG(10, ("%s: %d bytes left, expected at least %d\n",
				   __location__, (int)len, (int)needed));
			goto inval;
		}

		/*
		 * we allocate 2 iovec structures for words and bytes
		 */
		iov_tmp = talloc_realloc(mem_ctx, iov, struct iovec,
					 num_iov + 2);
		if (iov_tmp == NULL) {
			TALLOC_FREE(iov);
			return NT_STATUS_NO_MEMORY;
		}
		iov = iov_tmp;
		cur = &iov[num_iov];
		num_iov += 2;

		cur[0].iov_len = wct * sizeof(uint16_t);
		cur[0].iov_base = hdr + (wct_ofs + sizeof(uint8_t));
		cur[1].iov_len = bcc * sizeof(uint8_t);
		cur[1].iov_base = hdr + (bcc_ofs + sizeof(uint16_t));

		taken += needed;

		if (!smb1cli_is_andx_req(cmd)) {
			/*
			 * If the current command does not have AndX chanining
			 * we are done.
			 */
			break;
		}

		if (wct == 0 && bcc == 0) {
			/*
			 * An empty response also ends the chain,
			 * most likely with an error.
			 */
			break;
		}

		if (wct < 2) {
			DEBUG(10, ("%s: wct[%d] < 2 for cmd[0x%02X]\n",
				   __location__, (int)wct, (int)cmd));
			goto inval;
		}
		cmd = CVAL(cur[0].iov_base, 0);
		if (cmd == 0xFF) {
			/*
			 * If it is the end of the chain we are also done.
			 */
			break;
		}
		wct_ofs = SVAL(cur[0].iov_base, 2);

		if (wct_ofs < taken) {
			return NT_STATUS_INVALID_NETWORK_RESPONSE;
		}
		if (wct_ofs > buflen) {
			return NT_STATUS_INVALID_NETWORK_RESPONSE;
		}

		/*
		 * we consumed everything up to the start of the next
		 * parameter block.
		 */
		taken = wct_ofs;
	}

	remaining = buflen - taken;

	if (remaining > 0 && num_iov >= 3) {
		/*
		 * The last DATA block gets the remaining
		 * bytes, this is needed to support
		 * CAP_LARGE_WRITEX and CAP_LARGE_READX.
		 */
		iov[num_iov-1].iov_len += remaining;
	}

	*piov = iov;
	*pnum_iov = num_iov;
	return NT_STATUS_OK;

inval:
	TALLOC_FREE(iov);
	return NT_STATUS_INVALID_NETWORK_RESPONSE;
}
