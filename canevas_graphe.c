#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include <assert.h>

#define INVALID_PARAM 3

//graphe représenté par une matrice d'adjacence
typedef struct {
  	int nombre_sommet;
  	int ** matrice_adjacence;
} graphe;

void affiche_graphe(graphe G){
	int i, j;

	printf("Graphe avec %d sommets \n", G.nombre_sommet);

	for(i = 0; i<G.nombre_sommet; i++) {
		printf("Voisins de %d: ",i);

		for(j = 0; j < G.nombre_sommet; j++) {
			if(G.matrice_adjacence[i][j])
        printf("%d ",j);
      // else
      //   printf("X ");
		}

		printf("\n");
	}
}

//Crée le fichier "graphX.pdf" (X = str en param)
//Le fichier n'est généré que si la fonction init_graph est implémentée
void ecrire_fichier_dot(graphe G, char* str) {

	if(G.matrice_adjacence == NULL) return;

	FILE* f;
	char buf[64];
	sprintf(buf,"graph%s.dot",str);

	if( !(f = fopen(buf,"w")) ) {
		perror("Opening dot file failure\n");
    exit(2);
	}

	fprintf(f,"graph G { \n");
	for(int i=0;i<G.nombre_sommet;i++)
    fprintf(f,"%d \n",i);

	for(int i=0;i<G.nombre_sommet;i++) {
		for(int j=0;j<i;j++){
			if(G.matrice_adjacence[i][j])
				fprintf(f,"%d -- %d\n",i,j);
		}
	}
	fprintf(f,"} \n");

	fclose(f);

	sprintf(buf,"neato -Tpdf graph%s.dot -o graph%s.pdf",str,str);
	system(buf);

}

int degre(graphe G, int peak) {
  int sum_exit = 0, sum_entry = 0;

  for( int i = 0; i < G.nombre_sommet; i++ ) {
    sum_exit  += G.matrice_adjacence[i][peak];
    sum_entry += G.matrice_adjacence[peak][i];
  }

	return sum_exit+sum_entry;
}


graphe init_graphe(int num_peak) {
  //créé un graphe dont tous les sommets sont isolés
	graphe G;

  G.nombre_sommet = num_peak;

	G.matrice_adjacence = NULL;
  G.matrice_adjacence = calloc(G.nombre_sommet, sizeof(int *));
  assert( G.matrice_adjacence  !=  NULL );

  for( int i = 0; i < G.nombre_sommet; i++ ) {
    G.matrice_adjacence[i] = calloc(G.nombre_sommet, sizeof(int));
    assert( G.matrice_adjacence[i]  !=  NULL );
  }

	return G;
}

void libere_graphe(graphe G) {
  if( G.matrice_adjacence ) {
    for( int i = 0; i < G.nombre_sommet; i++ )
      free(G.matrice_adjacence[i]);
    free(G.matrice_adjacence);
  }
}

graphe cycle_graphe(int size) {
  //créé un cycle
  if( size  <  3 ) {
    printf("Error in 'cycle_graphe()': invalid parameter 'size'");
    exit(INVALID_PARAM);
  }

	graphe G = init_graphe(size);

  for( int i = 0; i < G.nombre_sommet; i++ ) {
    if( !i ) {
      G.matrice_adjacence[i][G.nombre_sommet - 1] = 1;
      G.matrice_adjacence[G.nombre_sommet - 1][i] = 1;

      G.matrice_adjacence[i][i+1] = 1;
      G.matrice_adjacence[i+1][i] = 1;
    }

    else if( i  ==  G.nombre_sommet - 1 ) {
      G.matrice_adjacence[i][i-1] = 1;
      G.matrice_adjacence[i-1][i] = 1;

      G.matrice_adjacence[i][0] = 1;
      G.matrice_adjacence[0][i] = 1;
    }

    else {
      G.matrice_adjacence[i][i-1] = 1;
      G.matrice_adjacence[i-1][i] = 1;

      G.matrice_adjacence[i][i+1] = 1;
      G.matrice_adjacence[i+1][i] = 1;
    }
  }

	return G;
}


