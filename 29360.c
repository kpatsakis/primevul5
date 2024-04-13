static int nfs_init(void)
{
	rootnfsopts = xstrdup("v3,tcp");

	globalvar_add_simple_string("linux.rootnfsopts", &rootnfsopts);

	return register_fs_driver(&nfs_driver);
}
