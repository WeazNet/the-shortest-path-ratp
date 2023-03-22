#include "limits.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIG_LENGTH 64
#define SMALL_LENGTH 10

#define NB_ALPHABET 26
#define NB_STATIONS 304
#define NB_ARETES 734

#define MAX_NUMBER 9999
#define NIL -1

typedef struct _neighbor {
  char *line;
  int32_t numero;
} neighbor;

typedef struct _data {
  uint32_t numero;
  uint32_t cost;
  char *nom;
  _Bool visited;
  int32_t parent_numero;
  neighbor **neighbors;
  uint32_t nb_neighbors;
} data;

typedef struct _maillon {
  data *d;
  struct _maillon *next;
} maillon;

typedef struct _liste {
  maillon *head;
  maillon *tail;
  uint32_t length;
} liste;

typedef struct _tab_hachage {
  liste **tab;
  uint32_t length;
} tab_hachage;

data *new_data(uint32_t numero, char *nom);
maillon *new_maillon(data *d);

liste *new_liste();

void add_head(liste *l, data *d);
data *rem_head(liste *l);

void *add_tail(liste *l, data *d);
data *rem_tail(liste *l);

void free_liste(liste *l);

void disp_elements(tab_hachage *th);
liste *disp_by_letter(tab_hachage *th, char c);

tab_hachage *new_tab_hachage(uint32_t length);
uint32_t hachage(tab_hachage *th, char c);
void add_head_th(tab_hachage *th, data *d);
void free_th(tab_hachage *th);

void trim(char *str);

maillon *get_maillon_by_numero(tab_hachage *th, uint32_t numero);
liste *get_liste_by_numero(tab_hachage *th, uint32_t numero);

uint32_t *get_number_of_neighbors(char *link);

maillon *find_maillon(liste *l, uint32_t position);
data *rem_maillon(liste *l, uint32_t position);