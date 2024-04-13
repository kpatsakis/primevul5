static int mlx5_fpga_conn_post_recv(struct mlx5_fpga_conn *conn,
				    struct mlx5_fpga_dma_buf *buf)
{
	struct mlx5_wqe_data_seg *data;
	unsigned int ix;
	int err = 0;

	err = mlx5_fpga_conn_map_buf(conn, buf);
	if (unlikely(err))
		goto out;

	if (unlikely(conn->qp.rq.pc - conn->qp.rq.cc >= conn->qp.rq.size)) {
		mlx5_fpga_conn_unmap_buf(conn, buf);
		return -EBUSY;
	}

	ix = conn->qp.rq.pc & (conn->qp.rq.size - 1);
	data = mlx5_wq_cyc_get_wqe(&conn->qp.wq.rq, ix);
	data->byte_count = cpu_to_be32(buf->sg[0].size);
	data->lkey = cpu_to_be32(conn->fdev->conn_res.mkey.key);
	data->addr = cpu_to_be64(buf->sg[0].dma_addr);

	conn->qp.rq.pc++;
	conn->qp.rq.bufs[ix] = buf;

	/* Make sure that descriptors are written before doorbell record. */
	dma_wmb();
	*conn->qp.wq.rq.db = cpu_to_be32(conn->qp.rq.pc & 0xffff);
out:
	return err;
}