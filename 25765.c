static bool virtio_net_set_vnet_endian(VirtIODevice *vdev, NetClientState *ncs,
                                       int queues, bool enable)
{
    int i;

    for (i = 0; i < queues; i++) {
        if (virtio_net_set_vnet_endian_one(vdev, ncs[i].peer, enable) < 0 &&
            enable) {
            while (--i >= 0) {
                virtio_net_set_vnet_endian_one(vdev, ncs[i].peer, false);
            }

            return true;
        }
    }

    return false;
}