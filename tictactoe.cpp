#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define PNG_SETJMP_NOT_SUPPORTED
#include <png.h>

#define WIDTH 660
#define HEIGHT 660
#define COLOR_DEPTH 8

struct Pixel {
	png_byte r, g, b, a;
};

struct Pixel *board[HEIGHT]; 

int n;

void drawCircle(int centerX, int centerY, int radius) {
	for(int i = centerY - radius; i < centerY + radius; i++) {
		for(int j = centerX - radius; j < centerX + radius; j++) {
			int distance = static_cast<int>(sqrt( (centerY-i)*(centerY-i) + (centerX-j)*(centerX-j) ));

			if(distance < radius && distance > radius - 10) {
                board[j][i].a = 255;
				board[j][i].r = 0;
				board[j][i].g = 0;
				board[j][i].b = 255;
			}
		}
	}
}

void draw_line(int x1, int y1, int x2, int y2) {
	int x, y;
	for(int i = 0; i <= 256; i++) { 
		x = (i * (x2 - x1)) / 256;
		y = (i * (y2 - y1)) / 256;
		board[x1+x][y1+y].a = 255;
		board[x1+x][y1+y].r = 255;
		board[x1+x][y1+y].g = 0;
		board[x1+x][y1+y].b = 0;
	}
}

//입력한 위치에 o그리기
int draw_o(int a, int b) {
	//원중심 (b-1)*(W/n)+(W/n/2),(a-1)*(W/n)+(W/n/2)
	drawCircle(a*(WIDTH/n)+(WIDTH/n/2), b*(WIDTH/n)+(WIDTH/n/2), (WIDTH/n)*5/12);
	return 0;
}

//입력한 위치에 x그리기
int draw_x(int a, int b) {
	int centerX = a*(WIDTH/n)+(WIDTH/n/2);
	int centerY = b*(WIDTH/n)+(WIDTH/n/2);
	int radius = (WIDTH/n)*5/12;

	for(int i = -5; i <= 5; i++) {
		draw_line(centerX-radius+i,centerY-radius,centerX+radius+i,centerY+radius);
		draw_line(centerX-radius+i,centerY+radius,centerX+radius+i,centerY-radius);
	}

	return 0;
}


int winner_exist=0;
//승리여부
void check_winner(int **arr, string *p, int chkVal, int cnt) {
	//가로줄 확인
	int chc=0;
	for(int i=0;i<n;i++){
		chc = 0;
		for(int j=0;j<n;j++){
			if(arr[i][j]==chkVal){
				chc++;
			}
		}
		//cout << "가로줄 : " << chc << endl;
		if(chc==n){
			cout << p[arr[i][0]] << " is winner!!" << endl;
			winner_exist++;
			return ;
		}
	}
	//세로줄 확인
	int chr=0;
	for(int i=0;i<n;i++){
		chr = 0;
		for(int j=0;j<n;j++){
			if(arr[j][i]==chkVal){
				chr++;
			}
		}
		//cout << "세로줄 : " << chr << endl;
		if(chr==n){
			cout << p[arr[0][i]] << " is winner!!" << endl;
			winner_exist++;
			return ;
		}
	}
	//음대각선 확인
	int chd=0;
	for(int i=0;i<n;i++){
		if(arr[i][i]==chkVal){
			chd++;
		}
		//cout << "음대각선 : " << chd << endl;
		if(chd==n){
			cout << p[arr[0][0]] << " is winner!!" << endl;
			winner_exist++;
			return ;
		}
	}
	//양대각선 확인
	int chdd=0;
	for(int i=0;i<n;i++){
		if(arr[n-1-i][i]==chkVal){
			chdd++;
		}
		//cout << "양대각선 : " << chdd << endl;
		if(chdd==n){
			cout << p[arr[0][i]] << " is winner!!" << endl;
			winner_exist++;
			return ;
		}
	}

	//무승부
	if(cnt==n*n){
		cout << "Game finished!It's tied!!" << endl;
		winner_exist++;
		return ;
	}


}


