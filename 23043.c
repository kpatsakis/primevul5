static int mlx5_fpga_conn_map_buf(struct mlx5_fpga_conn *conn,
				  struct mlx5_fpga_dma_buf *buf)
{
	struct device *dma_device;
	int err = 0;

	if (unlikely(!buf->sg[0].data))
		goto out;

	dma_device = &conn->fdev->mdev->pdev->dev;
	buf->sg[0].dma_addr = dma_map_single(dma_device, buf->sg[0].data,
					     buf->sg[0].size, buf->dma_dir);
	err = dma_mapping_error(dma_device, buf->sg[0].dma_addr);
	if (unlikely(err)) {
		mlx5_fpga_warn(conn->fdev, "DMA error on sg 0: %d\n", err);
		err = -ENOMEM;
		goto out;
	}

	if (!buf->sg[1].data)
		goto out;

	buf->sg[1].dma_addr = dma_map_single(dma_device, buf->sg[1].data,
					     buf->sg[1].size, buf->dma_dir);
	err = dma_mapping_error(dma_device, buf->sg[1].dma_addr);
	if (unlikely(err)) {
		mlx5_fpga_warn(conn->fdev, "DMA error on sg 1: %d\n", err);
		dma_unmap_single(dma_device, buf->sg[0].dma_addr,
				 buf->sg[0].size, buf->dma_dir);
		err = -ENOMEM;
	}

out:
	return err;
}