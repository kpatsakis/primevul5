static void php_enchant_broker_free(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	if (rsrc->ptr) {
		enchant_broker *broker = (enchant_broker *)rsrc->ptr;
		if (broker) {
			if (broker->pbroker) {
				if (broker->dictcnt && broker->dict) {
					if (broker->dict) {
						int total;
						total = broker->dictcnt-1;
						do {
							zend_list_delete(broker->dict[total]->rsrc_id);
							efree(broker->dict[total]);
							total--;
						} while (total>=0);
					}
					efree(broker->dict);
					broker->dict = NULL;
				}
				enchant_broker_free(broker->pbroker);
			}
			efree(broker);
		}
	}
}