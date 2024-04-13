mech_rpa_build_token2(struct rpa_auth_request *request, size_t *size)
{
	const struct auth_settings *set = request->auth_request.set;
	unsigned int realms_len, length;
	string_t *realms;
	buffer_t *buf;
	unsigned char timestamp[RPA_TIMESTAMP_LEN / 2];
	const char *const *tmp;

	realms = t_str_new(64);
	for (tmp = set->realms_arr; *tmp != NULL; tmp++) {
		rpa_add_realm(realms, *tmp, request->auth_request.service);
	}

	if (str_len(realms) == 0) {
		rpa_add_realm(realms, *set->default_realm != '\0' ?
			      set->default_realm : my_hostname,
			      request->auth_request.service);
	}

	realms_len = str_len(realms) - 1;
        length = sizeof(rpa_oid) + 3 + RPA_SCHALLENGE_LEN +
		RPA_TIMESTAMP_LEN + 2 + realms_len;

	buf = buffer_create_dynamic(request->pool, length + 4);

	buffer_append_c(buf, ASN1_APPLICATION);
	buffer_append_asn1_length(buf, length);
	buffer_append(buf, rpa_oid, sizeof(rpa_oid));

	/* Protocol version */
	buffer_append_c(buf, 3);
	buffer_append_c(buf, 0);

	/* Service challenge */
	request->service_challenge =
		p_malloc(request->pool, RPA_SCHALLENGE_LEN);
	random_fill(request->service_challenge, RPA_SCHALLENGE_LEN);
	buffer_append_c(buf, RPA_SCHALLENGE_LEN);
	buffer_append(buf, request->service_challenge, RPA_SCHALLENGE_LEN);

	/* Timestamp, looks like clients accept anything we send */
	random_fill(timestamp, sizeof(timestamp));
	request->service_timestamp = p_malloc(request->pool, RPA_TIMESTAMP_LEN);
	memcpy(request->service_timestamp,
	       binary_to_hex(timestamp, sizeof(timestamp)),
	       RPA_TIMESTAMP_LEN);
	buffer_append(buf, request->service_timestamp, RPA_TIMESTAMP_LEN);

	/* Realm list */
	buffer_append_c(buf, realms_len >> 8);
	buffer_append_c(buf, realms_len & 0xff);
	buffer_append(buf, str_c(realms), realms_len);

	*size = buf->used;
	return buffer_free_without_data(&buf);
}