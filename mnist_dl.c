#pragma warning(disable:4996)

#define INPUT_SIZE 784
#define OUTPUT_SIZE 10
#define STEP_SIZE 0.2

// training data 개수
#define MAX_NUM_0 5923
#define MAX_NUM_1 6742
#define MAX_NUM_2 5958
#define MAX_NUM_3 6131
#define MAX_NUM_4 5842
#define MAX_NUM_5 5421
#define MAX_NUM_6 5918
#define MAX_NUM_7 6265
#define MAX_NUM_8 5851
#define MAX_NUM_9 5949
#define MIN_TRAINING MAX_NUM_5

// test data 개수
#define TEST_0 980
#define TEST_1 1135
#define TEST_2 1032
#define TEST_3 1010
#define TEST_4 982
#define TEST_5 892
#define TEST_6 958
#define TEST_7 1028
#define TEST_8 974
#define TEST_9 1009
#define MAX_TEST TEST_1

#define PI 3.141592653589

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

double sigmoid(double x);
double normal(double stddev, double mean);
unsigned char* read_file(char* mode, int num1, int num2);
int write_file(int num_hidden_layer, int* num_node, double** node, double*** weight);
int train();
int train_online(int num_hidden_layer, int* num_node, double** node, double*** weight);
void feed_forward(int num_hidden_layer, int* num_node, double** node, double*** weight);
void back_propagate(int num_hidden_layer, int* num_node, double** node, double*** weight);
double next_node_derivative(int num_hidden_layer, int* num_node, double** node, double*** weight, int i, int j);
int test();

double sigmoid(double x) {
	return 1 / (1 + exp(-x));
}

double normal(double stddev, double mean){
	double r1, r2, z1, z2, result, dt, num;

	result = 0.0;
	dt = 1.0 / 100.0;

	for (int i = 0; i < 100; i++)
	{
		r1 = (float)rand() / (float)RAND_MAX;
		r2 = (float)rand() / (float)RAND_MAX;

		z1 = sqrt(-2 * log(r1)) * cos(2 * PI * r2);
		z2 = sqrt(-2 * log(r1)) * sin(2 * PI * r2);

		num = (mean * dt) + (stddev * z1 * sqrt(dt));
		result += num;
	}
	return result;
}

int main(void) {
	int mode;
	printf("1. Training Mode\n2. Test Mode\n원하는 mode의 번호 입력: ");
	scanf("%d", &mode);

	if (mode == 1)
		train();
	else if (mode == 2)
		test();
	else
		return 0;
}

unsigned char* read_file(char* mode, int num1, int num2) {
	char file_name[100] = { '\0' };
	sprintf(file_name, "..\\mnist_raw\\%s\\%d\\%d-%d.raw", mode, num1, num1, num2);

	FILE* input = NULL;
	input = fopen(file_name, "rb");
	if (input == NULL) {
		printf("\n파일 읽기 오류: %s", file_name);
		return NULL;
	}

	unsigned char* img;
	img = (unsigned char*)calloc(INPUT_SIZE, sizeof(unsigned char));

	fread(img, sizeof(unsigned char), INPUT_SIZE, input);
	fclose(input);

	return img;
}

int write_file(int num_hidden_layer, int* num_node, double** node, double*** weight) {
	char file_name[100] = { '\0' };
	printf("\n파일 이름 입력: ");
	scanf("%s", &file_name);

	FILE* output = fopen(file_name, "ab");
	if (output == NULL) {
		printf("\n파일 쓰기 오류: %s", file_name);
		return -1;
	}

	fwrite(&num_hidden_layer, sizeof(int), 1, output);
	fwrite(num_node, sizeof(int), num_hidden_layer + 3, output);
	for (int i = 0; i <= num_hidden_layer + 2; i++) {
		fwrite(node[i], sizeof(double), *(num_node + i), output);
	}
	for (int i = 0; i <= num_hidden_layer; i++) {
		for (int j = 0; j < *(num_node + i + 1); j++) {
			fwrite(weight[i][j], sizeof(double), *(num_node + i), output);
		}
	}
	fclose(output);

	return 0;
}

