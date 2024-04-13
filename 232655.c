static void emit_prologue(u8 **pprog, u32 stack_depth, bool ebpf_from_cbpf,
			  bool tail_call_reachable, bool is_subprog)
{
	u8 *prog = *pprog;
	int cnt = X86_PATCH_SIZE;

	/* BPF trampoline can be made to work without these nops,
	 * but let's waste 5 bytes for now and optimize later
	 */
	memcpy(prog, ideal_nops[NOP_ATOMIC5], cnt);
	prog += cnt;
	if (!ebpf_from_cbpf) {
		if (tail_call_reachable && !is_subprog)
			EMIT2(0x31, 0xC0); /* xor eax, eax */
		else
			EMIT2(0x66, 0x90); /* nop2 */
	}
	EMIT1(0x55);             /* push rbp */
	EMIT3(0x48, 0x89, 0xE5); /* mov rbp, rsp */
	/* sub rsp, rounded_stack_depth */
	if (stack_depth)
		EMIT3_off32(0x48, 0x81, 0xEC, round_up(stack_depth, 8));
	if (tail_call_reachable)
		EMIT1(0x50);         /* push rax */
	*pprog = prog;
}