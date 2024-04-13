static inline int mlx5_fpga_conn_reset_qp(struct mlx5_fpga_conn *conn)
{
	struct mlx5_core_dev *mdev = conn->fdev->mdev;

	mlx5_fpga_dbg(conn->fdev, "Modifying QP %u to RST\n", conn->qp.mqp.qpn);

	return mlx5_core_qp_modify(mdev, MLX5_CMD_OP_2RST_QP, 0, NULL,
				   &conn->qp.mqp);
}