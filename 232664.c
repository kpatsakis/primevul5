static void bpf_tail_call_direct_fixup(struct bpf_prog *prog)
{
	struct bpf_jit_poke_descriptor *poke;
	struct bpf_array *array;
	struct bpf_prog *target;
	int i, ret;

	for (i = 0; i < prog->aux->size_poke_tab; i++) {
		poke = &prog->aux->poke_tab[i];
		WARN_ON_ONCE(READ_ONCE(poke->tailcall_target_stable));

		if (poke->reason != BPF_POKE_REASON_TAIL_CALL)
			continue;

		array = container_of(poke->tail_call.map, struct bpf_array, map);
		mutex_lock(&array->aux->poke_mutex);
		target = array->ptrs[poke->tail_call.key];
		if (target) {
			/* Plain memcpy is used when image is not live yet
			 * and still not locked as read-only. Once poke
			 * location is active (poke->tailcall_target_stable),
			 * any parallel bpf_arch_text_poke() might occur
			 * still on the read-write image until we finally
			 * locked it as read-only. Both modifications on
			 * the given image are under text_mutex to avoid
			 * interference.
			 */
			ret = __bpf_arch_text_poke(poke->tailcall_target,
						   BPF_MOD_JUMP, NULL,
						   (u8 *)target->bpf_func +
						   poke->adj_off, false);
			BUG_ON(ret < 0);
			ret = __bpf_arch_text_poke(poke->tailcall_bypass,
						   BPF_MOD_JUMP,
						   (u8 *)poke->tailcall_target +
						   X86_PATCH_SIZE, NULL, false);
			BUG_ON(ret < 0);
		}
		WRITE_ONCE(poke->tailcall_target_stable, true);
		mutex_unlock(&array->aux->poke_mutex);
	}
}