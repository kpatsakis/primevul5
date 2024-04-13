R_API RBinJavaField *r_bin_java_read_next_method(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 len) {
	ut32 i, idx;
	const ut8 *f_buf = buf + offset;
	ut64 adv = 0;
	RBinJavaCPTypeObj *item = NULL;
	if (!bin || offset + 8 >= len) {
		return NULL;
	}
	RBinJavaField *method = (RBinJavaField *) R_NEW0 (RBinJavaField);
	if (!method) {
		eprintf ("Unable to allocate memory for method information\n");
		return NULL;
	}
	method->metas = (RBinJavaMetaInfo *) R_NEW0 (RBinJavaMetaInfo);
	if (!method->metas) {
		eprintf ("Unable to allocate memory for meta information\n");
		free (method);
		return NULL;
	}
	method->file_offset = offset;
	method->flags = R_BIN_JAVA_USHORT (f_buf, 0);
	method->flags_str = retrieve_method_access_string (method->flags);
	// need to subtract 1 for the idx
	method->name_idx = R_BIN_JAVA_USHORT (f_buf, 2);
	method->descriptor_idx = R_BIN_JAVA_USHORT (f_buf, 4);
	method->attr_count = R_BIN_JAVA_USHORT (f_buf, 6);
	method->attributes = r_list_newf (r_bin_java_attribute_free);
	method->type = R_BIN_JAVA_FIELD_TYPE_METHOD;
	method->metas->ord = bin->method_idx;
	adv += 8;
	idx = method->name_idx;
	item = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	method->name = r_bin_java_get_utf8_from_bin_cp_list (bin, (ut32) (method->name_idx));
	IFDBG eprintf("Method name_idx: %d, which is: ord: %d, name: %s, value: %s\n", idx, item->metas->ord, ((RBinJavaCPTypeMetas *)item->metas->type_info)->name, method->name);
	if (!method->name) {
		method->name = (char *) malloc (21);
		snprintf ((char *) method->name, 20, "sym.method_%08x", method->metas->ord);
		IFDBG eprintf("r_bin_java_read_next_method: Unable to find the name for 0x%02x index.\n", method->name_idx);
	}
	idx = method->descriptor_idx;
	item = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	method->descriptor = r_bin_java_get_utf8_from_bin_cp_list (bin, (ut32) method->descriptor_idx);
	IFDBG eprintf("Method descriptor_idx: %d, which is: ord: %d, name: %s, value: %s\n", idx, item->metas->ord, ((RBinJavaCPTypeMetas *)item->metas->type_info)->name, method->descriptor);
	if (!method->descriptor) {
		method->descriptor = r_str_dup (NULL, "NULL");
		IFDBG eprintf("r_bin_java_read_next_method: Unable to find the descriptor for 0x%02x index.\n", method->descriptor_idx);
	}
	IFDBG eprintf("Looking for a NameAndType CP with name_idx: %d descriptor_idx: %d\n", method->name_idx, method->descriptor_idx);
	method->field_ref_cp_obj = r_bin_java_find_cp_ref_info_from_name_and_type (bin, method->name_idx, method->descriptor_idx);
	if (method->field_ref_cp_obj) {
		IFDBG eprintf("Found the obj.\n");
		item = r_bin_java_get_item_from_bin_cp_list (bin, method->field_ref_cp_obj->info.cp_method.class_idx);
		IFDBG eprintf("Method class reference value: %d, which is: ord: %d, name: %s\n", method->field_ref_cp_obj->info.cp_method.class_idx, item->metas->ord, ((RBinJavaCPTypeMetas *)item->metas->type_info)->name);
		method->class_name = r_bin_java_get_item_name_from_bin_cp_list (bin, item);
		IFDBG eprintf("Method requesting ref_cp_obj the following which is: ord: %d, name: %s\n", method->field_ref_cp_obj->metas->ord, ((RBinJavaCPTypeMetas *)method->field_ref_cp_obj->metas->type_info)->name);
		IFDBG eprintf("MethodRef class name resolves to: %s\n", method->class_name);
		if (!method->class_name) {
			method->class_name = r_str_dup (NULL, "NULL");
		}
	} else {
		// XXX - default to this class?
		method->field_ref_cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, bin->cf2.this_class);
		method->class_name = r_bin_java_get_item_name_from_bin_cp_list (bin, method->field_ref_cp_obj);
	}
	IFDBG eprintf("Parsing %s(%s)\n", method->name, method->descriptor);
	if (method->attr_count > 0) {
		method->attr_offset = adv + offset;
		RBinJavaAttrInfo *attr = NULL;
		for (i = 0; i < method->attr_count; i++) {
			attr = r_bin_java_read_next_attr (bin, adv + offset, buf, len);
			if (!attr) {
				eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Method Attribute: %d.\n", i);
				break;
			}
			if ((r_bin_java_get_attr_type_by_name (attr->name))->type == R_BIN_JAVA_ATTR_TYPE_CODE_ATTR) {
				// This is necessary for determing the appropriate number of bytes when readin
				// uoffset, ustack, ulocalvar values
				bin->cur_method_code_length = attr->info.code_attr.code_length;
				bin->offset_sz = 2;// (attr->info.code_attr.code_length > 65535) ? 4 : 2;
				bin->ustack_sz = 2;// (attr->info.code_attr.max_stack > 65535) ? 4 : 2;
				bin->ulocalvar_sz = 2;// (attr->info.code_attr.max_locals > 65535) ? 4 : 2;
			}
			IFDBG eprintf("Parsing @ 0x%"PFMT64x " (%s) = 0x%"PFMT64x " bytes\n", attr->file_offset, attr->name, attr->size);
			r_list_append (method->attributes, attr);
			adv += attr->size;
			if (adv + offset >= len) {
				eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Method Attribute: %d.\n", i);
				break;
			}
		}
	}
	method->size = adv;
	// reset after parsing the method attributes
	IFDBG eprintf("Parsing @ 0x%"PFMT64x " %s(%s) = 0x%"PFMT64x " bytes\n", method->file_offset, method->name, method->descriptor, method->size);
	return method;
}