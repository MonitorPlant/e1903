#include <windows.h>
#include <winsock2.h>

#include "../common/debug.h"
#include "../common/object.h"
#include "data_sync_thread.h"

extern CommonObject map_source;
extern CommonObject self, enemy;
extern CommonObject target[ MAX_TARGET_NUM ];
extern CommonObject bullet[ MAX_BULLET_NUM ];
extern MouseState self_mouse, enemy_mouse;

extern BOOL end_program;
extern BOOL game_end;
extern char point_self, point_enemy;
extern char remain_time;

DWORD WINAPI DataSyncThread( LPVOID arg )
{
    //各変数を宣言
	char host_name[256];
	char self_address[16];
	char pair_address[16];
	
	struct WSAData wsaData;
	SOCKET sock;
	struct sockaddr_in addr;
	struct in_addr local_addr;
	int len;
	BOOL yes = 1;
    char send_data[6];
    char receive_data[4];
	
	//初期化
	WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons( 2021 ); //ポート2021を使用 参照: https://ja.wikipedia.org/wiki/TCPやUDPにおけるポート番号の一覧
	
	
	//自分のローカルIPアドレスを取得
	gethostname( host_name, sizeof( host_name ) );  //実行しているパソコンの名前を取得
	local_addr.S_un.S_addr = *( u_long *)gethostbyname( host_name )->h_addr_list[0];
	
	strcpy( self_address, inet_ntoa( local_addr ) ); //パソコンの名前からローカルIPアドレスを取得
	
	
	//クライアントからの接続を待つ
	sock = socket( AF_INET, SOCK_DGRAM, 0 ); //UDP受信用ソケット作成
	addr.sin_addr.S_un.S_addr = INADDR_ANY; //ブロードキャストの受信用に設定
	
	bind( sock, ( struct sockaddr *)&addr, sizeof( addr ) );
	
	memset( pair_address, 0, sizeof( pair_address ) );
	recv( sock, pair_address, sizeof( pair_address ), 0 );
	
	closesocket( sock ); //UDPのソケットを閉じる
	
	Sleep( 50 );
	
	
	//クライアントに自分のIPアドレスを送信する
	sock = socket( AF_INET, SOCK_DGRAM, 0 ); //UDP送信用ソケット作成
	addr.sin_addr.S_un.S_addr = inet_addr( "255.255.255.255" ); //ブロードキャストアドレスを指定
	
	setsockopt( sock, SOL_SOCKET, SO_BROADCAST, ( char *)&yes, sizeof( yes ) );
	sendto( sock, self_address, 16, 0, ( struct sockaddr *)&addr, sizeof( addr ) );
	
	closesocket( sock ); //UDPのソケットを閉じる
	
	
	//TCPでデータを送信する
	sock = socket( AF_INET, SOCK_STREAM, 0 ); //TCP送信用ソケット作成
	addr.sin_addr.S_un.S_addr = inet_addr( pair_address ); //送信先を指定
	
	bind( sock, ( struct sockaddr *)&addr, sizeof( addr ) );
	listen( sock, 5 ); //TCPクライントからの接続要求を待つ状態にする
	len = sizeof( addr );
	sock = accept( sock, ( struct sockaddr *)&addr, &len );


    enemy.isExist = TRUE; //メインスレッドに相手が見つかったことを報告
	
	while( game_end == FALSE )
	{
        //送信内容を書き込み
        send_data[ 0 ] = remain_time;
        send_data[ 1 ] = self.x;
        send_data[ 2 ] = self.y;
        send_data[ 3 ] = ( self_mouse.click_wheel << 2 ) + ( self_mouse.click_right << 1 ) + ( self_mouse.click_left );
        

        //データを送受信
		send( sock, send_data, sizeof( send_data ), 0 );
		recv( sock, receive_data, sizeof( receive_data ), 0 );
		

        //受信内容をコピー
        enemy.x = receive_data[ 0 ] / 8 + DISPLAY_MAX_CHAR_X / 2;
        enemy.y = receive_data[ 1 ] / 18;
        enemy_mouse.click_wheel = ( receive_data[ 2 ] >> 2 ) & 1;
        enemy_mouse.click_right = ( receive_data[ 2 ] >> 1 ) & 1;
        enemy_mouse.click_left = receive_data[ 2 ] & 1;

		Sleep( 1 );
	}
	
	closesocket( sock );
	
	WSACleanup();

    ( void )arg;

    return 0;
}
