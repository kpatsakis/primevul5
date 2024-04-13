static int tipc_aead_encrypt(struct tipc_aead *aead, struct sk_buff *skb,
			     struct tipc_bearer *b,
			     struct tipc_media_addr *dst,
			     struct tipc_node *__dnode)
{
	struct crypto_aead *tfm = tipc_aead_tfm_next(aead);
	struct tipc_crypto_tx_ctx *tx_ctx;
	struct aead_request *req;
	struct sk_buff *trailer;
	struct scatterlist *sg;
	struct tipc_ehdr *ehdr;
	int ehsz, len, tailen, nsg, rc;
	void *ctx;
	u32 salt;
	u8 *iv;

	/* Make sure message len at least 4-byte aligned */
	len = ALIGN(skb->len, 4);
	tailen = len - skb->len + aead->authsize;

	/* Expand skb tail for authentication tag:
	 * As for simplicity, we'd have made sure skb having enough tailroom
	 * for authentication tag @skb allocation. Even when skb is nonlinear
	 * but there is no frag_list, it should be still fine!
	 * Otherwise, we must cow it to be a writable buffer with the tailroom.
	 */
	SKB_LINEAR_ASSERT(skb);
	if (tailen > skb_tailroom(skb)) {
		pr_debug("TX(): skb tailroom is not enough: %d, requires: %d\n",
			 skb_tailroom(skb), tailen);
	}

	if (unlikely(!skb_cloned(skb) && tailen <= skb_tailroom(skb))) {
		nsg = 1;
		trailer = skb;
	} else {
		/* TODO: We could avoid skb_cow_data() if skb has no frag_list
		 * e.g. by skb_fill_page_desc() to add another page to the skb
		 * with the wanted tailen... However, page skbs look not often,
		 * so take it easy now!
		 * Cloned skbs e.g. from link_xmit() seems no choice though :(
		 */
		nsg = skb_cow_data(skb, tailen, &trailer);
		if (unlikely(nsg < 0)) {
			pr_err("TX: skb_cow_data() returned %d\n", nsg);
			return nsg;
		}
	}

	pskb_put(skb, trailer, tailen);

	/* Allocate memory for the AEAD operation */
	ctx = tipc_aead_mem_alloc(tfm, sizeof(*tx_ctx), &iv, &req, &sg, nsg);
	if (unlikely(!ctx))
		return -ENOMEM;
	TIPC_SKB_CB(skb)->crypto_ctx = ctx;

	/* Map skb to the sg lists */
	sg_init_table(sg, nsg);
	rc = skb_to_sgvec(skb, sg, 0, skb->len);
	if (unlikely(rc < 0)) {
		pr_err("TX: skb_to_sgvec() returned %d, nsg %d!\n", rc, nsg);
		goto exit;
	}

	/* Prepare IV: [SALT (4 octets)][SEQNO (8 octets)]
	 * In case we're in cluster-key mode, SALT is varied by xor-ing with
	 * the source address (or w0 of id), otherwise with the dest address
	 * if dest is known.
	 */
	ehdr = (struct tipc_ehdr *)skb->data;
	salt = aead->salt;
	if (aead->mode == CLUSTER_KEY)
		salt ^= __be32_to_cpu(ehdr->addr);
	else if (__dnode)
		salt ^= tipc_node_get_addr(__dnode);
	memcpy(iv, &salt, 4);
	memcpy(iv + 4, (u8 *)&ehdr->seqno, 8);

	/* Prepare request */
	ehsz = tipc_ehdr_size(ehdr);
	aead_request_set_tfm(req, tfm);
	aead_request_set_ad(req, ehsz);
	aead_request_set_crypt(req, sg, sg, len - ehsz, iv);

	/* Set callback function & data */
	aead_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
				  tipc_aead_encrypt_done, skb);
	tx_ctx = (struct tipc_crypto_tx_ctx *)ctx;
	tx_ctx->aead = aead;
	tx_ctx->bearer = b;
	memcpy(&tx_ctx->dst, dst, sizeof(*dst));

	/* Hold bearer */
	if (unlikely(!tipc_bearer_hold(b))) {
		rc = -ENODEV;
		goto exit;
	}

	/* Now, do encrypt */
	rc = crypto_aead_encrypt(req);
	if (rc == -EINPROGRESS || rc == -EBUSY)
		return rc;

	tipc_bearer_put(b);

exit:
	kfree(ctx);
	TIPC_SKB_CB(skb)->crypto_ctx = NULL;
	return rc;
}