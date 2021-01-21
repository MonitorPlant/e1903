@echo off
bcc32 -tW -WM main.c ../common/debug.c ../common/loading.c ../common/parts.c main_loop_thread.c graphics_thread.c data_sync_thread.c
