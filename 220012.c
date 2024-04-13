static void headerSort(Header h)
{
    if (!h->sorted) {
	qsort(h->index, h->indexUsed, sizeof(*h->index), indexCmp);
	h->sorted = 1;
    }
}