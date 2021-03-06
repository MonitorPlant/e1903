#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include "../common/debug.h"
#include "../common/object.h"
#include "../common/graphics_thread.h"
#include "main_loop_thread.h"

extern CommonObject background;
extern CommonObject self, enemy;
extern CommonObject target[ MAX_TARGET_NUM ];
extern CommonObject bullet[ MAX_BULLET_NUM ];
extern CommonObject number[8];
extern MouseState self_mouse, enemy_mouse;

extern BOOL end_program;
extern BOOL game_end;
extern BOOL target_generate;
extern BOOL bullet_enemy_generate;
extern BOOL bullet_self_generate;
extern BOOL start_program;
extern short point_self, point_enemy;
extern char remain_time;

extern const int char_target1[5][10], char_target2[5][10], char_target3[3][6], char_target4[2][4], char_num[10][7][7];

DWORD WINAPI MainLoopThread( LPVOID arg )
{
    int i;
    HTIMER screen_timer;

    
    //プレイヤーがマウスをクリックするまで待機 スペースを押すとルール説明
    background.x = background.y = 0;
    background.isExist = TRUE;
    background.type = TYPE_TITLE;
    enemy.isExist = FALSE;
    while( enemy_mouse.click_left == FALSE )
    {
        updateMouseState();
        if( GetAsyncKeyState( VK_SPACE ) & 0x8000 )
        {
            //ルール説明画面を表示
            background.type = TYPE_RULES;
            Sleep( 100 );
            while( GetAsyncKeyState( VK_SPACE ) & 0x8000 )
            {
                Sleep( 1 );
            }
            Sleep( 100 );
            while( ( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) == FALSE )
            {
                Sleep( 1 );
            }
            //タイトル画面に戻す
            background.type = TYPE_TITLE;
            Sleep( 100 );
            while( GetAsyncKeyState( VK_ESCAPE ) & 0x8000 )
            {
                Sleep( 1 );
            }
            Sleep( 100 );
        }
        Sleep( 1 );
    }
    enemy.isExist = TRUE;
    

    //対戦相手が見つかるまで待機
    SET_TIMER( screen_timer );
    while( self.isExist == FALSE )
    {
        //400msごとにロード画面を切り替え
        switch( PASSED_TIME( screen_timer ) / 400 % 4 )
        {
            case 0: background.type = TYPE_LOADING0; break;
            case 1: background.type = TYPE_LOADING1; break;
            case 2: background.type = TYPE_LOADING2; break;
            case 3: background.type = TYPE_LOADING3; break;
        }
        Sleep( 10 );
    }
    //背景を無効化
    background.isExist = FALSE;


    //カウントダウン
    number[4].isExist = TRUE;
    number[4].type = CHAR_NUM(3); //3秒前
    SET_TIMER( screen_timer );
    while( PASSED_TIME( screen_timer ) < 1000 )
    {
        Sleep( 10 );
    }
    number[4].type = CHAR_NUM(2); //2秒前
    while( PASSED_TIME( screen_timer ) < 2000 )
    {
        Sleep( 10 );
    }
    number[4].type = CHAR_NUM(1); //1秒前
    while( PASSED_TIME( screen_timer ) < 3000 )
    {
        Sleep( 10 );
    }
    remain_time = 0;
    Sleep( 20 );


    //ゲームスタート
    mainLoop();


    //結果画面
    self.isExist = FALSE;
    enemy.isExist = FALSE;
    for( i = 0; i < MAX_BULLET_NUM; i++ )
    {
        bullet[ i ].isExist = FALSE;
    }
    for( i = 0; i < MAX_TARGET_NUM; i++ )
    {
        target[ i ].isExist = FALSE;
    }
    for( i = 0; i < 3; i++ )
    {
        number[ i ].x = number[ i + 5 ].x;
        number[ i ].y = 22;
        number[ i + 5].y = 10;
    }
    number[3].isExist = FALSE;
    number[4].isExist = FALSE;
    background.isExist = TRUE;
    if( point_self > point_enemy )
    {
        background.type = TYPE_SELF_WIN;
    }
    else
    {
        background.type = TYPE_ENEMY_WIN;
    }

    Sleep( 1000 );
    while( self_mouse.click_left == FALSE )
    {
        Sleep( 10 );
    }

    ( void )arg;

    return 0;
}

