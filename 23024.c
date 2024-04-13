void mlx5_fpga_conn_destroy(struct mlx5_fpga_conn *conn)
{
	struct mlx5_fpga_device *fdev = conn->fdev;
	struct mlx5_core_dev *mdev = fdev->mdev;
	int err = 0;

	conn->qp.active = false;
	tasklet_disable(&conn->cq.tasklet);
	synchronize_irq(conn->cq.mcq.irqn);

	mlx5_fpga_destroy_qp(conn->fdev->mdev, conn->fpga_qpn);
	err = mlx5_core_qp_modify(mdev, MLX5_CMD_OP_2ERR_QP, 0, NULL,
				  &conn->qp.mqp);
	if (err)
		mlx5_fpga_warn(fdev, "qp_modify 2ERR failed: %d\n", err);
	mlx5_fpga_conn_destroy_qp(conn);
	mlx5_fpga_conn_destroy_cq(conn);

	mlx5_core_roce_gid_set(conn->fdev->mdev, conn->qp.sgid_index, 0, 0,
			       NULL, NULL, false, 0, MLX5_FPGA_PORT_NUM);
	mlx5_core_reserved_gid_free(conn->fdev->mdev, conn->qp.sgid_index);
	kfree(conn);
}