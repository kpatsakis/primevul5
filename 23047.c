static void mlx5_fpga_conn_cq_complete(struct mlx5_core_cq *mcq,
				       struct mlx5_eqe *eqe)
{
	struct mlx5_fpga_conn *conn;

	conn = container_of(mcq, struct mlx5_fpga_conn, cq.mcq);
	if (unlikely(!conn->qp.active))
		return;
	mlx5_fpga_conn_cqes(conn, MLX5_FPGA_CQ_BUDGET);
}