static void fuse_kill_sb_blk(struct super_block *sb)
{
	fuse_sb_destroy(sb);
	kill_block_super(sb);
}