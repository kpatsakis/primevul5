static void mlx5_fpga_conn_notify_hw(struct mlx5_fpga_conn *conn, void *wqe)
{
	/* ensure wqe is visible to device before updating doorbell record */
	dma_wmb();
	*conn->qp.wq.sq.db = cpu_to_be32(conn->qp.sq.pc);
	/* Make sure that doorbell record is visible before ringing */
	wmb();
	mlx5_write64(wqe, conn->fdev->conn_res.uar->map + MLX5_BF_OFFSET);
}