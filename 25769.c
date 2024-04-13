static int peer_attach(VirtIONet *n, int index)
{
    NetClientState *nc = qemu_get_subqueue(n->nic, index);

    if (!nc->peer) {
        return 0;
    }

    if (nc->peer->info->type == NET_CLIENT_DRIVER_VHOST_USER) {
        vhost_set_vring_enable(nc->peer, 1);
    }

    if (nc->peer->info->type != NET_CLIENT_DRIVER_TAP) {
        return 0;
    }

    if (n->max_queues == 1) {
        return 0;
    }

    return tap_enable(nc->peer);
}