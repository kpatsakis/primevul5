int mlx5_fpga_conn_send(struct mlx5_fpga_conn *conn,
			struct mlx5_fpga_dma_buf *buf)
{
	unsigned long flags;
	int err;

	if (!conn->qp.active)
		return -ENOTCONN;

	buf->dma_dir = DMA_TO_DEVICE;
	err = mlx5_fpga_conn_map_buf(conn, buf);
	if (err)
		return err;

	spin_lock_irqsave(&conn->qp.sq.lock, flags);

	if (conn->qp.sq.pc - conn->qp.sq.cc >= conn->qp.sq.size) {
		list_add_tail(&buf->list, &conn->qp.sq.backlog);
		goto out_unlock;
	}

	mlx5_fpga_conn_post_send(conn, buf);

out_unlock:
	spin_unlock_irqrestore(&conn->qp.sq.lock, flags);
	return err;
}