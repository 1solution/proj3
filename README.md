#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <ctype.h>

/*
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

struct obj_t { // OBJEKT: id, x a y
    int id;
    float x;
    float y;
};

struct cluster_t { // CLUSTER X OBJEKTU:
    int size; // pocet objektu ve shluku
    int capacity; //pocet objektu, pro ktere je rezervovano misto v poli
    struct obj_t *obj; // ukazatel na vlastni objekt
};

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap) {
    assert(c != NULL);
    assert(cap >= 0);
    c->size = 0;
    c->obj = malloc(cap * sizeof(struct obj_t));
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c) {
  free(c->obj);
  init_cluster(c,0);
}

const int CLUSTER_CHUNK = 10; // when reallocationg, use this value as step+
/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap) {
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

void append_cluster(struct cluster_t *c, struct obj_t obj) {
  if(c->size >= c->capacity)
    resize_cluster(c,CLUSTER_CHUNK+c->capacity);
     c->obj[c->size] = obj; // merge new obj
    c->size += 1;
}

int load_clusters(char *filename, struct cluster_t **arr) {
	assert(arr != NULL);

	FILE *foo;
  foo = fopen(filename,"r");

  if(foo == NULL)
    printf("File does not exist.\n");

  int c = 0, i = 0;
  char fraze[] = "count=";

  while(c != EOF || c!= '\n') {
  if(i == 6) {
    char number[100] = {'\0'};
    int j = 0;
    while(c != EOF || c != '\n') {
      if(isdigit(c)) {
        number[j] = (c = fgetc(foo));
      }
      else {
        // error
      }
    }

    char *end;
    int newInt = strtol(number, &end);
    printf("%d", newInt);
  }
    c = fgetc(foo);
  if(c == fraze[i] && i < 6)
    i++;
  }

  int n = 0;

  struct cluster_t *clusters = malloc(n*sizeof(struct cluster_t));
  *arr = clusters; // uloz ukazatel na prvni shluk v poli *arr[] - **arr

  int id = 0;
  float x = 0.0;
  float y = 0.0;
  char endline;

  for(int i = 0; i < n; i++) {
    fscanf(foo,"%d %f %f%c",&id,&x,&y,&endline);

    if(endline == EOF || endline == '\n') {
      struct cluster_t soloCluster;
      init_cluster(&soloCluster,1);
      soloCluster.size = 1;

      if(x >= 0 || x <= 1000 || y >= 0 || y <= 1000) {
        clusters[i] = soloCluster;
        clusters[i].obj->id = id;
        clusters[i].obj->x = x;
        clusters[i].obj->y = y;
      }
      else {
        *arr = NULL;
        fprintf(stderr,"%s","X or Y from file is not in correct format.\n");
        break;
      }
    }
    else {
      *arr = NULL;
      fprintf(stderr,"%s","Something wrong in input file.\n");
      break;
    }
  }
  fclose(foo);
	return n;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    // TODO
}

/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    // TODO
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    // TODO
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    // TODO
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // TODO
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b) {
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c) {
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c) {
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    load_clusters("C:\\Users\\xplsek03\\Desktop\\proj3\\bin\\Debug\\objekty",&clusters);

    print_clusters(clusters, 5);


    return 0;

}
