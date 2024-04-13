static void mlx5_fpga_conn_destroy_qp(struct mlx5_fpga_conn *conn)
{
	mlx5_core_destroy_qp(conn->fdev->mdev, &conn->qp.mqp);
	mlx5_fpga_conn_free_recv_bufs(conn);
	mlx5_fpga_conn_flush_send_bufs(conn);
	kvfree(conn->qp.sq.bufs);
	kvfree(conn->qp.rq.bufs);
	mlx5_wq_destroy(&conn->qp.wq_ctrl);
}