 *  %NULL on failure
 */
struct device *
iscsi_find_flashnode_sess(struct Scsi_Host *shost, void *data,
			  int (*fn)(struct device *dev, void *data))