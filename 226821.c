static int sctp_msghdr_parse(const struct msghdr *msg, struct sctp_cmsgs *cmsgs)
{
	struct msghdr *my_msg = (struct msghdr *)msg;
	struct cmsghdr *cmsg;

	for_each_cmsghdr(cmsg, my_msg) {
		if (!CMSG_OK(my_msg, cmsg))
			return -EINVAL;

		/* Should we parse this header or ignore?  */
		if (cmsg->cmsg_level != IPPROTO_SCTP)
			continue;

		/* Strictly check lengths following example in SCM code.  */
		switch (cmsg->cmsg_type) {
		case SCTP_INIT:
			/* SCTP Socket API Extension
			 * 5.3.1 SCTP Initiation Structure (SCTP_INIT)
			 *
			 * This cmsghdr structure provides information for
			 * initializing new SCTP associations with sendmsg().
			 * The SCTP_INITMSG socket option uses this same data
			 * structure.  This structure is not used for
			 * recvmsg().
			 *
			 * cmsg_level    cmsg_type      cmsg_data[]
			 * ------------  ------------   ----------------------
			 * IPPROTO_SCTP  SCTP_INIT      struct sctp_initmsg
			 */
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(struct sctp_initmsg)))
				return -EINVAL;

			cmsgs->init = CMSG_DATA(cmsg);
			break;

		case SCTP_SNDRCV:
			/* SCTP Socket API Extension
			 * 5.3.2 SCTP Header Information Structure(SCTP_SNDRCV)
			 *
			 * This cmsghdr structure specifies SCTP options for
			 * sendmsg() and describes SCTP header information
			 * about a received message through recvmsg().
			 *
			 * cmsg_level    cmsg_type      cmsg_data[]
			 * ------------  ------------   ----------------------
			 * IPPROTO_SCTP  SCTP_SNDRCV    struct sctp_sndrcvinfo
			 */
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(struct sctp_sndrcvinfo)))
				return -EINVAL;

			cmsgs->srinfo = CMSG_DATA(cmsg);

			if (cmsgs->srinfo->sinfo_flags &
			    ~(SCTP_UNORDERED | SCTP_ADDR_OVER |
			      SCTP_SACK_IMMEDIATELY | SCTP_SENDALL |
			      SCTP_PR_SCTP_MASK | SCTP_ABORT | SCTP_EOF))
				return -EINVAL;
			break;

		case SCTP_SNDINFO:
			/* SCTP Socket API Extension
			 * 5.3.4 SCTP Send Information Structure (SCTP_SNDINFO)
			 *
			 * This cmsghdr structure specifies SCTP options for
			 * sendmsg(). This structure and SCTP_RCVINFO replaces
			 * SCTP_SNDRCV which has been deprecated.
			 *
			 * cmsg_level    cmsg_type      cmsg_data[]
			 * ------------  ------------   ---------------------
			 * IPPROTO_SCTP  SCTP_SNDINFO    struct sctp_sndinfo
			 */
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(struct sctp_sndinfo)))
				return -EINVAL;

			cmsgs->sinfo = CMSG_DATA(cmsg);

			if (cmsgs->sinfo->snd_flags &
			    ~(SCTP_UNORDERED | SCTP_ADDR_OVER |
			      SCTP_SACK_IMMEDIATELY | SCTP_SENDALL |
			      SCTP_PR_SCTP_MASK | SCTP_ABORT | SCTP_EOF))
				return -EINVAL;
			break;
		case SCTP_PRINFO:
			/* SCTP Socket API Extension
			 * 5.3.7 SCTP PR-SCTP Information Structure (SCTP_PRINFO)
			 *
			 * This cmsghdr structure specifies SCTP options for sendmsg().
			 *
			 * cmsg_level    cmsg_type      cmsg_data[]
			 * ------------  ------------   ---------------------
			 * IPPROTO_SCTP  SCTP_PRINFO    struct sctp_prinfo
			 */
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(struct sctp_prinfo)))
				return -EINVAL;

			cmsgs->prinfo = CMSG_DATA(cmsg);
			if (cmsgs->prinfo->pr_policy & ~SCTP_PR_SCTP_MASK)
				return -EINVAL;

			if (cmsgs->prinfo->pr_policy == SCTP_PR_SCTP_NONE)
				cmsgs->prinfo->pr_value = 0;
			break;
		case SCTP_AUTHINFO:
			/* SCTP Socket API Extension
			 * 5.3.8 SCTP AUTH Information Structure (SCTP_AUTHINFO)
			 *
			 * This cmsghdr structure specifies SCTP options for sendmsg().
			 *
			 * cmsg_level    cmsg_type      cmsg_data[]
			 * ------------  ------------   ---------------------
			 * IPPROTO_SCTP  SCTP_AUTHINFO  struct sctp_authinfo
			 */
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(struct sctp_authinfo)))
				return -EINVAL;

			cmsgs->authinfo = CMSG_DATA(cmsg);
			break;
		case SCTP_DSTADDRV4:
		case SCTP_DSTADDRV6:
			/* SCTP Socket API Extension
			 * 5.3.9/10 SCTP Destination IPv4/6 Address Structure (SCTP_DSTADDRV4/6)
			 *
			 * This cmsghdr structure specifies SCTP options for sendmsg().
			 *
			 * cmsg_level    cmsg_type         cmsg_data[]
			 * ------------  ------------   ---------------------
			 * IPPROTO_SCTP  SCTP_DSTADDRV4 struct in_addr
			 * ------------  ------------   ---------------------
			 * IPPROTO_SCTP  SCTP_DSTADDRV6 struct in6_addr
			 */
			cmsgs->addrs_msg = my_msg;
			break;
		default:
			return -EINVAL;
		}
	}

	return 0;
}