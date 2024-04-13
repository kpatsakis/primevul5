ntlmssp_create_challenge(pool_t pool, const struct ntlmssp_request *request,
			 size_t *size)
{
	buffer_t *buf;
	uint32_t flags = ntlmssp_flags(read_le32(&request->flags));
	bool unicode = (flags & NTLMSSP_NEGOTIATE_UNICODE) != 0;
	struct ntlmssp_challenge c;

	buf = buffer_create_dynamic(pool, sizeof(struct ntlmssp_challenge));

	i_zero(&c);
	write_le64(&c.magic, NTLMSSP_MAGIC);
	write_le32(&c.type, NTLMSSP_MSG_TYPE2);
	write_le32(&c.flags, flags);
	random_fill(c.challenge, sizeof(c.challenge));

	buffer_write(buf, 0, &c, sizeof(c));

	if ((flags & NTLMSSP_TARGET_TYPE_SERVER) != 0)
		ntlmssp_append_string(buf,
			offsetof(struct ntlmssp_challenge, target_name),
			my_hostname, unicode);

	ntlmssp_append_target_info(buf, offsetof(struct ntlmssp_challenge,
						 target_info),
				   NTPLMSSP_V2_TARGET_FQDN, my_hostname,
				   NTPLMSSP_V2_TARGET_END);

	*size = buf->used;
	return buffer_free_without_data(&buf);
}