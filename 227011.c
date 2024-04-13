is_valid_virt_queue_idx(uint32_t idx, int is_tx, uint32_t nr_vring)
{
	return (is_tx ^ (idx & 1)) == 0 && idx < nr_vring;
}