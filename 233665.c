static int utf8_encoded_to_unichar(const char *str)
{
	int unichar;
	int len;
	int i;

	len = utf8_encoded_expected_len(str);
	switch (len) {
	case 1:
		return (int)str[0];
	case 2:
		unichar = str[0] & 0x1f;
		break;
	case 3:
		unichar = (int)str[0] & 0x0f;
		break;
	case 4:
		unichar = (int)str[0] & 0x07;
		break;
	case 5:
		unichar = (int)str[0] & 0x03;
		break;
	case 6:
		unichar = (int)str[0] & 0x01;
		break;
	default:
		return -1;
	}

	for (i = 1; i < len; i++) {
		if (((int)str[i] & 0xc0) != 0x80)
			return -1;
		unichar <<= 6;
		unichar |= (int)str[i] & 0x3f;
	}

	return unichar;
}