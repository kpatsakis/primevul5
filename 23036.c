static void mlx5_fpga_conn_arm_cq(struct mlx5_fpga_conn *conn)
{
	mlx5_cq_arm(&conn->cq.mcq, MLX5_CQ_DB_REQ_NOT,
		    conn->fdev->conn_res.uar->map, conn->cq.wq.cc);
}