void mainLoop( void )
{
    int i, j;
    HTIMER target_timer;
    HTIMER bullet_timer;

    //初期化
    for( i = 0; i < 8; i++ )
    {
        number[ i ].isExist = TRUE;
    }
    srand( ( unsigned )time( NULL ) );
    self.isExist = TRUE;
    enemy.isExist = TRUE;
    point_enemy = point_self = 0;
    SET_TIMER( bullet_timer );
    SET_TIMER( target_timer );

    //60秒間ループ
    while( game_end == FALSE )
    {
        //残り時間設定
        //remain_time = 60 - PASSED_TIME( main_timer ) / 1000;
        number[3].type = CHAR_NUM( remain_time / 10 );
        number[4].type = CHAR_NUM( remain_time % 10 );

        //得点を設定
        number[0].type = CHAR_NUM( point_self / 100 );
        number[1].type = CHAR_NUM( point_self % 100 / 10 );
        number[2].type = CHAR_NUM( point_self % 10 );
        number[5].type = CHAR_NUM( point_enemy / 100 );
        number[6].type = CHAR_NUM( point_enemy % 100 / 10 );
        number[7].type = CHAR_NUM( point_enemy % 10 );

        //キャラクターの座標を更新
        updateMouseState();

        //自分の弾を生成
        if( bullet_self_generate && self_mouse.click_right == FALSE )
        {
            for( i = 0; i < MAX_BULLET_NUM; i++ )
            {
                if( bullet[ i ].isExist == FALSE )
                {
                    bullet[ i ].isExist = TRUE;
                    bullet[ i ].type = TYPE_BULLET_SELF;
                    bullet[ i ].x = self.x + self.size_x;
                    bullet[ i ].y = self.y + self.size_y / 2;
                    break;
                }
            }
        }
        bullet_self_generate = FALSE;

        //敵の弾を生成
        if( enemy_mouse.click_left && enemy_mouse.click_left_pass == FALSE && enemy_mouse.click_right == FALSE )
        {
            for( i = 0; i < MAX_BULLET_NUM; i++ )
            {
                if( bullet[ i ].isExist == FALSE )
                {
                    bullet[ i ].isExist = TRUE;
                    bullet[ i ].type = TYPE_BULLET_ENEMY;
                    bullet[ i ].x = enemy.x;
                    bullet[ i ].y = enemy.y + enemy.size_y / 2;
                    bullet_enemy_generate = TRUE;
                    break;
                }
            }
        }

        //10msごとに弾を移動
        if( PASSED_TIME( bullet_timer ) > 10 )
        {
            SET_TIMER( bullet_timer );
            for( i = 0; i < MAX_BULLET_NUM; i++ )
            {
                //自分の弾なら右に移動 敵の弾なら左に移動
                if( bullet[ i ].isExist )
                {
                    if( bullet[ i ].type == TYPE_BULLET_SELF )
                    {
                        bullet[ i ].x += 10;
                    }
                    else
                    {
                        bullet[ i ].x -= 10;
                    }
                    //範囲外に出れば削除
                    if( bullet[ i ].x < 0 || DISPLAY_MAX_CHAR_X - bullet[ i ].size_x < bullet[ i ].x )
                    {
                        bullet[ i ].isExist = FALSE;
                    }
                }
            }
        }

        //100msごとに的を移動
        if( PASSED_TIME( target_timer ) > 100 )
        {
            SET_TIMER( target_timer );
            for( i = 0; i < MAX_TARGET_NUM; i++ )
            {
                if( target[ i ].isExist )
                {
                    target[ i ].y += 2;
                    //範囲外に出れば削除
                    if( target[ i ].y > DISPLAY_MAX_CHAR_Y - target[ i ].size_y )
                    {
                        target[ i ].isExist = FALSE;
                    }
                }
            }
        }

        //弾丸と的が重なっていないか判定
        for( i = 0; i < MAX_TARGET_NUM; i++ )
        {
            if( target[ i ].isExist )
            {
                for( j = 0; j < MAX_BULLET_NUM; j++ )
                {
                    if( bullet[ j ].isExist )
                    {
                        if( bullet[ j ].x + bullet[ j ].size_x > target[ i ].x && bullet[ j ].x < target[ i ].x + target[ i ].size_x && bullet[ j ].y + bullet[ j ].size_y > target[ i ].y && bullet[ j ].y < target[ i ].y + target[ i ].size_y )
                        {
                            //着弾すれば的を削除
                            target[ i ].isExist = FALSE;
                        }
                    }
                }
            }
        }
    }

    //終了処理
    self.isExist = FALSE;
    enemy.isExist = FALSE;
    for( i = 0; i < MAX_BULLET_NUM; i++ )
    {
        bullet[ i ].isExist = FALSE;
    }
    for( i = 0; i < MAX_TARGET_NUM; i++ )
    {
        target[ i ].isExist = FALSE;
    }
    game_end = TRUE;
}


