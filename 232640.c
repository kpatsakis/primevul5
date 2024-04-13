static int __bpf_arch_text_poke(void *ip, enum bpf_text_poke_type t,
				void *old_addr, void *new_addr,
				const bool text_live)
{
	const u8 *nop_insn = ideal_nops[NOP_ATOMIC5];
	u8 old_insn[X86_PATCH_SIZE];
	u8 new_insn[X86_PATCH_SIZE];
	u8 *prog;
	int ret;

	memcpy(old_insn, nop_insn, X86_PATCH_SIZE);
	if (old_addr) {
		prog = old_insn;
		ret = t == BPF_MOD_CALL ?
		      emit_call(&prog, old_addr, ip) :
		      emit_jump(&prog, old_addr, ip);
		if (ret)
			return ret;
	}

	memcpy(new_insn, nop_insn, X86_PATCH_SIZE);
	if (new_addr) {
		prog = new_insn;
		ret = t == BPF_MOD_CALL ?
		      emit_call(&prog, new_addr, ip) :
		      emit_jump(&prog, new_addr, ip);
		if (ret)
			return ret;
	}

	ret = -EBUSY;
	mutex_lock(&text_mutex);
	if (memcmp(ip, old_insn, X86_PATCH_SIZE))
		goto out;
	ret = 1;
	if (memcmp(ip, new_insn, X86_PATCH_SIZE)) {
		if (text_live)
			text_poke_bp(ip, new_insn, X86_PATCH_SIZE, NULL);
		else
			memcpy(ip, new_insn, X86_PATCH_SIZE);
		ret = 0;
	}
out:
	mutex_unlock(&text_mutex);
	return ret;
}