static void ntlmssp_append_target_info(buffer_t *buf, size_t buffer_offset, ...)
{
	struct ntlmssp_v2_target_info info;
	struct ntlmssp_buffer buffer;
	va_list args;
	unsigned int length, total_length = 0;
	int type;

	write_le32(&buffer.offset, buf->used);

	va_start(args, buffer_offset);

	do {
		const char *data;
		type = va_arg(args, int);

		i_zero(&info);
		write_le16(&info.type, type);

		switch (type) {
			case NTPLMSSP_V2_TARGET_END:
				buffer_append(buf, &info, sizeof(info));
				length = sizeof(info);
				break;
			case NTPLMSSP_V2_TARGET_SERVER:
			case NTPLMSSP_V2_TARGET_DOMAIN:
			case NTPLMSSP_V2_TARGET_FQDN:
			case NTPLMSSP_V2_TARGET_DNS:
				data = va_arg(args, const char *);
				write_le16(&info.length,
					   strlen(data) * sizeof(ucs2le_t));
				buffer_append(buf, &info, sizeof(info));
				length = append_string(buf, data, FALSE, TRUE) +
					 sizeof(info);
				break;
			default:
				i_panic("Invalid NTLM target info block type "
					"%u", type);
		}

		total_length += length;
	
	} while (type != NTPLMSSP_V2_TARGET_END);

	va_end(args);

	write_le16(&buffer.length, total_length);
	write_le16(&buffer.space, total_length);
	buffer_write(buf, buffer_offset, &buffer, sizeof(buffer));
}