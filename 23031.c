static void mlx5_fpga_conn_event(struct mlx5_core_qp *mqp, int event)
{
	struct mlx5_fpga_conn *conn;

	conn = container_of(mqp, struct mlx5_fpga_conn, qp.mqp);
	mlx5_fpga_warn(conn->fdev, "QP event %u on QP #%u\n", event, mqp->qpn);
}