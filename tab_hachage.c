#include "tab_hachage.h"

data *new_data(uint32_t numero, char *nom) {
  data *d = malloc(sizeof(data));
  assert(d != NULL);
  d->numero = numero;
  d->nom = nom;
  d->visited = 0;
  d->cost = MAX_NUMBER;
  d->neighbors = NULL;
  d->parent_numero = NIL;
  return d;
}
maillon *new_maillon(data *d) {
  maillon *m = malloc(sizeof(maillon));
  assert(m != NULL);
  m->d = d;
  m->next = NULL;
  return m;
}

liste *new_liste() {
  liste *r = malloc(sizeof(liste));
  assert(r != NULL);
  r->length = 0;
  r->head = NULL;
  r->tail = NULL;
  return r;
}

void add_head(liste *l, data *d) {
  maillon *m = new_maillon(d);
  m->next = l->head;
  l->head = m;

  if (l->length == 0)
    l->tail = m;
  l->length++;
}

data *rem_head(liste *l) {
  maillon *t = l->head;
  data *r = t->d;

  l->head = l->head->next;
  free(t);
  l->length -= 1;
  if (l->length == 0)
    l->tail = NULL;
  return r;
}

void *add_tail(liste *l, data *d) {
  maillon *m = new_maillon(d);
  m->next = NULL;
  if (l->length > 0)
    l->tail->next = m;
  else
    l->head = m;
  l->tail = m;

  l->length++;
}

data *rem_tail(liste *l) {
  maillon *avant_dernier = NULL;
  maillon *dernier = l->head;

  while (dernier != l->tail) {
    avant_dernier = dernier;
    dernier = dernier->next;
  }
  data *d = dernier->d;
  l->tail = avant_dernier;
  l->tail->next = NULL;
  free(dernier);
  l->length -= 1;

  if (l->length == 0)
    l->head = NULL;
  return d;
}

void free_liste(liste *l) {
  uint32_t i;
  while (l->length > 0) {
    data *d = rem_head(l);
    for (i = 0; i < d->nb_neighbors; i++) {
      free(d->neighbors[i]->line);
      free(d->neighbors[i]);
    }
    free(d->nom);
    free(d->neighbors);
    free(d);
  }
  free(l);
}

tab_hachage *new_tab_hachage(uint32_t length) {
  uint32_t i;
  tab_hachage *th = malloc((sizeof(tab_hachage)));
  assert(th != NULL);
  th->tab = malloc(length * sizeof(liste *));
  assert(th->tab != NULL);
  for (int i = 0; i < length; i++)
    th->tab[i] = new_liste();
  th->length = length;
  return th;
}

void free_th(tab_hachage *th) {
  uint32_t i;
  for (i = 0; i < th->length; i++)
    free_liste(th->tab[i]);
  free(th->tab);
  free(th);
}

uint32_t hachage(tab_hachage *th, char c) {
  uint32_t r = c - 'A';
  if ((int32_t)c == -61) // Les É seront rangés avec les E.
    r = 'E' - 'A';
  return r % th->length;
}
void add_head_th(tab_hachage *th, data *d) {
  uint32_t p = hachage(th, d->nom[0]);
  add_head(th->tab[p], d);
}

void disp_elements(tab_hachage *th) {
  uint32_t i;
  for (i = 0; i < th->length; i++) {
    maillon *m = th->tab[i]->head;
    while (m != NULL) {
      printf("\n%s, %d", m->d->nom, m->d->numero);
      m = m->next;
    }
  }
}

liste *disp_by_letter(tab_hachage *th, char c) {
  liste *l = th->tab[hachage(th, c)];

  maillon *m = l->head;
  while (m != NULL) {
    printf("\n%s, %d", m->d->nom, m->d->numero);
    m = m->next;
  }
  return l;
}

liste *get_liste_by_numero(tab_hachage *th, uint32_t numero) {
  uint32_t i = 0;

  while (i < th->length) {
    if (th->tab[i] != NULL && th->tab[i]->head != NULL) {
      uint32_t n = th->tab[i]->head->d->numero;
      if (numero <= n)
        break;
    }
    i++;
  }

  if (i == th->length)
    return NULL;
  else
    return th->tab[i];
}

maillon *get_maillon_by_numero(tab_hachage *th, uint32_t numero) {
  liste *l = get_liste_by_numero(th, numero);
  if (l == NULL || l->head == NULL)
    return NULL;
  int i = 0;
  maillon *m = l->head;
  while (m != NULL && numero < m->d->numero) {
    m = m->next;
  }

  return m;
}

void trim(char *str) {
  char *end;
  while (isspace((unsigned char)*str))
    str++;
  if (*str == 0)
    return;
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';
}

uint32_t *get_number_of_neighbors(char *link) {

  FILE *aretes_f;
  char c[BIG_LENGTH];
  char source[SMALL_LENGTH];
  char destination[SMALL_LENGTH];
  char ligne[SMALL_LENGTH];
  uint32_t *stations_n = malloc(sizeof(uint32_t) * NB_STATIONS);
  uint32_t i = 0;
  for (i = 0; i < NB_STATIONS; i++)
    stations_n[i] = 0;
  aretes_f = fopen(link, "r");

  fseek(aretes_f, 0, SEEK_SET);

  if (aretes_f == NULL) {
    printf("%s", "Le fichier n'existe pas !");
    return NULL;
  }

  while (fgets(c, sizeof(c), aretes_f)) {
    sscanf(c, "%[^,],%[^,],%[^\n]", source, destination, ligne);

    trim(ligne);
    if (strcmp(ligne, "Ligne") != 0) // Pour ne pas prendre la 1ere ligne
    {
      trim(destination);
      trim(source);
      uint32_t destination_format = strtol(destination, NULL, 0);
      uint32_t source_format = strtol(source, NULL, 0);

      stations_n[source_format - 1]++;
    }
  }

  return stations_n;
}

maillon *find_maillon(liste *l, uint32_t position) {
  maillon *r = l->head;
  uint32_t i;

  if (position >= l->length) {
    return NULL;
  }
  for (i = 0; i < position; i++) {
    r = r->next;
  }
  return r;
}

data *rem_maillon(liste *l, uint32_t position) {
  maillon *m, *r;
  data *d;
  if (position >= l->length) {
    return NULL;
  } else if (position == 0) {
    return rem_head(l);
  } else if (position == l->length - 1) {
    return rem_tail(l);
  } else {
    r = find_maillon(l, position - 1);
    m = r->next;
    d = m->d;
    r->next = m->next;
    free(m);
    l->length -= 1;
    return d;
  }
}