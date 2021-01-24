@echo off
taskkill /im main.exe /f
bcc32 -tW main.c ../common/debug.c ../common/loading.c ../common/parts.c ../common/rules.c ../common/result.c ../common/title.c ../common/graphics_thread.c data_sync_thread.c main_loop_thread.c 
del *.obj *.tds *.bat~ *.un~
