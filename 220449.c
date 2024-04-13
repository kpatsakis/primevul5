static void nf_tables_commit_chain_prepare_cancel(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_trans *trans, *next;

	list_for_each_entry_safe(trans, next, &nft_net->commit_list, list) {
		struct nft_chain *chain = trans->ctx.chain;

		if (trans->msg_type == NFT_MSG_NEWRULE ||
		    trans->msg_type == NFT_MSG_DELRULE) {
			kvfree(chain->rules_next);
			chain->rules_next = NULL;
		}
	}
}