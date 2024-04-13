static int tipc_aead_decrypt(struct net *net, struct tipc_aead *aead,
			     struct sk_buff *skb, struct tipc_bearer *b)
{
	struct tipc_crypto_rx_ctx *rx_ctx;
	struct aead_request *req;
	struct crypto_aead *tfm;
	struct sk_buff *unused;
	struct scatterlist *sg;
	struct tipc_ehdr *ehdr;
	int ehsz, nsg, rc;
	void *ctx;
	u32 salt;
	u8 *iv;

	if (unlikely(!aead))
		return -ENOKEY;

	nsg = skb_cow_data(skb, 0, &unused);
	if (unlikely(nsg < 0)) {
		pr_err("RX: skb_cow_data() returned %d\n", nsg);
		return nsg;
	}

	/* Allocate memory for the AEAD operation */
	tfm = tipc_aead_tfm_next(aead);
	ctx = tipc_aead_mem_alloc(tfm, sizeof(*rx_ctx), &iv, &req, &sg, nsg);
	if (unlikely(!ctx))
		return -ENOMEM;
	TIPC_SKB_CB(skb)->crypto_ctx = ctx;

	/* Map skb to the sg lists */
	sg_init_table(sg, nsg);
	rc = skb_to_sgvec(skb, sg, 0, skb->len);
	if (unlikely(rc < 0)) {
		pr_err("RX: skb_to_sgvec() returned %d, nsg %d\n", rc, nsg);
		goto exit;
	}

	/* Reconstruct IV: */
	ehdr = (struct tipc_ehdr *)skb->data;
	salt = aead->salt;
	if (aead->mode == CLUSTER_KEY)
		salt ^= __be32_to_cpu(ehdr->addr);
	else if (ehdr->destined)
		salt ^= tipc_own_addr(net);
	memcpy(iv, &salt, 4);
	memcpy(iv + 4, (u8 *)&ehdr->seqno, 8);

	/* Prepare request */
	ehsz = tipc_ehdr_size(ehdr);
	aead_request_set_tfm(req, tfm);
	aead_request_set_ad(req, ehsz);
	aead_request_set_crypt(req, sg, sg, skb->len - ehsz, iv);

	/* Set callback function & data */
	aead_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
				  tipc_aead_decrypt_done, skb);
	rx_ctx = (struct tipc_crypto_rx_ctx *)ctx;
	rx_ctx->aead = aead;
	rx_ctx->bearer = b;

	/* Hold bearer */
	if (unlikely(!tipc_bearer_hold(b))) {
		rc = -ENODEV;
		goto exit;
	}

	/* Now, do decrypt */
	rc = crypto_aead_decrypt(req);
	if (rc == -EINPROGRESS || rc == -EBUSY)
		return rc;

	tipc_bearer_put(b);

exit:
	kfree(ctx);
	TIPC_SKB_CB(skb)->crypto_ctx = NULL;
	return rc;
}