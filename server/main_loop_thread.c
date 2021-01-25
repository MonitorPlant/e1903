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
extern BOOL bullet_self_generate;
extern BOOL bullet_enemy_generate;
extern int last_target_generate;
extern short point_self, point_enemy;
extern char remain_time;

extern const int char_target1[5][10], char_target2[5][10], char_target3[3][6], char_target4[2][4], char_num[10][7][7];

DWORD WINAPI MainLoopThread( LPVOID arg )
{
    int i;
    HTIMER screen_timer;

    
    //プレイヤーがマウスをクリックするまで待機 スペースを押すとルール説明
    background.isExist = TRUE;
    background.x = background.y = 0;
    background.type = TYPE_TITLE;
    self.isExist = FALSE;
    while( self_mouse.click_left == FALSE )
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
    self.isExist = TRUE;


    //対戦相手が見つかるまで待機
    SET_TIMER( screen_timer );
    while( enemy.isExist == FALSE )
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
    HTIMER main_timer;
    HTIMER target_timer;
    HTIMER bullet_timer;
    HTIMER generate_target_timer;
    HTIMER generate_bullet_self;
    HTIMER generate_bullet_enemy;

    //初期化
    for( i = 0; i < 8; i++ )
    {
        number[ i ].isExist = TRUE;
    }
    srand( ( unsigned )time( NULL ) );
    self.isExist = TRUE;
    enemy.isExist = TRUE;
    point_enemy = point_self = 0;
    SET_TIMER( main_timer );
    SET_TIMER( target_timer );
    SET_TIMER( bullet_timer );
    SET_TIMER( generate_target_timer );
    SET_TIMER( generate_bullet_self );
    SET_TIMER( generate_bullet_enemy );

    //60秒間ループ
    while( PASSED_TIME( main_timer ) < 60000 )
    {
        //残り時間設定
        remain_time = 60 - PASSED_TIME( main_timer ) / 1000;
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
        if( self_mouse.click_left && self_mouse.click_left_pass == FALSE && self_mouse.click_right == FALSE && PASSED_TIME( generate_bullet_self ) > 200 )
        {
            SET_TIMER( generate_bullet_self );
            for( i = 0; i < MAX_BULLET_NUM; i++ )
            {
                if( bullet[ i ].isExist == FALSE )
                {
                    bullet[ i ].isExist = TRUE;
                    bullet[ i ].type = TYPE_BULLET_SELF;
                    bullet[ i ].x = self.x + self.size_x;
                    bullet[ i ].y = self.y + self.size_y / 2;
                    bullet_self_generate = TRUE;
                    break;
                }
            }
        }

        //敵の弾を生成
        if( bullet_enemy_generate && enemy_mouse.click_right == FALSE && PASSED_TIME( generate_bullet_enemy ) )
        {
            SET_TIMER( generate_bullet_enemy );
            for( i = 0; i < MAX_BULLET_NUM; i++ )
            {
                if( bullet[ i ].isExist == FALSE )
                {
                    bullet[ i ].isExist = TRUE;
                    bullet[ i ].type = TYPE_BULLET_ENEMY;
                    bullet[ i ].x = enemy.x;
                    bullet[ i ].y = enemy.y + enemy.size_y / 2;
                    break;
                }
            }
        }
        bullet_enemy_generate = FALSE;

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

        //500msごとに的を追加
        if( PASSED_TIME( generate_target_timer ) > 500 )
        {
            SET_TIMER( generate_target_timer );
            for( i = 0; i < MAX_TARGET_NUM; i++ )
            {
                if( target[ i ].isExist == FALSE )
                {
                    target_generate = TRUE;
                    last_target_generate = i;
                    target[ i ].isExist = TRUE;
                    target[ i ].y = 0;
                    target[ i ].x = rand() % ( DISPLAY_MAX_CHAR_X / 2 ) + DISPLAY_MAX_CHAR_X / 4;
                    //的の種類をランダムに生成
                    switch( rand() & 0x03 )
                    {
                        case 0x00:
                            target[ i ].type = TYPE_TARGET1;
                            target[ i ].size_x = 10;
                            target[ i ].size_y = 5;
                            break;
                        case 0x01:
                            target[ i ].type = TYPE_TARGET2;
                            target[ i ].size_x = 10;
                            target[ i ].size_y = 5;
                            break;
                        case 0x02:
                            target[ i ].type = TYPE_TARGET3;
                            target[ i ].size_x = 6;
                            target[ i ].size_y = 3;
                            break;
                        case 0x03:
                            target[ i ].type = TYPE_TARGET4;
                            target[ i ].size_x = 4;
                            target[ i ].size_y = 2;
                            break;
                    }
                    break;
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

        //敵の弾の自分への着弾判定
        if( self_mouse.click_right == FALSE )
        {
            for( i = 0; i < MAX_BULLET_NUM; i++ )
            {
                if( bullet[ i ].isExist == TRUE && bullet[ i ].type == TYPE_BULLET_ENEMY )
                {
                    if( bullet[ i ].x + bullet[ i ].size_x > self.x && bullet[ i ].x < self.x + self.size_x && bullet[ i ].y + bullet[ i ].size_y > self.y && bullet[ i ].y < self.y + self.size_y )
                    {
                        bullet[ i ].isExist = FALSE;
                        point_enemy += 5;
                        if( point_enemy > 999 )
                        {
                            point_enemy = 999;
                        }
                    }
                }
            }
        }

        //自分の弾の敵への着弾判定
        if( enemy_mouse.click_right == FALSE )
        {
            for( i = 0; i < MAX_BULLET_NUM; i++ )
            {
                if( bullet[ i ].isExist == TRUE && bullet[ i ].type == TYPE_BULLET_SELF )
                {
                    if( bullet[ i ].x + bullet[ i ].size_x > enemy.x && bullet[ i ].x < enemy.x + enemy.size_x && bullet[ i ].y + bullet[ i ].size_y > enemy.y && bullet[ i ].y < enemy.y + enemy.size_y )
                    {
                        bullet[ i ].isExist = FALSE;
                        point_self += 5;
                        if( point_self > 999 )
                        {
                            point_self = 999;
                        }
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
                            if( bullet[ j ].type == TYPE_BULLET_SELF )
                            {
                                point_self += 1;
                                if( point_self > 999 )
                                {
                                    point_self = 999;
                                }
                            }
                            else
                            {
                                point_enemy += 1;
                                if( point_enemy > 999 )
                                {
                                    point_enemy = 999;
                                }
                            }
                            //着弾すれば的を削除
                            target[ i ].isExist = FALSE;
                        }
                    }
                }
            }
        }
    }

    //終了処理
    game_end = TRUE;
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
    if( pt.x / 8 > DISPLAY_MAX_CHAR_X / 2 - ( signed )self.size_x / 2 )
    {
        pt.x = ( DISPLAY_MAX_CHAR_X / 2 - ( signed )self.size_x / 2 ) * 8;
        if( !SetCursorPos( pt.x, pt.y ) )
        {
            printErrorMessage( "main_loop_thread.c", "SetCursorPos()" );
            return;
        }
    }
    if( pt.x / 8 < ( signed )self.size_x / 2 - 2 )
    {
        pt.x = ( ( signed )self.size_x / 2 - 2 ) * 8;
        if( !SetCursorPos( pt.x, pt.y ) )
        {
            printErrorMessage( "main_loop_thread.c", "SetCursorPos()" );
            return;
        }
    }
    if( pt.y / 16 < ( signed )self.size_y / 2 - 3 )
    {
        pt.y = ( ( signed )self.size_y / 2 - 3 ) * 16;
        if( !SetCursorPos( pt.x, pt.y ) )
        {
            printErrorMessage( "main_loop_thread.c", "SetCursorPos()" );
            return;
        }
    }

    //座標をコピー、各ボタンの状態を更新
    self.x = pt.x / 8 - 13;
    self.y = pt.y / 18 - 3;
    if( self.x < 0 )
    {
        self.x = 0;
    }
    if( self.y > DISPLAY_MAX_CHAR_Y - self.size_y )
    {
        self.y = DISPLAY_MAX_CHAR_Y - self.size_y;
    }
    self_mouse.click_left_pass = self_mouse.click_left;
    self_mouse.click_wheel_pass = self_mouse.click_wheel;
    self_mouse.click_left = ( GetKeyState( VK_LBUTTON ) >> 15 ) & 1;
    self_mouse.click_right = ( GetKeyState( VK_RBUTTON ) >> 15 ) & 1;
    self_mouse.click_wheel = ( GetKeyState( VK_MBUTTON ) >> 15 ) & 1;
}
