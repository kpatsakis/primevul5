static void mlx5_fpga_conn_sq_cqe(struct mlx5_fpga_conn *conn,
				  struct mlx5_cqe64 *cqe, u8 status)
{
	struct mlx5_fpga_dma_buf *buf, *nextbuf;
	unsigned long flags;
	int ix;

	spin_lock_irqsave(&conn->qp.sq.lock, flags);

	ix = be16_to_cpu(cqe->wqe_counter) & (conn->qp.sq.size - 1);
	buf = conn->qp.sq.bufs[ix];
	conn->qp.sq.bufs[ix] = NULL;
	conn->qp.sq.cc++;

	/* Handle backlog still under the spinlock to ensure message post order */
	if (unlikely(!list_empty(&conn->qp.sq.backlog))) {
		if (likely(conn->qp.active)) {
			nextbuf = list_first_entry(&conn->qp.sq.backlog,
						   struct mlx5_fpga_dma_buf, list);
			list_del(&nextbuf->list);
			mlx5_fpga_conn_post_send(conn, nextbuf);
		}
	}

	spin_unlock_irqrestore(&conn->qp.sq.lock, flags);

	if (unlikely(status && (status != MLX5_CQE_SYNDROME_WR_FLUSH_ERR)))
		mlx5_fpga_warn(conn->fdev, "SQ buf %p on FPGA QP %u completion status %d\n",
			       buf, conn->fpga_qpn, status);
	else
		mlx5_fpga_dbg(conn->fdev, "SQ buf %p on FPGA QP %u completion status %d\n",
			      buf, conn->fpga_qpn, status);

	mlx5_fpga_conn_unmap_buf(conn, buf);

	if (likely(buf->complete))
		buf->complete(conn, conn->fdev, buf, status);

	if (unlikely(status))
		conn->qp.active = false;
}