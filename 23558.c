static __init int vdso_fixup_features(struct lib32_elfinfo *v32,
				      struct lib64_elfinfo *v64)
{
	void *start32;
	unsigned long size32;

#ifdef CONFIG_PPC64
	void *start64;
	unsigned long size64;

	start64 = find_section64(v64->hdr, "__ftr_fixup", &size64);
	if (start64)
		do_feature_fixups(cur_cpu_spec->cpu_features,
				  start64, start64 + size64);

	start64 = find_section64(v64->hdr, "__fw_ftr_fixup", &size64);
	if (start64)
		do_feature_fixups(powerpc_firmware_features,
				  start64, start64 + size64);
#endif /* CONFIG_PPC64 */

	start32 = find_section32(v32->hdr, "__ftr_fixup", &size32);
	if (start32)
		do_feature_fixups(cur_cpu_spec->cpu_features,
				  start32, start32 + size32);

#ifdef CONFIG_PPC64
	start32 = find_section32(v32->hdr, "__fw_ftr_fixup", &size32);
	if (start32)
		do_feature_fixups(powerpc_firmware_features,
				  start32, start32 + size32);
#endif /* CONFIG_PPC64 */

	return 0;
}