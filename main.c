#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

double** matA;
double** matB;
double** matC;
int aRow,aCol,bRow,bCol;
 struct points {
    int r;int col;
 };
 struct points point;
/******** This function reads a file into a matrix ***********/
 double** fileReader(char *file){
    char line[3000];
    FILE *fptr = fopen(file,"r");
    if(fptr == NULL){printf("ERROR");return NULL;}
    fgets(line,300,fptr);
    sscanf(line , "row=%d col=%d", &point.r,&point.col);
    double** mat;
    mat = (double**) malloc(point.r*sizeof(double*));
    int i,j;
    for(i = 0; i < point.r ; i++){mat[i] = (double*) malloc (point.col*sizeof(double));}
    for(i=0; i < point.r ; i++){
        for(j=0;j<point.col;j++){
            fscanf(fptr, "%lf" , &mat[i][j]);
        }
    }
    fclose(fptr);
    return mat;
}
/******* This function writes the result matrix 'matC' to a file ****/

void outputGenerator(char fName[100]){
    FILE *outputfptr = fopen(fName,"w");
    int i , j;
    for(i=0; i < aRow ; i++){
        for(j=0; j < bCol ; j++){
            fprintf(outputfptr,"%.3f",matC[i][j]);
        } fprintf(outputfptr,"\n");
    } fclose(outputfptr);
}

/** A method of matrix multiplication that does not use threads **/
void nonThreading(){
    int i,j,k;
	for (i=0; i<aRow;i++) //loop over matC rows
		for(j=0; j<bCol; j++){ //loop over matC columns
			matC[i][j]=0;
			for(k=0; k<bRow; k++) //loop over each element in a matrix row and in matrix b column
				matC[i][j] += matA[i][k] * matB[k][j];
			}
}
/** A method where thread computes each row in the output C matrix. (A thread per row). **/
void *rowThreading(void *arg){
    int i=*(int*)arg;
    int j=0,k=0;
    double res=0;
    for(j = 0 ; j < bCol ; j++){ //loop over marC rows
        for(k = 0;k < bRow ; k++){res+=(matA[i][k]*matB[k][j]);} //loop over matC columns
        matC[i][j] = res;
        res = 0;
    }
    pthread_exit(NULL);
}
/** A method where a thread computes each element in the output C matrix. (A thread per element) **/
void *elementThreading(void *arg){
    struct points *p=arg;
    int z;
    double res = 0;
    for(z=0;z<bRow;z++){res+=(matA[p->r][z]*matB[z][p->col]);}
    matC[p->r][p->col] = res;
    pthread_exit(0);
}
void method1Execution(){
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    nonThreading();
    gettimeofday(&stop, NULL);
    printf("Method 1: Seconds taken %lu \n",stop.tv_sec-start.tv_sec);
    printf("          Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}
void method2Execution(){
    struct timeval start,stop;
    gettimeofday(&start,NULL);
    //defining aRow threads as the number of rows in matC
    pthread_t thread[aRow];
    int tid[aRow];
    int i;
    for(i = 0; i<aRow ; i++){
        tid[i] = i;
        if(pthread_create(&thread[i],NULL,rowThreading,&tid[i])){ //create threads
            printf("ERROR! in threads!");
            free(matA);free(matB);free(matC);
            exit(1);
           }
    }
    for(i = 0 ; i < aRow ; i++){ //checking thread has terminated
        pthread_join(thread[i],NULL);
    }
    gettimeofday(&stop,NULL);
    printf("Method 2: Seconds taken %lu \n",stop.tv_sec-start.tv_sec);
    printf("          Microseconds taken : %lu number of threads is %d \n",stop.tv_usec-start.tv_usec,aRow);
}
void method3Execution(){
    //defining aRow*bCol threads as the number of elements in matC
    pthread_t threads2[aRow][bCol];
    struct points *p;
    struct timeval start,stop;
    gettimeofday(&start,NULL);
    int i,j;
    for(i=0; i<aRow ;i++){
        for(j=0; j<bCol ; j++){
            p = malloc(sizeof(struct points));
            p->r = i;
            p->col = j;
            if(pthread_create(&threads2[i][j],NULL,elementThreading,p)){ //create threads
                printf("ERROR in threads");
                free(matA);free(matB);free(matC);
                exit(1);
            }
        }
    }
    for(i=0; i<aRow ; i++){ //checking thread has terminated
        for(j=0; j<bCol;j++){
            pthread_join(threads2[i][j],NULL);
        }
    }
    gettimeofday(&stop,NULL);
    printf("Method 3: Seconds taken %lu \n",stop.tv_sec-start.tv_sec);
    printf("          Microseconds taken : %lu number of threads is %d \n",stop.tv_usec-start.tv_usec,aRow*bCol);
}
int main(int argc , char *argv[])
{
    char output1[100],output2[100],output3[100];
    if(argc==4)
    {
        matA=fileReader(argv[1]); //read a matrix and put it in aMatrix array
        aRow=point.r;
        aCol=point.col;
        matB=fileReader(argv[2]); //read b matrix and put it in bMatrix array
        bRow=point.r;
        bCol=point.col;
        strcpy(output1,argv[3]);
        strcpy(output2,argv[3]);
        strcat(output2,"_2");
        strcpy(output3,argv[3]);
        strcat(output3,"_3");
    }
    else
    {
        matA=fileReader("a.txt");
        aRow=point.r;
        aCol=point.col;
        matB=fileReader("b.txt");
        bRow=point.r;
        bCol=point.col;
        strcpy(output1,"c.out");
        strcpy(output2,"c_2.out");
        strcpy(output3,"c_3.out");
    }
    //checks the dim of matrices
    if(aCol != bRow){
        printf("EROR in matrices dimensions \n");
        free(matA);free(matB);
        return 0;
    }
    // allocate Matrix c in memory
    int i;
    matC = (double**) malloc (aRow*sizeof(double*));
    for(i = 0; i < aRow ; i++){matC[i] = (double*) malloc(bCol*sizeof(double));}
    // method 1
    method1Execution();
    outputGenerator(output1);
    //method 2
    method2Execution();
    outputGenerator(output2);
    //method 3
    method3Execution();
    outputGenerator(output3);
    free(matA);free(matB);free(matC);
    return 0;
}
