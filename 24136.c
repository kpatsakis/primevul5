buffer_append_asn1_length(buffer_t *buf, unsigned int length)
{
	if (length < 0x80) {
		buffer_append_c(buf, length);
	} else if (length < 0x100) {
		buffer_append_c(buf, 0x81);
		buffer_append_c(buf, length);
	} else {
		buffer_append_c(buf, 0x82);
		buffer_append_c(buf, length >> 8);
		buffer_append_c(buf, length & 0xff);
	}
}