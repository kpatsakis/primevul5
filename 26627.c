int get_rock_ridge_filename(struct iso_directory_record *de,
			    char *retname, struct inode *inode)
{
	struct rock_state rs;
	struct rock_ridge *rr;
	int sig;
 	int retnamlen = 0;
 	int truncate = 0;
 	int ret = 0;
 
 	if (!ISOFS_SB(inode->i_sb)->s_rock)
 		return 0;
	*retname = 0;

	init_rock_state(&rs, inode);
	setup_rock_ridge(de, inode, &rs);
repeat:

	while (rs.len > 2) { /* There may be one byte for padding somewhere */
		rr = (struct rock_ridge *)rs.chr;
		/*
		 * Ignore rock ridge info if rr->len is out of range, but
		 * don't return -EIO because that would make the file
		 * invisible.
		 */
		if (rr->len < 3)
			goto out;	/* Something got screwed up here */
		sig = isonum_721(rs.chr);
		if (rock_check_overflow(&rs, sig))
			goto eio;
		rs.chr += rr->len;
		rs.len -= rr->len;
		/*
		 * As above, just ignore the rock ridge info if rr->len
		 * is bogus.
		 */
		if (rs.len < 0)
			goto out;	/* Something got screwed up here */

		switch (sig) {
		case SIG('R', 'R'):
			if ((rr->u.RR.flags[0] & RR_NM) == 0)
				goto out;
			break;
		case SIG('S', 'P'):
			if (check_sp(rr, inode))
				goto out;
			break;
		case SIG('C', 'E'):
			rs.cont_extent = isonum_733(rr->u.CE.extent);
			rs.cont_offset = isonum_733(rr->u.CE.offset);
			rs.cont_size = isonum_733(rr->u.CE.size);
			break;
		case SIG('N', 'M'):
			if (truncate)
				break;
			if (rr->len < 5)
				break;
			/*
			 * If the flags are 2 or 4, this indicates '.' or '..'.
			 * We don't want to do anything with this, because it
			 * screws up the code that calls us.  We don't really
			 * care anyways, since we can just use the non-RR
			 * name.
			 */
			if (rr->u.NM.flags & 6)
				break;

			if (rr->u.NM.flags & ~1) {
				printk("Unsupported NM flag settings (%d)\n",
 					rr->u.NM.flags);
 				break;
 			}
			if ((strlen(retname) + rr->len - 5) >= 254) {
 				truncate = 1;
 				break;
 			}
			strncat(retname, rr->u.NM.name, rr->len - 5);
			retnamlen += rr->len - 5;
 			break;
 		case SIG('R', 'E'):
 			kfree(rs.buffer);
			return -1;
		default:
			break;
		}
	}
	ret = rock_continue(&rs);
	if (ret == 0)
		goto repeat;
	if (ret == 1)
		return retnamlen; /* If 0, this file did not have a NM field */
out:
	kfree(rs.buffer);
	return ret;
eio:
	ret = -EIO;
	goto out;
}