int train() {
	// hidden layer의 개수 설정
	int num_hidden_layer;
	printf("\nhidden layer 개수 입력: ");
	scanf("%d", &num_hidden_layer);

	// 각 layer의 node 개수 설정
	int* num_node = (int*)calloc(num_hidden_layer + 3, sizeof(int));
	printf("\n각 hidden layer의 node 개수 입력: ");
	for (int i = 1; i <= num_hidden_layer; i++) {
		scanf("%d", num_node + i);
	}
	*(num_node) = INPUT_SIZE;
	*(num_node + num_hidden_layer + 1) = OUTPUT_SIZE;
	*(num_node + num_hidden_layer + 2) = OUTPUT_SIZE;

	// 각 node에 메모리 할당
	double** node = (double**)calloc(num_hidden_layer + 3, sizeof(double*));
	for (int i = 0; i <= num_hidden_layer + 2; i++) {
		node[i] = (double*)calloc(*(num_node + i), sizeof(double));
	}

	srand(time(NULL));
	// h+1개의 weight가 존재 (weight_0 ~ weight_h)
	double*** weight = (double***)calloc(num_hidden_layer + 1, sizeof(double**));
	// row = *(node+i+1), column = *(node+i)의 수만큼 메모리 할당
	for (int i = 0; i <= num_hidden_layer; i++) {
		weight[i] = (double**)calloc(*(num_node + i + 1), sizeof(double*));
		for (int j = 0; j < *(num_node + i + 1); j++) {
			weight[i][j] = (double*)calloc(*(num_node + i), sizeof(double));
			// Xavier Initialization
			for (int k = 0; k < *(num_node + i); k++) {
				weight[i][j][k] = normal((double)(1 / sqrt(*(num_node + i))), 0);
				if (weight[i][j][k] == INFINITY || weight[i][j][k] == -INFINITY)
					weight[i][j][k] = 0;
			}
		}
	}

	train_online(num_hidden_layer, num_node, node, weight);

	// weight 메모리 할당 해제
	for (int i = 0; i <= num_hidden_layer; i++) {
		for (int j = 0; j < *(num_node + i + 1); j++) {
			free(weight[i][j]);
		}
	}
	for (int i = 0; i <= num_hidden_layer; i++) {
		free(weight[i]);
	}
	free(weight);

	// node 메모리 할당 해제
	for (int i = 0; i <= num_hidden_layer + 2; i++) {
		free(node[i]);
	}
	free(node);

	// num_node 메모리 할당 해제
	free(num_node);

	return 0;
}

int train_online(int num_hidden_layer, int* num_node, double** node, double*** weight) {
	unsigned char* img;
	int num_runs = 1;
	printf("\n *실행 시작*\n");

	// 실행 시간 측정 시작
	clock_t start, finish;
	double result;
	start = clock();

	for (int num2 = 0; num2 < MIN_TRAINING; num2++) {
		for (int num1 = 0; num1 <= 9; num1++) {
			if (num1 == 0 && num2 >= MAX_NUM_0)
				continue;
			else if (num1 == 1 && num2 >= MAX_NUM_1)
				continue;
			else if (num1 == 2 && num2 >= MAX_NUM_2)
				continue;
			else if (num1 == 3 && num2 >= MAX_NUM_3)
				continue;
			else if (num1 == 4 && num2 >= MAX_NUM_4)
				continue;
			else if (num1 == 5 && num2 >= MAX_NUM_5)
				continue;
			else if (num1 == 6 && num2 >= MAX_NUM_6)
				continue;
			else if (num1 == 7 && num2 >= MAX_NUM_7)
				continue;
			else if (num1 == 8 && num2 >= MAX_NUM_8)
				continue;
			else if (num1 == 9 && num2 >= MAX_NUM_9)
				continue;
			else {
				// node[0]에 raw 이미지 저장
				img = read_file("training", num1, num2);
				for (int n = 0; n < INPUT_SIZE; n++) {
					node[0][n] = (double)*(img + n) / 1000;
				}
				node[num_hidden_layer + 2][num1] = 1;

				// feed_forward, back_propagate 수행
				feed_forward(num_hidden_layer, num_node, node, weight);
				back_propagate(num_hidden_layer, num_node, node, weight);

				free(img);
				printf("\n실행 횟수: %d", num_runs);
				num_runs++;
				node[num_hidden_layer + 2][num1] = 0;
			}
		}
	}

	// 실행 시간 측정 종료
	finish = clock();
	result = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("\n*실행 완료*\n실행 시간: %.3f초\n", result);

	write_file(num_hidden_layer, num_node, node, weight);

	return 0;
}

void feed_forward(int num_hidden_layer, int* num_node, double** node, double*** weight) {
	for (int i = 0; i <= num_hidden_layer; i++) {
		for (int j = 0; j < *(num_node + i + 1); j++) {
			node[i + 1][j] = 0;
			for (int k = 0; k < *(num_node + i); k++) {
				node[i + 1][j] += node[i][k] * weight[i][j][k];
			}
			node[i + 1][j] = sigmoid(node[i + 1][j]);
		}
	}
}

void back_propagate(int num_hidden_layer, int* num_node, double** node, double*** weight) {
	for (int i = num_hidden_layer; i >= 0; i--) {
		for (int k = 0; k < *(num_node + i); k++) {
			for (int j = 0; j < *(num_node + i + 1); j++) {
				weight[i][j][k] += STEP_SIZE * node[i + 1][j] * (1 - node[i + 1][j]) * node[i][k] * next_node_derivative(num_hidden_layer, num_node, node, weight, i + 1, j);
			}
		}
	}
}

