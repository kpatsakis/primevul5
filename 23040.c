void mlx5_fpga_conn_device_cleanup(struct mlx5_fpga_device *fdev)
{
	mlx5_core_destroy_mkey(fdev->mdev, &fdev->conn_res.mkey);
	mlx5_core_dealloc_pd(fdev->mdev, fdev->conn_res.pdn);
	mlx5_put_uars_page(fdev->mdev, fdev->conn_res.uar);
	mlx5_nic_vport_disable_roce(fdev->mdev);
}