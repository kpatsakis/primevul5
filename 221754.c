static void fuse_kill_sb_anon(struct super_block *sb)
{
	fuse_sb_destroy(sb);
	kill_anon_super(sb);
}