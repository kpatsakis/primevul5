static struct ip *ip_reass(Slirp *slirp, struct ip *ip, struct ipq *fp)
{
    register struct mbuf *m = dtom(slirp, ip);
    register struct ipasfrag *q;
    int hlen = ip->ip_hl << 2;
    int i, next;

    DEBUG_CALL("ip_reass");
    DEBUG_ARG("ip = %p", ip);
    DEBUG_ARG("fp = %p", fp);
    DEBUG_ARG("m = %p", m);

    /*
     * Presence of header sizes in mbufs
     * would confuse code below.
     * Fragment m_data is concatenated.
     */
    m->m_data += hlen;
    m->m_len -= hlen;

    /*
     * If first fragment to arrive, create a reassembly queue.
     */
    if (fp == NULL) {
        struct mbuf *t = m_get(slirp);

        if (t == NULL) {
            goto dropfrag;
        }
        fp = mtod(t, struct ipq *);
        insque(&fp->ip_link, &slirp->ipq.ip_link);
        fp->ipq_ttl = IPFRAGTTL;
        fp->ipq_p = ip->ip_p;
        fp->ipq_id = ip->ip_id;
        fp->frag_link.next = fp->frag_link.prev = &fp->frag_link;
        fp->ipq_src = ip->ip_src;
        fp->ipq_dst = ip->ip_dst;
        q = (struct ipasfrag *)fp;
        goto insert;
    }

    /*
     * Find a segment which begins after this one does.
     */
    for (q = fp->frag_link.next; q != (struct ipasfrag *)&fp->frag_link;
         q = q->ipf_next)
        if (q->ipf_off > ip->ip_off)
            break;

    /*
     * If there is a preceding segment, it may provide some of
     * our data already.  If so, drop the data from the incoming
     * segment.  If it provides all of our data, drop us.
     */
    if (q->ipf_prev != &fp->frag_link) {
        struct ipasfrag *pq = q->ipf_prev;
        i = pq->ipf_off + pq->ipf_len - ip->ip_off;
        if (i > 0) {
            if (i >= ip->ip_len)
                goto dropfrag;
            m_adj(dtom(slirp, ip), i);
            ip->ip_off += i;
            ip->ip_len -= i;
        }
    }

    /*
     * While we overlap succeeding segments trim them or,
     * if they are completely covered, dequeue them.
     */
    while (q != (struct ipasfrag *)&fp->frag_link &&
           ip->ip_off + ip->ip_len > q->ipf_off) {
        i = (ip->ip_off + ip->ip_len) - q->ipf_off;
        if (i < q->ipf_len) {
            q->ipf_len -= i;
            q->ipf_off += i;
            m_adj(dtom(slirp, q), i);
            break;
        }
        q = q->ipf_next;
        m_free(dtom(slirp, q->ipf_prev));
        ip_deq(q->ipf_prev);
    }

insert:
    /*
     * Stick new segment in its place;
     * check for complete reassembly.
     */
    ip_enq(iptofrag(ip), q->ipf_prev);
    next = 0;
    for (q = fp->frag_link.next; q != (struct ipasfrag *)&fp->frag_link;
         q = q->ipf_next) {
        if (q->ipf_off != next)
            return NULL;
        next += q->ipf_len;
    }
    if (((struct ipasfrag *)(q->ipf_prev))->ipf_tos & 1)
        return NULL;

    /*
     * Reassembly is complete; concatenate fragments.
     */
    q = fp->frag_link.next;
    m = dtom(slirp, q);

    int was_ext = m->m_flags & M_EXT;

    q = (struct ipasfrag *)q->ipf_next;
    while (q != (struct ipasfrag *)&fp->frag_link) {
        struct mbuf *t = dtom(slirp, q);
        q = (struct ipasfrag *)q->ipf_next;
        m_cat(m, t);
    }

    /*
     * Create header for new ip packet by
     * modifying header of first packet;
     * dequeue and discard fragment reassembly header.
     * Make header visible.
     */
    q = fp->frag_link.next;

    /*
     * If the fragments concatenated to an mbuf that's
     * bigger than the total size of the fragment, then and
     * m_ext buffer was alloced. But fp->ipq_next points to
     * the old buffer (in the mbuf), so we must point ip
     * into the new buffer.
     */
    if (!was_ext && m->m_flags & M_EXT) {
        int delta = (char *)q - m->m_dat;
        q = (struct ipasfrag *)(m->m_ext + delta);
    }

    ip = fragtoip(q);
    ip->ip_len = next;
    ip->ip_tos &= ~1;
    ip->ip_src = fp->ipq_src;
    ip->ip_dst = fp->ipq_dst;
    remque(&fp->ip_link);
    (void)m_free(dtom(slirp, fp));
    m->m_len += (ip->ip_hl << 2);
    m->m_data -= (ip->ip_hl << 2);

    return ip;

dropfrag:
    m_free(m);
    return NULL;
}