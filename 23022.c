static void mlx5_fpga_conn_cq_tasklet(unsigned long data)
{
	struct mlx5_fpga_conn *conn = (void *)data;

	if (unlikely(!conn->qp.active))
		return;
	mlx5_fpga_conn_cqes(conn, MLX5_FPGA_CQ_BUDGET);
}