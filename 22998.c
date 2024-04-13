R_API void r_bin_java_reset_bin_info(RBinJavaObj *bin) {
	free (bin->cf2.flags_str);
	free (bin->cf2.this_class_name);
	r_list_free (bin->imports_list);
	r_list_free (bin->methods_list);
	r_list_free (bin->fields_list);
	r_list_free (bin->attrs_list);
	r_list_free (bin->cp_list);
	r_list_free (bin->interfaces_list);
	r_str_constpool_fini (&bin->constpool);
	memset (bin, 0, sizeof (RBinJavaObj));
	r_str_constpool_init (&bin->constpool);
	bin->cf2.flags_str = strdup ("unknown");
	bin->cf2.this_class_name = strdup ("unknown");
	bin->imports_list = r_list_newf (free);
	bin->methods_list = r_list_newf (r_bin_java_fmtype_free);
	bin->fields_list = r_list_newf (r_bin_java_fmtype_free);
	bin->attrs_list = r_list_newf (r_bin_java_attribute_free);
	bin->cp_list = r_list_newf (r_bin_java_constant_pool);
	bin->interfaces_list = r_list_newf (r_bin_java_interface_free);
}