#pragma warning(disable : 4996)
#include <windows.h>
#include <mmsystem.h>;                //음악
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <process.h>
#pragma comment (lib, "winmm.lib")    //음악

#define COL				GetStdHandle(STD_OUTPUT_HANDLE)
#define BLUE			SetConsoleTextAttribute(COL, 9);
#define GREEN			SetConsoleTextAttribute(COL, 10);
#define YELLOW			SetConsoleTextAttribute(COL, 14);
#define GRAY            SetConsoleTextAttribute(COL, 7);
#define RED             SetConsoleTextAttribute(COL, 4);
#define PURPLE          SetConsoleTextAttribute(COL, 5);
#define GOLD            SetConsoleTextAttribute(COL, 6);
#define HEART           SetConsoleTextAttribute(COL, 12);
#define BOX             SetConsoleTextAttribute(COL, 11);
#define WHITE           SetConsoleTextAttribute(COL, 15);
#define TREE			SetConsoleTextAttribute(COL, 2);

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define PAUSE 32
#define ESC 27

#define MAP_X 5
#define MAP_Y 6
#define MAP_WIDTH 30
#define MAP_HEIGHT 20

#define SIZE 100
    

CRITICAL_SECTION cs1;
CRITICAL_SECTION cs2;
CRITICAL_SECTION cs3;


int x[100], y[100]; //x,y 좌표값을 저장 총 100개 
int food_x, food_y; //food의 좌표값을 저장 
int length; //몸길이를 기억 
int speed; //게임 속도 
int score; //점수 저장  --reset함수에 의해 초기화됨
int best_score; //최고 점수 저장 --reset함수에 의해 초기화 되지 않음 
int direct_key; //방향키 저장 
int key; //입력받은 키 저장 
int snake_color; // 색깔 입력받은거 저장
int item;   //아이템 생성 종류 저장
int item_flag; //아이템 생성 플래그 변수
int move_flag;  //벽에 닿이거나  자기 몸에 닿이면 on -> 목숨 하나 잃음
int heart_cnt; //목숨 저장
int food_flag; //food함수 on/off
int i_score;  //사과 먹었을 때 메시지 출력할 때 사용 100점 밑/위 확인용
int heart_dis; //목숨 잃었을 때 메시지


void SetConsoleView() //콘솔 창의 크기와 제목을 지정하는 함수
{
    system("mode con:cols=100 lines=35");
    system("title Snake game by. 7조");
}

void CursorView() //커서 숨기기
{
    CONSOLE_CURSOR_INFO cursorInfo = { 0, };
    cursorInfo.dwSize = 1; //커서 굵기 (1 ~ 100)
    cursorInfo.bVisible = FALSE; //커서 Visible TRUE(보임) FALSE(숨김)
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


void gotoxy(int x, int y, char* s) { //x값을 2x로 변경, 좌표값에 바로 문자열을 입력할 수 있도록 printf함수 삽입  
    COORD pos = { 2 * x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("%s", s);
}


struct PERSON  //회원정보 저장
{
    char id[20];
    char password[20];
};
struct SCOME  //이름과 점수 저장
{ 
    char nname[20]; 
    int sscore; 
};

int log_dis(struct PERSON* st_p); //로그인 회원가입 선택 화면
int join(struct PERSON* st_p);  //회원가입
void getid(char* in_id);    //아이디 입력
void getpass(char* inpass);  //패스워드 입력
void t2f(char* inpass1, int f); //암호화
int login(struct PERSON* st_p);  //로그인
void title(struct SCOME* s_n); //게임 시작화면
void story(void);  //스토리 출력
int help(void);  //게임 도움말 출력
int ranking(struct SCOME* s_n);  //게임 이용자들 랭킹 출력
void reset(struct SCOME* s_n); //게임을 초기화 
void game_map1(void); // 게임판 테두리를 그림 
void draw_menu(void); // 메뉴 화면 테두리를 그림
void move(struct SCOME* s_n); //뱀머리를 이동 
void pause(void); //일시정지 
void Score_N(struct SCOME* s_n); //이름, 점수 저장
void game_over(struct SCOME* s_n); //게임 오버를 확인 
int food(void); // 음식 생성  
int color1(void); //뱀 색깔 지정
int color2(void); //뱀 독사과 먹었을때 색깔 지정
int food_box(void); //아이템 먹으면 나타나는 그림
void minus_heart(struct SCOME* s_n); //목숨 줄어들면 출력, 실행
int message(void);  //아이템 먹거나 목숨 잃었을 때 나타나는 메시지
void gameover_dis(void); //다시 돌아가기 선택 화면

////////////////////////////MAIN START//////////////////////////////
int main()
{
    struct PERSON st_p[SIZE] = { 0 };
    struct SCOME s_n[SIZE] = { 0 };
    int login_s;
    SetConsoleView();
    PlaySound(TEXT("BGM1.wav"), NULL, SND_LOOP | SND_ASYNC);
    login_s = log_dis(st_p); //로그인 또는 회원가입 선택할 수 있는 화면으로 먼저 감
    CursorView();
    title(s_n);



    while (1) {
        if (kbhit()) do { key = getch(); } while (key == 224); //키 입력받음
        Sleep(speed);

        switch (key) { //입력받은 키를 파악하고 실행  
        case LEFT:
        case RIGHT:
        case UP:
        case DOWN:
            if ((direct_key == LEFT && key != RIGHT) || (direct_key == RIGHT && key != LEFT) || (direct_key == UP && key != DOWN) ||
                (direct_key == DOWN && key != UP))//180회전이동을 방지하기 위해 필요. 
                direct_key = key;
            key = 0; // 키값을 저장하는 함수를 reset 
            break;
        case PAUSE: // 스페이스바를 누르면 일시정지 
            pause();
            break;
        case ESC: //ESC키를 누르면 프로그램 종료 
            exit(0);
        }
        move(s_n);   //키 입력 받은거 함수가 받음
        HANDLE thread3 = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)food, 0, 0, NULL);// food 생성
        if (heart_cnt == 1 || heart_cnt == 2) {
            WaitForSingleObject(thread3, INFINITE);
            food_flag = 0;
        }
        HANDLE thread1 = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)food_box, 0, 0, NULL);
        WaitForSingleObject(thread1, INFINITE);
        HANDLE thread2 = _beginthreadex(NULL, 0, (_beginthreadex_proc_type)message, 0, 0, NULL);
        WaitForSingleObject(thread2, INFINITE);
    }
}

///////////////////////////MAIN END////////////////////////////////

