static void mlx5_fpga_conn_post_send(struct mlx5_fpga_conn *conn,
				     struct mlx5_fpga_dma_buf *buf)
{
	struct mlx5_wqe_ctrl_seg *ctrl;
	struct mlx5_wqe_data_seg *data;
	unsigned int ix, sgi;
	int size = 1;

	ix = conn->qp.sq.pc & (conn->qp.sq.size - 1);

	ctrl = mlx5_wq_cyc_get_wqe(&conn->qp.wq.sq, ix);
	data = (void *)(ctrl + 1);

	for (sgi = 0; sgi < ARRAY_SIZE(buf->sg); sgi++) {
		if (!buf->sg[sgi].data)
			break;
		data->byte_count = cpu_to_be32(buf->sg[sgi].size);
		data->lkey = cpu_to_be32(conn->fdev->conn_res.mkey.key);
		data->addr = cpu_to_be64(buf->sg[sgi].dma_addr);
		data++;
		size++;
	}

	ctrl->imm = 0;
	ctrl->fm_ce_se = MLX5_WQE_CTRL_CQ_UPDATE;
	ctrl->opmod_idx_opcode = cpu_to_be32(((conn->qp.sq.pc & 0xffff) << 8) |
					     MLX5_OPCODE_SEND);
	ctrl->qpn_ds = cpu_to_be32(size | (conn->qp.mqp.qpn << 8));

	conn->qp.sq.pc++;
	conn->qp.sq.bufs[ix] = buf;
	mlx5_fpga_conn_notify_hw(conn, ctrl);
}