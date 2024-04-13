static void ntlmssp_append_string(buffer_t *buf, size_t buffer_offset,
				  const char *str, bool unicode)
{
	struct ntlmssp_buffer buffer;
	unsigned int length;

	write_le32(&buffer.offset, buf->used);

	length = append_string(buf, str, FALSE, unicode);

	write_le16(&buffer.length, length);
	write_le16(&buffer.space, length);
	buffer_write(buf, buffer_offset, &buffer, sizeof(buffer));
}