///////로그인 회원가입////////
int join(struct PERSON* st_p)      ///*를 붙임으로서 후에 다시 이 주소로 돌아오더라도 수정 가능성을 없애고 메모리에서 데이터를 효율적으로 관리 <- 아마즘 *포인터를 붙이면 함수들끼리 교차해서 그 변수들을 쓸 수 있다 그랬어 값을 불러올 수 잇음
{                                    //문자열과 같은 데이터 다루기에 적합  -> join함수는 struct PERSON의 포인터=st_p를 받아서 해당 내용체의 값 수정 가능
    FILE* idfp;
    char line[50] = "NULL";
    int j, flag = 0;
    char in_id[20], inpass1[20], inpass2[20], ID[20] = "NULL", Password[20] = "NULL";
    system("cls");
    draw_menu();
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 5, "ID : ");
    getid(in_id);
    if (strlen(in_id) == 0) exit(1);
    idfp = fopen("sign", "a+");   //읽기, 쓰기 모드로 파일 열기, 혹시 모르니 파일이 없다면 파일 생성
    if (idfp == NULL) exit(1);
    while (!feof(idfp)) {      //파일 처음부터 끝까지 다 읽기
        fgets(line, 50, idfp); //line에 한 줄씩 입력받음
        if (feof(idfp)) break;
        sscanf(line, "%s %s ", ID, Password);
        if (strcmp(in_id, ID) == 0) {    //파일에 같은 아이디 있으면 0으로, 없으면 1로 플래그 변수 지정
            flag = 0;
            break;
        }
        else flag = 1;
    }
    if (flag == 0) {    //파일에 같은 아이디 있으면 다시 회원가입 시도
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 6, "이미 사용 중인 아이디 입니다.");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 7, "다시 입력해주세요.");
        Sleep(1000);
        join(st_p);
    }
    else {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 6, "사용 가능한 아이디입니다.");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 8, "password : ");
        getpass(inpass1);
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 9, "password 재입력: ");
        getpass(inpass2);
        if (strcmp(inpass1, inpass2) == 0) {
            t2f(inpass1, 1);           //암호화 하기
            strcpy(st_p->id, in_id);     //입력받은 아이디와 비번 구조체에 저장
            strcpy(st_p->password, inpass1);
            fprintf(idfp, "%s %s", st_p->id, st_p->password);   //파일에 아이디와 비번 저장
            fputc('\n', idfp);   //다른 데이터와 구분하기 편하도록 엔터 넣어줌
            fclose(idfp);
            printf("\n");
            for (j = 0; j < 5; j++) {
                gotoxy(MAP_X + (MAP_WIDTH / 2) - 5 + j, MAP_Y + 12, ".");
                Sleep(400);
            }
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 12, "회원가입 성공! ^^");
            Sleep(800);
            return 0;   //회원가입 성공하면 로그인 선택할 수 있게 돌아감
        }
        else {
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 10, "!비밀번호가 다릅니다! ");
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 11, "다시 회원가입을 시도하여주세요.");
            Sleep(1000);
            join(st_p);  //다시 회원가입 처음으로 돌아가기


        }
    }
}

int login(struct PERSON* st_p)
{
    FILE* idpwfp;
    int i, j, flag = 0, wrong = 0;
    char idpw_line[50] = "NULL";
    char in_id[20], inpass[20];
    char log_su[7] = { 'W', 'E', 'L', 'C', 'O', 'M', 'E'};

    for (i = 0; i < 5; i++)
    {
        system("cls");
        draw_menu();
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 5, "ID : ");
        getid(in_id);
        if (strlen(in_id) == 0) exit(1);
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 6, "PASSWORD : ");
        getpass(inpass);
        idpwfp = fopen("sign", "r"); //읽기 가능하게 r모드로 파일 엶
        if (idpwfp == NULL) exit(1);
        while (1) {      //파일 처음부터 끝까지 다 읽기
            fgets(idpw_line, 50, idpwfp); //line에 한 줄씩 입력받음
            if (feof(idpwfp)) break;
            sscanf(idpw_line, "%s %s ", st_p->id, st_p->password);   //한 줄씩 입력받은 line에 있는 아이디와 비밀번호 다른 char형 배열에 저장 
            t2f(st_p->password, -1);  //암호화 풀기
            if (strcmp(in_id, st_p->id) == 0 && strcmp(inpass, st_p->password) == 0) {     //아이디와 비밀번호 일치 => 1, X => 0으로 플래그 변수 지정
                flag = 1; break;
            }
            else flag = 0;
        }
        fclose(idpwfp);
        if (flag == 1) {
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 4, MAP_Y + 8, "로그인 성공!^^");
            for (j = 0; j < 7; j++) {
                gotoxy(MAP_X + (MAP_WIDTH / 2) - 8 + 2 * j, MAP_Y + 10, " "); printf("%c", log_su[j]); Sleep(400);
            }
            printf("  *^^*\n"); Sleep(400);
            return 1;
            break;
        }
        else {
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 9, "아이디/비밀번호가 틀렸습니다."); 
            Sleep(400);
        }
        continue;  //flag==1일떼 이미 반복문 break로 나감 -> continue는 반복문에서 건너뛰기의 역할을 하는 것이기 때문에 여기서는 굳이 필요 X 
    }
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 10, "5번 틀림! 게임이 강제 종료됩니다");
    Sleep(400);
    exit(1);
}


void getid(char* in_id)
{
    char c, * cp;  //cp: 입력받은 문자열을 저장할 포인터로, in_id의 시작 주소를 가리킵니다.
    int cc = 0;
    cp = in_id;
    while (1) {
        c = _getche();
        if (('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
            *cp++ = c; cc++;
        }
        else if (c == 13) {
            *cp = '\0';  break;
        }
        else {
            if (c == 8 && cc > 0) {  //커서가 창을 벗어날 수 없도록 함
                putchar(c);
                putchar(' ');
                putchar(c);
                cp--; cc--;
            }
        }
    }
}

void getpass(char* inpass)   //비밀번호 입력받을 때는 화면에 *출력되게 만들기(영문자, 숫자만 가능)
{
    char c, * cp;
    int cc = 0;
    cp = inpass;
    while (1) {
        c = _getch();
        if (('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
            printf("*");
            *cp++ = c; cc++;
        }
        else if (c == 13) {         
            *cp = '\0';  break;
        }
        else {
            if (c == 8 && cc > 0) {  //커서가 창을 벗어날 수 없도록 함
                putchar(c);
                putchar(' ');
                putchar(c);
                cp--; cc--;
            }
        }
    }
}

void t2f(char* inpass1, int f)   //-1, +1로 암호화해줌
{
    while (*inpass1) {
        if (f == 1) *inpass1 -= 1;
        else  *inpass1 += 1;
        inpass1++;
    }
}

int log_dis(struct PERSON* st_p)
{
    int i, joinS = 1, loginS = 0;
    char ch;
    char wc_7[32] = { 'W','e','l','c','o','m','e',' ','t','o',' ','S','n','a','k','e',' ','g','a','m','e',' ','B','y','.','G','r','o','u','p',' ','7'};
    system("cls");
    draw_menu();
    for (i = 0; i < 32; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7 + i, MAP_Y + 6, " "); 
        if (i >= 11) gotoxy(MAP_X - 5 + i, MAP_Y + 8, " ");
        printf("\b%c", wc_7[i]);
        Sleep(300);
    }
    for (i = 31; i >= 0; i--) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7 + i, MAP_Y + 6, " ");
        if (i >= 11) gotoxy(MAP_X - 5 + i, MAP_Y + 8, " ");
        Sleep(100);
    }
    CursorView();
    Sleep(800);
    
