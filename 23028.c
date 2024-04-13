static void mlx5_fpga_conn_rq_cqe(struct mlx5_fpga_conn *conn,
				  struct mlx5_cqe64 *cqe, u8 status)
{
	struct mlx5_fpga_dma_buf *buf;
	int ix, err;

	ix = be16_to_cpu(cqe->wqe_counter) & (conn->qp.rq.size - 1);
	buf = conn->qp.rq.bufs[ix];
	conn->qp.rq.bufs[ix] = NULL;
	conn->qp.rq.cc++;

	if (unlikely(status && (status != MLX5_CQE_SYNDROME_WR_FLUSH_ERR)))
		mlx5_fpga_warn(conn->fdev, "RQ buf %p on FPGA QP %u completion status %d\n",
			       buf, conn->fpga_qpn, status);
	else
		mlx5_fpga_dbg(conn->fdev, "RQ buf %p on FPGA QP %u completion status %d\n",
			      buf, conn->fpga_qpn, status);

	mlx5_fpga_conn_unmap_buf(conn, buf);

	if (unlikely(status || !conn->qp.active)) {
		conn->qp.active = false;
		kfree(buf);
		return;
	}

	buf->sg[0].size = be32_to_cpu(cqe->byte_cnt);
	mlx5_fpga_dbg(conn->fdev, "Message with %u bytes received successfully\n",
		      buf->sg[0].size);
	conn->recv_cb(conn->cb_arg, buf);

	buf->sg[0].size = MLX5_FPGA_RECV_SIZE;
	err = mlx5_fpga_conn_post_recv(conn, buf);
	if (unlikely(err)) {
		mlx5_fpga_warn(conn->fdev,
			       "Failed to re-post recv buf: %d\n", err);
		kfree(buf);
	}
}