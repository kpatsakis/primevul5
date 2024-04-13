int force_sig_fault(int sig, int code, void __user *addr
	___ARCH_SI_TRAPNO(int trapno)
	___ARCH_SI_IA64(int imm, unsigned int flags, unsigned long isr))
{
	return force_sig_fault_to_task(sig, code, addr
				       ___ARCH_SI_TRAPNO(trapno)
				       ___ARCH_SI_IA64(imm, flags, isr), current);
}