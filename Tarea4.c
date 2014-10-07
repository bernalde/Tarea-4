#include <stdio.h>
#include <stdlib.h>
#include <math.h>
float *new_matrix(int n, int m);
void trasponer(float *matrixA, float *matrixB, int n, int m);
float *load_matrix(char *filename, int *n, int *m);
void multiplicacion(float *matrixA, float *matrixB, float *matrixC, int n, int m, int p);
void cholesky(float *matrixA, float *matrixB, int n, int m);
void forward(float *matrixA, float *matrixB, float *matrixC, int n);
void backward(float *matrixA, float *matrixB, float *matrixC, int n);
float rcuad(float *matrixA, float *matrixB, int n);
float *soluc(float *matrixA, float r2, int n);
int main(int argc, char **argv){
  float *G;
  float *Gt;
  float *GtG;
  float *d;
  float *b;
  float *L;
  float *U;
  float *y;
  float *x;
  float *dcalc;
  float *resp;
  int n_row1, n_cols1, n_row;
  int n_row2, n_cols2, n_cols;
  int i, j, k;
  float r2;

  G = load_matrix(argv[1], &n_row1, &n_cols1);
  d = load_matrix(argv[2], &n_row, &n_cols);
  Gt = new_matrix(n_cols1, n_row1);

  n_row2=n_cols1;
  n_cols2=n_row1;

  trasponer(G,Gt,n_row1,n_cols1);

  GtG = new_matrix(n_cols1, n_cols1);
  multiplicacion(Gt,G,GtG,n_row2,n_cols1,n_cols2);

  b = new_matrix(n_row2, n_cols);
  multiplicacion(Gt,d,b,n_row2,n_cols,n_cols2);

  L = new_matrix(n_row2, n_cols1);
  cholesky(GtG,L,n_cols1,n_cols1);

  U = new_matrix(n_row2, n_cols1);
  trasponer(L,U,n_cols1,n_cols1);

  y = new_matrix(n_row2, n_cols);
  forward(L,b,y,n_row2);

  x = new_matrix(n_row2, n_cols);
  backward(U,y,x,n_row2);

  dcalc = load_matrix(argv[2], &n_row, &n_cols);
  multiplicacion(G,x,dcalc,n_row1,n_cols,n_cols1);  

  r2=rcuad(d,dcalc,n_row1);

  //  resp=soluc(x,r2,n_row2);


    for(i=0;i<n_row2;i++){
      for(j=0;j<n_cols;j++){
	printf("%f ", x[i*n_cols + j]);
      }
      printf("\n");
    }
    printf("%f\n", r2);

    return 0;
}




void multiplicacion(float *matrixA, float *matrixB, float *matrixC, int n, int m, int p){

  int i, j, k;
 
  for(i=0;i<n;i++){
    matrixC[i*m+j]=0.0;
    for(j=0;j<m;j++){
      for(k=0;k<p;k++){
	matrixC[i*m+j]+=matrixA[i*p+k]*matrixB[k*m+j];
      }
    }
  }
}



float *load_matrix(char *filename, int *n, int *m){
  float *matrix;
  FILE *in;
  int n_row, n_cols;
  int i;
  int j;

  if(!(in=fopen(filename, "r"))){
    printf("Problem opening file %s\n", filename);
    exit(1);
  }
  fscanf(in, "%d %d\n", &n_row, &n_cols);
  
  matrix = malloc(n_row * n_cols * sizeof(float));

  for(i=0;i<n_row;i++){
    for(j=0;j<n_cols;j++){
      fscanf(in, "%f", &matrix[i*n_cols + j]);
    }
  }    
  *n = n_row;
  *m = n_cols;
  return matrix;
}


void trasponer(float *matrixA, float *matrixB, int n, int m){

  int i, j;
 
  for(i=0;i<m;i++){
    for(j=0;j<n;j++){
      matrixB[i*n+j]=matrixA[j*m+i];
    }
  }
}


void cholesky(float *matrixA, float *matrixL, int n, int m){

  int i, j, k;
  double sumjj, sumij;
  for(j=0;j<m;j++){
    sumjj=0.0;
    for(k=0;k<(j);k++){
      sumjj+=matrixL[j*m+k]*matrixL[j*m+k];
    }
    matrixL[j*m+j]=sqrt(matrixA[j*m+j]-sumjj);
    for(i=j+1;i<m;i++){
      sumij=0.0;
      for(k=0;k<(j);k++){
	sumij=matrixL[i*m+k]*matrixL[j*m+k];
      }
      matrixL[i*m+j]=(matrixA[i*m+j]-sumij)/(matrixL[j*m+j]);
    }
  }
}

float *new_matrix(int n_row, int n_cols){
  float *matrix;
  int i;
  int j;

  matrix = malloc(n_row * n_cols * sizeof(float));

  for(i=0;i<n_row;i++){
    for(j=0;j<n_cols;j++){
      matrix[i*n_cols + j]=0.0;
    }
  }    
  return matrix;
}


void forward(float *matrixA, float *matrixB, float *matrixC, int n){

  int i, j;
  double PE;
  matrixC[0]=matrixB[0]/matrixA[0];
  for(i=0;i<n;i++){
    PE=0.0;
    for(j=0;j<i;j++){
      PE+=matrixA[i*n+j]*matrixC[j];
    }
    matrixC[i]=(matrixB[i]-PE)/matrixA[i*n+i];
  }
}

void backward(float *matrixA, float *matrixB, float *matrixC, int n){

  int i, j;
  double PE;
  matrixC[n]=matrixB[n]/matrixA[n*n];
  for(i=n-1;i>=0;i--){
    PE=0.0;
    for(j=i+1;j<n;j++){
      PE+=matrixA[i*n+j]*matrixC[j];
    }
    matrixC[i]=(matrixB[i]-PE)/matrixA[i*n+i];
  }
}

float rcuad(float *matrixA, float *matrixB, int n){
  int i;
  double sumx=0, sumy=0, sumx2=0, sumy2=0, sumxy=0;
  double r, r2;
  for(i=0;i<n;i++){
    sumx+=matrixA[i];
    sumy+=matrixB[i];
    sumxy+=matrixA[i]*matrixB[i];
    sumx2+=matrixA[i]*matrixA[i];
    sumy2+=matrixB[i]*matrixB[i];
  }
  r=(n*sumxy-sumx*sumy)/((sqrt(n*sumx2-sumx*sumx))*(sqrt(n*sumy2-sumy*sumy)));
  r2=r*r;
  return r2;
}

float *soluc(float *matrixA, float r2, int n){
  float *matrix;
  int i;

  matrix = malloc((n+1)*sizeof(float));

  for(i=0;i<n;i++){
    matrix[i]=matrixA[i];
  }
  matrix[n-1]=r2;
  return matrix;
}
