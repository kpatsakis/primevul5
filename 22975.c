R_API void r_bin_java_attribute_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		IFDBG eprintf("Deleting attr %s, %p\n", attr->name, attr);
		if (attr && attr->metas && attr->metas->type_info) {
			RBinJavaAttrMetas *a = attr->metas->type_info;
			if (a && a->allocs && a->allocs->delete_obj) {
				a->allocs->delete_obj (attr);
			}
		}
		// free (attr->metas);
		// free (attr);
	}
}