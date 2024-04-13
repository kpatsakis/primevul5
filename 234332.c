static unsigned int append_string(buffer_t *buf, const char *str, 
				  bool ucase, bool unicode)
{
	unsigned int length = 0;

	for ( ; *str != '\0'; str++) {
		buffer_append_c(buf, ucase ? i_toupper(*str) : *str);
		if (unicode) {
			buffer_append_c(buf, 0);
			length++; 
		}
		length++;
	}

	return length;
}