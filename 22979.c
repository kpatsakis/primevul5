R_API void r_bin_java_print_annotation_summary(RBinJavaAnnotation *annotation) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaElementValuePair *evp = NULL;
	if (!annotation) {
		// TODO eprintf invalid annotation
		return;
	}
	Eprintf ("  Annotation Type Index: 0x%02x\n", annotation->type_idx);
	Eprintf ("  Annotation Number of EV Pairs: 0x%04x\n", annotation->num_element_value_pairs);
	Eprintf ("  Annotation EV Pair Values:\n");
	if (annotation->element_value_pairs) {
		r_list_foreach_safe (annotation->element_value_pairs, iter, iter_tmp, evp) {
			r_bin_java_print_element_pair_summary (evp);
		}
	}
}