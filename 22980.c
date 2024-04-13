R_API void r_bin_java_print_element_pair_summary(RBinJavaElementValuePair *evp) {
	if (!evp) {
		eprintf ("Attempting to print an invalid RBinJavaElementValuePair *pair.\n");
		return;
	}
	Eprintf ("Element Value Pair information:\n");
	Eprintf ("  EV Pair File Offset: 0x%08"PFMT64x "\n", evp->file_offset);
	Eprintf ("  EV Pair Element Name index: 0x%02x\n", evp->element_name_idx);
	Eprintf ("  EV Pair Element Name: %s\n", evp->name);
	Eprintf ("  EV Pair Element Value:\n");
	r_bin_java_print_element_value_summary (evp->value);
}