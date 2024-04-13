static void mlx5_fpga_conn_destroy_cq(struct mlx5_fpga_conn *conn)
{
	tasklet_disable(&conn->cq.tasklet);
	tasklet_kill(&conn->cq.tasklet);
	mlx5_core_destroy_cq(conn->fdev->mdev, &conn->cq.mcq);
	mlx5_wq_destroy(&conn->cq.wq_ctrl);
}