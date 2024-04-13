static void FreePictureMemoryList (PictureMemory* head) {
  PictureMemory* next;
  while(head != NULL) {
    next = head->next;
    if(head->pixel_info != NULL)
      RelinquishVirtualMemory(head->pixel_info);
    free(head);
    head = next;
  }
}