static void mlx5_fpga_conn_flush_send_bufs(struct mlx5_fpga_conn *conn)
{
	struct mlx5_fpga_dma_buf *buf, *temp;
	int ix;

	for (ix = 0; ix < conn->qp.sq.size; ix++) {
		buf = conn->qp.sq.bufs[ix];
		if (!buf)
			continue;
		conn->qp.sq.bufs[ix] = NULL;
		mlx5_fpga_conn_unmap_buf(conn, buf);
		if (!buf->complete)
			continue;
		buf->complete(conn, conn->fdev, buf, MLX5_CQE_SYNDROME_WR_FLUSH_ERR);
	}
	list_for_each_entry_safe(buf, temp, &conn->qp.sq.backlog, list) {
		mlx5_fpga_conn_unmap_buf(conn, buf);
		if (!buf->complete)
			continue;
		buf->complete(conn, conn->fdev, buf, MLX5_CQE_SYNDROME_WR_FLUSH_ERR);
	}
}