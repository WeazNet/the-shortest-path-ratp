#include "tab_hachage.h"
#include <time.h>
void purge() {
  int c = 0;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

tab_hachage *get_stations(tab_hachage *th, char *link) {
  FILE *stations_f;
  char c[BIG_LENGTH];
  char nom[BIG_LENGTH];
  char numero[BIG_LENGTH];

  stations_f = fopen(link, "r");

  if (stations_f == NULL) {
    printf("%s", "Le fichier n'existe pas !");
    return NULL;
  }

  while (fgets(c, sizeof(c), stations_f)) {
    sscanf(c, "%[^,],%[^\n]", nom, numero);

    char *nom_format = strdup(nom);
    uint32_t numero_format = strtol(numero, NULL, 0);

    data *d = new_data(numero_format, nom_format);
    if (strcmp(nom_format, "Nom")) // Pour passer la 1ere ligne
    {
      add_head_th(th, d);
    }
  }

  fclose(stations_f);
  return th;
}
tab_hachage *get_paths(tab_hachage *th, char *link) {
  FILE *paths_f;
  char c[BIG_LENGTH];
  char source[SMALL_LENGTH];
  char destination[SMALL_LENGTH];
  char ligne[SMALL_LENGTH];

  uint32_t *neighbors_n = get_number_of_neighbors(link);
  uint32_t i, j;
  for (i = 1; i <= NB_STATIONS; i++) {
    uint32_t length = neighbors_n[i - 1];

    if (length > 0) {
      maillon *m = get_maillon_by_numero(th, i);
      m->d->neighbors = malloc(sizeof(neighbor *) * length);
      assert(m->d->neighbors != NULL);

      for (j = 0; j < length; j++) {
        neighbor *v = malloc(sizeof(neighbor));
        assert(v != NULL);
        v->line = "";
        v->numero = 0;
        m->d->neighbors[j] = v;
      }
      m->d->nb_neighbors = 0;
    }
  }
  free(neighbors_n);

  paths_f = fopen(link, "r");

  if (paths_f == NULL) {
    printf("%s", "Le fichier n'existe pas !");
    return NULL;
  }

  fseek(paths_f, 0, SEEK_SET);

  while (fgets(c, sizeof(c), paths_f)) {

    sscanf(c, "%[^,],%[^,],%[^\n]", source, destination, ligne);

    trim(ligne);
    if (strcmp(ligne, "Ligne") != 0) // Pour ne pas prendre la 1ere ligne
    {

      char *ligne_format = strdup(ligne);
      trim(destination);
      trim(source);
      uint32_t destination_format = strtol(destination, NULL, 0);
      uint32_t source_format = strtol(source, NULL, 0);

      maillon *temp = get_maillon_by_numero(th, source_format);

      temp->d->neighbors[temp->d->nb_neighbors]->line = ligne_format;
      temp->d->neighbors[temp->d->nb_neighbors]->numero = destination_format;
      temp->d->nb_neighbors++;
    }
  }

  fclose(paths_f);
  return th;
}

tab_hachage *get_data(char *link_stations, char *link_paths) {

  tab_hachage *th = new_tab_hachage(NB_ALPHABET);
  th = get_stations(th, link_stations);
  th = get_paths(th, link_paths);
  return th;
}

int32_t get_numero(tab_hachage *th, _Bool start) {
  char c;
  uint32_t k;

  if (start)
    printf("Entrez s'il vous plaît la première lettre de votre station de "
           "départ: ");
  else
    printf("Entrez s'il vous plaît la première lettre de votre station "
           "d'arrivée: ");
  scanf(" %c", &c);
  c = toupper(c);

  if (!isalpha(c)) {
    purge();
    printf("\nCette lettre n'est pas reconnue ...\n\n");
    return -1;
  }
  liste *l = disp_by_letter(th, c);

  if (l->head == NULL || l->length == 0) {
    printf("\nCette lettre n'a aucun nom attribué ...\n\n");
    return -1;
  }

  _Bool isPresent = 0;

  while (!isPresent) {
    if (start)
      printf("\n\nChoisissez parmi la liste présente le numéro de votre "
             "station de "
             "départ: ");
    else
      printf(
          "\n\nChoisissez parmi la liste présente le numéro de votre station "
          "d'arrivée: ");
    purge();
    scanf("%d", &k);
    isPresent = 0;
    maillon *m = l->head;
    while (m != NULL) {
      if (m->d->numero == k) {
        isPresent = 1;
        break;
      }
      if (m->next == NULL) {
        printf("\nCe numéro n'existe pas, réessayez s'il vous "
               "plaît, voici la liste à nouveau :\n");

        disp_by_letter(th, c);
      }
      m = m->next;
    }
  }
  if (isPresent)
    printf("\nMerci, c'est noté !\n\n");

  return k;
}

int main() {

  srand(time(NULL));
  tab_hachage *th = get_data("stations.csv", "paths.csv");

  uint32_t num_depart = -1;
  uint32_t num_arrivee = -1;

  while (num_depart == -1)
    num_depart = get_numero(th, 1);
  while (num_arrivee == -1)
    num_arrivee = get_numero(th, 0);

  clock_t begin_prog = clock();
  dijkstra(th, num_depart);

  maillon *m_depart = get_maillon_by_numero(th, num_depart);
  maillon *m_arrivee = get_maillon_by_numero(th, num_arrivee);
  printf("\nRécap trajet : \n (départ : %s(%d) => destination : %s(%d))",
         m_depart->d->nom, num_depart, m_arrivee->d->nom, num_arrivee);

  uint32_t max_cost = 0;

  printf("%s", "\nItinéraire : \n");
  uint32_t i;
  char *line = malloc(sizeof(char) * SMALL_LENGTH);

  data *d = m_arrivee->d;
  line[0] = '*';

  while (d->numero != num_depart) {
    maillon *parent = get_maillon_by_numero(th, d->parent_numero);

    if (parent == NULL)
      break;

    for (i = 0; i < parent->d->nb_neighbors; i++) {
      if (parent->d->neighbors[i]->numero == d->numero) {
        max_cost++;
        if (line[0] != '*') {
          if (strcmp(line, parent->d->neighbors[i]->line) != 0) {
            max_cost += 5;
          }
        }
        strcpy(line, parent->d->neighbors[i]->line);
      }
    }
    printf("(%s(%d) => %s(%d)) ligne %s\n", parent->d->nom, parent->d->numero,
           d->nom, d->numero, line);
    d = parent->d;
  }
  printf("\nIl vous faudra %d minutes pour arriver à destination.", max_cost);

  free(line);
  free_th(th);

  clock_t end_prog = clock();

  double time_spent = (double)(end_prog - begin_prog) / CLOCKS_PER_SEC;
  printf("\nTemps pour le programme : %lf\n", time_spent);
  return EXIT_SUCCESS;
}
