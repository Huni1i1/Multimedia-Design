#pragma warning(disable:4996)

#define ROW 512
#define COL 512
#define TRAINING 19
#define K 4
#define N 32
#define FILE_NAME "clustering_K4_N32"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef struct Vector {
    unsigned char V[K];
    int cluster;
    double minDistance;
}vector;

int main();
unsigned char* read_file(char* mode, int num);
int write_file(double** X);
void training();
void slice_image(unsigned char* img, vector** v, int i, int j, int new_ROW, int new_COL);
double calc_distance(vector** v, double** X, int cluster_num, int i, int j);
void kMeansClustering(int* T, vector** v, double** X, int n);
double diff_X(double** X, double** X_tmp, int n);
int test();

int main() {
   /* int mode;
    printf("1. Training Mode\n2. Test Mode\n원하는 mode의 번호 입력: ");
    scanf("%d", &mode);

    if (mode == 1)*/
        training();
    /*else if (mode == 2)
        test();
    else
        return 0;*/
}

unsigned char* read_file(char* mode, int num) {
    char file_name[100] = { '\0' };
    if (mode == "training") {
        sprintf(&file_name, "..\\training_directory\\I%d.img", num);
    }
    else if (mode == "test") {
        sprintf(&file_name, "..\\training_directory\\lena.img");
    }
    else
        return NULL;

    FILE* input = NULL;
    input = fopen(file_name, "rb");
    if (input == NULL) {
        printf("\n파일 읽기 오류: %s", file_name);
        return NULL;
    }

    unsigned char* img;
    img = (unsigned char*)calloc(ROW * COL, sizeof(unsigned char));

    fread(img, sizeof(unsigned char), ROW * COL, input);
    fclose(input);

    return img;
}

int write_file(double** X) {
    char file_name[100] = { '\0' };
    sprintf(file_name, "clustering_K%d_N%d", K, N);

    FILE* output = fopen(file_name, "wb");
    if (output == NULL) {
        printf("파일 쓰기 오류: %s\n", file_name);
        return -1;
    }

    for (int i = 0; i < N; i++) {
        fwrite(X[i], sizeof(double), K, output);
    }

    fclose(output);

    return 0;
}

void training() {
    // Memory allocation
    int* T = (int*)calloc(N, sizeof(int));

    vector** v = (vector**)calloc(TRAINING, sizeof(vector*));
    for (int i = 0; i < TRAINING; i++) {
        v[i] = (vector*)calloc(ROW * COL / K, sizeof(vector));
    }

    double** X = (double**)calloc(N, sizeof(double*));
    for (int i = 0; i < N; i++) {
        X[i] = (double*)calloc(K, sizeof(double));
    }

    double** X_tmp = (double**)calloc(N, sizeof(double*));
    for (int i = 0; i < N; i++) {
        X_tmp[i] = (double*)calloc(K, sizeof(double));
    }

    // Number of repetitions caused by slicing image
    unsigned char* img;
    int new_ROW = ROW / sqrt(K);
    int new_COL = COL / sqrt(K);

    // Save V
    for (int i = 0; i < TRAINING; i++) {
        img = read_file("training", i);
        for (int j = 0; j < ROW * COL / K; j++) {
            slice_image(img, v, i, j, new_ROW, new_COL);
        }

    }
    free(img);

    // Initialization
    double sum;
    for (int k = 0; k < K; k++) {
        sum = 0;
        for (int i = 0; i < TRAINING; i++) {
                sum += v[i][j].V[k];
                v[i][j].minDistance = calc_distance(v, X, 0, i, j);
                v[i][j].cluster = 0;
            }
        }
        X[0][k] = sum / (TRAINING * ROW * COL / K);
    }

    srand(time(NULL));
    for (int j = 0; j < K; j++) {
        X[1][j] = X[0][j] - (rand() % 64);
        X[0][j] = X[0][j] + (rand() % 64);
    }

    // k-Means Clustering
    printf("\nStart k-Means Clustering");
    for (int n = 2; n <= N; n = 2 * n) {
        printf("\nN = %d", n);
        while (diff_X(X, X_tmp, n) > 0.000001 * n * K) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < K; j++) {
                    X_tmp[i][j] = X[i][j];
                }
            }
            kMeansClustering(T, v, X, n);
        }

        if (n < N) {
            srand(time(NULL));
            for (int i = n - 1; i >= 0; i--) {
                for (int j = 0; j < K; j++) {
                    X[2 * i + 1][j] = X[i][j] - (rand() % 64);
                    X[2 * i][j] = X[i][j] + (rand() % 64);
                }
            }
        }
    }

    write_file(X);

    // Memory deallocation
    free(T);

    for (int j = 0; j < TRAINING; j++) {
        free(v[j]);
    }
    free(v);

    for (int i = 0; i < N; i++) {
        free(X[i]);
    }
    free(X);

    for (int i = 0; i < N; i++) {
        free(X_tmp[i]);
    }
    free(X_tmp);
}

