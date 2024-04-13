static void virgl_resource_attach_backing(VirtIOGPU *g,
                                          struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_resource_attach_backing att_rb;
    struct iovec *res_iovs;
    int ret;

    VIRTIO_GPU_FILL_CMD(att_rb);
    trace_virtio_gpu_cmd_res_back_attach(att_rb.resource_id);

    ret = virtio_gpu_create_mapping_iov(&att_rb, cmd, NULL, &res_iovs);
    if (ret != 0) {
        cmd->error = VIRTIO_GPU_RESP_ERR_UNSPEC;
         return;
     }
 
    virgl_renderer_resource_attach_iov(att_rb.resource_id,
                                       res_iovs, att_rb.nr_entries);
 }
