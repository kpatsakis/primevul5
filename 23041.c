static int mlx5_fpga_conn_create_qp(struct mlx5_fpga_conn *conn,
				    unsigned int tx_size, unsigned int rx_size)
{
	struct mlx5_fpga_device *fdev = conn->fdev;
	struct mlx5_core_dev *mdev = fdev->mdev;
	u32 temp_qpc[MLX5_ST_SZ_DW(qpc)] = {0};
	void *in = NULL, *qpc;
	int err, inlen;

	conn->qp.rq.pc = 0;
	conn->qp.rq.cc = 0;
	conn->qp.rq.size = roundup_pow_of_two(rx_size);
	conn->qp.sq.pc = 0;
	conn->qp.sq.cc = 0;
	conn->qp.sq.size = roundup_pow_of_two(tx_size);

	MLX5_SET(qpc, temp_qpc, log_rq_stride, ilog2(MLX5_SEND_WQE_DS) - 4);
	MLX5_SET(qpc, temp_qpc, log_rq_size, ilog2(conn->qp.rq.size));
	MLX5_SET(qpc, temp_qpc, log_sq_size, ilog2(conn->qp.sq.size));
	err = mlx5_fpga_conn_create_wq(conn, temp_qpc);
	if (err)
		goto out;

	conn->qp.rq.bufs = kvcalloc(conn->qp.rq.size,
				    sizeof(conn->qp.rq.bufs[0]),
				    GFP_KERNEL);
	if (!conn->qp.rq.bufs) {
		err = -ENOMEM;
		goto err_wq;
	}

	conn->qp.sq.bufs = kvcalloc(conn->qp.sq.size,
				    sizeof(conn->qp.sq.bufs[0]),
				    GFP_KERNEL);
	if (!conn->qp.sq.bufs) {
		err = -ENOMEM;
		goto err_rq_bufs;
	}

	inlen = MLX5_ST_SZ_BYTES(create_qp_in) +
		MLX5_FLD_SZ_BYTES(create_qp_in, pas[0]) *
		conn->qp.wq_ctrl.buf.npages;
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in) {
		err = -ENOMEM;
		goto err_sq_bufs;
	}

	qpc = MLX5_ADDR_OF(create_qp_in, in, qpc);
	MLX5_SET(qpc, qpc, uar_page, fdev->conn_res.uar->index);
	MLX5_SET(qpc, qpc, log_page_size,
		 conn->qp.wq_ctrl.buf.page_shift - MLX5_ADAPTER_PAGE_SHIFT);
	MLX5_SET(qpc, qpc, fre, 1);
	MLX5_SET(qpc, qpc, rlky, 1);
	MLX5_SET(qpc, qpc, st, MLX5_QP_ST_RC);
	MLX5_SET(qpc, qpc, pm_state, MLX5_QP_PM_MIGRATED);
	MLX5_SET(qpc, qpc, pd, fdev->conn_res.pdn);
	MLX5_SET(qpc, qpc, log_rq_stride, ilog2(MLX5_SEND_WQE_DS) - 4);
	MLX5_SET(qpc, qpc, log_rq_size, ilog2(conn->qp.rq.size));
	MLX5_SET(qpc, qpc, rq_type, MLX5_NON_ZERO_RQ);
	MLX5_SET(qpc, qpc, log_sq_size, ilog2(conn->qp.sq.size));
	MLX5_SET(qpc, qpc, cqn_snd, conn->cq.mcq.cqn);
	MLX5_SET(qpc, qpc, cqn_rcv, conn->cq.mcq.cqn);
	MLX5_SET64(qpc, qpc, dbr_addr, conn->qp.wq_ctrl.db.dma);
	if (MLX5_CAP_GEN(mdev, cqe_version) == 1)
		MLX5_SET(qpc, qpc, user_index, 0xFFFFFF);

	mlx5_fill_page_frag_array(&conn->qp.wq_ctrl.buf,
				  (__be64 *)MLX5_ADDR_OF(create_qp_in, in, pas));

	err = mlx5_core_create_qp(mdev, &conn->qp.mqp, in, inlen);
	if (err)
		goto err_sq_bufs;

	conn->qp.mqp.event = mlx5_fpga_conn_event;
	mlx5_fpga_dbg(fdev, "Created QP #0x%x\n", conn->qp.mqp.qpn);

	goto out;

err_sq_bufs:
	kvfree(conn->qp.sq.bufs);
err_rq_bufs:
	kvfree(conn->qp.rq.bufs);
err_wq:
	mlx5_wq_destroy(&conn->qp.wq_ctrl);
out:
	kvfree(in);
	return err;
}