static __init void vdso_setup_trampolines(struct lib32_elfinfo *v32,
					  struct lib64_elfinfo *v64)
{
	/*
	 * Find signal trampolines
	 */

#ifdef CONFIG_PPC64
	vdso64_rt_sigtramp = find_function64(v64, "__kernel_sigtramp_rt64");
#endif
	vdso32_sigtramp	   = find_function32(v32, "__kernel_sigtramp32");
	vdso32_rt_sigtramp = find_function32(v32, "__kernel_sigtramp_rt32");
}