static void mlx5_fpga_conn_cq_event(struct mlx5_core_cq *mcq,
				    enum mlx5_event event)
{
	struct mlx5_fpga_conn *conn;

	conn = container_of(mcq, struct mlx5_fpga_conn, cq.mcq);
	mlx5_fpga_warn(conn->fdev, "CQ event %u on CQ #%u\n", event, mcq->cqn);
}