double next_node_derivative(int num_hidden_layer, int* num_node, double** node, double*** weight, int i, int j) {
	if (i == num_hidden_layer + 1) {
		return (node[i + 1][j] - node[i][j]);
	}
	else {
		double sum = 0;
		for (int next_node = 0; next_node < *(num_node + i + 1); next_node++) {
			sum += weight[i][next_node][j] * node[i + 1][next_node] * (1 - node[i + 1][next_node]) * next_node_derivative(num_hidden_layer, num_node, node, weight, i + 1, next_node);
		}
		return sum;
	}
}

int test() {
	int sum = 0;

	printf("\n파일 이름 입력: ");
	char file_name_input[100] = { '\0' };
	scanf("%s", &file_name_input);
	char file_name[100] = { '\0' };
	sprintf(file_name, "..\\Project2\\%s", file_name_input);

	FILE* input = NULL;
	input = fopen(file_name, "rb");
	if (input == NULL) {
		printf("\n파일 읽기 오류: %s", file_name);
		return -1;
	}

	// hidden layer 읽기
	int num_hidden_layer;
	fread(&num_hidden_layer, sizeof(int), 1, input);

	// num_node 읽기
	int* num_node = (int*)calloc(num_hidden_layer + 3, sizeof(int));
	fread(num_node, sizeof(int), num_hidden_layer + 3, input);

	// node 읽기
	double** node = (double**)calloc(num_hidden_layer + 3, sizeof(double*));
	for (int i = 0; i <= num_hidden_layer + 2; i++) {
		node[i] = (double*)calloc(*(num_node + i), sizeof(double));
		fread(node[i], sizeof(double), *(num_node + i), input);
	}

	// weight 읽기
	double*** weight = (double***)calloc(num_hidden_layer + 1, sizeof(double**));
	for (int i = 0; i <= num_hidden_layer; i++) {
		weight[i] = (double**)calloc(*(num_node + i + 1), sizeof(double*));
		for (int j = 0; j < *(num_node + i + 1); j++) {
			weight[i][j] = (double*)calloc(*(num_node + i), sizeof(double));
			fread(weight[i][j], sizeof(double), *(num_node + i), input);
		}
	}

	fclose(input);

	printf("\n *실행 시작*\n");

	// 실행 시간 측정 시작
	clock_t start, finish;
	double result;
	start = clock();

	int num_correct = 0;
	int num_repeat = 0;
	int n;
	unsigned char* img;

	for (int num2 = 0; num2 < MAX_TEST; num2++) {
		for (int num1 = 0; num1 <= 9; num1++) {
			if (num1 == 0 && num2 >= TEST_0)
				continue;
			else if (num1 == 1 && num2 >= TEST_1)
				continue;
			else if (num1 == 2 && num2 >= TEST_2)
				continue;
			else if (num1 == 3 && num2 >= TEST_3)
				continue;
			else if (num1 == 4 && num2 >= TEST_4)
				continue;
			else if (num1 == 5 && num2 >= TEST_5)
				continue;
			else if (num1 == 6 && num2 >= TEST_6)
				continue;
			else if (num1 == 7 && num2 >= TEST_7)
				continue;
			else if (num1 == 8 && num2 >= TEST_8)
				continue;
			else if (num1 == 9 && num2 >= TEST_9)
				continue;
			else {
				img = read_file("test", num1, num2);
				for (int n = 0; n < INPUT_SIZE; n++) {
					node[0][n] = (double)*(img + n) / 1000;
				}

				// feed_forward
				feed_forward(num_hidden_layer, num_node, node, weight);

				// Y-O 의 최솟값 찾기
				double min = 100;
				double diff = 0;
				int guess = 0;
				for (n = 0; n <= 9; n++) {
					diff = 1 - node[num_hidden_layer + 1][n];
					if (diff < min) {
						min = diff;
						guess = n;
					}
				}

				// 맞힌 횟수, 반복 횟수 저장
				if (num1 == guess) {
					num_correct++;
				}
				num_repeat++;
				printf("\n맞힌 횟수: %d 실행 횟수: %d", num_correct, num_repeat);
				free(img);
			}
		}
	}

	// 실행 시간 측정 종료
	finish = clock();
	result = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("\n *실행 완료*\n실행 시간: %.3f초\n", result);

	double accuracy;
	accuracy = (double)num_correct / (double)num_repeat;
	printf("\n정확도: %.2f%%", accuracy * 100);

	// weight 메모리 할당 해제
	for (int i = 0; i <= num_hidden_layer; i++) {
		for (int j = 0; j < *(num_node + i + 1); j++) {
			free(weight[i][j]);
		}
	}
	for (int i = 0; i <= num_hidden_layer; i++) {
		free(weight[i]);
	}
	free(weight);

	// node 메모리 할당 해제
	for (int i = 0; i <= num_hidden_layer + 2; i++) {
		free(node[i]);
	}
	free(node);

	// num_node 메모리 할당 해제
	free(num_node);

	return 0;
}