LOGIN:
    for (i = 0; i < 8; i++)
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 5 + i, "                               ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 6, "Login Or Create An Account");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 7, MAP_Y + 8, "▣ 1  로그인	▣ 2  회원가입");

    ch = getch();    //1을 입력받으면 로그인 함수로, 2를 입력받으면 회원가입 함수으로 넘어감
    if (ch == '1') loginS = login(st_p);
    else if(ch == '2')  joinS = join(st_p);
    else {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y + 10, "잘못 입력됨! [1][2] 중에서 선택하세요."); Sleep(2000); goto LOGIN;
    }
    if (joinS == 0) {
        joinS = 1;  goto LOGIN;
    }
    return loginS;
}

/////////로그인 후 게임시작 선택////////////
void title(struct SCOME* s_n)
{
    TITLE :
    system("cls");
    int h_re, r_re, g_in;

    while (kbhit()) getch(); //버퍼에 있는 키값을 버림 

    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y - 2, "                                               ____");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y - 1, "     ______      ______      ______      _____/    O＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y, " ___/ ___  ＼___/  ___ ＼___/  ___ ＼___/  ___      <");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 1, " ＼__/    ＼______/    ＼_____/    ＼_____/   ＼____/ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 3, "  ■■■   ■     ■    ■■■    ■   ■   ■■■■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 4, " ■    ■  ■■   ■   ■    ■   ■ ■     ■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 5, "   ■      ■ ■  ■  ■■■■■  ■■      ■■■■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 6, "     ■    ■  ■ ■  ■      ■  ■ ■     ■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 7, " ■    ■  ■   ■■  ■      ■  ■  ■    ■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "  ■■■   ■     ■  ■      ■  ■    ■  ■■■■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "  _____                                             ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, " / X   ＼____      ______      ______      _____    ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, " >      ___  ＼___/  ___ ＼___/  ___ ＼___/  ___＼___      ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, " ＼____/    ＼______/    ＼_____/    ＼_____/   ＼__/ ");


    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 15, "▶   1. START    ◀");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 16, "▷   2. Help     ◁");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 17, "▷   3. Ranking  ◁");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 18, "  ※ ESC : Quit");

    while (1) {
        if (kbhit()) { //키입력받음 
            key = getch();

            if (key == ESC) exit(0);// ESC키면 종료 
            else if (key == 49) {
                g_in = color1(); reset(s_n); break;//  1 입력받으면 뱀 컬러 지정 함수로
            }
            else if (key == 50) { h_re = help(); goto TITLE; break; }// 2를 입력받으면 게임 도움말 볼 수 있도록 함
            else if(key == 51) { r_re = ranking(s_n); goto TITLE; break; }    // 3을 입력받으면 이전 사용자들의 랭킹 볼 수 있도록 함
            else { gotoxy(MAP_X + (MAP_WIDTH / 2) - 10, MAP_Y + 19, "잘못 입력됨! [1][2][3] 중에서 선택하세요."); Sleep(1000); goto TITLE; }
        }

    }

}

int help(void)
{
    int i;
    system("cls");

    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y - 4, "+++++++++++++++++++++++++++++++++++   HOW TO PLAY   +++++++++++++++++++++++++++++++++++++++");

    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 20, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y - 1, "           CONTROL                                              SCORE                        ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y , "             ↑                                    사과      황금사과      랜덤박스");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 1, "           ←↓→");
    RED  gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 1, "♥");
    GOLD gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 1, "★");
    BOX gotoxy(MAP_X + (MAP_WIDTH / 2) + 19, MAP_Y + 1, "▧ ▨ ▥");
    GRAY

        gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 2, "                                                    +10        +50            ???       ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 4, "   화살표를 누르면 뱀이 방향을 바꿉니다.           음식의 모양마다 점수가 다릅니다.  ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 9, "           LIFE                                        -----------------                        ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 10, "            ___________                              |       SPACE      |                ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 11, "   ■      | 아얏!ㅠㅠ |                              ------------------                    ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 12, "   ■      | __________|                        ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 13, "   ■       V                           스페이스 바를 누르면 게임을 잠시 중지할 수 있습니다.  ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 14, "   ■ㅎㅇㅇㅇㅇ                                    ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 15, "   ■                                                     ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 17, "     뱀의 머리가 벽이나 자신의 몸에 닿으면 생명이 하나 깎입니다.   ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 15, MAP_Y + 26, "아무키나 누르시오>>>>>>>>>   ");
    for (i = MAP_Y - 3; i < MAP_Y + 20; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, i, "+");
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 28, i, "+");
    }

    while (1) {
        if (kbhit()) { //키입력받음 
                return 0;
                break;
        }
    }
}

