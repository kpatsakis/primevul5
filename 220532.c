static void mark_ptr_or_null_reg(struct bpf_func_state *state,
				 struct bpf_reg_state *reg, u32 id,
				 bool is_null)
{
	if (reg_type_may_be_null(reg->type) && reg->id == id) {
		/* Old offset (both fixed and variable parts) should
		 * have been known-zero, because we don't allow pointer
		 * arithmetic on pointers that might be NULL.
		 */
		if (WARN_ON_ONCE(reg->smin_value || reg->smax_value ||
				 !tnum_equals_const(reg->var_off, 0) ||
				 reg->off)) {
			__mark_reg_known_zero(reg);
			reg->off = 0;
		}
		if (is_null) {
			reg->type = SCALAR_VALUE;
		} else if (reg->type == PTR_TO_MAP_VALUE_OR_NULL) {
			const struct bpf_map *map = reg->map_ptr;

			if (map->inner_map_meta) {
				reg->type = CONST_PTR_TO_MAP;
				reg->map_ptr = map->inner_map_meta;
			} else if (map->map_type == BPF_MAP_TYPE_XSKMAP) {
				reg->type = PTR_TO_XDP_SOCK;
			} else if (map->map_type == BPF_MAP_TYPE_SOCKMAP ||
				   map->map_type == BPF_MAP_TYPE_SOCKHASH) {
				reg->type = PTR_TO_SOCKET;
			} else {
				reg->type = PTR_TO_MAP_VALUE;
			}
		} else if (reg->type == PTR_TO_SOCKET_OR_NULL) {
			reg->type = PTR_TO_SOCKET;
		} else if (reg->type == PTR_TO_SOCK_COMMON_OR_NULL) {
			reg->type = PTR_TO_SOCK_COMMON;
		} else if (reg->type == PTR_TO_TCP_SOCK_OR_NULL) {
			reg->type = PTR_TO_TCP_SOCK;
		} else if (reg->type == PTR_TO_BTF_ID_OR_NULL) {
			reg->type = PTR_TO_BTF_ID;
		} else if (reg->type == PTR_TO_MEM_OR_NULL) {
			reg->type = PTR_TO_MEM;
		} else if (reg->type == PTR_TO_RDONLY_BUF_OR_NULL) {
			reg->type = PTR_TO_RDONLY_BUF;
		} else if (reg->type == PTR_TO_RDWR_BUF_OR_NULL) {
			reg->type = PTR_TO_RDWR_BUF;
		}
		if (is_null) {
			/* We don't need id and ref_obj_id from this point
			 * onwards anymore, thus we should better reset it,
			 * so that state pruning has chances to take effect.
			 */
			reg->id = 0;
			reg->ref_obj_id = 0;
		} else if (!reg_may_point_to_spin_lock(reg)) {
			/* For not-NULL ptr, reg->ref_obj_id will be reset
			 * in release_reg_references().
			 *
			 * reg->id is still used by spin_lock ptr. Other
			 * than spin_lock ptr type, reg->id can be reset.
			 */
			reg->id = 0;
		}
	}
}