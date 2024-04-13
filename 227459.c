static UINT parallel_process_irp_close(PARALLEL_DEVICE* parallel, IRP* irp)
{
	if (close(parallel->file) < 0)
	{
	}
	else
	{
	}

	Stream_Zero(irp->output, 5); /* Padding(5) */
	return irp->Complete(irp);
}