int ranking(struct SCOME* s_n)
{
    FILE* rfp;
    int i, j, user = 0, scoretemp;
    char user_line[40] = "NULL";
    char nametemp[20] = "NULL";

    system("cls");
    rfp = fopen("scorename", "r");
    if(rfp == NULL) exit(1);
    while (1) {      //파일 처음부터 끝까지 다 읽기
        fgets(user_line, sizeof user_line, rfp); //line에 한 줄씩 입력받음
        if (feof(rfp)) break;   //feof(fp)는 파일 포인터가 끝을 가리켜도 함수는 한 번 더 실행이 되고, 거기서 에러가 나와야 true가 되어서 while을 빠져 나온다
        sscanf(user_line, "%s %d ", s_n[user].nname, &s_n[user].sscore);
        user++;
    }
    fclose(rfp);

    gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y, " 순위 |              이름 |        점수  ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y + 1, "-------------------------------------------");
    
    for (i = 0; i < user - 1; i++) {
        for (j = i + 1; j < user; j++) {
            if (s_n[i].sscore < s_n[j].sscore) {
                scoretemp = s_n[i].sscore;
                s_n[i].sscore = s_n[j].sscore;
                s_n[j].sscore = scoretemp;

                strcpy(nametemp, s_n[i].nname);
                strcpy(s_n[i].nname, s_n[j].nname);
                strcpy(s_n[j].nname, nametemp);
            }
        }
    }
    for (i = 0; i < user; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 18, MAP_Y + 2 + i, " ");
        printf("%7d|%20s|%10d\n", i + 1, s_n[i].nname, s_n[i].sscore);
        if (i == 19) break;
    }
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 2, " ------------------------ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 3, "|  1위에 도전 해봐 ~ ☆  |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 4, " --   ------------------- ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 5, "    V ");

    gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 7, " _______");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 8, "  /       ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 9, " /  0      | ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 10, " ＼__      |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 11, "     |     |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 12, "     |     |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 13, "     |     |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 14, "     |     |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 15, "     |     |                /|");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 16, "     |     ＼              / |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 17, "     ＼      ＼           /  |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 18, "       ＼      -----------   |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 19, "         ＼                  |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 11, MAP_Y + 20, "           ＼_______________/ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 10, "  >--");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 15, MAP_Y + 26, "아무키나 누르시오>>>>>>>>>   ");
    while (1) {
        if (kbhit()) { //키입력받음 
                return 0;
                break;
        }
    }
}

void story(void) {
    int i;
    system("cls");

    Sleep(500);
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 3, "                                       _____");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 4, "                                      /     ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 5, "                                     | /      |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 6, "                                     | O      |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 7, "                                      ＼___   ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "           ___                               ＼  ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "          /  o|                                )   )");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         | __/                                (   (     _____      ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "  ______/ /                                     ＼ ＼__/  __ ＼__|＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "  ＼_____/                                        ＼_____/  ＼____/ ");
    Sleep(300);
    system("cls");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 3, "                                     _____");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 4, "                                    /     ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 5, "                                   | /      |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 6, "                                   | O      |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 7, "                                    ＼___   ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "           ___                            ＼  ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "          /  o|                              ＼  ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         | __/                                ＼  ＼     ____     ___ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "  ______/ /                                     ＼  ＼__/  __＼__/  /");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "  ＼_____/                                        ＼______/  ＼____/ ");
    Sleep(300);
    system("cls");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 3, "                                   _____");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 4, "                                  /     ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 5, "                                 | /      |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 6, "                                 | O      |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 7, "                                  ＼_v_   ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "           ___                           ＼  ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "          /  o|                            )   )");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         | __/                           (   (     _____      ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "  ______/ /                                ＼ ＼__/  __ ＼_|＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "  ＼_____/                                   ＼_____/  ＼____/ ");
    Sleep(300);
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 1, "  __________________________");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 2, " |                          |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 3, " | 꼬맹이 녀석...           |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 4, " | 넌 한입거리도 안 되겠다. |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 5, " | 언제 클래? ㅋㅋㅋ        |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 6, "  -------------------------- ＼");
    Sleep(5000);
    for (i = 0; i < 2; i++) {
        system("cls");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "           ___                      ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "          / ㅠ|            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         | __/o          ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "  ______/ /   o         ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "  ＼_____/             ");
        Sleep(300);
        system("cls");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "           ___                      ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "          / ㅠ|            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         | __/o          ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "  ______/ /   o         ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "  ＼_____/    o        ");
        Sleep(300);
        system("cls");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "           ___                      ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "          / ㅠ|            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         | __/          ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "  ______/ /   o         ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "  ＼_____/    o        ");
        Sleep(300);
    }
    system("cls");
    RED gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y + 8, "♥"); GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "           ___   엇...?    ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "          /  o|            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         | __/          ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "  ______/ /            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "  ＼_____/    o        ");
    Sleep(700);
    system("cls");
    RED gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y + 8, "♥"); GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "                  ___       ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "                 /  o|            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "         __     | __/          ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "      __/  ＼__/ /            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "      ＼__/＼___/            ");
    Sleep(300);
    system("cls");
    RED gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y + 8, "♥"); GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 8, "                       ___       ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 9, "                      /  o|            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 10, "              ___    | __/          ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 11, "             /   ＼_/ /            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 12, "            /__/＼___/            ");
    Sleep(300);
    system("cls");
    RED gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y + 8, "♥"); GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 8, "           ___ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          /  o(            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 10, "         | __/          ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 11, "  ______/ /            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 12, "  ＼_____/     ");
    Sleep(700);
    system("cls");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 8, "           ___ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          /  o )            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 10, "         | __/          ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 11, "  ______/ /            ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 12, "  ＼_____/     ");
    Sleep(300);
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          /  o |            ");
    Sleep(300);
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          /  o )            ");
    Sleep(300);
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y + 8, " 헉!!");
    Sleep(1000);
    for (i = 0; i < 2; i++) {
        system("cls");
        Sleep(300);
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 8, "           ___  ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          /  >|            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 10, "         | __/          ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 11, "  ______/ /            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 12, "  ＼_____/     ");
        Sleep(300);
        system("cls");
        Sleep(300);
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 8, "           ___  ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          /  o|            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 10, "         | __/          ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 11, "  ______/ /            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 12, "  ＼_____/     ");
    }
    for (i = 0; i < 2; i++) {
        Sleep(300);
        system("cls");
        Sleep(300);
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 7, "           ____");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 8, "          /   >＼");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          |    __)");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 10, "          /  /");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 11, " ________/  /            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 12, " ＼________/     ");
        Sleep(300);
        system("cls");
        Sleep(300);
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 7, "           ____");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 8, "          /   O＼");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 9, "          |    __)");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 10, "          /  /");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 11, " ________/  /            ");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 12, " ＼________/     ");

    }
    Sleep(1000);
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + 7, "      _______________________");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + 8, "     |                       |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + 9, "     |  좋았어!!             |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + 10, "     |  나도 많이 먹어서     |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + 11, "     |  얼른 큰 뱀이 돼야지! |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + 12, "     |_______________________|");
    Sleep(4000);


}

