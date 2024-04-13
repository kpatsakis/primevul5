static int mlx5_fpga_conn_create_wq(struct mlx5_fpga_conn *conn, void *qpc)
{
	struct mlx5_fpga_device *fdev = conn->fdev;
	struct mlx5_core_dev *mdev = fdev->mdev;
	struct mlx5_wq_param wqp;

	wqp.buf_numa_node = mdev->priv.numa_node;
	wqp.db_numa_node  = mdev->priv.numa_node;

	return mlx5_wq_qp_create(mdev, &wqp, qpc, &conn->qp.wq,
				 &conn->qp.wq_ctrl);
}