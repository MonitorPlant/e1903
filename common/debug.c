#include <windows.h>
#include <string.h>

#include "debug.h"

void printErrorMessage( char* file_name, char* error_type )
{
    char message[1024] = "プログラムの実行中にエラーが発生しました\n";

    //エラーメッセージ作成
    strcat( message, "ファイル名 : ");
    strcat( message, file_name );
    strcat( message, "\n" );
    strcat( message, "エラー箇所 : " );
    strcat( message, error_type );

    //エラーメッセージウィンドウ生成
    MessageBox( NULL, TEXT( message ), TEXT( "実行時エラー" ), MB_ICONERROR );
}
