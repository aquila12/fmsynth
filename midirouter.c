// midirouter.c
// MIDI event router

#include "midirouter.h"

#define ROUTER_SIZE 1024

midiroute_t route[ROUTER_SIZE];
size_t n=0;

void mrouter_insert(midifilter_t mask, midifilter_t value, fmel_t *target) {
  if(n<ROUTER_SIZE) {
    midiroute_t *r = &route[n++];

    r->mask = mask;
    r->value = value;
    r->target = target;
    r->remove = 0;
  }
  //TODO: Handle full table
}

void mrouter_delete(midifilter_t mask, midifilter_t value) {
  for(int i=0; i<n; ++i) {
    if(route[i].mask.v==mask.v && route[i].value.v==value.v) {
      route[i].remove = 1;
      return;
    }
  }
}

void mrouter_killtarget(fmel_t *target) {
  for(int i=0; i<n; ++i) {
    if(route[i].target==target) {
      route[i].remove = 1;
      return;
    }
  }
}

void mrouter_compact() {
  int i=0, j=0;

  for(int i=0; i<n; ++i) {
    if(route[i].remove) continue;

    if(i>j) route[j] = route[i];
    ++j;
  }

  n = j;
}

void mrouter_route(midifilter_t input, fmevent_t *event) {
  midipacked_t mask, value;
  for(int i=n-1; i>=0; --i) {
    mask = route[i].mask.v;
    value = route[i].value.v;
    if(!route[i].remove && (input.v & mask) == value) {
      route[i].target->event(route[i].target, *event);
      return;
    }
  }
}
