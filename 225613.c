static void iwl_read_prph_block(struct iwl_trans *trans, u32 start,
				u32 len_bytes, __le32 *data)
{
	u32 i;

	for (i = 0; i < len_bytes; i += 4)
		*data++ = cpu_to_le32(iwl_read_prph_no_grab(trans, start + i));
}