#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "macros.h"
#include "seq.h"

struct c4seq *c4seq_init(struct c4seq *self) {
  self->eof = false;
  self->idx = 0;
  self->free = NULL;
  self->next = NULL;
  return self;
}

static void *map_next(struct c4seq *_seq) {
  struct c4seq_map *seq = C4PTROF(c4seq_map, super, _seq);
  void *it = NULL;

  while (!it && !seq->src->eof) {
    it = c4seq_next(seq->src);
    if (!it) { return NULL; }
    it = seq->fn(it);
  }
  
  return it;
}

struct c4seq *c4seq_map(struct c4seq *self, c4seq_map_fnt fn,
			struct c4seq_map *out) {
  c4seq_init(&out->super);
  out->super.next = map_next;
  out->src = self;
  out->fn = fn;  
  return &out->super;
}

void *c4seq_next(struct c4seq *self) {
  assert(self->next);
  void *it = self->next(self);

  if (it) { self->idx++; }
  else {
    self->eof = true;
    c4seq_free(self);
  }

  return it;
}

void c4seq_free(struct c4seq *self) {
  if (self->free) { self->free(self); }
}
