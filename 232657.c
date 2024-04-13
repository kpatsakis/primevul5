int arch_prepare_bpf_trampoline(struct bpf_tramp_image *im, void *image, void *image_end,
				const struct btf_func_model *m, u32 flags,
				struct bpf_tramp_progs *tprogs,
				void *orig_call)
{
	int ret, i, cnt = 0, nr_args = m->nr_args;
	int stack_size = nr_args * 8;
	struct bpf_tramp_progs *fentry = &tprogs[BPF_TRAMP_FENTRY];
	struct bpf_tramp_progs *fexit = &tprogs[BPF_TRAMP_FEXIT];
	struct bpf_tramp_progs *fmod_ret = &tprogs[BPF_TRAMP_MODIFY_RETURN];
	u8 **branches = NULL;
	u8 *prog;

	/* x86-64 supports up to 6 arguments. 7+ can be added in the future */
	if (nr_args > 6)
		return -ENOTSUPP;

	if ((flags & BPF_TRAMP_F_RESTORE_REGS) &&
	    (flags & BPF_TRAMP_F_SKIP_FRAME))
		return -EINVAL;

	if (flags & BPF_TRAMP_F_CALL_ORIG)
		stack_size += 8; /* room for return value of orig_call */

	if (flags & BPF_TRAMP_F_SKIP_FRAME)
		/* skip patched call instruction and point orig_call to actual
		 * body of the kernel function.
		 */
		orig_call += X86_PATCH_SIZE;

	prog = image;

	EMIT1(0x55);		 /* push rbp */
	EMIT3(0x48, 0x89, 0xE5); /* mov rbp, rsp */
	EMIT4(0x48, 0x83, 0xEC, stack_size); /* sub rsp, stack_size */
	EMIT1(0x53);		 /* push rbx */

	save_regs(m, &prog, nr_args, stack_size);

	if (flags & BPF_TRAMP_F_CALL_ORIG) {
		/* arg1: mov rdi, im */
		emit_mov_imm64(&prog, BPF_REG_1, (long) im >> 32, (u32) (long) im);
		if (emit_call(&prog, __bpf_tramp_enter, prog)) {
			ret = -EINVAL;
			goto cleanup;
		}
	}

	if (fentry->nr_progs)
		if (invoke_bpf(m, &prog, fentry, stack_size))
			return -EINVAL;

	if (fmod_ret->nr_progs) {
		branches = kcalloc(fmod_ret->nr_progs, sizeof(u8 *),
				   GFP_KERNEL);
		if (!branches)
			return -ENOMEM;

		if (invoke_bpf_mod_ret(m, &prog, fmod_ret, stack_size,
				       branches)) {
			ret = -EINVAL;
			goto cleanup;
		}
	}

	if (flags & BPF_TRAMP_F_CALL_ORIG) {
		restore_regs(m, &prog, nr_args, stack_size);

		/* call original function */
		if (emit_call(&prog, orig_call, prog)) {
			ret = -EINVAL;
			goto cleanup;
		}
		/* remember return value in a stack for bpf prog to access */
		emit_stx(&prog, BPF_DW, BPF_REG_FP, BPF_REG_0, -8);
		im->ip_after_call = prog;
		memcpy(prog, ideal_nops[NOP_ATOMIC5], X86_PATCH_SIZE);
		prog += X86_PATCH_SIZE;
	}

	if (fmod_ret->nr_progs) {
		/* From Intel 64 and IA-32 Architectures Optimization
		 * Reference Manual, 3.4.1.4 Code Alignment, Assembly/Compiler
		 * Coding Rule 11: All branch targets should be 16-byte
		 * aligned.
		 */
		emit_align(&prog, 16);
		/* Update the branches saved in invoke_bpf_mod_ret with the
		 * aligned address of do_fexit.
		 */
		for (i = 0; i < fmod_ret->nr_progs; i++)
			emit_cond_near_jump(&branches[i], prog, branches[i],
					    X86_JNE);
	}

	if (fexit->nr_progs)
		if (invoke_bpf(m, &prog, fexit, stack_size)) {
			ret = -EINVAL;
			goto cleanup;
		}

	if (flags & BPF_TRAMP_F_RESTORE_REGS)
		restore_regs(m, &prog, nr_args, stack_size);

	/* This needs to be done regardless. If there were fmod_ret programs,
	 * the return value is only updated on the stack and still needs to be
	 * restored to R0.
	 */
	if (flags & BPF_TRAMP_F_CALL_ORIG) {
		im->ip_epilogue = prog;
		/* arg1: mov rdi, im */
		emit_mov_imm64(&prog, BPF_REG_1, (long) im >> 32, (u32) (long) im);
		if (emit_call(&prog, __bpf_tramp_exit, prog)) {
			ret = -EINVAL;
			goto cleanup;
		}
		/* restore original return value back into RAX */
		emit_ldx(&prog, BPF_DW, BPF_REG_0, BPF_REG_FP, -8);
	}

	EMIT1(0x5B); /* pop rbx */
	EMIT1(0xC9); /* leave */
	if (flags & BPF_TRAMP_F_SKIP_FRAME)
		/* skip our return address and return to parent */
		EMIT4(0x48, 0x83, 0xC4, 8); /* add rsp, 8 */
	EMIT1(0xC3); /* ret */
	/* Make sure the trampoline generation logic doesn't overflow */
	if (WARN_ON_ONCE(prog > (u8 *)image_end - BPF_INSN_SAFETY)) {
		ret = -EFAULT;
		goto cleanup;
	}
	ret = prog - (u8 *)image;

cleanup:
	kfree(branches);
	return ret;
}