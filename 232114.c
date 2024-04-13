 */
void __bfq_weights_tree_remove(struct bfq_data *bfqd,
			       struct bfq_queue *bfqq,
			       struct rb_root_cached *root)
{
	if (!bfqq->weight_counter)
		return;

	bfqq->weight_counter->num_active--;
	if (bfqq->weight_counter->num_active > 0)
		goto reset_entity_pointer;

	rb_erase_cached(&bfqq->weight_counter->weights_node, root);
	kfree(bfqq->weight_counter);

reset_entity_pointer:
	bfqq->weight_counter = NULL;
	bfq_put_queue(bfqq);