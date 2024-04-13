static void mlx5_fpga_conn_handle_cqe(struct mlx5_fpga_conn *conn,
				      struct mlx5_cqe64 *cqe)
{
	u8 opcode, status = 0;

	opcode = get_cqe_opcode(cqe);

	switch (opcode) {
	case MLX5_CQE_REQ_ERR:
		status = ((struct mlx5_err_cqe *)cqe)->syndrome;
		/* Fall through */
	case MLX5_CQE_REQ:
		mlx5_fpga_conn_sq_cqe(conn, cqe, status);
		break;

	case MLX5_CQE_RESP_ERR:
		status = ((struct mlx5_err_cqe *)cqe)->syndrome;
		/* Fall through */
	case MLX5_CQE_RESP_SEND:
		mlx5_fpga_conn_rq_cqe(conn, cqe, status);
		break;
	default:
		mlx5_fpga_warn(conn->fdev, "Unexpected cqe opcode %u\n",
			       opcode);
	}
}