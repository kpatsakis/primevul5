static inline int mlx5_fpga_conn_init_qp(struct mlx5_fpga_conn *conn)
{
	struct mlx5_fpga_device *fdev = conn->fdev;
	struct mlx5_core_dev *mdev = fdev->mdev;
	u32 *qpc = NULL;
	int err;

	mlx5_fpga_dbg(conn->fdev, "Modifying QP %u to INIT\n", conn->qp.mqp.qpn);

	qpc = kzalloc(MLX5_ST_SZ_BYTES(qpc), GFP_KERNEL);
	if (!qpc) {
		err = -ENOMEM;
		goto out;
	}

	MLX5_SET(qpc, qpc, st, MLX5_QP_ST_RC);
	MLX5_SET(qpc, qpc, pm_state, MLX5_QP_PM_MIGRATED);
	MLX5_SET(qpc, qpc, primary_address_path.pkey_index, MLX5_FPGA_PKEY_INDEX);
	MLX5_SET(qpc, qpc, primary_address_path.vhca_port_num, MLX5_FPGA_PORT_NUM);
	MLX5_SET(qpc, qpc, pd, conn->fdev->conn_res.pdn);
	MLX5_SET(qpc, qpc, cqn_snd, conn->cq.mcq.cqn);
	MLX5_SET(qpc, qpc, cqn_rcv, conn->cq.mcq.cqn);
	MLX5_SET64(qpc, qpc, dbr_addr, conn->qp.wq_ctrl.db.dma);

	err = mlx5_core_qp_modify(mdev, MLX5_CMD_OP_RST2INIT_QP, 0, qpc,
				  &conn->qp.mqp);
	if (err) {
		mlx5_fpga_warn(fdev, "qp_modify RST2INIT failed: %d\n", err);
		goto out;
	}

out:
	kfree(qpc);
	return err;
}