graphe complet_graphe(int size) {
  //créé un graphe complet
  if( size  <  3 ) {
    printf("Error in 'complet_graphe()': invalid parameter 'size'");
    exit(INVALID_PARAM);
  }

	graphe G=init_graphe(size);

  for( int i = 0; i < G.nombre_sommet; i++ ){
    for( int j = 0; j < G.nombre_sommet; j++ ) {
      if( i  !=  j )
        G.matrice_adjacence[i][j] = 1;
    }
  }

	return G;
}

int proba(float p) {
  p*=100;
  int alea = rand() % 100;
  if( alea  <=  p )
    return 1;
  return 0;
}

graphe alea_graphe(int size, float p) {
  // Créer un graphe dont les arêtes sont aléatoire entre les sommets
  if( size  <  3 ) {
    printf("Error in 'alea_graphe()': invalid parameter 'size'");
    exit(INVALID_PARAM);
  }

	graphe G=init_graphe(size);

  for( int i = 0; i < G.nombre_sommet; i++ ){
    for( int j = 0; j < G.nombre_sommet; j++ ) {
      if( i  !=  j ) {
        int alea = proba(p);
        G.matrice_adjacence[i][j] = alea;
        G.matrice_adjacence[j][i] = alea;
      }
    }
  }

	return G;
}

graphe graphe_arbre(int *pere, int size) {
 //créer un arbre couvrant à partir du tableau des pères représenté comme un graphe
  if( size  <  3 ) {
    printf("Error in 'graphe_arbre()': invalid parameter 'size'");
    exit(INVALID_PARAM);
  }

	graphe G=init_graphe(size);

  for( int p = 0; p < G.nombre_sommet; p++) {
    if( pere[p]  !=  -1 )
      G.matrice_adjacence[p][pere[p]] = 1;
  }

	return G;
}

void parcours_sommet(graphe G, int s, int *couleur, int *pere){
  couleur[s] = 1;
  for( int v = 0; v < G.nombre_sommet; v++ ) {
    if( ( G.matrice_adjacence[s][v] )  &&  ( !couleur[v] ) ) {
      pere[v] = s;
      parcours_sommet(G, v, couleur, pere);
    }
  }
  couleur[s] = 2;
}

graphe parcours_graphe(graphe G){
	int *couleur = calloc(G.nombre_sommet,sizeof(int));
  // 0 est blanc, 1 gris et 2 noir
	int *pere = malloc(G.nombre_sommet*sizeof(int));
  for( int i = 0; i < G.nombre_sommet; i++)
    pere[i] = -1;

  for( int s = 0; s < G.nombre_sommet; s++ ) {
    if( !couleur[s] )
      parcours_sommet(G, s, couleur, pere);
  }

	graphe res = graphe_arbre(pere, G.nombre_sommet);

	free(couleur);
	free(pere);

	return res;
}

void cycle_eulerien(graphe G){
  //on suppose le graphe connexe
}

void print_mat(int **mat, int size) {
  for( int i = 0; i < size; i++) {
    for(int j = 0; j < size; j++)
      printf("%d ", mat[i][j]);
    printf("\n");
  }
}

int main(){
	/* Tests pour vérifier si vos implémentations sont correctes*/
	srand(time(NULL));

  printf("\n\n##### Graphe G avec cycle #####\n");
  graphe G = cycle_graphe(5);
  affiche_graphe(G);
  printf("##### End graphe G #####\n");

  // ecrire_fichier_dot(G,"Cycle");

  printf("\n\n##### Graphe H complet #####\n");
	graphe H = complet_graphe(5);
	affiche_graphe(H);
  printf("##### End graphe H #####\n");

	// ecrire_fichier_dot(G,"Complet");

  printf("\n\n##### Graphe L aléatoire #####\n");
	graphe L = alea_graphe(5,0.2);
	affiche_graphe(L);
  // print_mat(L.matrice_adjacence, L.nombre_sommet);
  printf("##### End graphe L #####\n");

	// ecrire_fichier_dot(G,"Alea");

  printf("\n\n##### Graphe A arbre #####\n");
	graphe A = parcours_graphe(L);
	affiche_graphe(A);
  printf("##### End graphe A #####\n");

	// ecrire_fichier_dot(G,"Arbre");

	// cycle_eulerien(L);
	// cycle_eulerien(H);

	libere_graphe(G);
	libere_graphe(H);
	libere_graphe(L);
	libere_graphe(A);
}
