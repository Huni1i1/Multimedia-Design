// 화면 돌리는 프로그램
#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 512
#define HEIGHT 512

#include <stdio.h>
#include <stdlib.h>
//calloc �Լ��� ����ϱ� ���� stdlib.h ���̺귯�� ���

int clockwise_90();
int clockwise_180();
int clockwise_270();
int flip_hor();
int flip_ver();

int main(void) {
	int command;

	printf("1. �ð���� 90��\n");
	printf("2. �ð���� 180��\n");
	printf("3. �ð���� 270��\n");
	printf("4. �¿� ��Ī\n");
	printf("5. ���� ��Ī\n");
	
	scanf("%d", &command);

	if (command == 1)
		clockwise_90();
	else if (command == 2)
		clockwise_180();
	else if (command == 3)
		clockwise_270();
	else if (command == 4)
		flip_hor();
	else if (command == 5)
		flip_ver();
	//command�� ����� ���� ���� ���� ����	

	return 0;
}

int clockwise_90(void) {
	unsigned char* img;
	img = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));
	unsigned char* img_90;
	img_90 = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));
	//������ ���� �� calloc���� ���� �Ҵ� �� �ʱ�ȭ

	FILE* input = fopen("..\\mnist_raw\\testing\\", "r");
	if (input == NULL) {
		printf("���� �б� ����");
		return -1;
	} 
	//���� ���� input FILE �����Ϳ� ����
	//���� ���� ���� �� "���� �б� ����" ���

	fread(img, sizeof(unsigned char), WIDTH * HEIGHT, input);
	fclose(input); 
	//input FILE �����͸� img�� ����

	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			*(img_90 + i * HEIGHT + j) = *(img + i + WIDTH * (HEIGHT - 1 - j));
		}
	} 
	//img�� �ð� ���� 90�� ȸ���ؼ� img_90�� ����

	FILE* output = NULL;
	output = fopen("C:\\Users\\luze8\\source\\image\\img_90.img", "w");
	fwrite(img_90, sizeof(unsigned char), WIDTH * HEIGHT, output);
	fclose(output); 
	//img_90�� output FILE �����Ϳ� ����

	free(img);
	free(img_90);
	//�޸� �Ҵ� ����

	return 0;
}

int clockwise_180(void) {
	unsigned char* img;
	img = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));
	unsigned char* img_180;
	img_180 = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));

	FILE* input = fopen("..\\mnist_raw\\testing\\", "r");
	if (input == NULL) {
		printf("File doesn't exist");
		return -1;
	}

	fread(img, sizeof(unsigned char), WIDTH * HEIGHT, input);
	fclose(input);

	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			*(img_180 + i * HEIGHT + j) = *(img + (WIDTH - i) * HEIGHT - j);
		}
	}

	FILE* output = NULL;
	output = fopen("C:\\Users\\luze8\\source\\image\\img_180.img", "w");
	fwrite(img_180, sizeof(unsigned char), WIDTH * HEIGHT, output);
	fclose(output);

	free(img);
	free(img_180);

	return 0;
}

int clockwise_270(void) {
	unsigned char* img;
	img = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));
	unsigned char* img_270;
	img_270 = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));

	FILE* input = fopen("..\\mnist_raw\\testing\\", "r");
	if (input == NULL) {
		printf("File doesn't exist");
		return -1;
	}

	fread(img, sizeof(unsigned char), WIDTH * HEIGHT, input);
	fclose(input);

	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			*(img_270 + i * HEIGHT + j) = *(img + HEIGHT - i + WIDTH * j);
		}
	}

	FILE* output = NULL;
	output = fopen("C:\\Users\\luze8\\source\\image\\img_270.img", "w");
	fwrite(img_270, sizeof(unsigned char), WIDTH * HEIGHT, output);
	fclose(output);

	free(img);
	free(img_270);

	return 0;
}

int flip_hor(void) {
	unsigned char* img;
	img = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));
	unsigned char* img_hor;
	img_hor = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));

	FILE* input = fopen("..\\mnist_raw\\testing\\", "r");
	if (input == NULL) {
		printf("File doesn't exist");
		return -1;
	}

	fread(img, sizeof(unsigned char), WIDTH * HEIGHT, input);
	fclose(input);

	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			*(img_hor + i * HEIGHT + j) = *(img + (WIDTH * (i + 1)) - j);
		}
	}

	FILE* output = NULL;
	output = fopen("C:\\Users\\luze8\\source\\image\\img_hor.img", "w");
	fwrite(img_hor, sizeof(unsigned char), WIDTH * HEIGHT, output);
	fclose(output);

	free(img);
	free(img_hor);

	return 0;
}

int flip_ver(void) {
	unsigned char* img;
	img = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));
	unsigned char* img_ver;
	img_ver = (unsigned char*)calloc(WIDTH * HEIGHT, sizeof(unsigned char));

	FILE* input = fopen("..\\mnist_raw\\testing\\", "r");
	if (input == NULL) {
		printf("File doesn't exist");
		return -1;
	}

	fread(img, sizeof(unsigned char), WIDTH * HEIGHT, input);
	fclose(input);

	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < WIDTH; j++) {
			*(img_ver + i * HEIGHT + j) = *(img + HEIGHT * WIDTH - (HEIGHT * (i + 1)) + j);
		}
	}

	FILE* output = NULL;
	output = fopen("C:\\Users\\luze8\\source\\image\\img_ver.img", "w");
	fwrite(img_ver, sizeof(unsigned char), WIDTH * HEIGHT, output);
	fclose(output);

	free(img);
	free(img_ver);

	return 0;
}