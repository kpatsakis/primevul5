int parse_checksum_choice(void)
{
	char *cp = checksum_choice ? strchr(checksum_choice, ',') : NULL;
	if (cp) {
		xfersum_type = parse_csum_name(checksum_choice, cp - checksum_choice);
		checksum_type = parse_csum_name(cp+1, -1);
	} else
		xfersum_type = checksum_type = parse_csum_name(checksum_choice, -1);
	return xfersum_type == CSUM_NONE;
}
