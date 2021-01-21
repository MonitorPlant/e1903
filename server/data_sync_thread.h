#ifndef DATA_SYNC_THREAD_H_
#define DATA_SYNC_THREAD_H_

#include <windows.h>
#include <winsock2.h>

#include "graphics_thread.h"
#include "../common/object.h"
#include "../common/debug.h"

DWORD WINAPI DataSyncThread( LPVOID arg );

#endif /* DATA_SYNC_THREAD_H_ */
