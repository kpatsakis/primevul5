static inline int mlx5_fpga_conn_rtr_qp(struct mlx5_fpga_conn *conn)
{
	struct mlx5_fpga_device *fdev = conn->fdev;
	struct mlx5_core_dev *mdev = fdev->mdev;
	u32 *qpc = NULL;
	int err;

	mlx5_fpga_dbg(conn->fdev, "QP RTR\n");

	qpc = kzalloc(MLX5_ST_SZ_BYTES(qpc), GFP_KERNEL);
	if (!qpc) {
		err = -ENOMEM;
		goto out;
	}

	MLX5_SET(qpc, qpc, mtu, MLX5_QPC_MTU_1K_BYTES);
	MLX5_SET(qpc, qpc, log_msg_max, (u8)MLX5_CAP_GEN(mdev, log_max_msg));
	MLX5_SET(qpc, qpc, remote_qpn, conn->fpga_qpn);
	MLX5_SET(qpc, qpc, next_rcv_psn,
		 MLX5_GET(fpga_qpc, conn->fpga_qpc, next_send_psn));
	MLX5_SET(qpc, qpc, primary_address_path.pkey_index, MLX5_FPGA_PKEY_INDEX);
	MLX5_SET(qpc, qpc, primary_address_path.vhca_port_num, MLX5_FPGA_PORT_NUM);
	ether_addr_copy(MLX5_ADDR_OF(qpc, qpc, primary_address_path.rmac_47_32),
			MLX5_ADDR_OF(fpga_qpc, conn->fpga_qpc, fpga_mac_47_32));
	MLX5_SET(qpc, qpc, primary_address_path.udp_sport,
		 MLX5_CAP_ROCE(mdev, r_roce_min_src_udp_port));
	MLX5_SET(qpc, qpc, primary_address_path.src_addr_index,
		 conn->qp.sgid_index);
	MLX5_SET(qpc, qpc, primary_address_path.hop_limit, 0);
	memcpy(MLX5_ADDR_OF(qpc, qpc, primary_address_path.rgid_rip),
	       MLX5_ADDR_OF(fpga_qpc, conn->fpga_qpc, fpga_ip),
	       MLX5_FLD_SZ_BYTES(qpc, primary_address_path.rgid_rip));

	err = mlx5_core_qp_modify(mdev, MLX5_CMD_OP_INIT2RTR_QP, 0, qpc,
				  &conn->qp.mqp);
	if (err) {
		mlx5_fpga_warn(fdev, "qp_modify RST2INIT failed: %d\n", err);
		goto out;
	}

out:
	kfree(qpc);
	return err;
}