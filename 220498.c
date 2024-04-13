static int check_attach_btf_id(struct bpf_verifier_env *env)
{
	struct bpf_prog *prog = env->prog;
	bool prog_extension = prog->type == BPF_PROG_TYPE_EXT;
	struct bpf_prog *tgt_prog = prog->aux->linked_prog;
	u32 btf_id = prog->aux->attach_btf_id;
	const char prefix[] = "btf_trace_";
	struct btf_func_model fmodel;
	int ret = 0, subprog = -1, i;
	struct bpf_trampoline *tr;
	const struct btf_type *t;
	bool conservative = true;
	const char *tname;
	struct btf *btf;
	long addr;
	u64 key;

	if (prog->type == BPF_PROG_TYPE_STRUCT_OPS)
		return check_struct_ops_btf_id(env);

	if (prog->type != BPF_PROG_TYPE_TRACING &&
	    prog->type != BPF_PROG_TYPE_LSM &&
	    !prog_extension)
		return 0;

	if (!btf_id) {
		verbose(env, "Tracing programs must provide btf_id\n");
		return -EINVAL;
	}
	btf = bpf_prog_get_target_btf(prog);
	if (!btf) {
		verbose(env,
			"FENTRY/FEXIT program can only be attached to another program annotated with BTF\n");
		return -EINVAL;
	}
	t = btf_type_by_id(btf, btf_id);
	if (!t) {
		verbose(env, "attach_btf_id %u is invalid\n", btf_id);
		return -EINVAL;
	}
	tname = btf_name_by_offset(btf, t->name_off);
	if (!tname) {
		verbose(env, "attach_btf_id %u doesn't have a name\n", btf_id);
		return -EINVAL;
	}
	if (tgt_prog) {
		struct bpf_prog_aux *aux = tgt_prog->aux;

		for (i = 0; i < aux->func_info_cnt; i++)
			if (aux->func_info[i].type_id == btf_id) {
				subprog = i;
				break;
			}
		if (subprog == -1) {
			verbose(env, "Subprog %s doesn't exist\n", tname);
			return -EINVAL;
		}
		conservative = aux->func_info_aux[subprog].unreliable;
		if (prog_extension) {
			if (conservative) {
				verbose(env,
					"Cannot replace static functions\n");
				return -EINVAL;
			}
			if (!prog->jit_requested) {
				verbose(env,
					"Extension programs should be JITed\n");
				return -EINVAL;
			}
			env->ops = bpf_verifier_ops[tgt_prog->type];
			prog->expected_attach_type = tgt_prog->expected_attach_type;
		}
		if (!tgt_prog->jited) {
			verbose(env, "Can attach to only JITed progs\n");
			return -EINVAL;
		}
		if (tgt_prog->type == prog->type) {
			/* Cannot fentry/fexit another fentry/fexit program.
			 * Cannot attach program extension to another extension.
			 * It's ok to attach fentry/fexit to extension program.
			 */
			verbose(env, "Cannot recursively attach\n");
			return -EINVAL;
		}
		if (tgt_prog->type == BPF_PROG_TYPE_TRACING &&
		    prog_extension &&
		    (tgt_prog->expected_attach_type == BPF_TRACE_FENTRY ||
		     tgt_prog->expected_attach_type == BPF_TRACE_FEXIT)) {
			/* Program extensions can extend all program types
			 * except fentry/fexit. The reason is the following.
			 * The fentry/fexit programs are used for performance
			 * analysis, stats and can be attached to any program
			 * type except themselves. When extension program is
			 * replacing XDP function it is necessary to allow
			 * performance analysis of all functions. Both original
			 * XDP program and its program extension. Hence
			 * attaching fentry/fexit to BPF_PROG_TYPE_EXT is
			 * allowed. If extending of fentry/fexit was allowed it
			 * would be possible to create long call chain
			 * fentry->extension->fentry->extension beyond
			 * reasonable stack size. Hence extending fentry is not
			 * allowed.
			 */
			verbose(env, "Cannot extend fentry/fexit\n");
			return -EINVAL;
		}
		key = ((u64)aux->id) << 32 | btf_id;
	} else {
		if (prog_extension) {
			verbose(env, "Cannot replace kernel functions\n");
			return -EINVAL;
		}
		key = btf_id;
	}

	switch (prog->expected_attach_type) {
	case BPF_TRACE_RAW_TP:
		if (tgt_prog) {
			verbose(env,
				"Only FENTRY/FEXIT progs are attachable to another BPF prog\n");
			return -EINVAL;
		}
		if (!btf_type_is_typedef(t)) {
			verbose(env, "attach_btf_id %u is not a typedef\n",
				btf_id);
			return -EINVAL;
		}
		if (strncmp(prefix, tname, sizeof(prefix) - 1)) {
			verbose(env, "attach_btf_id %u points to wrong type name %s\n",
				btf_id, tname);
			return -EINVAL;
		}
		tname += sizeof(prefix) - 1;
		t = btf_type_by_id(btf, t->type);
		if (!btf_type_is_ptr(t))
			/* should never happen in valid vmlinux build */
			return -EINVAL;
		t = btf_type_by_id(btf, t->type);
		if (!btf_type_is_func_proto(t))
			/* should never happen in valid vmlinux build */
			return -EINVAL;

		/* remember two read only pointers that are valid for
		 * the life time of the kernel
		 */
		prog->aux->attach_func_name = tname;
		prog->aux->attach_func_proto = t;
		prog->aux->attach_btf_trace = true;
		return 0;
	case BPF_TRACE_ITER:
		if (!btf_type_is_func(t)) {
			verbose(env, "attach_btf_id %u is not a function\n",
				btf_id);
			return -EINVAL;
		}
		t = btf_type_by_id(btf, t->type);
		if (!btf_type_is_func_proto(t))
			return -EINVAL;
		prog->aux->attach_func_name = tname;
		prog->aux->attach_func_proto = t;
		if (!bpf_iter_prog_supported(prog))
			return -EINVAL;
		ret = btf_distill_func_proto(&env->log, btf, t,
					     tname, &fmodel);
		return ret;
	default:
		if (!prog_extension)
			return -EINVAL;
		fallthrough;
	case BPF_MODIFY_RETURN:
	case BPF_LSM_MAC:
	case BPF_TRACE_FENTRY:
	case BPF_TRACE_FEXIT:
		prog->aux->attach_func_name = tname;
		if (prog->type == BPF_PROG_TYPE_LSM) {
			ret = bpf_lsm_verify_prog(&env->log, prog);
			if (ret < 0)
				return ret;
		}

		if (!btf_type_is_func(t)) {
			verbose(env, "attach_btf_id %u is not a function\n",
				btf_id);
			return -EINVAL;
		}
		if (prog_extension &&
		    btf_check_type_match(env, prog, btf, t))
			return -EINVAL;
		t = btf_type_by_id(btf, t->type);
		if (!btf_type_is_func_proto(t))
			return -EINVAL;
		tr = bpf_trampoline_lookup(key);
		if (!tr)
			return -ENOMEM;
		/* t is either vmlinux type or another program's type */
		prog->aux->attach_func_proto = t;
		mutex_lock(&tr->mutex);
		if (tr->func.addr) {
			prog->aux->trampoline = tr;
			goto out;
		}
		if (tgt_prog && conservative) {
			prog->aux->attach_func_proto = NULL;
			t = NULL;
		}
		ret = btf_distill_func_proto(&env->log, btf, t,
					     tname, &tr->func.model);
		if (ret < 0)
			goto out;
		if (tgt_prog) {
			if (subprog == 0)
				addr = (long) tgt_prog->bpf_func;
			else
				addr = (long) tgt_prog->aux->func[subprog]->bpf_func;
		} else {
			addr = kallsyms_lookup_name(tname);
			if (!addr) {
				verbose(env,
					"The address of function %s cannot be found\n",
					tname);
				ret = -ENOENT;
				goto out;
			}
		}

		if (prog->expected_attach_type == BPF_MODIFY_RETURN) {
			ret = check_attach_modify_return(prog, addr);
			if (ret)
				verbose(env, "%s() is not modifiable\n",
					prog->aux->attach_func_name);
		}

		if (ret)
			goto out;
		tr->func.addr = (void *)addr;
		prog->aux->trampoline = tr;
out:
		mutex_unlock(&tr->mutex);
		if (ret)
			bpf_trampoline_put(tr);
		return ret;
	}
}