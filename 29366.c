static int nfs_probe(struct device_d *dev)
{
	struct fs_device_d *fsdev = dev_to_fs_device(dev);
	struct nfs_priv *npriv = xzalloc(sizeof(struct nfs_priv));
	struct super_block *sb = &fsdev->sb;
	char *tmp = xstrdup(fsdev->backingstore);
	char *path;
	struct inode *inode;
	int ret;

	dev->priv = npriv;

	debug("nfs: mount: %s\n", fsdev->backingstore);

	path = strchr(tmp, ':');
	if (!path) {
		ret = -EINVAL;
		goto err;
	}

	*path = 0;

	npriv->path = xstrdup(path + 1);

	ret = resolv(tmp, &npriv->server);
	if (ret) {
		printf("cannot resolve \"%s\": %s\n", tmp, strerror(-ret));
		goto err1;
	}

	debug("nfs: server: %s path: %s\n", tmp, npriv->path);

	npriv->con = net_udp_new(npriv->server, SUNRPC_PORT, nfs_handler, npriv);
	if (IS_ERR(npriv->con)) {
		ret = PTR_ERR(npriv->con);
		goto err1;
	}

	/* Need a priviliged source port */
	net_udp_bind(npriv->con, 1000);

	parseopt_hu(fsdev->options, "mountport", &npriv->mount_port);
	if (!npriv->mount_port) {
		ret = rpc_lookup_req(npriv, PROG_MOUNT, 3);
		if (ret < 0) {
			printf("lookup mount port failed with %d\n", ret);
			goto err2;
		}
		npriv->mount_port = ret;
	} else {
		npriv->manual_mount_port = 1;
	}
	debug("mount port: %hu\n", npriv->mount_port);

	parseopt_hu(fsdev->options, "port", &npriv->nfs_port);
	if (!npriv->nfs_port) {
		ret = rpc_lookup_req(npriv, PROG_NFS, 3);
		if (ret < 0) {
			printf("lookup nfs port failed with %d\n", ret);
			goto err2;
		}
		npriv->nfs_port = ret;
	} else {
		npriv->manual_nfs_port = 1;
	}
	debug("nfs port: %d\n", npriv->nfs_port);

	ret = nfs_mount_req(npriv);
	if (ret) {
		printf("mounting failed with %d\n", ret);
		goto err2;
	}

	nfs_set_rootarg(npriv, fsdev);

	free(tmp);

	sb->s_op = &nfs_ops;

	inode = new_inode(sb);
	nfs_set_fh(inode, &npriv->rootfh);
	nfs_init_inode(npriv, inode, S_IFDIR);
	sb->s_root = d_make_root(inode);

	return 0;

err2:
	net_unregister(npriv->con);
err1:
	free(npriv->path);
err:
	free(tmp);
	free(npriv);

	return ret;
}
