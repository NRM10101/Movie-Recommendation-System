//Contains all logic and function calls
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#include "utility_matrix.h"
#include "matrix_normalization.h"
#include "pearsons.h"
#include "predictions.h"
#include "sorting.h"

#define No_of_movies 9125
#define DATASET_RATINGS "Dataset/ratings_learn.csv"
#define DATASET_MOVIES "Dataset/movies.csv"
#define DATASET_GENRES "Dataset/movies_genres.csv"
#define TOP_SIMILAR_USERS 64

typedef struct {
	int user_index;
	double similarity;
} UserSimilarity;

static int compare_user_similarity(const void *a, const void *b){
	const UserSimilarity *lhs = (const UserSimilarity *)a;
	const UserSimilarity *rhs = (const UserSimilarity *)b;
	if(lhs->similarity < rhs->similarity){
		return 1;
	}
	if(lhs->similarity > rhs->similarity){
		return -1;
	}
	return 0;
}

static int select_top_similar_users(double *similarity, int no_of_users, int active_uid, int *similar_users, int max_neighbors){
	int i = 0;
	int no_of_susers = 0;
	UserSimilarity *pairs = (UserSimilarity *)malloc(sizeof(UserSimilarity) * no_of_users);
	if(pairs == NULL){
		return 0;
	}

	for(i = 0; i < no_of_users; i++){
		pairs[i].user_index = i;
		pairs[i].similarity = similarity[i];
	}

	qsort(pairs, no_of_users, sizeof(UserSimilarity), compare_user_similarity);
	for(i = 0; i < no_of_users && no_of_susers < max_neighbors; i++){
		if(pairs[i].user_index == active_uid - 1){
			continue;
		}
		if(pairs[i].similarity <= 0.0){
			continue;
		}
		similar_users[no_of_susers++] = pairs[i].user_index;
	}

	free(pairs);
	return no_of_susers;
}

int findusers(){
    char *line, *record;
    char tmp[1024];
    FILE *fstream = fopen(DATASET_RATINGS,"r");
    int j=0;
    int max = 0;
	if(fstream == NULL){
		return 0;
	}
    while((line=fgets(tmp,sizeof(tmp),fstream))!=NULL){
    record = strtok(line,",");
    while(record!=NULL){
        if(j==0){
            int t = atoi(record);
        if(t > max) max = t;
        }
        j++;
        record = strtok(NULL,","); //iterate
    }
    j=0;
    }
    fclose(fstream);
    return max;
}

void recommender(int uid){
    int No_of_users = findusers();
	int i=0;
	double time_taken;
	int userid = uid;
	clock_t t = clock();
	clock_t stage_start;
	double load_time = 0.0, normalize_time = 0.0, similarity_time = 0.0, predict_time = 0.0, sort_time = 0.0;
	double *utility_matrix = (double *)calloc((size_t)No_of_users * No_of_movies, sizeof(double));
	char *movienames = (char *)malloc(sizeof(char) * No_of_movies * 1024);
	char *moviegenres = (char *)malloc(sizeof(char) * No_of_movies * 1024);
	if(No_of_users == 0 || utility_matrix == NULL || movienames == NULL || moviegenres == NULL){
		printf("Unable to allocate recommender resources.\n");
		free(utility_matrix);
		free(movienames);
		free(moviegenres);
		return;
	}

	stage_start = clock();
	get_utility_matrix(utility_matrix,DATASET_RATINGS,No_of_movies, No_of_users, userid); //getting utility matrix which will contain ratings according to user id(row) and movie id(coloumn)
	get_movie_names(movienames,DATASET_MOVIES); //getting movie names according to movie id(index)
	get_movie_genres(moviegenres,DATASET_GENRES); //getting movie genres according to movie id(index)
	load_time = ((double)(clock() - stage_start)) / CLOCKS_PER_SEC;

	stage_start = clock();
	double *normalized_matrix = (double *)calloc((size_t)No_of_users * No_of_movies, sizeof(double));
	normalize_matrix(utility_matrix,normalized_matrix,No_of_users,No_of_movies); //normalizing the utility matrix for similarity calculations
	normalize_time = ((double)(clock() - stage_start)) / CLOCKS_PER_SEC;

	double *newuser = (double *)calloc(No_of_movies, sizeof(double)); //take ratings from user
	extract_user_movies_from_matrix(newuser, utility_matrix, userid, No_of_movies);
    double *normalizednewuser = (double *)calloc(No_of_movies, sizeof(double));
	normalize(newuser,normalizednewuser,No_of_movies); //normalizing the users ratings for similarity calculations

	stage_start = clock();
	double *similarity = (double *)calloc(No_of_users, sizeof(double));
	calc_similarity(normalizednewuser,normalized_matrix,similarity,No_of_users,No_of_movies); //calculating similarity between new user and users present in dataset
	similarity_time = ((double)(clock() - stage_start)) / CLOCKS_PER_SEC;

	int *similar_users = malloc(sizeof(int) * No_of_users);
	int no_of_susers = select_top_similar_users(similarity, No_of_users, uid, similar_users, TOP_SIMILAR_USERS);

	int *recommended_movies = malloc(sizeof(int) * No_of_movies); //array containing index of recommended movies(whose ratings were predicted)
	double *predicted_ratings = malloc(sizeof(double) * No_of_movies); //array of ratings of those recommended movies
	int no_of_recommended_movies = 0;

	stage_start = clock();
	no_of_recommended_movies = make_prediction(newuser, similar_users, no_of_susers, similarity, utility_matrix, recommended_movies, predicted_ratings,No_of_movies); //making predictions and saving them in recommended_movies and predicted_ratings arrays
	predict_time = ((double)(clock() - stage_start)) / CLOCKS_PER_SEC;

	stage_start = clock();
	sort(recommended_movies,predicted_ratings,no_of_recommended_movies); //sorting the recommended movies in decreasing order according to their predicted ratings
	sort_time = ((double)(clock() - stage_start)) / CLOCKS_PER_SEC;

#ifndef BENCHMARK
	for(i=0;i<no_of_recommended_movies;i++){ //printing the recommended movies to get an idea (movies whose ratings was calculated)
		printf("Rating for movie %d: %.1lf\n",recommended_movies[i]+1,predicted_ratings[i]);
	}
	printf("Top 10 movies recommended for you: \n");

	for(i=0;i<10;i++){ //selecting top 10 movies from the recommended movies
		if(i==no_of_recommended_movies)  // if number of recommended movies is less than 10 then we will have to end loop early
		{
		    printf("\nSorry these are the only movies that can be recommended based on input.\nPlease enter more input.\n\n");
		    break;
		}
		printf("%d. %s %s",i+1,&movienames[recommended_movies[i] * 1024], &moviegenres[recommended_movies[i] * 1024]);
	}
#endif
	t = clock() - t;
    time_taken = ((double) t)/CLOCKS_PER_SEC;
	printf("Stage timings (seconds): load=%.3lf normalize=%.3lf similarity=%.3lf predict=%.3lf sort=%.3lf\n",
		load_time, normalize_time, similarity_time, predict_time, sort_time);
    printf("\nTime taken to process: %.2lf seconds\n",time_taken);

	//freeing the memory

	free(utility_matrix);
	free(movienames);
	free(moviegenres);
	free(normalized_matrix);
	free(newuser);
	free(normalizednewuser);
	free(similarity);
	free(similar_users);
	free(recommended_movies);
	free(predicted_ratings);

	//THE END
}
