
static void bfq_update_rate_reset(struct bfq_data *bfqd, struct request *rq)
{
	u32 rate, weight, divisor;

	/*
	 * For the convergence property to hold (see comments on
	 * bfq_update_peak_rate()) and for the assessment to be
	 * reliable, a minimum number of samples must be present, and
	 * a minimum amount of time must have elapsed. If not so, do
	 * not compute new rate. Just reset parameters, to get ready
	 * for a new evaluation attempt.
	 */
	if (bfqd->peak_rate_samples < BFQ_RATE_MIN_SAMPLES ||
	    bfqd->delta_from_first < BFQ_RATE_MIN_INTERVAL)
		goto reset_computation;

	/*
	 * If a new request completion has occurred after last
	 * dispatch, then, to approximate the rate at which requests
	 * have been served by the device, it is more precise to
	 * extend the observation interval to the last completion.
	 */
	bfqd->delta_from_first =
		max_t(u64, bfqd->delta_from_first,
		      bfqd->last_completion - bfqd->first_dispatch);

	/*
	 * Rate computed in sects/usec, and not sects/nsec, for
	 * precision issues.
	 */
	rate = div64_ul(bfqd->tot_sectors_dispatched<<BFQ_RATE_SHIFT,
			div_u64(bfqd->delta_from_first, NSEC_PER_USEC));

	/*
	 * Peak rate not updated if:
	 * - the percentage of sequential dispatches is below 3/4 of the
	 *   total, and rate is below the current estimated peak rate
	 * - rate is unreasonably high (> 20M sectors/sec)
	 */
	if ((bfqd->sequential_samples < (3 * bfqd->peak_rate_samples)>>2 &&
	     rate <= bfqd->peak_rate) ||
		rate > 20<<BFQ_RATE_SHIFT)
		goto reset_computation;

	/*
	 * We have to update the peak rate, at last! To this purpose,
	 * we use a low-pass filter. We compute the smoothing constant
	 * of the filter as a function of the 'weight' of the new
	 * measured rate.
	 *
	 * As can be seen in next formulas, we define this weight as a
	 * quantity proportional to how sequential the workload is,
	 * and to how long the observation time interval is.
	 *
	 * The weight runs from 0 to 8. The maximum value of the
	 * weight, 8, yields the minimum value for the smoothing
	 * constant. At this minimum value for the smoothing constant,
	 * the measured rate contributes for half of the next value of
	 * the estimated peak rate.
	 *
	 * So, the first step is to compute the weight as a function
	 * of how sequential the workload is. Note that the weight
	 * cannot reach 9, because bfqd->sequential_samples cannot
	 * become equal to bfqd->peak_rate_samples, which, in its
	 * turn, holds true because bfqd->sequential_samples is not
	 * incremented for the first sample.
	 */
	weight = (9 * bfqd->sequential_samples) / bfqd->peak_rate_samples;

	/*
	 * Second step: further refine the weight as a function of the
	 * duration of the observation interval.
	 */
	weight = min_t(u32, 8,
		       div_u64(weight * bfqd->delta_from_first,
			       BFQ_RATE_REF_INTERVAL));

	/*
	 * Divisor ranging from 10, for minimum weight, to 2, for
	 * maximum weight.
	 */
	divisor = 10 - weight;

	/*
	 * Finally, update peak rate:
	 *
	 * peak_rate = peak_rate * (divisor-1) / divisor  +  rate / divisor
	 */
	bfqd->peak_rate *= divisor-1;
	bfqd->peak_rate /= divisor;
	rate /= divisor; /* smoothing constant alpha = 1/divisor */

	bfqd->peak_rate += rate;

	/*
	 * For a very slow device, bfqd->peak_rate can reach 0 (see
	 * the minimum representable values reported in the comments
	 * on BFQ_RATE_SHIFT). Push to 1 if this happens, to avoid
	 * divisions by zero where bfqd->peak_rate is used as a
	 * divisor.
	 */
	bfqd->peak_rate = max_t(u32, 1, bfqd->peak_rate);

	update_thr_responsiveness_params(bfqd);

reset_computation:
	bfq_reset_rate_computation(bfqd, rq);