void updateMouseState( void )
{
    POINT pt;

    //カーソルの座標を取得
    if( !GetCursorPos( &pt ) )
    {
        printErrorMessage( "mouse.c", "GetCursorPos()" );
        return;
    }
    
    //カーソルの位置を範囲内に収める
    if( pt.x / 8 > DISPLAY_MAX_CHAR_X - ( signed )enemy.size_x / 2 - 2 )
    {
        pt.x = ( DISPLAY_MAX_CHAR_X - ( signed )enemy.size_x / 2 - 2 ) * 8;
        if( !SetCursorPos( pt.x, pt.y ) )
        {
            printErrorMessage( "main_loop_thread.c", "SetCursorPos()" );
            return;
        }
    }
    if( pt.x / 8 < DISPLAY_MAX_CHAR_X / 2 + ( signed )enemy.size_x / 2 )
    {
        pt.x = ( DISPLAY_MAX_CHAR_X / 2 + ( signed )enemy.size_x / 2 ) * 8;
        if( !SetCursorPos( pt.x, pt.y ) )
        {
            printErrorMessage( "main_loop_thread.c", "SetCursorPos()" );
            return;
        }
    }
    if( pt.y / 16 < ( signed )enemy.size_y / 2 - 3 )
    {
        pt.y = ( ( signed )enemy.size_y / 2 - 3 ) * 16;
        if( !SetCursorPos( pt.x, pt.y ) )
        {
            printErrorMessage( "main_loop_thread.c", "SetCursorPos()" );
            return;
        }
    }

    //座標をコピー、各ボタンの状態を更新
    enemy.x = pt.x / 8 - 13;
    enemy.y = pt.y / 18 - 3;
    if( enemy.x < 0 )
    {
        enemy.x = 0;
    }
    if( enemy.y > DISPLAY_MAX_CHAR_Y - enemy.size_y )
    {
        enemy.y = DISPLAY_MAX_CHAR_Y - enemy.size_y;
    }
    enemy_mouse.click_left_pass = enemy_mouse.click_left;
    enemy_mouse.click_wheel_pass = enemy_mouse.click_wheel;
    enemy_mouse.click_left = ( GetKeyState( VK_LBUTTON ) >> 15 ) & 1;
    enemy_mouse.click_right = ( GetKeyState( VK_RBUTTON ) >> 15 ) & 1;
    enemy_mouse.click_wheel = ( GetKeyState( VK_MBUTTON ) >> 15 ) & 1;
}
