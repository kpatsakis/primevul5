static inline int mlx5_fpga_conn_rts_qp(struct mlx5_fpga_conn *conn)
{
	struct mlx5_fpga_device *fdev = conn->fdev;
	struct mlx5_core_dev *mdev = fdev->mdev;
	u32 *qpc = NULL;
	u32 opt_mask;
	int err;

	mlx5_fpga_dbg(conn->fdev, "QP RTS\n");

	qpc = kzalloc(MLX5_ST_SZ_BYTES(qpc), GFP_KERNEL);
	if (!qpc) {
		err = -ENOMEM;
		goto out;
	}

	MLX5_SET(qpc, qpc, log_ack_req_freq, 8);
	MLX5_SET(qpc, qpc, min_rnr_nak, 0x12);
	MLX5_SET(qpc, qpc, primary_address_path.ack_timeout, 0x12); /* ~1.07s */
	MLX5_SET(qpc, qpc, next_send_psn,
		 MLX5_GET(fpga_qpc, conn->fpga_qpc, next_rcv_psn));
	MLX5_SET(qpc, qpc, retry_count, 7);
	MLX5_SET(qpc, qpc, rnr_retry, 7); /* Infinite retry if RNR NACK */

	opt_mask = MLX5_QP_OPTPAR_RNR_TIMEOUT;
	err = mlx5_core_qp_modify(mdev, MLX5_CMD_OP_RTR2RTS_QP, opt_mask, qpc,
				  &conn->qp.mqp);
	if (err) {
		mlx5_fpga_warn(fdev, "qp_modify RST2INIT failed: %d\n", err);
		goto out;
	}

out:
	kfree(qpc);
	return err;
}