int color1(void)   //뱀 색깔 지정
{
    int pressedKey;
    int i, j, k;
    char bam_s[11][5] = { {' ', 'Y', ' ',' '},{' '}, {' ','|',' ','|'},{' '},{' ','|',' ','|'},{' ',' ','/',' ','/'},{' ','|',' ','|'},{' '},{' ','|',' ','|'},{'/',' ','/',' '},{'|','/',' ',' '}
    };

    story();

Color:
    system("cls");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y - 2, "색깔을 선택하세요");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 15, MAP_Y - 2, "====================");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y - 2, "=====================");
    BLUE
        for (i = 0; i < 11; i++) {
            if (i == 1) gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y + 2, "('')");
            else if (i == 3 || i == 7) gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y + 1 + i, " ＼ ＼");
            else { if (i == 4 || i > 7) gotoxy(MAP_X + (MAP_WIDTH / 2) - 8, MAP_Y + 1 + i, " \b");
            else  gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y + 1 + i, " \b");
                for (j = 0; j < 5; j++) {
                    printf("%c", bam_s[i][j]);
                }
            }
        }
    GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 10, MAP_Y + 14, " [1]파랑");

    GREEN
        for (i = 0; i < 11; i++) {
            if (i == 1) gotoxy(MAP_X + (MAP_WIDTH / 2), MAP_Y + 2, "('')");
            else if (i == 3 || i == 7) gotoxy(MAP_X + (MAP_WIDTH / 2), MAP_Y + 1 + i, " ＼ ＼");
            else {
                if (i == 4 || i > 7) gotoxy(MAP_X + (MAP_WIDTH / 2) + 1, MAP_Y + 1 + i, " \b");
                else  gotoxy(MAP_X + (MAP_WIDTH / 2), MAP_Y + 1 + i, " \b");
                for (j = 0; j < 5; j++) {
                    printf("%c", bam_s[i][j]);
                }
            }
        }
    GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 1, MAP_Y + 14, " [2]초록");

    YELLOW
        for (i = 0; i < 11; i++) {
            if (i == 1) gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 2, "('')");
            else if (i == 3 || i == 7) gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 1 + i, " ＼ ＼");
            else {
                if (i == 4 || i > 7) gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + 1 + i, " \b");
                else  gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 1 + i, " \b");
                for (j = 0; j < 5; j++) {
                    printf("%c", bam_s[i][j]);
                }
            }
        }
    GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 8, MAP_Y + 14, " [3]노랑");

    for (i = 1; i < 20; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 16, MAP_Y - 2 + i, " ∥");
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 17, MAP_Y - 2 + i, "∥");
    }
    for (i = 1; i < 33; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 16 + i, MAP_Y + 18, "==");
    }
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 16 + 33, MAP_Y + 18, "=");

    pressedKey = getch(); // getch로 숫자를 입력받는다

    switch (pressedKey) { //swith로 입력받은 숫자에 해당되는 색깔 출력
    case 49: //아스키코드 49 = 1
        BLUE
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 2, MAP_Y + 21, " 파랑");
        snake_color = 9;
        break;

    case 50: //아스키코드 50 = 2
        GREEN
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 2, MAP_Y + 21, " 초록");
        snake_color = 10;
        break;

    case 51: //아스키코드 51 = 3
        YELLOW
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 2, MAP_Y + 21, " 노랑");
        snake_color = 14;
        break;
    default:
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 23, "잘못 입력됨! [1][2][3] 중 하나를 선택하세요.");
        Sleep(800);
        goto Color;

    }
    GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2), MAP_Y + 21, "뱀 멋져!\n");
    SetConsoleTextAttribute(COL, snake_color);
    for (k = 0; k < 3; k++) {
        for (i = 0; i < 11; i++)
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 9 + (pressedKey - 49) * 9, MAP_Y + 1 + i, "        ");
        Sleep(200);
        for (i = 0; i < 11; i++) {
            if (i == 1) gotoxy(MAP_X + (MAP_WIDTH / 2) - 9 + (pressedKey - 49) * 9, MAP_Y + 2, "('')");
            else if (i == 3 || i == 7) gotoxy(MAP_X + (MAP_WIDTH / 2) - 9 + (pressedKey - 49) * 9, MAP_Y + 1 + i, " ＼ ＼");
            else {
                if (i == 4 || i > 7) gotoxy(MAP_X + (MAP_WIDTH / 2) - 8 + (pressedKey - 49) * 9, MAP_Y + 1 + i, " \b");
                else  gotoxy(MAP_X + (MAP_WIDTH / 2) - 9 + (pressedKey - 49) * 9, MAP_Y + 1 + i, " \b");
                for (j = 0; j < 5; j++) {
                    printf("%c", bam_s[i][j]);
                }
            }
        }
        Sleep(200);
    }
    GRAY
        PlaySound(NULL, 0, 0);
    return 0;
}


int color2(void)
{
    int item3_color;
     
        item3_color = snake_color;
        snake_color = 0;
        Sleep(3000);
        snake_color = item3_color;

        return 1;
}

