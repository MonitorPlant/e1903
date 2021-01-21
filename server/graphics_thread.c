#include <windows.h>

#include "../common/debug.h"
#include "../common/object.h"
#include "graphics_thread.h"

extern CommonObject map_source;
extern CommonObject self, enemy;
extern CommonObject target[ MAX_TARGET_NUM ];
extern CommonObject bullet[ MAX_BULLET_NUM ];
extern MouseState self_mouse, enemy_mouse;

extern BOOL end_program;
extern BOOL game_end;
extern char point_self, point_enemy;
extern char remain_time;

extern const int char_loading0[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], char_loading1[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], char_loading2[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], char_loading3[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ];
extern const int char_target1[5][10], char_target2[5][10], char_target3[3][6], char_target4[2][4];
extern const int char_bullet_self[2][8], char_bullet_enemy[2][8];
extern const int char_self[15][30], char_self_shield[15][30], char_enemy[15][30], char_enemy_shield[15][30];

CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hCon1, hCon2;

char* msg1 = " ";
char* msg1_2 = "  ";
char* msg2 = "\n";

DWORD cbWriten;

int write_end = 0;
int buffer_1or2 = 1;

DWORD WINAPI GraphicsThread( LPVOID arg )
{
    SMALL_RECT rc = { 0, 0, DISPLAY_MAX_CHAR_X, DISPLAY_MAX_CHAR_Y };
    COORD coord;
    int i, j;
    
    coord.X = rc.Right + 1;
	coord.Y = rc.Bottom + 1;
	SetConsoleScreenBufferSize(hCon1, coord);
	SetConsoleScreenBufferSize(hCon2, coord);
	SetConsoleWindowInfo(hCon1, TRUE, &rc);
	SetConsoleWindowInfo(hCon2, TRUE, &rc);

    AllocConsole();
	SetConsoleTitle(TEXT("STAR SHOOT"));

    hCon1 = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);

	hCon2 = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);

    while( end_program == FALSE )
    {
        int map[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ] = {0};
        //背景を設定
        if( FALSE || map_source.isExist  )
        {
            for( i = 0; i < DISPLAY_MAX_CHAR_Y; i++ )
            {
                for( j = 0; j < DISPLAY_MAX_CHAR_X; j++ )
                {
                    switch( map_source.type )
                    {
                        case TYPE_LOADING0: map[ i ][ j ] = char_loading0[ i ][ j ]; break;
                        case TYPE_LOADING1: map[ i ][ j ] = char_loading1[ i ][ j ]; break;
                        case TYPE_LOADING2: map[ i ][ j ] = char_loading2[ i ][ j ]; break;
                        case TYPE_LOADING3: map[ i ][ j ] = char_loading3[ i ][ j ]; break;
                    }
                }
            }
        }
        for( i = 0; i < DISPLAY_MAX_CHAR_Y; i++ )
        {
            for( j = 0; j < DISPLAY_MAX_CHAR_X; j++ )
            {
                map[ i ][ j ] = char_loading0[ i ][ j ];
            }
        }

        //画面を出力
        displayUpdate( map );
    }

    ( void )arg;
    return 0;
}

//引数として渡されたオブジェクトをint型の配列に数字として書き込む関数
void writeObject( int map[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], CommonObject object )
{
    unsigned int i, j;
    for( i = 0; i < object.size_y; i++ )
    {
        for( j = 0; j < object.size_x; j++ )
        {
            //map[ i + object.y ][ j + object.x ] = object.design[ i ][ j ];
        }
    }
    ( void )map;
}


void displayUpdate( int map[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ] )
{
    int i, j, l;
    int dont_write = 0;
    int count_same = 0;

    if (buffer_1or2 == 1 & write_end == 0) {

		for (i = 0; i < 67; i++) {

			for (j = 1; j < 235; j++) {
				if (map[i][j] == map[i][j + 1]) {
					count_same++;
				}
				if (count_same == 234) {
					setCursorPos_1(j, i + 1);
					dont_write = 1;
			    }
		    }
            for (j = 0; j < 236; j++) {
            
                dont_write = 0;
                if (j != 0 & map[i][j] != map[i][j - 1]) {
                    switch (map[i][j]) {
                        case 0:
                            if (map[i][j - 1] != 0) {
                                SetConsoleTextAttribute(hCon1, NULL);
                            }
                            else {
                                setCursorPos_1(j+1, i);
                                dont_write = 1;
                            }
                            break;
                        case 0x01: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE); break;
                        case 0x02: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN); break;
                        case 0x03: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_BLUE); break;
                        case 0x04: SetConsoleTextAttribute(hCon1, BACKGROUND_RED); break;
                        case 0x05: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE | BACKGROUND_RED); break;
                        case 0x06: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED); break;
                        case 0x07: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE); break;
                        case 0x08: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
                        case 0x09: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
                        case 0x0A: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_INTENSITY); break;
                        case 0x0B: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
                        case 0x0C: SetConsoleTextAttribute(hCon1, BACKGROUND_RED | BACKGROUND_INTENSITY); break;
                        case 0x0D: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY); break;
                        case 0x0E: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY); break;
                        case 0x0F: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY);break;
                    }
                }
                if(dont_write != 1)
                {
                    WriteConsole(hCon1, msg1, lstrlen(msg1), &cbWriten, NULL);
                }
            }
        setCursorPos_1(0, i + 1);
		}
		SetConsoleActiveScreenBuffer(hCon1);

		buffer_1or2 = 2;
		write_end = 1;
	}


	if (buffer_1or2 == 2 & write_end == 0) {

		for (i = 0; i < 67; i++) {

			for (j = 1; j < 235; j++) {
				if (map[i][j] == map[i][j + 1]) {
					count_same++;
				}
				if (count_same == 234){
					setCursorPos_2(j, i+1);
					dont_write = 1;
		    	}
		    }
        
            for (j = 0; j < 236; j++) {
                dont_write = 0;

                if (j != 0 & map[i][j] != map[i][j - 1]) {
                    switch (map[i][j]) {
                        case 0:
                            if (map[i][j - 1] != 0) {
                                SetConsoleTextAttribute(hCon2, NULL);
                            }
                            else
                            {
                                setCursorPos_2(j + 1, i);
                                dont_write = 1;
                            }
                            break;
                        case 0x01: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE); break;
                        case 0x02: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN); break;
                        case 0x03: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_BLUE); break;
                        case 0x04: SetConsoleTextAttribute(hCon1, BACKGROUND_RED); break;
                        case 0x05: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE | BACKGROUND_RED); break;
                        case 0x06: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED); break;
                        case 0x07: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE); break;
                        case 0x08: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
                        case 0x09: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
                        case 0x0A: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_INTENSITY); break;
                        case 0x0B: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY); break;
                        case 0x0C: SetConsoleTextAttribute(hCon1, BACKGROUND_RED | BACKGROUND_INTENSITY); break;
                        case 0x0D: SetConsoleTextAttribute(hCon1, BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY); break;
                        case 0x0E: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY); break;
                        case 0x0F: SetConsoleTextAttribute(hCon1, BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY);break;
                    }
                }
                if (dont_write != 1)
                    WriteConsole(hCon2, msg1, lstrlen(msg1), &cbWriten, NULL);
            }
            setCursorPos_2(0, i + 1);
		}
		SetConsoleActiveScreenBuffer(hCon2);

		buffer_1or2 = 1;
		write_end = 1;
	}
    ( void )dont_write;
}

void setCursorPos_1(int x, int y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hCon1, pos);
}

void setCursorPos_2(int x, int y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hCon2, pos);
}
