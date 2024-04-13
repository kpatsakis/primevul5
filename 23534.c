static __init int vdso_fixup_alt_funcs(struct lib32_elfinfo *v32,
				       struct lib64_elfinfo *v64)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(vdso_patches); i++) {
		struct vdso_patch_def *patch = &vdso_patches[i];
		int match = (cur_cpu_spec->cpu_features & patch->ftr_mask)
			== patch->ftr_value;
		if (!match)
			continue;

		DBG("replacing %s with %s...\n", patch->gen_name,
		    patch->fix_name ? "NONE" : patch->fix_name);

		/*
		 * Patch the 32 bits and 64 bits symbols. Note that we do not
		 * patch the "." symbol on 64 bits.
		 * It would be easy to do, but doesn't seem to be necessary,
		 * patching the OPD symbol is enough.
		 */
		vdso_do_func_patch32(v32, v64, patch->gen_name,
				     patch->fix_name);
#ifdef CONFIG_PPC64
		vdso_do_func_patch64(v32, v64, patch->gen_name,
				     patch->fix_name);
#endif /* CONFIG_PPC64 */
	}

	return 0;
}