void reset(struct SCOME* s_n)
{

    FILE* rfp;
    int i, j, user = 0, scoretemp;
    char user_line[40] = "NULL";

    system("cls"); //화면을 지움 
    game_map1(); //맵 테두리를 그림
    PlaySound(TEXT("BGM2.wav"), NULL, SND_LOOP | SND_ASYNC);

    while (kbhit()) getch(); //버퍼에 있는 키값을 버림 

    direct_key = LEFT; // 방향 초기화  
    speed = 105; // 속도 초기화 
    length = 5; //뱀 길이 초기화 
    if (heart_cnt == 0) {
        score = 0; //점수 초기화 
    }


    rfp = fopen("scorename", "r");
    if (rfp == NULL) exit(1);
    while (1) {
        fgets(user_line, sizeof user_line, rfp);
        if (feof(rfp)) break;
        sscanf(user_line, "%s %d ", s_n[user].nname, &s_n[user].sscore);
        user++;
    }
    fclose(rfp);

    for (i = 0; i < user - 1; i++) {
        for (j = i + 1; j < user; j++) {
            if (s_n[i].sscore < s_n[j].sscore) {
                scoretemp = s_n[i].sscore;
                s_n[i].sscore = s_n[j].sscore;
                s_n[j].sscore = scoretemp;
            }
        }
    }
    best_score = s_n[0].sscore;
    gotoxy(MAP_X, MAP_Y + MAP_HEIGHT, " YOUR SCORE: "); //점수표시 
    printf("%3d               BEST SCORE: %3d", score, best_score);

    for (i = 0; i < length; i++) { //뱀 몸통값 입력 
        x[i] = MAP_WIDTH / 2 + i;
        y[i] = MAP_HEIGHT / 2;
        SetConsoleTextAttribute(COL, snake_color); gotoxy(MAP_X + x[i], MAP_Y + y[i], "ㅇ");
    }
    gotoxy(MAP_X + x[0], MAP_Y + y[0], "ㅎ"); //뱀 머리 그림 
    GRAY
        if (heart_cnt == 0) {
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 2, "  ■■■   ■■■■■   ■■■    ■■■    ■■■■■");
            Sleep(200);
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 3, " ■    ■      ■      ■    ■   ■   ■       ■  ");
            Sleep(200);
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 4, "   ■          ■     ■■■■■  ■■■        ■  ");
            Sleep(200);
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 5, "     ■        ■     ■      ■  ■ ■         ■  ");
            Sleep(200);
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 6, " ■    ■      ■     ■      ■  ■  ■        ■  ");
            Sleep(200);
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 7, "  ■■■       ■     ■      ■  ■    ■      ■  ");
            Sleep(100);
            for (j = 0; j < 6; j++) {
                gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 2 + j, "                                                       ");
            }
            Sleep(100);
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 2, "  ■■■   ■■■■■   ■■■    ■■■    ■■■■■");
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 3, " ■    ■      ■      ■    ■   ■   ■       ■  ");
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 4, "   ■          ■     ■■■■■  ■■■        ■  ");
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 5, "     ■        ■     ■      ■  ■ ■         ■  ");
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 6, " ■    ■      ■     ■      ■  ■  ■        ■  ");
            gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 7, "  ■■■       ■     ■      ■  ■    ■      ■  ");
            Sleep(200);
            for (j = 0; j < 6; j++) {
                gotoxy(MAP_X + 3, MAP_Y + MAP_HEIGHT + 2 + j, "                                                       ");
            }
        }
    if (food_flag == 2) {
        SetConsoleTextAttribute(COL, 8);
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 4, " --------------------- ");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 3, "|                     |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 2, "|                     |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 1, " --   ---------------- ");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT, "    V                  ");
    }
    if (food_flag == 0) food_flag = 1;
}

void draw_menu(void) { //메뉴 화면 테두리 그리는 함수 
    int i;
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 8, MAP_Y - 2, "____");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y - 1, "／ o  ＼____________________________________");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 11, MAP_Y, " >---＼________/___/___/___/___/___/___/___/___ ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 13, " | |_______________________________________________| |");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 14, " ＼___/__/___/___/___/___/___/___/___/___/___/___/___/");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 12, MAP_Y + 1, "_");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 2, " / |");
    RED gotoxy(MAP_X + (MAP_WIDTH / 2) - 12, MAP_Y, "♥");
    GRAY;

    for (i = 1; i < 7; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 2 * i, " |/|");
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + (2 * i) - 1, " | |");

    }
    for (i = 2; i < 7; i++) {

        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 2 * i, " |/|");
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + (2 * i) - 1, " | |");

    }

}

void game_map1(void) { //맵 테두리 그리는 함수 (+ 맵 테두리 업그레이드도 하나 시킬 것(뱀이 움직이는 것이 어렵도록 대신 더 넓게 맵 만들기), 스테이지 2단계 만들거임)
    int i;
    
    HEART gotoxy(MAP_X + MAP_WIDTH - 5, MAP_Y - 1, " ♥ ♥ ♥");
    GRAY
    for (i = 0; i < MAP_WIDTH; i++) {
        gotoxy(MAP_X + i, MAP_Y, "■");
    }
    for (i = MAP_Y + 1; i < MAP_Y + MAP_HEIGHT - 1; i++) {
        gotoxy(MAP_X, i, "■");
        gotoxy(MAP_X + MAP_WIDTH - 1, i, "■");
    }
    for (i = 0; i < MAP_WIDTH; i++) {
        gotoxy(MAP_X + i, MAP_Y + MAP_HEIGHT - 1, "■");
    }
}



int food(void) {
    int i;
    int food_re = 0;//food가 뱀 몸통좌표에 생길 경우 on 
    int r = 0, k = 0; //난수 생성에 사용되는 변수
    int dis_time = 0;

    if (food_flag == 1) {
        while (1) {
            food_flag = 0;
            food_re = 0;
            item_flag = 0;
            dis_time = 0;
            srand((unsigned)time(NULL) + r); //난수표생성 
            food_x = (rand() % (MAP_WIDTH - 2)) + 1;    //난수를 좌표값에 넣음, 맵 테두리에 좌표 생기지 않게 + 1 만 해줌
            food_y = (rand() % (MAP_HEIGHT - 2)) + 1;

            for (i = 0; i < length; i++) { //food가 뱀 몸통과 겹치는지 확인  
                if (food_x == x[i] && food_y == y[i]) {
                    food_re = 1; //겹치면 다시 시작
                    r++;         //다시 시작하면 while문 첫번째로 가 실행하는 것이기 때문에 시간차를 위해 r 넣어줌
                    continue;
                }
            }
            if (food_re == 1) continue; //겹쳤을 경우 while문을 다시 시작 
            //안겹쳤을 경우 좌표값에 food를 출력
            else {
                if (food_x % 5 == 0 || food_x % 5 == 1 || food_x % 5 == 2)
                {
                    item = 1;
                    RED gotoxy(MAP_X + food_x, MAP_Y + food_y, "♥");
                    dis_time = 8000;
                }
                if (food_x % 5 == 3) {
                    if (food_y % 3 == 0)
                    {
                        item = 3;
                        BOX gotoxy(MAP_X + food_x, MAP_Y + food_y, "▧");
                    }
                    if (food_y % 3 == 1)
                    {
                        item = 4;
                        BOX gotoxy(MAP_X + food_x, MAP_Y + food_y, "▨");
                    }
                    if (food_y % 3 == 2) {
                        item = 5;
                        BOX gotoxy(MAP_X + food_x, MAP_Y + food_y, "▥");
                    }
                    dis_time = 5000;
                }
                if (food_x % 5 == 4) {
                    item = 2;
                    GOLD gotoxy(MAP_X + food_x, MAP_Y + food_y, "★");
                    dis_time = 3000;
                }
                Sleep(dis_time);  GRAY
                    if (food_flag > 1)   return 1;
                gotoxy(MAP_X + food_x, MAP_Y + food_y, "  ");
                item_flag = 0; food_flag = 1; k++;
            }
        }
    }
    else   return 1;
}


