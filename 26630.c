void read_boot(DOS_FS * fs)
{
     struct boot_sector b;
     unsigned total_sectors;
     unsigned short logical_sector_size, sectors;
    unsigned fat_length;
     unsigned total_fat_entries;
     off_t data_size;
 
    fs_read(0, sizeof(b), &b);
    logical_sector_size = GET_UNALIGNED_W(b.sector_size);
    if (!logical_sector_size)
	die("Logical sector size is zero.");

    /* This was moved up because it's the first thing that will fail */
    /* if the platform needs special handling of unaligned multibyte accesses */
    /* but such handling isn't being provided. See GET_UNALIGNED_W() above. */
    if (logical_sector_size & (SECTOR_SIZE - 1))
	die("Logical sector size (%d bytes) is not a multiple of the physical "
	    "sector size.", logical_sector_size);

    fs->cluster_size = b.cluster_size * logical_sector_size;
    if (!fs->cluster_size)
	die("Cluster size is zero.");
    if (b.fats != 2 && b.fats != 1)
	die("Currently, only 1 or 2 FATs are supported, not %d.\n", b.fats);
    fs->nfats = b.fats;
    sectors = GET_UNALIGNED_W(b.sectors);
    total_sectors = sectors ? sectors : le32toh(b.total_sect);
    if (verbose)
	printf("Checking we can access the last sector of the filesystem\n");
     /* Can't access last odd sector anyway, so round down */
     fs_test((off_t)((total_sectors & ~1) - 1) * logical_sector_size,
 	    logical_sector_size);
     fat_length = le16toh(b.fat_length) ?
 	le16toh(b.fat_length) : le32toh(b.fat32_length);
     fs->fat_start = (off_t)le16toh(b.reserved) * logical_sector_size;
     fs->root_start = ((off_t)le16toh(b.reserved) + b.fats * fat_length) *
 	logical_sector_size;
     fs->root_entries = GET_UNALIGNED_W(b.dir_entries);
     fs->data_start = fs->root_start + ROUND_TO_MULTIPLE(fs->root_entries <<
 							MSDOS_DIR_BITS,
 							logical_sector_size);
     data_size = (off_t)total_sectors * logical_sector_size - fs->data_start;
     fs->data_clusters = data_size / fs->cluster_size;
     fs->root_cluster = 0;	/* indicates standard, pre-FAT32 root dir */
     fs->fsinfo_start = 0;	/* no FSINFO structure */
    fs->free_clusters = -1;	/* unknown */
    if (!b.fat_length && b.fat32_length) {
	fs->fat_bits = 32;
	fs->root_cluster = le32toh(b.root_cluster);
	if (!fs->root_cluster && fs->root_entries)
	    /* M$ hasn't specified this, but it looks reasonable: If
	     * root_cluster is 0 but there is a separate root dir
	     * (root_entries != 0), we handle the root dir the old way. Give a
	     * warning, but convertig to a root dir in a cluster chain seems
	     * to complex for now... */
	    printf("Warning: FAT32 root dir not in cluster chain! "
		   "Compatibility mode...\n");
	else if (!fs->root_cluster && !fs->root_entries)
	    die("No root directory!");
	else if (fs->root_cluster && fs->root_entries)
	    printf("Warning: FAT32 root dir is in a cluster chain, but "
		   "a separate root dir\n"
		   "  area is defined. Cannot fix this easily.\n");
	if (fs->data_clusters < FAT16_THRESHOLD)
	    printf("Warning: Filesystem is FAT32 according to fat_length "
		   "and fat32_length fields,\n"
		   "  but has only %lu clusters, less than the required "
		   "minimum of %d.\n"
		   "  This may lead to problems on some systems.\n",
		   (unsigned long)fs->data_clusters, FAT16_THRESHOLD);

	check_fat_state_bit(fs, &b);
	fs->backupboot_start = le16toh(b.backup_boot) * logical_sector_size;
	check_backup_boot(fs, &b, logical_sector_size);

	read_fsinfo(fs, &b, logical_sector_size);
    } else if (!atari_format) {
	/* On real MS-DOS, a 16 bit FAT is used whenever there would be too
	 * much clusers otherwise. */
	fs->fat_bits = (fs->data_clusters >= FAT12_THRESHOLD) ? 16 : 12;
	if (fs->data_clusters >= FAT16_THRESHOLD)
	    die("Too many clusters (%lu) for FAT16 filesystem.", fs->data_clusters);
	check_fat_state_bit(fs, &b);
    } else {
	/* On Atari, things are more difficult: GEMDOS always uses 12bit FATs
	 * on floppies, and always 16 bit on harddisks. */
	fs->fat_bits = 16;	/* assume 16 bit FAT for now */
	/* If more clusters than fat entries in 16-bit fat, we assume
	 * it's a real MSDOS FS with 12-bit fat. */
	if (fs->data_clusters + 2 > fat_length * logical_sector_size * 8 / 16 ||
	    /* if it has one of the usual floppy sizes -> 12bit FAT  */
	    (total_sectors == 720 || total_sectors == 1440 ||
	     total_sectors == 2880))
	    fs->fat_bits = 12;
    }
    /* On FAT32, the high 4 bits of a FAT entry are reserved */
    fs->eff_fat_bits = (fs->fat_bits == 32) ? 28 : fs->fat_bits;
    fs->fat_size = fat_length * logical_sector_size;

    fs->label = calloc(12, sizeof(uint8_t));
    if (fs->fat_bits == 12 || fs->fat_bits == 16) {
	struct boot_sector_16 *b16 = (struct boot_sector_16 *)&b;
	if (b16->extended_sig == 0x29)
	    memmove(fs->label, b16->label, 11);
	else
	    fs->label = NULL;
    } else if (fs->fat_bits == 32) {
	if (b.extended_sig == 0x29)
	    memmove(fs->label, &b.label, 11);
	else
	    fs->label = NULL;
    }

    total_fat_entries = (uint64_t)fs->fat_size * 8 / fs->fat_bits;
    if (fs->data_clusters > total_fat_entries - 2)
	die("Filesystem has %u clusters but only space for %u FAT entries.",
	    fs->data_clusters, total_fat_entries - 2);
    if (!fs->root_entries && !fs->root_cluster)
	die("Root directory has zero size.");
    if (fs->root_entries & (MSDOS_DPS - 1))
	die("Root directory (%d entries) doesn't span an integral number of "
	    "sectors.", fs->root_entries);
    if (logical_sector_size & (SECTOR_SIZE - 1))
	die("Logical sector size (%d bytes) is not a multiple of the physical "
	    "sector size.", logical_sector_size);
#if 0				/* linux kernel doesn't check that either */
    /* ++roman: On Atari, these two fields are often left uninitialized */
    if (!atari_format && (!b.secs_track || !b.heads))
	die("Invalid disk format in boot sector.");
#endif
    if (verbose)
	dump_boot(fs, &b, logical_sector_size);
}
