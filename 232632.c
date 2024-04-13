static void emit_bpf_tail_call_indirect(u8 **pprog, bool *callee_regs_used,
					u32 stack_depth)
{
	int tcc_off = -4 - round_up(stack_depth, 8);
	u8 *prog = *pprog;
	int pop_bytes = 0;
	int off1 = 42;
	int off2 = 31;
	int off3 = 9;
	int cnt = 0;

	/* count the additional bytes used for popping callee regs from stack
	 * that need to be taken into account for each of the offsets that
	 * are used for bailing out of the tail call
	 */
	pop_bytes = get_pop_bytes(callee_regs_used);
	off1 += pop_bytes;
	off2 += pop_bytes;
	off3 += pop_bytes;

	if (stack_depth) {
		off1 += 7;
		off2 += 7;
		off3 += 7;
	}

	/*
	 * rdi - pointer to ctx
	 * rsi - pointer to bpf_array
	 * rdx - index in bpf_array
	 */

	/*
	 * if (index >= array->map.max_entries)
	 *	goto out;
	 */
	EMIT2(0x89, 0xD2);                        /* mov edx, edx */
	EMIT3(0x39, 0x56,                         /* cmp dword ptr [rsi + 16], edx */
	      offsetof(struct bpf_array, map.max_entries));
#define OFFSET1 (off1 + RETPOLINE_RCX_BPF_JIT_SIZE) /* Number of bytes to jump */
	EMIT2(X86_JBE, OFFSET1);                  /* jbe out */

	/*
	 * if (tail_call_cnt > MAX_TAIL_CALL_CNT)
	 *	goto out;
	 */
	EMIT2_off32(0x8B, 0x85, tcc_off);         /* mov eax, dword ptr [rbp - tcc_off] */
	EMIT3(0x83, 0xF8, MAX_TAIL_CALL_CNT);     /* cmp eax, MAX_TAIL_CALL_CNT */
#define OFFSET2 (off2 + RETPOLINE_RCX_BPF_JIT_SIZE)
	EMIT2(X86_JA, OFFSET2);                   /* ja out */
	EMIT3(0x83, 0xC0, 0x01);                  /* add eax, 1 */
	EMIT2_off32(0x89, 0x85, tcc_off);         /* mov dword ptr [rbp - tcc_off], eax */

	/* prog = array->ptrs[index]; */
	EMIT4_off32(0x48, 0x8B, 0x8C, 0xD6,       /* mov rcx, [rsi + rdx * 8 + offsetof(...)] */
		    offsetof(struct bpf_array, ptrs));

	/*
	 * if (prog == NULL)
	 *	goto out;
	 */
	EMIT3(0x48, 0x85, 0xC9);                  /* test rcx,rcx */
#define OFFSET3 (off3 + RETPOLINE_RCX_BPF_JIT_SIZE)
	EMIT2(X86_JE, OFFSET3);                   /* je out */

	*pprog = prog;
	pop_callee_regs(pprog, callee_regs_used);
	prog = *pprog;

	EMIT1(0x58);                              /* pop rax */
	if (stack_depth)
		EMIT3_off32(0x48, 0x81, 0xC4,     /* add rsp, sd */
			    round_up(stack_depth, 8));

	/* goto *(prog->bpf_func + X86_TAIL_CALL_OFFSET); */
	EMIT4(0x48, 0x8B, 0x49,                   /* mov rcx, qword ptr [rcx + 32] */
	      offsetof(struct bpf_prog, bpf_func));
	EMIT4(0x48, 0x83, 0xC1,                   /* add rcx, X86_TAIL_CALL_OFFSET */
	      X86_TAIL_CALL_OFFSET);
	/*
	 * Now we're ready to jump into next BPF program
	 * rdi == ctx (1st arg)
	 * rcx == prog->bpf_func + X86_TAIL_CALL_OFFSET
	 */
	RETPOLINE_RCX_BPF_JIT();

	/* out: */
	*pprog = prog;
}