//보드만들기
int prepare_board(int num) {

	for (int row = 0; row < HEIGHT; row++) {
		board[row] = (Pixel *)calloc(WIDTH*2, sizeof(struct Pixel)); 
	}

	//보드판 채우기
	for(int i = 0; i < WIDTH; i++) {
		for(int j = 0; j < HEIGHT; j++) {
			board[j][i].a = 255;
			board[j][i].r = 0;
			board[j][i].g = 0;
			board[j][i].b = 0;
		}
	}

	//칸 나누기(세로)
	for(int i = 1; i < num; i ++ )  {
		for(int j = 0; j < HEIGHT; j ++ ) {
			board[j][i*(WIDTH/num)].a = 255;
			board[j][i*(WIDTH/num)].r = 255;
			board[j][i*(WIDTH/num)].g = 255;
			board[j][i*(WIDTH/num)].b = 255;
		}
	}
	//칸 나누기(가로)
	for(int i = 1; i < num; i ++ )  {
		for(int j = 0; j < WIDTH; j ++ ) {
			board[i*(HEIGHT/num)][j].a = 255;
			board[i*(HEIGHT/num)][j].r = 255;
			board[i*(HEIGHT/num)][j].g = 255;
			board[i*(HEIGHT/num)][j].b = 255;
		}
	}

	return 0;
}


int make_image() {
	srand(time(NULL));

	/* open PNG file for writing */
	FILE *f = fopen("tictactoe.png", "wb");
	if (!f) {
		fprintf(stderr, "could not open out.png\n");
		return 1;
	}

	/* initialize png data structures */
	png_structp png_ptr;
	png_infop info_ptr;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fprintf(stderr, "could not initialize png struct\n");
		return 1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(f);
		return 1;
	}
	/* begin writing PNG File */
	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info_ptr, WIDTH, HEIGHT, COLOR_DEPTH,
	             PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);

	/* write image data to disk */
	png_write_image(png_ptr, (png_byte **)board);

	/* finish writing PNG file */
	png_write_end(png_ptr, NULL);

	/* clean up PNG-related data structures */
	png_destroy_write_struct(&png_ptr, &info_ptr);

	/* close the file */
	fclose(f);
	f = NULL;

	return 0;
}

string p[2];

int main(int argc, char *argv[]) {
	/*
	if(argc<2) {
		fprintf(stderr, "file_name is needed!!\n");
		return 1;
	}
*/
	
	cout << "Choose tictecto version : ";
	cin >> n;
	int** arr = new int*[n];
    for (int i = 0; i < n; ++i) {
        arr[i] = new int[n];
    }

	for(int i = 0; i < n; i++) 
		for(int j=0; j<n; j++)
			arr[i][j]= 2;

	prepare_board(n);

	make_image();

	string A, B, n1;
		cout << "1st PlayerName : ";
		cin >> A;
		cout << "2nd PlayerName : ";
		cin >> B;

		while (1){
			if ( B == A ) {
				cout << "Same with 1st Player.Choose another one." << endl << "2nd Player : ";
				cin >> B;
			}
			else
				break;
		}

		while (1) {
			cout << "WHo start? ";
			cin >> n1;
			if ( n1 != A && n1 != B ) {
				cout << "No Player.Enter 1st or 2nd player." << endl;
			}
			else {
				break;
			}
		}
	int cnt=0;
	while(1){
		cnt++;

		char OorX, PrevOorX; 
		int a,b;

		cin.ignore();
		cout << " Which Shape&Space : ";
		cin >> OorX >> a >> b;
		cin.clear();

		// 입력오류 체크
		int error_exist=0;

		if(OorX==PrevOorX || (OorX != 'o' && OorX != 'x')) {
			cout << "Wrong Player!!!!!" << endl;
			error_exist=1;
		}
		if( a > n-1 || a < 0 || b > n-1 || b < 0 ){
			cout << "Out of range!!!!!" << endl;
			error_exist=1;
		}
		else if(arr[a][b] == 0 || arr[a][b] == 1) {
			cout << "Already occupied location!!!!!" << endl;
			error_exist=1;
		}
		if(error_exist==1){
			continue;
		}

		PrevOorX = OorX;
		if(OorX=='o'){
			draw_o(a, b);
			arr[a][b]=0;
			if(cnt==1){
				p[0]=n1;
				if(n1==A)
					p[1]=B;
				else
					p[1]=A;
			}
		}
		else if(OorX=='x'){
			draw_x(a, b);
			arr[a][b]=1;
			if(cnt==1){
				p[1]=n1;
				if(n1==A)
					p[0]=B;
				else
					p[0]=A;
			}
		}

		make_image();

		//적고나면 매번 승리여부 확인
		check_winner(arr, p, arr[a][b], cnt);

		if(winner_exist==1){
			break;
		}
	}


	//적고나면 매번 승리여부 확인



	for (int row = 0; row < HEIGHT; row++) {
		free(board[row]);
	}

	return 0;
}
