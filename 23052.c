int mlx5_fpga_conn_device_init(struct mlx5_fpga_device *fdev)
{
	int err;

	err = mlx5_nic_vport_enable_roce(fdev->mdev);
	if (err) {
		mlx5_fpga_err(fdev, "Failed to enable RoCE: %d\n", err);
		goto out;
	}

	fdev->conn_res.uar = mlx5_get_uars_page(fdev->mdev);
	if (IS_ERR(fdev->conn_res.uar)) {
		err = PTR_ERR(fdev->conn_res.uar);
		mlx5_fpga_err(fdev, "get_uars_page failed, %d\n", err);
		goto err_roce;
	}
	mlx5_fpga_dbg(fdev, "Allocated UAR index %u\n",
		      fdev->conn_res.uar->index);

	err = mlx5_core_alloc_pd(fdev->mdev, &fdev->conn_res.pdn);
	if (err) {
		mlx5_fpga_err(fdev, "alloc pd failed, %d\n", err);
		goto err_uar;
	}
	mlx5_fpga_dbg(fdev, "Allocated PD %u\n", fdev->conn_res.pdn);

	err = mlx5_fpga_conn_create_mkey(fdev->mdev, fdev->conn_res.pdn,
					 &fdev->conn_res.mkey);
	if (err) {
		mlx5_fpga_err(fdev, "create mkey failed, %d\n", err);
		goto err_dealloc_pd;
	}
	mlx5_fpga_dbg(fdev, "Created mkey 0x%x\n", fdev->conn_res.mkey.key);

	return 0;

err_dealloc_pd:
	mlx5_core_dealloc_pd(fdev->mdev, fdev->conn_res.pdn);
err_uar:
	mlx5_put_uars_page(fdev->mdev, fdev->conn_res.uar);
err_roce:
	mlx5_nic_vport_disable_roce(fdev->mdev);
out:
	return err;
}