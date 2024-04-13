NTSTATUS smb1cli_req_recv(struct tevent_req *req,
			  TALLOC_CTX *mem_ctx,
			  struct iovec **piov,
			  uint8_t **phdr,
			  uint8_t *pwct,
			  uint16_t **pvwv,
			  uint32_t *pvwv_offset,
			  uint32_t *pnum_bytes,
			  uint8_t **pbytes,
			  uint32_t *pbytes_offset,
			  uint8_t **pinbuf,
			  const struct smb1cli_req_expected_response *expected,
			  size_t num_expected)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);
	NTSTATUS status = NT_STATUS_OK;
	struct iovec *recv_iov = NULL;
	uint8_t *hdr = NULL;
	uint8_t wct = 0;
	uint32_t vwv_offset = 0;
	uint16_t *vwv = NULL;
	uint32_t num_bytes = 0;
	uint32_t bytes_offset = 0;
	uint8_t *bytes = NULL;
	size_t i;
	bool found_status = false;
	bool found_size = false;

	if (piov != NULL) {
		*piov = NULL;
	}
	if (phdr != NULL) {
		*phdr = 0;
	}
	if (pwct != NULL) {
		*pwct = 0;
	}
	if (pvwv != NULL) {
		*pvwv = NULL;
	}
	if (pvwv_offset != NULL) {
		*pvwv_offset = 0;
	}
	if (pnum_bytes != NULL) {
		*pnum_bytes = 0;
	}
	if (pbytes != NULL) {
		*pbytes = NULL;
	}
	if (pbytes_offset != NULL) {
		*pbytes_offset = 0;
	}
	if (pinbuf != NULL) {
		*pinbuf = NULL;
	}

	if (state->inbuf != NULL) {
		recv_iov = state->smb1.recv_iov;
		state->smb1.recv_iov = NULL;
		if (state->smb1.recv_cmd != SMBreadBraw) {
			hdr = (uint8_t *)recv_iov[0].iov_base;
			wct = recv_iov[1].iov_len/2;
			vwv = (uint16_t *)recv_iov[1].iov_base;
			vwv_offset = PTR_DIFF(vwv, hdr);
			num_bytes = recv_iov[2].iov_len;
			bytes = (uint8_t *)recv_iov[2].iov_base;
			bytes_offset = PTR_DIFF(bytes, hdr);
		}
	}

	if (tevent_req_is_nterror(req, &status)) {
		for (i=0; i < num_expected; i++) {
			if (NT_STATUS_EQUAL(status, expected[i].status)) {
				found_status = true;
				break;
			}
		}

		if (found_status) {
			return NT_STATUS_UNEXPECTED_NETWORK_ERROR;
		}

		return status;
	}

	if (num_expected == 0) {
		found_status = true;
		found_size = true;
	}

	status = state->smb1.recv_status;

	for (i=0; i < num_expected; i++) {
		if (!NT_STATUS_EQUAL(status, expected[i].status)) {
			continue;
		}

		found_status = true;
		if (expected[i].wct == 0) {
			found_size = true;
			break;
		}

		if (expected[i].wct == wct) {
			found_size = true;
			break;
		}
	}

	if (!found_status) {
		return status;
	}

	if (!found_size) {
		return NT_STATUS_INVALID_NETWORK_RESPONSE;
	}

	if (piov != NULL) {
		*piov = talloc_move(mem_ctx, &recv_iov);
	}

	if (phdr != NULL) {
		*phdr = hdr;
	}
	if (pwct != NULL) {
		*pwct = wct;
	}
	if (pvwv != NULL) {
		*pvwv = vwv;
	}
	if (pvwv_offset != NULL) {
		*pvwv_offset = vwv_offset;
	}
	if (pnum_bytes != NULL) {
		*pnum_bytes = num_bytes;
	}
	if (pbytes != NULL) {
		*pbytes = bytes;
	}
	if (pbytes_offset != NULL) {
		*pbytes_offset = bytes_offset;
	}
	if (pinbuf != NULL) {
		*pinbuf = state->inbuf;
	}

	return status;
}
