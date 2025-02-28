static int jit_subprogs(struct bpf_verifier_env *env)
{
	struct bpf_prog *prog = env->prog, **func, *tmp;
	int i, j, subprog_start, subprog_end = 0, len, subprog;
	struct bpf_insn *insn;
	void *old_bpf_func;
	int err, num_exentries;

	if (env->subprog_cnt <= 1)
		return 0;

	for (i = 0, insn = prog->insnsi; i < prog->len; i++, insn++) {
		if (insn->code != (BPF_JMP | BPF_CALL) ||
		    insn->src_reg != BPF_PSEUDO_CALL)
			continue;
		/* Upon error here we cannot fall back to interpreter but
		 * need a hard reject of the program. Thus -EFAULT is
		 * propagated in any case.
		 */
		subprog = find_subprog(env, i + insn->imm + 1);
		if (subprog < 0) {
			WARN_ONCE(1, "verifier bug. No program starts at insn %d\n",
				  i + insn->imm + 1);
			return -EFAULT;
		}
		/* temporarily remember subprog id inside insn instead of
		 * aux_data, since next loop will split up all insns into funcs
		 */
		insn->off = subprog;
		/* remember original imm in case JIT fails and fallback
		 * to interpreter will be needed
		 */
		env->insn_aux_data[i].call_imm = insn->imm;
		/* point imm to __bpf_call_base+1 from JITs point of view */
		insn->imm = 1;
	}

	err = bpf_prog_alloc_jited_linfo(prog);
	if (err)
		goto out_undo_insn;

	err = -ENOMEM;
	func = kcalloc(env->subprog_cnt, sizeof(prog), GFP_KERNEL);
	if (!func)
		goto out_undo_insn;

	for (i = 0; i < env->subprog_cnt; i++) {
		subprog_start = subprog_end;
		subprog_end = env->subprog_info[i + 1].start;

		len = subprog_end - subprog_start;
		/* BPF_PROG_RUN doesn't call subprogs directly,
		 * hence main prog stats include the runtime of subprogs.
		 * subprogs don't have IDs and not reachable via prog_get_next_id
		 * func[i]->aux->stats will never be accessed and stays NULL
		 */
		func[i] = bpf_prog_alloc_no_stats(bpf_prog_size(len), GFP_USER);
		if (!func[i])
			goto out_free;
		memcpy(func[i]->insnsi, &prog->insnsi[subprog_start],
		       len * sizeof(struct bpf_insn));
		func[i]->type = prog->type;
		func[i]->len = len;
		if (bpf_prog_calc_tag(func[i]))
			goto out_free;
		func[i]->is_func = 1;
		func[i]->aux->func_idx = i;
		/* the btf and func_info will be freed only at prog->aux */
		func[i]->aux->btf = prog->aux->btf;
		func[i]->aux->func_info = prog->aux->func_info;

		/* Use bpf_prog_F_tag to indicate functions in stack traces.
		 * Long term would need debug info to populate names
		 */
		func[i]->aux->name[0] = 'F';
		func[i]->aux->stack_depth = env->subprog_info[i].stack_depth;
		func[i]->jit_requested = 1;
		func[i]->aux->linfo = prog->aux->linfo;
		func[i]->aux->nr_linfo = prog->aux->nr_linfo;
		func[i]->aux->jited_linfo = prog->aux->jited_linfo;
		func[i]->aux->linfo_idx = env->subprog_info[i].linfo_idx;
		num_exentries = 0;
		insn = func[i]->insnsi;
		for (j = 0; j < func[i]->len; j++, insn++) {
			if (BPF_CLASS(insn->code) == BPF_LDX &&
			    BPF_MODE(insn->code) == BPF_PROBE_MEM)
				num_exentries++;
		}
		func[i]->aux->num_exentries = num_exentries;
		func[i] = bpf_int_jit_compile(func[i]);
		if (!func[i]->jited) {
			err = -ENOTSUPP;
			goto out_free;
		}
		cond_resched();
	}
	/* at this point all bpf functions were successfully JITed
	 * now populate all bpf_calls with correct addresses and
	 * run last pass of JIT
	 */
	for (i = 0; i < env->subprog_cnt; i++) {
		insn = func[i]->insnsi;
		for (j = 0; j < func[i]->len; j++, insn++) {
			if (insn->code != (BPF_JMP | BPF_CALL) ||
			    insn->src_reg != BPF_PSEUDO_CALL)
				continue;
			subprog = insn->off;
			insn->imm = BPF_CAST_CALL(func[subprog]->bpf_func) -
				    __bpf_call_base;
		}

		/* we use the aux data to keep a list of the start addresses
		 * of the JITed images for each function in the program
		 *
		 * for some architectures, such as powerpc64, the imm field
		 * might not be large enough to hold the offset of the start
		 * address of the callee's JITed image from __bpf_call_base
		 *
		 * in such cases, we can lookup the start address of a callee
		 * by using its subprog id, available from the off field of
		 * the call instruction, as an index for this list
		 */
		func[i]->aux->func = func;
		func[i]->aux->func_cnt = env->subprog_cnt;
	}
	for (i = 0; i < env->subprog_cnt; i++) {
		old_bpf_func = func[i]->bpf_func;
		tmp = bpf_int_jit_compile(func[i]);
		if (tmp != func[i] || func[i]->bpf_func != old_bpf_func) {
			verbose(env, "JIT doesn't support bpf-to-bpf calls\n");
			err = -ENOTSUPP;
			goto out_free;
		}
		cond_resched();
	}

	/* finally lock prog and jit images for all functions and
	 * populate kallsysm
	 */
	for (i = 0; i < env->subprog_cnt; i++) {
		bpf_prog_lock_ro(func[i]);
		bpf_prog_kallsyms_add(func[i]);
	}

	/* Last step: make now unused interpreter insns from main
	 * prog consistent for later dump requests, so they can
	 * later look the same as if they were interpreted only.
	 */
	for (i = 0, insn = prog->insnsi; i < prog->len; i++, insn++) {
		if (insn->code != (BPF_JMP | BPF_CALL) ||
		    insn->src_reg != BPF_PSEUDO_CALL)
			continue;
		insn->off = env->insn_aux_data[i].call_imm;
		subprog = find_subprog(env, i + insn->off + 1);
		insn->imm = subprog;
	}

	prog->jited = 1;
	prog->bpf_func = func[0]->bpf_func;
	prog->aux->func = func;
	prog->aux->func_cnt = env->subprog_cnt;
	bpf_prog_free_unused_jited_linfo(prog);
	return 0;
out_free:
	for (i = 0; i < env->subprog_cnt; i++)
		if (func[i])
			bpf_jit_free(func[i]);
	kfree(func);
out_undo_insn:
	/* cleanup main prog to be interpreted */
	prog->jit_requested = 0;
	for (i = 0, insn = prog->insnsi; i < prog->len; i++, insn++) {
		if (insn->code != (BPF_JMP | BPF_CALL) ||
		    insn->src_reg != BPF_PSEUDO_CALL)
			continue;
		insn->off = 0;
		insn->imm = env->insn_aux_data[i].call_imm;
	}
	bpf_prog_free_jited_linfo(prog);
	return err;
}