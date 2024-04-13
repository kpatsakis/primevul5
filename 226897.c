static void sctp_for_each_tx_datachunk(struct sctp_association *asoc,
				       bool clear,
				       void (*cb)(struct sctp_chunk *))

{
	struct sctp_datamsg *msg, *prev_msg = NULL;
	struct sctp_outq *q = &asoc->outqueue;
	struct sctp_chunk *chunk, *c;
	struct sctp_transport *t;

	list_for_each_entry(t, &asoc->peer.transport_addr_list, transports)
		list_for_each_entry(chunk, &t->transmitted, transmitted_list)
			traverse_and_process();

	list_for_each_entry(chunk, &q->retransmit, transmitted_list)
		traverse_and_process();

	list_for_each_entry(chunk, &q->sacked, transmitted_list)
		traverse_and_process();

	list_for_each_entry(chunk, &q->abandoned, transmitted_list)
		traverse_and_process();

	list_for_each_entry(chunk, &q->out_chunk_list, list)
		traverse_and_process();
}