void slice_image(unsigned char* img, vector** v, int i, int j, int new_ROW, int new_COL) {
    /* (int k = 0; k < K; k++) {
       v[i][j].V[k] = *(img + (j * (int)sqrt(K)) + k);
    }*/
    int k = 0;
    for (int y = 0; y < sqrt(K); y++) {
        for (int x = 0; x < sqrt(K); x++) {
            v[i][j].V[k] = *(img + y * COL + (int)(j / new_COL) * COL * (int)sqrt(K) + x + (j % new_COL) * (int)sqrt(K));
            k++;
        }
    }
}

double calc_distance(vector** v, double** X, int cluster_num, int i, int j) {
    double sum = 0;

    for (int k = 0; k < K; k++) {
        sum += (X[cluster_num][k] - v[i][j].V[k]) * (X[cluster_num][k] - v[i][j].V[k]);
    }

    return sqrt(sum);
}

void kMeansClustering(int* T, vector** v, double** X, int n) {
    // Reset T
    for (int t = 0; t < n; t++) {
        T[t] = 0;
    }

    // Get Omega
    // Save minDistance, cluster
    for (int t = 0; t < n; t++) {
        for (int i = 0; i < TRAINING; i++) {
            for (int j = 0; j < ROW * COL / K; j++) {
                if (calc_distance(v, X, t, i, j) < v[i][j].minDistance) {
                    v[i][j].minDistance = calc_distance(v, X, t, i, j);
                    v[i][j].cluster = t;
                }
            }
        }
    }

    // Get T
    for (int t = 0; t < n; t++) {
        for (int i = 0; i < TRAINING; i++) {
            for (int j = 0; j < ROW * COL / K; j++) {
                if (v[i][j].cluster == t) {
                    T[t]++;
                }
            }
        }
    }

    // Reset X
    for (int t = 0; t < n; t++) {
        for (int k = 0; k < K; k++) {
            X[t][k] = 0;
        }
    }

    // Get X
    double sum[K];
    for (int t = 0; t < n; t++) {
        for (int k = 0; k < K; k++) {
            sum[k] = 0;
        }
        for (int i = 0; i < TRAINING; i++) {
            for (int j = 0; j < ROW * COL / K; j++) {
                if (v[i][j].cluster == t) {
                    for (int k = 0; k < K; k++) {
                        sum[k] += v[i][j].V[k];
                        // printf("\nsum[%d] = %.16lf", k,sum[k]);
                    }
                }
            }
        }
        for (int k = 0; k < K; k++) {
            if (T[t] > 0) {
                X[t][k] = sum[k] / T[t];
                // printf("\nX[%d][%d] = %.16lf", t, k, X[t][k]);
            }
        }
    }

}

double diff_X(double** X, double** X_tmp, int n) {
    double sum = 0;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < K; j++) {
            sum += fabs(X[i][j] - X_tmp[i][j]);
        }
    }

    return sum;
}

int test() {
    char file_name[100] = FILE_NAME;
    char file_path[100];
    sprintf(file_path, "..\\Project5\\%s", file_name);

    FILE* input = NULL;
    input = fopen(file_name, "rb");
    if (input == NULL) {
        printf("\n파일 읽기 오류: %s", file_name);
        return -1;
    }

    double** X = (double**)calloc(N, sizeof(double*));
    for (int i = 0; i < N; i++) {
        X[i] = (double*)calloc(K, sizeof(double));
        fread(X[i], sizeof(double), K, input);
    }

    fclose(input);

    vector** v = (vector**)calloc(1, sizeof(vector*));
    v[0] = (vector*)calloc(ROW * COL / K, sizeof(vector));
    for (int j = 0; j < ROW * COL / K; j++) {
        v[0][j].minDistance = 1000000;
    }

    // Load lena.img
    unsigned char* img;
    int new_ROW = ROW / sqrt(K);
    int new_COL = COL / sqrt(K);

    img = read_file("test", 0);
    for (int j = 0; j < ROW * COL / K; j++) {
        slice_image(img, v, 0, j, new_ROW, new_COL);
    }
    free(img);

    for (int t = 0; t < N; t++) {
        for (int j = 0; j < ROW * COL / K; j++) {
            if (calc_distance(v, X, t, 0, j) < v[0][j].minDistance) {
                v[0][j].minDistance = calc_distance(v, X, t, 0, j);
            }
        }
    }

    double sum = 0;
    for (int j = 0; j < ROW * COL / K; j++) {
        sum += v[0][j].minDistance * v[0][j].minDistance;
    }

    double MSE = sum / (ROW * COL / K);

    printf("\nMSE = %.2lf", MSE);

    // Memory deallocation
    for (int i = 0; i < N; i++) {
        free(X[i]);
    }
    free(X);

    free(v);

    return 0;
}