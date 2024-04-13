static int iwl_dump_ini_txf_iter(struct iwl_fw_runtime *fwrt,
				 struct iwl_fw_ini_region_cfg *reg,
				 void *range_ptr, int idx)
{
	struct iwl_fw_ini_error_dump_range *range = range_ptr;
	struct iwl_txf_iter_data *iter = &fwrt->dump.txf_iter_data;
	struct iwl_fw_ini_error_dump_register *reg_dump = (void *)range->data;
	u32 offs = le32_to_cpu(reg->offset), addr;
	u32 registers_size =
		le32_to_cpu(reg->fifos.num_of_registers) * sizeof(*reg_dump);
	__le32 *data;
	unsigned long flags;
	int i;

	if (!iwl_ini_txf_iter(fwrt, reg, idx))
		return -EIO;

	if (!iwl_trans_grab_nic_access(fwrt->trans, &flags))
		return -EBUSY;

	range->fifo_hdr.fifo_num = cpu_to_le32(iter->fifo);
	range->fifo_hdr.num_of_registers = reg->fifos.num_of_registers;
	range->range_data_size = cpu_to_le32(iter->fifo_size + registers_size);

	iwl_write_prph_no_grab(fwrt->trans, TXF_LARC_NUM + offs, iter->fifo);

	/*
	 * read txf registers. for each register, write to the dump the
	 * register address and its value
	 */
	for (i = 0; i < le32_to_cpu(reg->fifos.num_of_registers); i++) {
		addr = le32_to_cpu(reg->start_addr[i]) + offs;

		reg_dump->addr = cpu_to_le32(addr);
		reg_dump->data = cpu_to_le32(iwl_read_prph_no_grab(fwrt->trans,
								   addr));

		reg_dump++;
	}

	if (reg->fifos.header_only) {
		range->range_data_size = cpu_to_le32(registers_size);
		goto out;
	}

	/* Set the TXF_READ_MODIFY_ADDR to TXF_WR_PTR */
	iwl_write_prph_no_grab(fwrt->trans, TXF_READ_MODIFY_ADDR + offs,
			       TXF_WR_PTR + offs);

	/* Dummy-read to advance the read pointer to the head */
	iwl_read_prph_no_grab(fwrt->trans, TXF_READ_MODIFY_DATA + offs);

	/* Read FIFO */
	addr = TXF_READ_MODIFY_DATA + offs;
	data = (void *)reg_dump;
	for (i = 0; i < iter->fifo_size; i += sizeof(*data))
		*data++ = cpu_to_le32(iwl_read_prph_no_grab(fwrt->trans, addr));

out:
	iwl_trans_release_nic_access(fwrt->trans, &flags);

	return sizeof(*range) + le32_to_cpu(range->range_data_size);
}