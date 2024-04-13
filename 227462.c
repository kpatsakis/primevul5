static UINT parallel_process_irp_device_control(PARALLEL_DEVICE* parallel, IRP* irp)
{
	Stream_Write_UINT32(irp->output, 0); /* OutputBufferLength */
	return irp->Complete(irp);
}