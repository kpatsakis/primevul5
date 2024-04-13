static int emit_atomic(u8 **pprog, u8 atomic_op,
		       u32 dst_reg, u32 src_reg, s16 off, u8 bpf_size)
{
	u8 *prog = *pprog;
	int cnt = 0;

	EMIT1(0xF0); /* lock prefix */

	maybe_emit_mod(&prog, dst_reg, src_reg, bpf_size == BPF_DW);

	/* emit opcode */
	switch (atomic_op) {
	case BPF_ADD:
	case BPF_SUB:
	case BPF_AND:
	case BPF_OR:
	case BPF_XOR:
		/* lock *(u32/u64*)(dst_reg + off) <op>= src_reg */
		EMIT1(simple_alu_opcodes[atomic_op]);
		break;
	case BPF_ADD | BPF_FETCH:
		/* src_reg = atomic_fetch_add(dst_reg + off, src_reg); */
		EMIT2(0x0F, 0xC1);
		break;
	case BPF_XCHG:
		/* src_reg = atomic_xchg(dst_reg + off, src_reg); */
		EMIT1(0x87);
		break;
	case BPF_CMPXCHG:
		/* r0 = atomic_cmpxchg(dst_reg + off, r0, src_reg); */
		EMIT2(0x0F, 0xB1);
		break;
	default:
		pr_err("bpf_jit: unknown atomic opcode %02x\n", atomic_op);
		return -EFAULT;
	}

	emit_insn_suffix(&prog, dst_reg, src_reg, off);

	*pprog = prog;
	return 0;
}