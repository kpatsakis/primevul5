R_API ut64 r_bin_java_parse_methods(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 len) {
	int i = 0;
	ut64 adv = 0;
	RBinJavaField *method;
	const ut8 *fm_buf = buf + offset;
	r_list_free (bin->methods_list);
	bin->methods_list = r_list_newf (r_bin_java_fmtype_free);

	if (offset + 2 >= len) {
		return 0LL;
	}
	bin->methods_offset = offset;
	bin->methods_count = R_BIN_JAVA_USHORT (fm_buf, 0);
	adv += 2;
	IFDBG eprintf("Methods count: %d 0x%"PFMT64x "\n", bin->methods_count, bin->methods_offset);
	bin->main = NULL;
	bin->entrypoint = NULL;
	bin->main_code_attr = NULL;
	bin->entrypoint_code_attr = NULL;
	for (i = 0; i < bin->methods_count; i++, bin->method_idx++) {
		method = r_bin_java_read_next_method (bin, offset + adv, buf, len);
		if (method) {
			adv += method->size;
			r_list_append (bin->methods_list, method);
		}
		// Update Main, Init, or Class Init
		if (method && !strcmp ((const char *) method->name, "main")) {
			bin->main = method;
			// get main code attr
			bin->main_code_attr = r_bin_java_get_attr_from_field (method, R_BIN_JAVA_ATTR_TYPE_CODE_ATTR, 0);
		} else if (method && (!strcmp ((const char *) method->name, "<init>") || !strcmp ((const char *) method->name, "init")))   {
			IFDBG eprintf("Found an init function.\n");
			bin->entrypoint = method;
			bin->entrypoint_code_attr = r_bin_java_get_attr_from_field (method, R_BIN_JAVA_ATTR_TYPE_CODE_ATTR, 0);
		} else if (method && (!strcmp ((const char *) method->name, "<cinit>") || !strcmp ((const char *) method->name, "cinit")))   {
			bin->cf2.this_class_entrypoint = method;
			bin->cf2.this_class_entrypoint_code_attr = r_bin_java_get_attr_from_field (method, R_BIN_JAVA_ATTR_TYPE_CODE_ATTR, 0);
		}
		if (adv + offset > len) {
			eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Method: %d.\n", i);
			break;
		}
		IFDBG r_bin_java_print_field_summary(method);
	}
	bin->methods_size = adv;
	return adv;
}