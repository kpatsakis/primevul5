static void extract_buf(struct entropy_store *r, __u8 *out)
{
	int i;
	__u32 hash[5], workspace[SHA_WORKSPACE_WORDS];
	__u8 extract[64];

	/* Generate a hash across the pool, 16 words (512 bits) at a time */
	sha_init(hash);
	for (i = 0; i < r->poolinfo->poolwords; i += 16)
		sha_transform(hash, (__u8 *)(r->pool + i), workspace);

	/*
	 * We mix the hash back into the pool to prevent backtracking
	 * attacks (where the attacker knows the state of the pool
	 * plus the current outputs, and attempts to find previous
	 * ouputs), unless the hash function can be inverted. By
	 * mixing at least a SHA1 worth of hash data back, we make
	 * brute-forcing the feedback as hard as brute-forcing the
	 * hash.
	 */
	mix_pool_bytes_extract(r, hash, sizeof(hash), extract);

	/*
	 * To avoid duplicates, we atomically extract a portion of the
	 * pool while mixing, and hash one final time.
	 */
	sha_transform(hash, extract, workspace);
	memset(extract, 0, sizeof(extract));
	memset(workspace, 0, sizeof(workspace));

	/*
	 * In case the hash function has some recognizable output
	 * pattern, we fold it in half. Thus, we always feed back
	 * twice as much data as we output.
	 */
	hash[0] ^= hash[3];
	hash[1] ^= hash[4];
	hash[2] ^= rol32(hash[2], 16);
	memcpy(out, hash, EXTRACT_SIZE);
	memset(hash, 0, sizeof(hash));
}