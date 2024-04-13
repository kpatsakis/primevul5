static void mlx5_fpga_conn_unmap_buf(struct mlx5_fpga_conn *conn,
				     struct mlx5_fpga_dma_buf *buf)
{
	struct device *dma_device;

	dma_device = &conn->fdev->mdev->pdev->dev;
	if (buf->sg[1].data)
		dma_unmap_single(dma_device, buf->sg[1].dma_addr,
				 buf->sg[1].size, buf->dma_dir);

	if (likely(buf->sg[0].data))
		dma_unmap_single(dma_device, buf->sg[0].dma_addr,
				 buf->sg[0].size, buf->dma_dir);
}