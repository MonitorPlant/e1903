#include <windows.h>

#include "../common/debug.h"
#include "../common/object.h"
#include "graphics_thread.h"

extern CommonObject background;
extern CommonObject self, enemy;
extern CommonObject target[ MAX_TARGET_NUM ];
extern CommonObject bullet[ MAX_BULLET_NUM ];
extern CommonObject number[8];
extern MouseState self_mouse, enemy_mouse;

extern BOOL end_program;
extern BOOL game_end;
extern char point_self, point_enemy;
extern char remain_time;

extern const int char_loading0[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], char_loading1[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], char_loading2[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ], char_loading3[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ];
extern const int char_target1[5][10], char_target2[5][10], char_target3[3][6], char_target4[2][4];
extern const int char_bullet_self[2][8], char_bullet_enemy[2][8];
extern const int char_self[15][30], char_self_shield[15][30], char_enemy[15][30], char_enemy_shield[15][30];
extern const int char_rules[67][236], char_result_self[67][236], char_result_enemy[67][236], char_title[67][236];
extern const int char_num[10][7][7];

HANDLE hCon1, hCon2;

DWORD WINAPI GraphicsThread( LPVOID arg )
{
    int i, j, k;

    SMALL_RECT rc = { 0, 0, DISPLAY_MAX_CHAR_X, DISPLAY_MAX_CHAR_Y };
	COORD coord;
	
	AllocConsole();
	SetConsoleTitle( TEXT( "STAR WARS" ) );
	
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
	
	coord.X = rc.Right;
	coord.Y = rc.Bottom;
	
	SetConsoleScreenBufferSize(hCon1, coord);
	SetConsoleScreenBufferSize(hCon2, coord);
	
	SetConsoleWindowInfo(hCon1, TRUE, &rc);
	SetConsoleWindowInfo(hCon2, TRUE, &rc);

    while( end_program == FALSE )
    {
        int map[ DISPLAY_MAX_CHAR_Y ][ DISPLAY_MAX_CHAR_X ] = {0};

        //背景を設定
        if( background.isExist )
        {
            for( i = background.y; i < DISPLAY_MAX_CHAR_Y; i++ )
            {
                for( j = background.x; j < DISPLAY_MAX_CHAR_X; j++ )
                {
                    switch( background.type )
                    {
                        case TYPE_LOADING0: map[ i ][ j ] = char_loading0[ i ][ j ]; break;
                        case TYPE_LOADING1: map[ i ][ j ] = char_loading1[ i ][ j ]; break;
                        case TYPE_LOADING2: map[ i ][ j ] = char_loading2[ i ][ j ]; break;
                        case TYPE_LOADING3: map[ i ][ j ] = char_loading3[ i ][ j ]; break;
                        case TYPE_TITLE: map[ i ][ j ] = char_title[ i ][ j ]; break;
                        case TYPE_RULES: map[ i ][ j ] = char_rules[ i ][ j ]; break;
                        case TYPE_SELF_WIN: map[ i ][ j ] = char_result_self[ i ][ j ]; break;
                        case TYPE_ENEMY_WIN: map[ i ][ j ] = char_result_enemy[ i ][ j ]; break;
                    }
                }
            }
        }

        //数字を合成
        for( i = 0; i < 8; i++ )
        {
            if( number[ i ].isExist )
            {
                for( j = 0; j < 7; j++ )
                {
                    for( k = 0; k < 7; k++ )
                    {
                        map[ number[ i ].y + j ][ number[ i ].x + k ] = char_num[ 9 - ( CHAR_NUM(9) - number[ i ].type ) ][ j ][ k ];
                    }
                }
            }
        }

        //メインキャラを合成
        if( self.isExist )
        {
            for( i = 0; i < ( signed )self.size_y; i++ )
            {
                for( j = 0; j < ( signed )self.size_x; j++ )
                {
                    if( self_mouse.click_right )
                    {
                        map[ self.y + i ][ self.x + j ] = char_self_shield[ i ][ j ];
                    }
                    else
                    {
                        map[ self.y + i ][ self.x + j ] = char_self[ i ][ j ];
                    }
                }
            }
        }

        //敵キャラを合成
        if( enemy.isExist )
        {
            for( i = 0; i < ( signed )enemy.size_y; i++ )
            {
                for( j = 0; j < ( signed )enemy.size_x; j++ )
                {
                    if( self_mouse.click_right )
                    {
                        map[ enemy.y + i ][ enemy.x + j ] = char_enemy_shield[ i ][ j ];
                    }
                    else
                    {
                        map[ enemy.y + i ][ enemy.x + j ] = char_enemy[ i ][ j ];
                    }
                }
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

void displayUpdate(int map[DISPLAY_MAX_CHAR_Y][DISPLAY_MAX_CHAR_X] )
{
	//static int buffer_1or2 = 1;
	int i, j, k;
	CHAR_INFO ci[67][236] = {0};
	COORD buffer_size = { 236, 67 };
	COORD start_coord = { 0, 0 };
	SMALL_RECT sr = { 0, 0, 235, 66 };
	
	for (i = 0; i < 67; i++) {
		for (j = 0; j < 236; j++) {
			ci[i][j].Char.AsciiChar = ' ';
			ci[i][j].Attributes = 16 * map[i][j];
		}
	}
    WriteConsoleOutput(hCon2, (CHAR_INFO*)ci, buffer_size, start_coord, &sr);
	SetConsoleActiveScreenBuffer(hCon2);
    
    /*
	if (buffer_1or2) {
		buffer_1or2 = 0;
		
		WriteConsoleOutput(hCon1, (CHAR_INFO*)ci, buffer_size, start_coord, &sr);
		SetConsoleActiveScreenBuffer(hCon1);
	}
	else{
		buffer_1or2 = 1;
		
		WriteConsoleOutput(hCon2, (CHAR_INFO*)ci, buffer_size, start_coord, &sr);
		SetConsoleActiveScreenBuffer(hCon2);
	}*/
}
