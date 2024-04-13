static int check_func_arg(struct bpf_verifier_env *env, u32 arg,
			  struct bpf_call_arg_meta *meta,
			  const struct bpf_func_proto *fn)
{
	u32 regno = BPF_REG_1 + arg;
	struct bpf_reg_state *regs = cur_regs(env), *reg = &regs[regno];
	enum bpf_reg_type expected_type, type = reg->type;
	enum bpf_arg_type arg_type = fn->arg_type[arg];
	int err = 0;

	if (arg_type == ARG_DONTCARE)
		return 0;

	err = check_reg_arg(env, regno, SRC_OP);
	if (err)
		return err;

	if (arg_type == ARG_ANYTHING) {
		if (is_pointer_value(env, regno)) {
			verbose(env, "R%d leaks addr into helper function\n",
				regno);
			return -EACCES;
		}
		return 0;
	}

	if (type_is_pkt_pointer(type) &&
	    !may_access_direct_pkt_data(env, meta, BPF_READ)) {
		verbose(env, "helper access to the packet is not allowed\n");
		return -EACCES;
	}

	if (arg_type == ARG_PTR_TO_MAP_KEY ||
	    arg_type == ARG_PTR_TO_MAP_VALUE ||
	    arg_type == ARG_PTR_TO_UNINIT_MAP_VALUE ||
	    arg_type == ARG_PTR_TO_MAP_VALUE_OR_NULL) {
		expected_type = PTR_TO_STACK;
		if (register_is_null(reg) &&
		    arg_type == ARG_PTR_TO_MAP_VALUE_OR_NULL)
			/* final test in check_stack_boundary() */;
		else if (!type_is_pkt_pointer(type) &&
			 type != PTR_TO_MAP_VALUE &&
			 type != expected_type)
			goto err_type;
	} else if (arg_type == ARG_CONST_SIZE ||
		   arg_type == ARG_CONST_SIZE_OR_ZERO ||
		   arg_type == ARG_CONST_ALLOC_SIZE_OR_ZERO) {
		expected_type = SCALAR_VALUE;
		if (type != expected_type)
			goto err_type;
	} else if (arg_type == ARG_CONST_MAP_PTR) {
		expected_type = CONST_PTR_TO_MAP;
		if (type != expected_type)
			goto err_type;
	} else if (arg_type == ARG_PTR_TO_CTX ||
		   arg_type == ARG_PTR_TO_CTX_OR_NULL) {
		expected_type = PTR_TO_CTX;
		if (!(register_is_null(reg) &&
		      arg_type == ARG_PTR_TO_CTX_OR_NULL)) {
			if (type != expected_type)
				goto err_type;
			err = check_ctx_reg(env, reg, regno);
			if (err < 0)
				return err;
		}
	} else if (arg_type == ARG_PTR_TO_SOCK_COMMON) {
		expected_type = PTR_TO_SOCK_COMMON;
		/* Any sk pointer can be ARG_PTR_TO_SOCK_COMMON */
		if (!type_is_sk_pointer(type))
			goto err_type;
		if (reg->ref_obj_id) {
			if (meta->ref_obj_id) {
				verbose(env, "verifier internal error: more than one arg with ref_obj_id R%d %u %u\n",
					regno, reg->ref_obj_id,
					meta->ref_obj_id);
				return -EFAULT;
			}
			meta->ref_obj_id = reg->ref_obj_id;
		}
	} else if (arg_type == ARG_PTR_TO_SOCKET ||
		   arg_type == ARG_PTR_TO_SOCKET_OR_NULL) {
		expected_type = PTR_TO_SOCKET;
		if (!(register_is_null(reg) &&
		      arg_type == ARG_PTR_TO_SOCKET_OR_NULL)) {
			if (type != expected_type)
				goto err_type;
		}
	} else if (arg_type == ARG_PTR_TO_BTF_ID) {
		expected_type = PTR_TO_BTF_ID;
		if (type != expected_type)
			goto err_type;
		if (!fn->check_btf_id) {
			if (reg->btf_id != meta->btf_id) {
				verbose(env, "Helper has type %s got %s in R%d\n",
					kernel_type_name(meta->btf_id),
					kernel_type_name(reg->btf_id), regno);

				return -EACCES;
			}
		} else if (!fn->check_btf_id(reg->btf_id, arg)) {
			verbose(env, "Helper does not support %s in R%d\n",
				kernel_type_name(reg->btf_id), regno);

			return -EACCES;
		}
		if (!tnum_is_const(reg->var_off) || reg->var_off.value || reg->off) {
			verbose(env, "R%d is a pointer to in-kernel struct with non-zero offset\n",
				regno);
			return -EACCES;
		}
	} else if (arg_type == ARG_PTR_TO_SPIN_LOCK) {
		if (meta->func_id == BPF_FUNC_spin_lock) {
			if (process_spin_lock(env, regno, true))
				return -EACCES;
		} else if (meta->func_id == BPF_FUNC_spin_unlock) {
			if (process_spin_lock(env, regno, false))
				return -EACCES;
		} else {
			verbose(env, "verifier internal error\n");
			return -EFAULT;
		}
	} else if (arg_type_is_mem_ptr(arg_type)) {
		expected_type = PTR_TO_STACK;
		/* One exception here. In case function allows for NULL to be
		 * passed in as argument, it's a SCALAR_VALUE type. Final test
		 * happens during stack boundary checking.
		 */
		if (register_is_null(reg) &&
		    (arg_type == ARG_PTR_TO_MEM_OR_NULL ||
		     arg_type == ARG_PTR_TO_ALLOC_MEM_OR_NULL))
			/* final test in check_stack_boundary() */;
		else if (!type_is_pkt_pointer(type) &&
			 type != PTR_TO_MAP_VALUE &&
			 type != PTR_TO_MEM &&
			 type != PTR_TO_RDONLY_BUF &&
			 type != PTR_TO_RDWR_BUF &&
			 type != expected_type)
			goto err_type;
		meta->raw_mode = arg_type == ARG_PTR_TO_UNINIT_MEM;
	} else if (arg_type_is_alloc_mem_ptr(arg_type)) {
		expected_type = PTR_TO_MEM;
		if (register_is_null(reg) &&
		    arg_type == ARG_PTR_TO_ALLOC_MEM_OR_NULL)
			/* final test in check_stack_boundary() */;
		else if (type != expected_type)
			goto err_type;
		if (meta->ref_obj_id) {
			verbose(env, "verifier internal error: more than one arg with ref_obj_id R%d %u %u\n",
				regno, reg->ref_obj_id,
				meta->ref_obj_id);
			return -EFAULT;
		}
		meta->ref_obj_id = reg->ref_obj_id;
	} else if (arg_type_is_int_ptr(arg_type)) {
		expected_type = PTR_TO_STACK;
		if (!type_is_pkt_pointer(type) &&
		    type != PTR_TO_MAP_VALUE &&
		    type != expected_type)
			goto err_type;
	} else {
		verbose(env, "unsupported arg_type %d\n", arg_type);
		return -EFAULT;
	}

	if (arg_type == ARG_CONST_MAP_PTR) {
		/* bpf_map_xxx(map_ptr) call: remember that map_ptr */
		meta->map_ptr = reg->map_ptr;
	} else if (arg_type == ARG_PTR_TO_MAP_KEY) {
		/* bpf_map_xxx(..., map_ptr, ..., key) call:
		 * check that [key, key + map->key_size) are within
		 * stack limits and initialized
		 */
		if (!meta->map_ptr) {
			/* in function declaration map_ptr must come before
			 * map_key, so that it's verified and known before
			 * we have to check map_key here. Otherwise it means
			 * that kernel subsystem misconfigured verifier
			 */
			verbose(env, "invalid map_ptr to access map->key\n");
			return -EACCES;
		}
		err = check_helper_mem_access(env, regno,
					      meta->map_ptr->key_size, false,
					      NULL);
	} else if (arg_type == ARG_PTR_TO_MAP_VALUE ||
		   (arg_type == ARG_PTR_TO_MAP_VALUE_OR_NULL &&
		    !register_is_null(reg)) ||
		   arg_type == ARG_PTR_TO_UNINIT_MAP_VALUE) {
		/* bpf_map_xxx(..., map_ptr, ..., value) call:
		 * check [value, value + map->value_size) validity
		 */
		if (!meta->map_ptr) {
			/* kernel subsystem misconfigured verifier */
			verbose(env, "invalid map_ptr to access map->value\n");
			return -EACCES;
		}
		meta->raw_mode = (arg_type == ARG_PTR_TO_UNINIT_MAP_VALUE);
		err = check_helper_mem_access(env, regno,
					      meta->map_ptr->value_size, false,
					      meta);
	} else if (arg_type_is_mem_size(arg_type)) {
		bool zero_size_allowed = (arg_type == ARG_CONST_SIZE_OR_ZERO);

		/* This is used to refine r0 return value bounds for helpers
		 * that enforce this value as an upper bound on return values.
		 * See do_refine_retval_range() for helpers that can refine
		 * the return value. C type of helper is u32 so we pull register
		 * bound from umax_value however, if negative verifier errors
		 * out. Only upper bounds can be learned because retval is an
		 * int type and negative retvals are allowed.
		 */
		meta->msize_max_value = reg->umax_value;

		/* The register is SCALAR_VALUE; the access check
		 * happens using its boundaries.
		 */
		if (!tnum_is_const(reg->var_off))
			/* For unprivileged variable accesses, disable raw
			 * mode so that the program is required to
			 * initialize all the memory that the helper could
			 * just partially fill up.
			 */
			meta = NULL;

		if (reg->smin_value < 0) {
			verbose(env, "R%d min value is negative, either use unsigned or 'var &= const'\n",
				regno);
			return -EACCES;
		}

		if (reg->umin_value == 0) {
			err = check_helper_mem_access(env, regno - 1, 0,
						      zero_size_allowed,
						      meta);
			if (err)
				return err;
		}

		if (reg->umax_value >= BPF_MAX_VAR_SIZ) {
			verbose(env, "R%d unbounded memory access, use 'var &= const' or 'if (var < const)'\n",
				regno);
			return -EACCES;
		}
		err = check_helper_mem_access(env, regno - 1,
					      reg->umax_value,
					      zero_size_allowed, meta);
		if (!err)
			err = mark_chain_precision(env, regno);
	} else if (arg_type_is_alloc_size(arg_type)) {
		if (!tnum_is_const(reg->var_off)) {
			verbose(env, "R%d unbounded size, use 'var &= const' or 'if (var < const)'\n",
				regno);
			return -EACCES;
		}
		meta->mem_size = reg->var_off.value;
	} else if (arg_type_is_int_ptr(arg_type)) {
		int size = int_ptr_type_to_size(arg_type);

		err = check_helper_mem_access(env, regno, size, false, meta);
		if (err)
			return err;
		err = check_ptr_alignment(env, reg, 0, size, true);
	}

	return err;
err_type:
	verbose(env, "R%d type=%s expected=%s\n", regno,
		reg_type_str[type], reg_type_str[expected_type]);
	return -EACCES;
}