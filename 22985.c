R_API ut64 r_bin_java_parse_fields(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 len) {
	int i = 0;
	ut64 adv = 0;
	RBinJavaField *field;
	const ut8 *fm_buf = buf + offset;
	r_list_free (bin->fields_list);
	bin->fields_list = r_list_newf (r_bin_java_fmtype_free);
	bin->fields_offset = offset;
	if (offset + 2 >= len) {
		return UT64_MAX;
	}
	bin->fields_count = R_BIN_JAVA_USHORT (fm_buf, 0);
	adv += 2;
	IFDBG eprintf("Fields count: %d 0x%"PFMT64x "\n", bin->fields_count, bin->fields_offset);
	if (bin->fields_count > 0) {
		for (i = 0; i < bin->fields_count; i++, bin->field_idx++) {
			field = r_bin_java_read_next_field (bin, offset + adv, buf, len);
			if (field) {
				adv += field->size;
				r_list_append (bin->fields_list, field);
				IFDBG r_bin_java_print_field_summary(field);
				if (adv + offset > len) {
					eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Field: %d.\n", i);
					break;
				}
			} else {
				IFDBG eprintf("Failed to read Field %d\n", i);
				break;
			}
		}
	}
	bin->fields_size = adv;
	return adv;
}