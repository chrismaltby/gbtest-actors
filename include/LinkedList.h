#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define DL_PUSH_HEAD(head, item) \
    (item)->prev = 0; \
    (item)->next = (head); \
    if (head) { \
        (head)->prev = (item); \
    } \
    (head) = (item)

#define DL_REMOVE_ITEM(head, item) \
  /* Hook next to prev */ \
  if ((item)->next && (item)->prev) \
  { \
    /* Middle of list */ \
    (item)->prev->next = (item)->next; \
    (item)->next->prev = (item)->prev; \
  } \
  else if (actor->next) \
  { \
    /* Start of list */ \
    (item)->next->prev = 0; \
    (head) = actor->next; \
  } \
  else if (actor->prev) \
  { \
    /* End of list */ \
    (item)->prev->next = 0; \
  } \
  else \
  { \
    (head) = 0; \
  }

#define DL_CONTAINS(head_mut, item, found) \
  (found) = 0; \
  while (head_mut) \
  { \
    if ((head_mut) == (item)) \
    { \
      (found) = 1; \
      break; \
    } \
    (head_mut) = (head_mut)->next; \
  }

#endif
