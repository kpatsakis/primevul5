R_API char *r_bin_java_print_utf8_cp_stringify(RBinJavaCPTypeObj *obj) {
	ut32 size = 255, consumed = 0;
	char *utf8_str = r_hex_bin2strdup (obj->info.cp_utf8.bytes, obj->info.cp_utf8.length);
	char *value = malloc (size + strlen (utf8_str));
	if (value) {
		memset (value, 0, size);
		consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%d.%s",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			obj->info.cp_utf8.length,
			utf8_str);
		if (consumed >= size - 1) {
			free (value);
			size += size >> 1;
			value = malloc (size + strlen (utf8_str));
			if (value) {
				memset (value, 0, size);
				(void)snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%d.%s",
					obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
					obj->info.cp_utf8.length,
					utf8_str);
			}
		}
	}
	free (utf8_str);
	return value;
}