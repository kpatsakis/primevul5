static void mlx5_fpga_conn_free_recv_bufs(struct mlx5_fpga_conn *conn)
{
	int ix;

	for (ix = 0; ix < conn->qp.rq.size; ix++) {
		if (!conn->qp.rq.bufs[ix])
			continue;
		mlx5_fpga_conn_unmap_buf(conn, conn->qp.rq.bufs[ix]);
		kfree(conn->qp.rq.bufs[ix]);
		conn->qp.rq.bufs[ix] = NULL;
	}
}