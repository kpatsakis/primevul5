static int receive_data(int f_in, char *fname_r, int fd_r, OFF_T size_r,
			const char *fname, int fd, OFF_T total_size)
{
	static char file_sum1[MAX_DIGEST_LEN];
	struct map_struct *mapbuf;
	struct sum_struct sum;
	int sum_len;
	int32 len;
	OFF_T offset = 0;
	OFF_T offset2;
	char *data;
	int32 i;
	char *map = NULL;

#ifdef SUPPORT_PREALLOCATION
	if (preallocate_files && fd != -1 && total_size > 0 && (!inplace || total_size > size_r)) {
		/* Try to preallocate enough space for file's eventual length.  Can
		 * reduce fragmentation on filesystems like ext4, xfs, and NTFS. */
		if ((preallocated_len = do_fallocate(fd, 0, total_size)) < 0)
			rsyserr(FWARNING, errno, "do_fallocate %s", full_fname(fname));
	} else
#endif
	if (inplace) {
#ifdef HAVE_FTRUNCATE
		/* The most compatible way to create a sparse file is to start with no length. */
		if (sparse_files > 0 && whole_file && fd >= 0 && do_ftruncate(fd, 0) == 0)
			preallocated_len = 0;
		else
#endif
			preallocated_len = size_r;
	} else
		preallocated_len = 0;

	read_sum_head(f_in, &sum);

	if (fd_r >= 0 && size_r > 0) {
		int32 read_size = MAX(sum.blength * 2, 16*1024);
		mapbuf = map_file(fd_r, size_r, read_size, sum.blength);
		if (DEBUG_GTE(DELTASUM, 2)) {
			rprintf(FINFO, "recv mapped %s of size %s\n",
				fname_r, big_num(size_r));
		}
	} else
		mapbuf = NULL;

	sum_init(xfersum_type, checksum_seed);

	if (append_mode > 0) {
		OFF_T j;
		sum.flength = (OFF_T)sum.count * sum.blength;
		if (sum.remainder)
			sum.flength -= sum.blength - sum.remainder;
		if (append_mode == 2 && mapbuf) {
			for (j = CHUNK_SIZE; j < sum.flength; j += CHUNK_SIZE) {
				if (INFO_GTE(PROGRESS, 1))
					show_progress(offset, total_size);
				sum_update(map_ptr(mapbuf, offset, CHUNK_SIZE),
					   CHUNK_SIZE);
				offset = j;
			}
			if (offset < sum.flength) {
				int32 len = (int32)(sum.flength - offset);
				if (INFO_GTE(PROGRESS, 1))
					show_progress(offset, total_size);
				sum_update(map_ptr(mapbuf, offset, len), len);
			}
		}
		offset = sum.flength;
		if (fd != -1 && (j = do_lseek(fd, offset, SEEK_SET)) != offset) {
			rsyserr(FERROR_XFER, errno, "lseek of %s returned %s, not %s",
				full_fname(fname), big_num(j), big_num(offset));
			exit_cleanup(RERR_FILEIO);
		}
	}

	while ((i = recv_token(f_in, &data)) != 0) {
		if (INFO_GTE(PROGRESS, 1))
			show_progress(offset, total_size);

		if (allowed_lull)
			maybe_send_keepalive(time(NULL), MSK_ALLOW_FLUSH | MSK_ACTIVE_RECEIVER);

		if (i > 0) {
			if (DEBUG_GTE(DELTASUM, 3)) {
				rprintf(FINFO,"data recv %d at %s\n",
					i, big_num(offset));
			}

			stats.literal_data += i;
			cleanup_got_literal = 1;

			sum_update(data, i);

			if (fd != -1 && write_file(fd, 0, offset, data, i) != i)
				goto report_write_error;
			offset += i;
			continue;
		}

		i = -(i+1);
		offset2 = i * (OFF_T)sum.blength;
		len = sum.blength;
		if (i == (int)sum.count-1 && sum.remainder != 0)
			len = sum.remainder;

		stats.matched_data += len;

		if (DEBUG_GTE(DELTASUM, 3)) {
			rprintf(FINFO,
				"chunk[%d] of size %ld at %s offset=%s%s\n",
				i, (long)len, big_num(offset2), big_num(offset),
				updating_basis_or_equiv && offset == offset2 ? " (seek)" : "");
		}

		if (mapbuf) {
			map = map_ptr(mapbuf,offset2,len);

			see_token(map, len);
			sum_update(map, len);
		}

		if (updating_basis_or_equiv) {
			if (offset == offset2 && fd != -1) {
				if (skip_matched(fd, offset, map, len) < 0)
					goto report_write_error;
				offset += len;
				continue;
			}
		}
		if (fd != -1 && map && write_file(fd, 0, offset, map, len) != (int)len)
			goto report_write_error;
		offset += len;
	}

	if (fd != -1 && offset > 0) {
		if (sparse_files > 0) {
			if (sparse_end(fd, offset) != 0)
				goto report_write_error;
		} else if (flush_write_file(fd) < 0) {
		    report_write_error:
			rsyserr(FERROR_XFER, errno, "write failed on %s", full_fname(fname));
			exit_cleanup(RERR_FILEIO);
		}
	}

#ifdef HAVE_FTRUNCATE
	/* inplace: New data could be shorter than old data.
	 * preallocate_files: total_size could have been an overestimate.
	 *     Cut off any extra preallocated zeros from dest file. */
	if ((inplace || preallocated_len > offset) && fd != -1 && do_ftruncate(fd, offset) < 0) {
		rsyserr(FERROR_XFER, errno, "ftruncate failed on %s",
			full_fname(fname));
	}
#endif

	if (INFO_GTE(PROGRESS, 1))
		end_progress(total_size);

	sum_len = sum_end(file_sum1);

	if (mapbuf)
		unmap_file(mapbuf);

	read_buf(f_in, sender_file_sum, sum_len);
	if (DEBUG_GTE(DELTASUM, 2))
		rprintf(FINFO,"got file_sum\n");
	if (fd != -1 && memcmp(file_sum1, sender_file_sum, sum_len) != 0)
		return 0;
	return 1;
}