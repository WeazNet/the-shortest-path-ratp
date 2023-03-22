#include "dijkstra.h"

int32_t get_minimum_cost_shortest_path(liste *file) {
  uint32_t min_cost = MAX_NUMBER, min_index = MAX_NUMBER, i, source;
  maillon *r = NULL, *m = NULL, *prev = NULL;
  data *d;

  for (i = 0; i < file->length; i++) {
    r = find_maillon(file, i);
    if (r->d->cost < min_cost && i < min_index) {
      min_cost = r->d->cost;
      min_index = i;
    }
  }
  if (min_index == 0)
    d = rem_maillon(file, min_index);
  else
    d = rem_maillon(file, min_index + 1);

  if (d == NULL)
    return CODE_END;
  source = d->numero;

  free(d);
  return source;
}

void dijkstra(tab_hachage *th, uint32_t source) {

  uint32_t cost_min, index_min, i, j, n;

  liste *file = new_liste();
  maillon *p = get_maillon_by_numero(th, source);

  p->d->cost = 0;
  index_min = CODE_END;
  char *last_line = "";

  while (p != NULL) {
    for (j = 0; j < p->d->nb_neighbors; j++) {
      maillon *neighbor = get_maillon_by_numero(th, p->d->neighbors[j]->numero);
      uint32_t new_cost = p->d->cost + COST_PER_STATION;
      if (strlen(last_line) > 0) {
        if (strcmp(last_line, p->d->neighbors[j]->line) != 0) {
          new_cost += COST_PER_LINE;
        }
      }
      if (neighbor->d->cost >= new_cost) {
        neighbor->d->cost = new_cost;
        neighbor->d->parent_numero = p->d->numero;
        if (neighbor->d->parent_numero != NIL) {
          data *d2 = new_data(neighbor->d->numero, neighbor->d->nom);
          d2->cost = neighbor->d->cost;
          add_tail(file, d2);
        }
      }
      if (neighbor->d->parent_numero == NIL) {
        neighbor->d->parent_numero = p->d->numero;
        data *d1 = new_data(neighbor->d->numero, neighbor->d->nom);
        d1->cost = neighbor->d->cost;
        add_tail(file, d1);
      }
    }
    p->d->visited = 1;

    index_min = get_minimum_cost_shortest_path(file);
    if (index_min == CODE_END)
      break;

    maillon *temp = get_maillon_by_numero(th, index_min);
    maillon *parent = get_maillon_by_numero(th, temp->d->parent_numero);

    for (j = 0; j < parent->d->nb_neighbors; j++) {
      if (parent->d->neighbors[j]->numero == index_min)
        last_line = parent->d->neighbors[j]->line;
    }
    p = temp;
  }
  free_liste(file);
}