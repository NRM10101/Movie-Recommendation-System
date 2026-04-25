//contains sorting
#include<stdlib.h>

typedef struct {
	int movie;
	double rating;
} Recommendation;

static int compare_recommendations(const void *a, const void *b){
	const Recommendation *lhs = (const Recommendation *)a;
	const Recommendation *rhs = (const Recommendation *)b;
	if(lhs->rating < rhs->rating){
		return 1;
	}
	if(lhs->rating > rhs->rating){
		return -1;
	}
	return 0;
}

void sort(int *recommended_movies, double *predicted_ratings, int no_of_recommended_movies){
	int i = 0;
	Recommendation *items = (Recommendation *)malloc(sizeof(Recommendation) * no_of_recommended_movies);
	if(items == NULL){
		return;
	}

	for(i = 0; i < no_of_recommended_movies; i++){
		items[i].movie = recommended_movies[i];
		items[i].rating = predicted_ratings[i];
	}

	qsort(items, no_of_recommended_movies, sizeof(Recommendation), compare_recommendations);

	for(i = 0; i < no_of_recommended_movies; i++){
		recommended_movies[i] = items[i].movie;
		predicted_ratings[i] = items[i].rating;
	}
	free(items);
}