int message(void)
{
  
    if (item_flag == 3) {
        SetConsoleTextAttribute(COL, 8);
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 4, " --------------------- ");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 3, "|                     |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 2, "|                     |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT - 1, " --   ---------------- ");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + MAP_HEIGHT, "    V                  ");
        WHITE
            if (item == 1) {
                gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 3, "얌! 얌!");
                gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + MAP_HEIGHT - 2, "맛있다! :P");
            }
        if (item == 2) {
            gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 3, "이건...");
            gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + MAP_HEIGHT - 2, "황금 사과?! $ 0 $");
        }
        if (item == 3) {
            gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + MAP_HEIGHT - 3, "허걱!");
            gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 2, "독사과야! @ㅁ@");
        }
        if (item == 4) {
            gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + MAP_HEIGHT - 3, "호랑이 기운이");
            gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 2, "쑥! 쑥! ★");
        }
        if (item == 5) {
            if (i_score >= 100) {
                gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 3, "헉!");
                gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + MAP_HEIGHT - 2, "갑자기 작아졌어 ㅜ");
            }
            if (i_score < 100) {
                gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 3, "어머나");
                gotoxy(MAP_X + MAP_WIDTH + 2, MAP_Y + MAP_HEIGHT - 2, "갑자기 엄청 커졌어:D");
            }
        }
    }
    return 1;
}

int food_box(void)
{
    int ritem_color = 0;

    if (item_flag == 3) {
        if (item == 1 || item == 2) WHITE
        if (item == 3 || item == 4 || item == 5) BOX
            gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + 1, " ---------------- ");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + 2, "|                |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + 3, "|                |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + 4, "|                |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + 5, "|                |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + 6, "|                |");
        gotoxy(MAP_X + MAP_WIDTH + 1, MAP_Y + 7, " ---------------- ");
        if (item == 1) ritem_color = 4;
        if (item == 2) ritem_color = 6;
        if (item == 3) ritem_color = 5;
        if (item == 4) {
            ritem_color = 14;
        }
        
        if (item == 5) {
            ritem_color = 3;
        }
        SetConsoleTextAttribute(COL, ritem_color);
        gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + 2, " ■■  ■■ ");
        gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + 3, "■■■■■■");
        gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + 4, "  ■■■■  ");
        gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + 5, "   ■■■   ");
        gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + 6, "     ■     ");
        GRAY

            return 1;
    }
}

void minus_heart(struct SCOME* s_n)
{
    if (heart_cnt == 1) {
        food_flag = 2;
        reset(s_n);
        HEART gotoxy(MAP_X + MAP_WIDTH - 5, MAP_Y - 1, " ♡"); 
        WHITE
        gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 3, "아얏! > - <");
        gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + MAP_HEIGHT - 2, "다시 해보자 *'^'*");
    }
    if (heart_cnt == 2) {
        food_flag = 2;
        reset(s_n);
        HEART gotoxy(MAP_X + MAP_WIDTH - 5, MAP_Y - 1, " ♡ ♡"); 
        WHITE
        gotoxy(MAP_X + MAP_WIDTH + 4, MAP_Y + MAP_HEIGHT - 3, "아얏! > - ㅜ");
        gotoxy(MAP_X + MAP_WIDTH + 3, MAP_Y + MAP_HEIGHT - 2, "더 먹을래 #'^'#");
    }

    if (heart_cnt == 3) {
        HEART gotoxy(MAP_X + MAP_WIDTH - 5, MAP_Y - 1, " ♡ ♡ ♡");
            food_flag = 3;  //food함수 off
        game_over(s_n);
    }
    return;
}

void move(struct SCOME* s_n) {
    int i;
    move_flag = 0;

    if (x[0] == food_x && y[0] == food_y) { //food와 충돌했을 경우 
        item_flag = 3;
        if (item == 1) {
            score += 10; //점수 증가
            speed -= 2; //속도 증가 (Sleep으로 해서 100에서 -될 수록 시간이 짧아져서 속도 빨라지는 것)
            length++; //길이증가 
            x[length - 1] = x[length - 2]; //새로만든 몸통에 값 입력 
            y[length - 1] = y[length - 2];
        }
        if (item == 2) {
            score += 50; //점수 50점! 
        }
        if (item == 3) {
            _beginthreadex(NULL, 0, (_beginthreadex_proc_type)color2, 0, 0, NULL); //독사과 먹으면 동시 실행으로 뱀 색 바꿔줌
        }
        if (item == 4) {
            score *= 2;  //점수 2배
        }
        if (item == 5) {  //점수 100점으로 만들기 -> 좋을 수도 있고 나쁠 수도 있는 아이템
            i_score = score;
            score = 100;
        }
        GRAY
            gotoxy(MAP_X, MAP_Y + MAP_HEIGHT, " YOUR SCORE: "); //점수표시 
        printf("%3d               BEST SCORE: %3d", score, best_score);
    }

    if (x[0] == 0 || x[0] == MAP_WIDTH - 1 || y[0] == 0 || y[0] == MAP_HEIGHT - 1) { //벽과 충돌했을 경우 
        heart_cnt++;
        minus_heart(s_n); return;
    }
        for (i = 1; i < length; i++) { //자기몸과 충돌했는지 검사 
            if (x[0] == x[i] && y[0] == y[i]) {
                heart_cnt++;
                minus_heart(s_n); return;
            }
        }
    if (heart_cnt == 3) return;

    gotoxy(MAP_X + x[length - 1], MAP_Y + y[length - 1], "  "); //몸통 마지막을 지움 
    for (i = length - 1; i > 0; i--) { //몸통좌표를 한칸씩 옮김 
        x[i] = x[i - 1];
        y[i] = y[i - 1];
    }
    SetConsoleTextAttribute(COL, snake_color);
    gotoxy(MAP_X + x[0], MAP_Y + y[0], "ㅇ"); //머리가 있던곳을 몸통으로 고침 
    if (direct_key == LEFT) --x[0]; //방향에 따라 새로운 머리좌표(x[0],y[0])값을 변경 
    if (direct_key == RIGHT) ++x[0];
    if (direct_key == UP) --y[0];
    if (direct_key == DOWN) ++y[0];
    gotoxy(MAP_X + x[i], MAP_Y + y[i], "ㅎ"); //새로운 머리좌표값에 머리를 그림 
    GRAY

}


void pause(void) { // 스페이스바 눌렀을 경우 게임을 일시 정지 
    while (1) {
        if (key == PAUSE) {
            GRAY
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y, "< 다시 시작하려면 아무키나 누르시오. > ");
            Sleep(400);
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y, "                                    ");
            Sleep(400);
        }
        else {
            game_map1(); //다른 키 누르면 다시 실행
            return;
        }
        if (kbhit()) {

            do {
                key = getch();
            } while (key == 224);
        }

    }
}

