static int mlx5_fpga_conn_post_recv_buf(struct mlx5_fpga_conn *conn)
{
	struct mlx5_fpga_dma_buf *buf;
	int err;

	buf = kzalloc(sizeof(*buf) + MLX5_FPGA_RECV_SIZE, 0);
	if (!buf)
		return -ENOMEM;

	buf->sg[0].data = (void *)(buf + 1);
	buf->sg[0].size = MLX5_FPGA_RECV_SIZE;
	buf->dma_dir = DMA_FROM_DEVICE;

	err = mlx5_fpga_conn_post_recv(conn, buf);
	if (err)
		kfree(buf);

	return err;
}