static bool em_syscall_is_enabled(struct x86_emulate_ctxt *ctxt)
{
	struct x86_emulate_ops *ops = ctxt->ops;
	u32 eax, ebx, ecx, edx;

	/*
	 * syscall should always be enabled in longmode - so only become
	 * vendor specific (cpuid) if other modes are active...
	 */
	if (ctxt->mode == X86EMUL_MODE_PROT64)
		return true;

	eax = 0x00000000;
	ecx = 0x00000000;
	if (ops->get_cpuid(ctxt, &eax, &ebx, &ecx, &edx)) {
		/*
		 * Intel ("GenuineIntel")
		 * remark: Intel CPUs only support "syscall" in 64bit
		 * longmode. Also an 64bit guest with a
		 * 32bit compat-app running will #UD !! While this
		 * behaviour can be fixed (by emulating) into AMD
		 * response - CPUs of AMD can't behave like Intel.
		 */
		if (ebx == X86EMUL_CPUID_VENDOR_GenuineIntel_ebx &&
		    ecx == X86EMUL_CPUID_VENDOR_GenuineIntel_ecx &&
		    edx == X86EMUL_CPUID_VENDOR_GenuineIntel_edx)
			return false;

		/* AMD ("AuthenticAMD") */
		if (ebx == X86EMUL_CPUID_VENDOR_AuthenticAMD_ebx &&
		    ecx == X86EMUL_CPUID_VENDOR_AuthenticAMD_ecx &&
		    edx == X86EMUL_CPUID_VENDOR_AuthenticAMD_edx)
			return true;

		/* AMD ("AMDisbetter!") */
		if (ebx == X86EMUL_CPUID_VENDOR_AMDisbetterI_ebx &&
		    ecx == X86EMUL_CPUID_VENDOR_AMDisbetterI_ecx &&
		    edx == X86EMUL_CPUID_VENDOR_AMDisbetterI_edx)
			return true;
	}

	/* default: (not Intel, not AMD), apply Intel's stricter rules... */
	return false;
}