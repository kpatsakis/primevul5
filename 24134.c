rpa_check_message(const unsigned char *data, const unsigned char *end,
		  const char **error)
{
	const unsigned char *p = data;
	unsigned int len = 0;

	if (p + 2 > end) {
		*error = "message too short";
		return NULL;
	}

	if (*p++ != ASN1_APPLICATION) {
		*error = "invalid data type";
		return NULL;
	}

	if ((*p & 0x80) != 0) {
		unsigned int nbytes = *p++ & 0x7f;

		while (nbytes-- > 0) {
			if (p >= end) {
				*error = "invalid structure length";
				return NULL;
			}

			len = (len << 8) | *p++;
		}
	} else
		len = *p++;

	if ((size_t)(end - p) != len) {
		*error = "structure length disagrees with data size";
		return NULL;
	}

	if (p + sizeof(rpa_oid) > end) {
		*error = "not enough space for object id";
		return NULL;
	}

	if (memcmp(p, rpa_oid, sizeof(rpa_oid)) != 0) {
		*error = "invalid object id";
		return NULL;
	}

	return p + sizeof(rpa_oid);
}