static void emit_bpf_tail_call_direct(struct bpf_jit_poke_descriptor *poke,
				      u8 **pprog, int addr, u8 *image,
				      bool *callee_regs_used, u32 stack_depth)
{
	int tcc_off = -4 - round_up(stack_depth, 8);
	u8 *prog = *pprog;
	int pop_bytes = 0;
	int off1 = 20;
	int poke_off;
	int cnt = 0;

	/* count the additional bytes used for popping callee regs to stack
	 * that need to be taken into account for jump offset that is used for
	 * bailing out from of the tail call when limit is reached
	 */
	pop_bytes = get_pop_bytes(callee_regs_used);
	off1 += pop_bytes;

	/*
	 * total bytes for:
	 * - nop5/ jmpq $off
	 * - pop callee regs
	 * - sub rsp, $val if depth > 0
	 * - pop rax
	 */
	poke_off = X86_PATCH_SIZE + pop_bytes + 1;
	if (stack_depth) {
		poke_off += 7;
		off1 += 7;
	}

	/*
	 * if (tail_call_cnt > MAX_TAIL_CALL_CNT)
	 *	goto out;
	 */
	EMIT2_off32(0x8B, 0x85, tcc_off);             /* mov eax, dword ptr [rbp - tcc_off] */
	EMIT3(0x83, 0xF8, MAX_TAIL_CALL_CNT);         /* cmp eax, MAX_TAIL_CALL_CNT */
	EMIT2(X86_JA, off1);                          /* ja out */
	EMIT3(0x83, 0xC0, 0x01);                      /* add eax, 1 */
	EMIT2_off32(0x89, 0x85, tcc_off);             /* mov dword ptr [rbp - tcc_off], eax */

	poke->tailcall_bypass = image + (addr - poke_off - X86_PATCH_SIZE);
	poke->adj_off = X86_TAIL_CALL_OFFSET;
	poke->tailcall_target = image + (addr - X86_PATCH_SIZE);
	poke->bypass_addr = (u8 *)poke->tailcall_target + X86_PATCH_SIZE;

	emit_jump(&prog, (u8 *)poke->tailcall_target + X86_PATCH_SIZE,
		  poke->tailcall_bypass);

	*pprog = prog;
	pop_callee_regs(pprog, callee_regs_used);
	prog = *pprog;
	EMIT1(0x58);                                  /* pop rax */
	if (stack_depth)
		EMIT3_off32(0x48, 0x81, 0xC4, round_up(stack_depth, 8));

	memcpy(prog, ideal_nops[NOP_ATOMIC5], X86_PATCH_SIZE);
	prog += X86_PATCH_SIZE;
	/* out: */

	*pprog = prog;
}