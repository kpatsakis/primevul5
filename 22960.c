static void tipc_crypto_do_cmd(struct net *net, int cmd)
{
	struct tipc_net *tn = tipc_net(net);
	struct tipc_crypto *tx = tn->crypto_tx, *rx;
	struct list_head *p;
	unsigned int stat;
	int i, j, cpu;
	char buf[200];

	/* Currently only one command is supported */
	switch (cmd) {
	case 0xfff1:
		goto print_stats;
	default:
		return;
	}

print_stats:
	/* Print a header */
	pr_info("\n=============== TIPC Crypto Statistics ===============\n\n");

	/* Print key status */
	pr_info("Key status:\n");
	pr_info("TX(%7.7s)\n%s", tipc_own_id_string(net),
		tipc_crypto_key_dump(tx, buf));

	rcu_read_lock();
	for (p = tn->node_list.next; p != &tn->node_list; p = p->next) {
		rx = tipc_node_crypto_rx_by_list(p);
		pr_info("RX(%7.7s)\n%s", tipc_node_get_id_str(rx->node),
			tipc_crypto_key_dump(rx, buf));
	}
	rcu_read_unlock();

	/* Print crypto statistics */
	for (i = 0, j = 0; i < MAX_STATS; i++)
		j += scnprintf(buf + j, 200 - j, "|%11s ", hstats[i]);
	pr_info("Counter     %s", buf);

	memset(buf, '-', 115);
	buf[115] = '\0';
	pr_info("%s\n", buf);

	j = scnprintf(buf, 200, "TX(%7.7s) ", tipc_own_id_string(net));
	for_each_possible_cpu(cpu) {
		for (i = 0; i < MAX_STATS; i++) {
			stat = per_cpu_ptr(tx->stats, cpu)->stat[i];
			j += scnprintf(buf + j, 200 - j, "|%11d ", stat);
		}
		pr_info("%s", buf);
		j = scnprintf(buf, 200, "%12s", " ");
	}

	rcu_read_lock();
	for (p = tn->node_list.next; p != &tn->node_list; p = p->next) {
		rx = tipc_node_crypto_rx_by_list(p);
		j = scnprintf(buf, 200, "RX(%7.7s) ",
			      tipc_node_get_id_str(rx->node));
		for_each_possible_cpu(cpu) {
			for (i = 0; i < MAX_STATS; i++) {
				stat = per_cpu_ptr(rx->stats, cpu)->stat[i];
				j += scnprintf(buf + j, 200 - j, "|%11d ",
					       stat);
			}
			pr_info("%s", buf);
			j = scnprintf(buf, 200, "%12s", " ");
		}
	}
	rcu_read_unlock();

	pr_info("\n======================== Done ========================\n");
}