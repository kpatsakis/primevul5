static inline void mlx5_fpga_conn_cqes(struct mlx5_fpga_conn *conn,
				       unsigned int budget)
{
	struct mlx5_cqe64 *cqe;

	while (budget) {
		cqe = mlx5_cqwq_get_cqe(&conn->cq.wq);
		if (!cqe)
			break;

		budget--;
		mlx5_cqwq_pop(&conn->cq.wq);
		mlx5_fpga_conn_handle_cqe(conn, cqe);
		mlx5_cqwq_update_db_record(&conn->cq.wq);
	}
	if (!budget) {
		tasklet_schedule(&conn->cq.tasklet);
		return;
	}

	mlx5_fpga_dbg(conn->fdev, "Re-arming CQ with cc# %u\n", conn->cq.wq.cc);
	/* ensure cq space is freed before enabling more cqes */
	wmb();
	mlx5_fpga_conn_arm_cq(conn);
}