R_API RBinJavaAttrInfo *r_bin_java_read_next_attr(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 buf_len) {
	RBinJavaAttrInfo *attr = NULL;
	ut32 sz = 0;
	ut8 *buffer = NULL;
	const ut8 *a_buf = offset + buf;
	ut8 attr_idx_len = 6;
	if (offset + 6 > buf_len) {
		eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile in Attribute offset "
			"(0x%"PFMT64x ") > len  of remaining bytes (0x%"PFMT64x ").\n", offset, buf_len);
		return NULL;
	}
	// ut16 attr_idx, ut32 length of attr.
	sz = R_BIN_JAVA_UINT (a_buf, 2) + attr_idx_len; // r_bin_java_read_int (bin, buf_offset+2) + attr_idx_len;
	if (sz + offset > buf_len) {
		eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile in Attribute len "
			"(0x%x) + offset (0x%"PFMT64x ") exceeds length of buffer (0x%"PFMT64x ").\n",
			sz, offset, buf_len);
		return NULL;
	}
	// when reading the attr bytes, need to also
	// include the initial 6 bytes, which
	// are not included in the attribute length
	// ,
	// sz, buf_offset, buf_offset+sz);
	buffer = r_bin_java_get_attr_buf (bin, sz, offset, buf, buf_len);
	// printf ("%d %d %d\n", sz, buf_len, offset);
	if (offset < buf_len) {
		attr = r_bin_java_read_next_attr_from_buffer (bin, buffer, buf_len - offset, offset);
		free (buffer);

		if (!attr) {
			return NULL;
		}
		attr->size = sz;
	} else {
		free (buffer);
		eprintf ("IS OOB\n");
	}
	return attr;
}