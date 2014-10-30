#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <3ds.h>

#include "costable.h"
#include "text.h"
#include "output.h"

char superStr[8192];
int cnt;

s32 pcCos(u16 v)
{
	return costable[v&0x1FF];
}

int countLines(char* str)
{
	if(!str)return 0;
	int cnt; for(cnt=1;*str=='\n'?++cnt:*str;str++);
	return cnt;
}

void cutLine(char* str)
{
	if(!str || !*str)return;
	char* str2=str;	for(;*str2&&*(str2+1)&&*str2!='\n';str2++);	str2++;
	memmove(str,str2,strlen(str2)+1);
}

void drawFrame()
{
	u8* bufAdr=gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

	int i, j;
	for(i=1;i<400;i++)
	{
		for(j=1;j<240;j++)
		{
			u32 v=(j+i*240)*3;
			bufAdr[v]=(pcCos(i+cnt)+4096)/32;
			bufAdr[v+1]=(pcCos(j-256+cnt)+4096)/64;
			bufAdr[v+2]=(pcCos(i+128-cnt)+4096)/32;
		}
	}
	gfxDrawText(GFX_TOP, GFX_LEFT, NULL, "ftPONY v0.0002\n", 240-fontDefault.height*1, 10);
	i = countLines(superStr);
	while(i>240/fontDefault.height-3){cutLine(superStr);i--;}
	gfxDrawText(GFX_TOP, GFX_LEFT, NULL, superStr, 240-fontDefault.height*3, 20);
	cnt++;

	gfxFlushBuffers();
	gfxSwapBuffers();
}

int main()
{
	srvInit();
	aptInit();
	hidInit(NULL);
	irrstInit(NULL);
	gfxInit();
	gfxSet3D(false);

	print("fsInit %08X\n", (unsigned int)
		fsInit()
	);

	FS_archive sdmcArchive = (FS_archive) {
		0x00000009, (FS_path) { PATH_EMPTY, 1, (u8*) "" }
	};

	print("FSUSER_OpenArchive %08X\n", (unsigned int)
		FSUSER_OpenArchive(NULL, &sdmcArchive)
	);

	superStr[0]=0;

	APP_STATUS status;
	while((status=aptGetStatus()) != APP_EXITING)
	{
		if(status == APP_RUNNING)
		{
			drawFrame();

			hidScanInput();
			if (hidKeysDown() & KEY_B) {
				break;
			} else if (hidKeysDown() & KEY_A) {
				print("FSUSER_CreateDirectory %08X\n", (unsigned int)
					FSUSER_CreateDirectory(NULL, sdmcArchive, FS_makePath(PATH_CHAR, "/new_dir"))
				);
			}
		}
		else if(status == APP_SUSPENDING)
		{
			aptReturnToMenu();
		}
		else if (status == APP_SLEEPMODE)
		{
			aptWaitStatusEvent();
		}
		gspWaitForEvent(GSPEVENT_VBlank0, false);
	}

	gfxExit();
	irrstExit();
	hidExit();
	aptExit();
	srvExit();
	return 0;
}
