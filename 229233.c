static void php_enchant_dict_free(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */

{
	if (rsrc->ptr) {
		enchant_dict *pdict = (enchant_dict *)rsrc->ptr;
		if (pdict) {
			if (pdict->pdict && pdict->pbroker) {
				enchant_broker_free_dict(pdict->pbroker->pbroker, pdict->pdict);
				if (pdict->id) {
					pdict->pbroker->dict[pdict->id-1]->next = NULL;
				}
				zend_list_delete(pdict->pbroker->rsrc_id);
			}

		}
	}
}