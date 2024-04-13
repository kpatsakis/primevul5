static void nfs_set_rootarg(struct nfs_priv *npriv, struct fs_device_d *fsdev)
{
	char *str, *tmp;
	const char *bootargs;

	str = basprintf("root=/dev/nfs nfsroot=%pI4:%s%s%s", &npriv->server, npriv->path,
			  rootnfsopts[0] ? "," : "", rootnfsopts);

	/* forward specific mount options on demand */
	if (npriv->manual_nfs_port == 1) {
		tmp = basprintf("%s,port=%hu", str, npriv->nfs_port);
		free(str);
		str = tmp;
	}

	if (npriv->manual_mount_port == 1) {
		tmp = basprintf("%s,mountport=%hu", str, npriv->mount_port);
		free(str);
		str = tmp;
	}

	bootargs = dev_get_param(&npriv->con->edev->dev, "linux.bootargs");
	if (bootargs) {
		tmp = basprintf("%s %s", str, bootargs);
		free(str);
		str = tmp;
	}

	fsdev_set_linux_rootarg(fsdev, str);

	free(str);
}
