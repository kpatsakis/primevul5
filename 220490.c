static int __nf_tables_abort(struct net *net, enum nfnl_abort_action action)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	struct nft_trans *trans, *next;
	struct nft_trans_elem *te;
	struct nft_hook *hook;

	if (action == NFNL_ABORT_VALIDATE &&
	    nf_tables_validate(net) < 0)
		return -EAGAIN;

	list_for_each_entry_safe_reverse(trans, next, &nft_net->commit_list,
					 list) {
		switch (trans->msg_type) {
		case NFT_MSG_NEWTABLE:
			if (nft_trans_table_update(trans)) {
				if (!(trans->ctx.table->flags & __NFT_TABLE_F_UPDATE)) {
					nft_trans_destroy(trans);
					break;
				}
				if (trans->ctx.table->flags & __NFT_TABLE_F_WAS_DORMANT) {
					nf_tables_table_disable(net, trans->ctx.table);
					trans->ctx.table->flags |= NFT_TABLE_F_DORMANT;
				} else if (trans->ctx.table->flags & __NFT_TABLE_F_WAS_AWAKEN) {
					trans->ctx.table->flags &= ~NFT_TABLE_F_DORMANT;
				}
				trans->ctx.table->flags &= ~__NFT_TABLE_F_UPDATE;
				nft_trans_destroy(trans);
			} else {
				list_del_rcu(&trans->ctx.table->list);
			}
			break;
		case NFT_MSG_DELTABLE:
			nft_clear(trans->ctx.net, trans->ctx.table);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWCHAIN:
			if (nft_trans_chain_update(trans)) {
				free_percpu(nft_trans_chain_stats(trans));
				kfree(nft_trans_chain_name(trans));
				nft_trans_destroy(trans);
			} else {
				if (nft_chain_is_bound(trans->ctx.chain)) {
					nft_trans_destroy(trans);
					break;
				}
				trans->ctx.table->use--;
				nft_chain_del(trans->ctx.chain);
				nf_tables_unregister_hook(trans->ctx.net,
							  trans->ctx.table,
							  trans->ctx.chain);
			}
			break;
		case NFT_MSG_DELCHAIN:
			trans->ctx.table->use++;
			nft_clear(trans->ctx.net, trans->ctx.chain);
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWRULE:
			trans->ctx.chain->use--;
			list_del_rcu(&nft_trans_rule(trans)->list);
			nft_rule_expr_deactivate(&trans->ctx,
						 nft_trans_rule(trans),
						 NFT_TRANS_ABORT);
			break;
		case NFT_MSG_DELRULE:
			trans->ctx.chain->use++;
			nft_clear(trans->ctx.net, nft_trans_rule(trans));
			nft_rule_expr_activate(&trans->ctx, nft_trans_rule(trans));
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWSET:
			trans->ctx.table->use--;
			if (nft_trans_set_bound(trans)) {
				nft_trans_destroy(trans);
				break;
			}
			list_del_rcu(&nft_trans_set(trans)->list);
			break;
		case NFT_MSG_DELSET:
			trans->ctx.table->use++;
			nft_clear(trans->ctx.net, nft_trans_set(trans));
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWSETELEM:
			if (nft_trans_elem_set_bound(trans)) {
				nft_trans_destroy(trans);
				break;
			}
			te = (struct nft_trans_elem *)trans->data;
			nft_setelem_remove(net, te->set, &te->elem);
			if (!nft_setelem_is_catchall(te->set, &te->elem))
				atomic_dec(&te->set->nelems);
			break;
		case NFT_MSG_DELSETELEM:
			te = (struct nft_trans_elem *)trans->data;

			nft_setelem_data_activate(net, te->set, &te->elem);
			nft_setelem_activate(net, te->set, &te->elem);
			if (!nft_setelem_is_catchall(te->set, &te->elem))
				te->set->ndeact--;

			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWOBJ:
			if (nft_trans_obj_update(trans)) {
				kfree(nft_trans_obj_newobj(trans));
				nft_trans_destroy(trans);
			} else {
				trans->ctx.table->use--;
				nft_obj_del(nft_trans_obj(trans));
			}
			break;
		case NFT_MSG_DELOBJ:
			trans->ctx.table->use++;
			nft_clear(trans->ctx.net, nft_trans_obj(trans));
			nft_trans_destroy(trans);
			break;
		case NFT_MSG_NEWFLOWTABLE:
			if (nft_trans_flowtable_update(trans)) {
				nft_unregister_flowtable_net_hooks(net,
						&nft_trans_flowtable_hooks(trans));
			} else {
				trans->ctx.table->use--;
				list_del_rcu(&nft_trans_flowtable(trans)->list);
				nft_unregister_flowtable_net_hooks(net,
						&nft_trans_flowtable(trans)->hook_list);
			}
			break;
		case NFT_MSG_DELFLOWTABLE:
			if (nft_trans_flowtable_update(trans)) {
				list_for_each_entry(hook, &nft_trans_flowtable(trans)->hook_list, list)
					hook->inactive = false;
			} else {
				trans->ctx.table->use++;
				nft_clear(trans->ctx.net, nft_trans_flowtable(trans));
			}
			nft_trans_destroy(trans);
			break;
		}
	}

	synchronize_rcu();

	list_for_each_entry_safe_reverse(trans, next,
					 &nft_net->commit_list, list) {
		list_del(&trans->list);
		nf_tables_abort_release(trans);
	}

	if (action == NFNL_ABORT_AUTOLOAD)
		nf_tables_module_autoload(net);
	else
		nf_tables_module_autoload_cleanup(net);

	return 0;
}