void Score_N(struct SCOME* s_n)  //게임오버 -> 이름 입력 받고 구조체에 점수와 함께 저장 -> 파일에 저장
{
    PlaySound(NULL, 0, 0);
    int i;
    system("cls");
    PlaySound(TEXT("BGM3.wav"), NULL, SND_LOOP | SND_ASYNC);
    GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y + 3, "  ■■■    ■■■   ■      ■  ■■■■     ■■■  ■        ■ ■■■■ ■■■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y + 4, " ■    ■  ■    ■  ■      ■  ■          ■    ■  ■      ■  ■       ■   ■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y + 5, " ■       ■■■■■ ■■  ■■  ■■■■    ■    ■   ■    ■   ■■■■ ■■■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y + 6, " ■  ■■ ■      ■ ■ ■■ ■  ■          ■    ■    ■  ■    ■       ■ ■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y + 7, " ■    ■ ■      ■ ■ ■■ ■  ■          ■    ■     ■■     ■       ■  ■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 17, MAP_Y + 8, "  ■■■  ■      ■ ■  ■  ■  ■■■■     ■■■       ■      ■■■■ ■   ■");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 10, " ---");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 11, "／ㅠ ＼__________________________");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y + 12, "   >-＼_____/___/___/___/___/___/___＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 14, " YOUR SCORE : ");
    printf("%d", score);

    if (score > best_score) {
        best_score = score;
        HEART
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 11, "  △");
        GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 9, MAP_Y + 11, "／< ＼__________________________");
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 7, MAP_Y + 12, "   >-＼_____/___/___/___/___/___/___＼");

        for (i = 0; i < 3; i++) {
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 4, MAP_Y + 1, "☆ BEST SCORE ☆");
            Sleep(100);
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 4, MAP_Y + 1, "                 ");
            Sleep(100);
            gotoxy(MAP_X + (MAP_WIDTH / 2) - 4, MAP_Y + 1, "☆ BEST SCORE ☆");
        }
    }
    while (kbhit()) getch(); //버퍼 비우기
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 16, "이름을 입력하세요. : ");
    gets_s(s_n->nname, sizeof(s_n->nname));  //구조체에 입력받음
    if (strlen(s_n->nname) == 0) exit(1);
    FILE* scorename;
    scorename = fopen("scorename", "a+");
    if (scorename == NULL) exit(1);
    s_n->sscore = score;
    fprintf(scorename, "%s %d", s_n->nname, s_n->sscore); //파일에 구조체 이용하여 이름과 점수 저장 
    fputc('\n', scorename);   //다른 데이터와 구분하기 쉽게 엔터 넣어줌
    fclose(scorename);

    gotoxy(MAP_X + (MAP_WIDTH / 2) - 14, MAP_Y + 18, " ");
    printf("%s님의 점수 [%d] 가 저장되었습니다! 랭킹은 홈화면에서 확인 가능합니다.", s_n->nname, s_n->sscore);
    Sleep(2000);
}

void game_over(struct SCOME* s_n)  //게임종료 함수 
{
    Score_N(s_n);

    OVER:
    system("cls");
    gameover_dis();

     Sleep(500);


    while (kbhit()) getch();
    key = getch();
    if (key == 49) {         //1 누르면 게임 재시작하게 만들고 2 누르면 홈으로 돌아가게 함
        heart_cnt = 0;       //목숨 초기화
        food_flag = 0;
        PlaySound(NULL, 0, 0);
        reset(s_n);          //원래는 y/n로 입력받으려 했으나 이름을 한글로 입력할 시 후에 한영 변환 키를 누른 후 y/n 키를 눌려야하는 번거러움이 생겨 1/2로 변경
        return;
    }
    else if (key == 50) {
        heart_cnt = 0;       //목숨 초기화
        food_flag = 0;
        PlaySound(NULL, 0, 0);
        PlaySound(TEXT("BGM1.wav"), NULL, SND_LOOP | SND_ASYNC);
        title(s_n);
        return;
    }
    else {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 13, " ! [1][2] 중에서 선택하세요 !");
        Sleep(1000);
        goto OVER;
    }
}


void gameover_dis(void)
{
    int i;

    WHITE
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 17, " 。");
    RED
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 3, MAP_Y + 18, "▲");
    GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 4, MAP_Y + 19, "／o ＼__________________________");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 20, "   >-＼_____/___/___/___/___/___/___＼");
    GOLD
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 10, MAP_Y + 16, " ▶◀");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y + 19, "┃");
    RED
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 11, MAP_Y + 17, "┏ ━ ━ ━ ┓");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 11, MAP_Y + 18, "┃       ┃");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 11, MAP_Y + 19, "┃       ┃");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 11, MAP_Y + 20, "┃       ┃");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 11, MAP_Y + 21, "┗ ━ ━ ━ ┛");

    GOLD
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y + 18, "┃ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 10, MAP_Y + 19, "━ ╋ ━ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 9, MAP_Y + 20, "┃ ");
    GRAY
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 1, "xxxxxxxxxxxxxxxxxxxxxxxxxx♥ MERRY CHRISTMAS ♥xxxxxxxxxxxxxxxxxxxxxxxx");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 5, " _______________________________ ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 9, " 게임재시작 하려면 [1]눌러! ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 5, MAP_Y + 11, " 홈으로 돌아가려면 [2]눌러! ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + 14, " _____  _________________________ ");
    for (i = 1; i < 10; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 6, MAP_Y + i + 5, "|");
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 10, MAP_Y + i + 5, "|");
    }
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 4, MAP_Y + 15, "   V");
    for (i = 1; i < 22; i++) {
        gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + i, "x");
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 22, MAP_Y + i, "x");
    }
    gotoxy(MAP_X + (MAP_WIDTH / 2) - 13, MAP_Y + 22, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    TREE
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 10, " / ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 11, "/    ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 12, "/    ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 13, MAP_Y + 13, " /       ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 14, "/     ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 13, MAP_Y + 15, " /        ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 16, "  /           ＼");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 12, MAP_Y + 17, " ---------------");
    RED
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 15, MAP_Y + 12, "＊");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 16, MAP_Y + 14, "*");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 16, "*");

    YELLOW
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 9, " ☆");
    GOLD
        gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 18, " |    |  ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 19, " |    |  ");
    gotoxy(MAP_X + (MAP_WIDTH / 2) + 14, MAP_Y + 20, " |____|  ");
    GRAY
}
