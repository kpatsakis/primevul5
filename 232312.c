static int check_stack_boundary(struct bpf_verifier_env *env, int regno,
				int access_size, bool zero_size_allowed,
				struct bpf_call_arg_meta *meta)
{
	struct bpf_reg_state *reg = reg_state(env, regno);
	struct bpf_func_state *state = func(env, reg);
	int err, min_off, max_off, i, j, slot, spi;

	if (tnum_is_const(reg->var_off)) {
		min_off = max_off = reg->var_off.value + reg->off;
		err = __check_stack_boundary(env, regno, min_off, access_size,
					     zero_size_allowed);
		if (err)
			return err;
	} else {
		/* Variable offset is prohibited for unprivileged mode for
		 * simplicity since it requires corresponding support in
		 * Spectre masking for stack ALU.
		 * See also retrieve_ptr_limit().
		 */
		if (!env->bypass_spec_v1) {
			char tn_buf[48];

			tnum_strn(tn_buf, sizeof(tn_buf), reg->var_off);
			verbose(env, "R%d indirect variable offset stack access prohibited for !root, var_off=%s\n",
				regno, tn_buf);
			return -EACCES;
		}
		/* Only initialized buffer on stack is allowed to be accessed
		 * with variable offset. With uninitialized buffer it's hard to
		 * guarantee that whole memory is marked as initialized on
		 * helper return since specific bounds are unknown what may
		 * cause uninitialized stack leaking.
		 */
		if (meta && meta->raw_mode)
			meta = NULL;

		if (reg->smax_value >= BPF_MAX_VAR_OFF ||
		    reg->smax_value <= -BPF_MAX_VAR_OFF) {
			verbose(env, "R%d unbounded indirect variable offset stack access\n",
				regno);
			return -EACCES;
		}
		min_off = reg->smin_value + reg->off;
		max_off = reg->smax_value + reg->off;
		err = __check_stack_boundary(env, regno, min_off, access_size,
					     zero_size_allowed);
		if (err) {
			verbose(env, "R%d min value is outside of stack bound\n",
				regno);
			return err;
		}
		err = __check_stack_boundary(env, regno, max_off, access_size,
					     zero_size_allowed);
		if (err) {
			verbose(env, "R%d max value is outside of stack bound\n",
				regno);
			return err;
		}
	}

	if (meta && meta->raw_mode) {
		meta->access_size = access_size;
		meta->regno = regno;
		return 0;
	}

	for (i = min_off; i < max_off + access_size; i++) {
		u8 *stype;

		slot = -i - 1;
		spi = slot / BPF_REG_SIZE;
		if (state->allocated_stack <= slot)
			goto err;
		stype = &state->stack[spi].slot_type[slot % BPF_REG_SIZE];
		if (*stype == STACK_MISC)
			goto mark;
		if (*stype == STACK_ZERO) {
			/* helper can write anything into the stack */
			*stype = STACK_MISC;
			goto mark;
		}

		if (state->stack[spi].slot_type[0] == STACK_SPILL &&
		    state->stack[spi].spilled_ptr.type == PTR_TO_BTF_ID)
			goto mark;

		if (state->stack[spi].slot_type[0] == STACK_SPILL &&
		    (state->stack[spi].spilled_ptr.type == SCALAR_VALUE ||
		     env->allow_ptr_leaks)) {
			__mark_reg_unknown(env, &state->stack[spi].spilled_ptr);
			for (j = 0; j < BPF_REG_SIZE; j++)
				state->stack[spi].slot_type[j] = STACK_MISC;
			goto mark;
		}

err:
		if (tnum_is_const(reg->var_off)) {
			verbose(env, "invalid indirect read from stack off %d+%d size %d\n",
				min_off, i - min_off, access_size);
		} else {
			char tn_buf[48];

			tnum_strn(tn_buf, sizeof(tn_buf), reg->var_off);
			verbose(env, "invalid indirect read from stack var_off %s+%d size %d\n",
				tn_buf, i - min_off, access_size);
		}
		return -EACCES;
mark:
		/* reading any byte out of 8-byte 'spill_slot' will cause
		 * the whole slot to be marked as 'read'
		 */
		mark_reg_read(env, &state->stack[spi].spilled_ptr,
			      state->stack[spi].spilled_ptr.parent,
			      REG_LIVE_READ64);
	}
	return update_stack_depth(env, state, min_off);
}