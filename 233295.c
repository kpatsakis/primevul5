static int xdr_xcode_array2(const struct xdr_buf *buf, unsigned int base,
			    struct xdr_array2_desc *desc, int encode)
{
	char *elem = NULL, *c;
	unsigned int copied = 0, todo, avail_here;
	struct page **ppages = NULL;
	int err;

	if (encode) {
		if (xdr_encode_word(buf, base, desc->array_len) != 0)
			return -EINVAL;
	} else {
		if (xdr_decode_word(buf, base, &desc->array_len) != 0 ||
		    desc->array_len > desc->array_maxlen ||
		    (unsigned long) base + 4 + desc->array_len *
				    desc->elem_size > buf->len)
			return -EINVAL;
	}
	base += 4;

	if (!desc->xcode)
		return 0;

	todo = desc->array_len * desc->elem_size;

	/* process head */
	if (todo && base < buf->head->iov_len) {
		c = buf->head->iov_base + base;
		avail_here = min_t(unsigned int, todo,
				   buf->head->iov_len - base);
		todo -= avail_here;

		while (avail_here >= desc->elem_size) {
			err = desc->xcode(desc, c);
			if (err)
				goto out;
			c += desc->elem_size;
			avail_here -= desc->elem_size;
		}
		if (avail_here) {
			if (!elem) {
				elem = kmalloc(desc->elem_size, GFP_KERNEL);
				err = -ENOMEM;
				if (!elem)
					goto out;
			}
			if (encode) {
				err = desc->xcode(desc, elem);
				if (err)
					goto out;
				memcpy(c, elem, avail_here);
			} else
				memcpy(elem, c, avail_here);
			copied = avail_here;
		}
		base = buf->head->iov_len;  /* align to start of pages */
	}

	/* process pages array */
	base -= buf->head->iov_len;
	if (todo && base < buf->page_len) {
		unsigned int avail_page;

		avail_here = min(todo, buf->page_len - base);
		todo -= avail_here;

		base += buf->page_base;
		ppages = buf->pages + (base >> PAGE_SHIFT);
		base &= ~PAGE_MASK;
		avail_page = min_t(unsigned int, PAGE_SIZE - base,
					avail_here);
		c = kmap(*ppages) + base;

		while (avail_here) {
			avail_here -= avail_page;
			if (copied || avail_page < desc->elem_size) {
				unsigned int l = min(avail_page,
					desc->elem_size - copied);
				if (!elem) {
					elem = kmalloc(desc->elem_size,
						       GFP_KERNEL);
					err = -ENOMEM;
					if (!elem)
						goto out;
				}
				if (encode) {
					if (!copied) {
						err = desc->xcode(desc, elem);
						if (err)
							goto out;
					}
					memcpy(c, elem + copied, l);
					copied += l;
					if (copied == desc->elem_size)
						copied = 0;
				} else {
					memcpy(elem + copied, c, l);
					copied += l;
					if (copied == desc->elem_size) {
						err = desc->xcode(desc, elem);
						if (err)
							goto out;
						copied = 0;
					}
				}
				avail_page -= l;
				c += l;
			}
			while (avail_page >= desc->elem_size) {
				err = desc->xcode(desc, c);
				if (err)
					goto out;
				c += desc->elem_size;
				avail_page -= desc->elem_size;
			}
			if (avail_page) {
				unsigned int l = min(avail_page,
					    desc->elem_size - copied);
				if (!elem) {
					elem = kmalloc(desc->elem_size,
						       GFP_KERNEL);
					err = -ENOMEM;
					if (!elem)
						goto out;
				}
				if (encode) {
					if (!copied) {
						err = desc->xcode(desc, elem);
						if (err)
							goto out;
					}
					memcpy(c, elem + copied, l);
					copied += l;
					if (copied == desc->elem_size)
						copied = 0;
				} else {
					memcpy(elem + copied, c, l);
					copied += l;
					if (copied == desc->elem_size) {
						err = desc->xcode(desc, elem);
						if (err)
							goto out;
						copied = 0;
					}
				}
			}
			if (avail_here) {
				kunmap(*ppages);
				ppages++;
				c = kmap(*ppages);
			}

			avail_page = min(avail_here,
				 (unsigned int) PAGE_SIZE);
		}
		base = buf->page_len;  /* align to start of tail */
	}

	/* process tail */
	base -= buf->page_len;
	if (todo) {
		c = buf->tail->iov_base + base;
		if (copied) {
			unsigned int l = desc->elem_size - copied;

			if (encode)
				memcpy(c, elem + copied, l);
			else {
				memcpy(elem + copied, c, l);
				err = desc->xcode(desc, elem);
				if (err)
					goto out;
			}
			todo -= l;
			c += l;
		}
		while (todo) {
			err = desc->xcode(desc, c);
			if (err)
				goto out;
			c += desc->elem_size;
			todo -= desc->elem_size;
		}
	}
	err = 0;

out:
	kfree(elem);
	if (ppages)
		kunmap(*ppages);
	return err;
}