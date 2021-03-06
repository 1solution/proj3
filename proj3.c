/* IZP - 3. projekt - shlukova analyza
* xplsek03, 163714
*/

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

/* ****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t { // definition of object (one line in file)
    int id;
    float x;
    float y;
};

struct cluster_t { // cluster of clusters
    int size; // actual count of objects
    int capacity; // capacity of field - for objects
    struct obj_t *obj; // ptr to object
};

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap) {
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0; // set size to zero - neccessary
    c->capacity = cap;
    c->obj = NULL; // obj in cluster must be NULLed if capacity is zero!

    if(cap > 0) {
        c->obj = malloc(cap * sizeof(struct obj_t));

    if(c->obj == NULL)
        c->capacity = 0;
    }
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

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj) {
  if(c->size >= c->capacity)
    resize_cluster(c,CLUSTER_CHUNK+c->capacity);
     c->obj[c->size] = obj; // merge new obj
    c->size += 1;
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr) {
	assert(arr != NULL);

	FILE *foo;
    foo = fopen(filename,"r");

    if(foo == NULL) {
        *arr = NULL;
        fprintf(stderr,"%s","File does not exist.\n");
        return 0;
    }

    char fraze[] = "count=";// [0=c 1=o 2=u 3=n 4=t 5==]
    int c = 0;
    int i = 0;
    int nStr[6] = {'\0'};
    int n = 0;
    int done = 0;

    while(c != EOF && c != '\n') {

        c = fgetc(foo);

        if(c == fraze[i] && i < 6)
            i++;

        if(i == 6) {
                c = fgetc(foo);
                    int j = 0;

                while(!done) {
                    if(isdigit(c)) {
                        nStr[j] = c - 48;
                        j++;
                        c = fgetc(foo);
                    }
                    else if((c == '\n' || c == '\r') && j > 0) {
                        for(int i = 0; i < j; i++)
                            n += (nStr[j-i-1] * pow(10,i));
                        done = 1;
                    }
                    else {
                        fprintf(stderr,"%s","Wrong input file format - N is not a number or missing.\n");
                        fclose(foo);
                        return 0;
                    }
                }

        }

    }

    if(n <= 0) {
        fclose(foo);
        return 0;
    }

  struct cluster_t *clusters = malloc(n*sizeof(struct cluster_t));
  *arr = clusters;

  int id;
  float x;
  float y;
  char endline;

  for(int i = 0; i < n; i++) {
    int c = fscanf(foo,"%i%f%f%c",&id,&x,&y,&endline);

    if(c != 4 || (endline != EOF && endline != '\n')) {
        *arr = NULL;
        fprintf(stderr,"%s","Wrong file input, incorrect format of line data.\n");
        fclose(foo);
        return -1;
    }

      struct cluster_t soloCluster;
      init_cluster(&soloCluster,1);
      soloCluster.size = 1;

      if(x >= 0 && x <= 1000 && y >= 0 && y <= 1000) {
        clusters[i] = soloCluster;
        clusters[i].obj->id = id;
        clusters[i].obj->x = x;
        clusters[i].obj->y = y;
      }
      else {
        *arr = NULL;
        fprintf(stderr,"%s","X or Y from file is not in correct format.\n");
        fclose(foo);
        return 0;
      }
  }
    fclose(foo);
	return n;
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
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c2 != NULL);

    for(int i = 0; i < c2->size; i++)
        append_cluster(c1, c2->obj[i]);

    sort_cluster(c1);
}

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
 int remove_cluster(struct cluster_t *carr, int narr, int idx) {
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);

    for(int i = idx; i < narr-1; i++)
        carr[i] = carr[i+1];

    narr -= 1;

    return narr;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2) {
    assert(o1 != NULL);
    assert(o2 != NULL);

    float d = sqrt(pow(o2->x-o1->x,2)+pow(o2->y-o1->y,2));

    return d;

}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float tmp = 0;

    for(int i = 0; i < c1->size; i++) {
        for(int j = 0; j < c2->size; j++) {
            tmp += obj_distance(&c1->obj[i],&c2->obj[j]);
        }
    }
    return (tmp / (c1->size * c2->size));
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2) {
    assert(narr > 0);

    float minimum;
    float tmp;
    for(int i = 0; i < narr; i++) {
        for(int j = i+1; j < narr; j++) {
            if(i == 0 && j == 1) {
                minimum = cluster_distance(&carr[i], &carr[j]);
                *c1 = i;
                *c2 = j;
            }
            else {
                 tmp = cluster_distance(&carr[i], &carr[j]);
                if(tmp < minimum) {
                    minimum = tmp;
                    *c1 = i;
                    *c2 = j;
                }
            }
        }
    }
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
void print_clusters(struct cluster_t *carr, int narr) {
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*
    autoclean function, cleans everytime if program ends up with -1
*/
    void finisher(int *imported, struct cluster_t *clusters) { // cleaning everytime
        for(int i = 0; i < *imported; i++)
            clear_cluster(&clusters[i]); // clear clusters inside CLUSTERS[]
        free(clusters); // clear CLUSTERS[]
    }

int main(int argc, char *argv[]) {

    struct cluster_t *clusters;

    if(argc >= 2) {

    int N = 1;
    int imported = load_clusters(argv[1],&clusters); // importer = how many lines was imported from file

        if (imported > 0) {
               if(argv[2]) { // N processing
                    char *end;
                    N = strtol(argv[2],&end,10);
                    if(N <= 0 || *end != '\0' || N > imported) {
                        fprintf(stderr,"%s","Wrong argument N (must be number > 0) and N <= number of lines in file.\n");
                        finisher(&imported,clusters);
                        return 1;
                    }
                }

            while(imported > N) {
                int c1, c2;
                find_neighbours(clusters,imported,&c1, &c2);
                merge_clusters(&clusters[c1],&clusters[c2]);
                imported = remove_cluster(clusters,imported,c2);
            }

            print_clusters(clusters, imported);

            finisher(&imported,clusters);
            return 0;
        }
        else if(imported == -1) {
            finisher(&imported,clusters);
            return 1;
        }
        else {
            fprintf(stderr,"%s","Minimum nr. of lines to read from file is 1.\n");
            return 1;
        }

    }
    else {
        fprintf(stderr,"%s","Incorrect argument count.\n");
        return 1;
    }
}
