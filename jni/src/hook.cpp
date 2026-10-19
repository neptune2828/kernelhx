#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <string.h>
#include <jni.h>
#include <draw.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <dlfcn.h>
#include <math.h>
#include <array>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <sys/inotify.h>
#include <wanbai.hpp>
#include <VecTool.h>
#include <timer.h>
#include <UeTools.h>
#include <hook.h>
#include <draw.h>
#include "Tools/shou.hpp"

using namespace std;

float camera;
uintptr_t libUE4;
TempData TempRead;
TempData TempUes;
TempData *tempRead = &TempUes; // <-- LANGSUNG INISIALISASI DI SINI

uintptr_t Matrix, oneself, Bone, Human, Mesh;
int AddrCount, MyTeam;

void getUTF8(UTF8 * buf, long namepy)
{
    UTF16 buf16[16] = { 0 };
    落叶->读取指针(namepy, buf16, 28);
    UTF16 *pTempUTF16 = buf16;
    UTF8 *pTempUTF8 = buf;
    UTF8 *pUTF8End = pTempUTF8 + 32;
    while (pTempUTF16 < pTempUTF16 + 28)
    {
        if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End)
        {
            *pTempUTF8++ = (UTF8) * pTempUTF16;
        }
        else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else
        {
            break;
        }
        pTempUTF16++;
    }
}

FTransform getBone(uintptr_t addr)
{
    FTransform transform;  
    落叶->读取指针(addr, reinterpret_cast<void*>(&transform), 4 * 11);
    return transform;
}

pid_t get_name_pid(char* name) {
    FILE* fp;
    pid_t pid;
    char cmd[0x100] = "pidof ";
    strcat(cmd, name);
    fp = popen(cmd,"r");
    fscanf(fp,"%d", &pid);
    pclose(fp);
    return pid;
}

void 读取进程() 
{
	pid_t pid = get_name_pid((char*)"com.tencent.ig");
	pid_t pid2 = get_name_pid ((char*)"com.rekoo.pubgm");
	pid_t pid3 = get_name_pid ((char*)"com.vng.pubgmobile");
	pid_t pid4 = get_name_pid ((char*)"com.pubg.krmobile");
	pid_t pid5 = get_name_pid ((char*)"com.pubg.imobile");
	pid_t pid6 = get_name_pid((char*)"com.tencent.igce");
	
	if (pid > 0) {		
		落叶->initialize(pid);
	} else if (pid2 > 0) {
		落叶->initialize(pid2);
	} else if (pid3 > 0) {
		落叶->initialize(pid3);
	} else if (pid4 > 0) {
		落叶->initialize(pid4);
	} else if (pid5 > 0) {
		落叶->initialize(pid5);
	} else if (pid6 > 0) {
		落叶->initialize(pid6);
	} else {	
		printf("游戏未启动");
		exit(0);
	}
	
    memset(matrix, 0, 16);
	libUE4 = 落叶->getModuleBase("libUE4.so");
	while (!libUE4) {
		libUE4 = 落叶->getModuleBase("libUE4.so");
		sleep(1);
	}
	
    tempRead = &TempUes;

	if (screen_x > screen_y) {
		Higtih = screen_y / 2;
		Widtih = screen_x / 2;
	} else {
		Higtih = screen_x / 2;
		Widtih = screen_y / 2;
	}
	
	for (;;) {
		if(初始化){
            for(int i = 0; i<10;i++){
                pid_t pid = get_name_pid((char*)"com.tencent.ig");
                if (pid > 0) {		
                    落叶->initialize(pid);
                }
                libUE4 = 落叶->getModuleBase("libUE4.so");
                测试++;
            }
		}
		
		TempRead.mPlayerArray.Count = AddrCount;
		*tempRead = TempRead;
		AddrCount = 0;
		模块地址 = libUE4;
	
        // --- UPDATE GWORLD CHAIN ---
		uintptr_t GEngine = 落叶->读取指针<uintptr_t>(libUE4 + 0xF621D90);
        if (!GEngine) { usleep(1000); continue; }
		uintptr_t GameViewport = 落叶->读取指针<uintptr_t>(GEngine + 0x810);
        if (!GameViewport) { usleep(1000); continue; }
		uintptr_t Uworld = 落叶->读取指针<uintptr_t>(GameViewport + 0x78);
        if (!Uworld) { usleep(1000); continue; }
		
    	uintptr_t Uleve = 落叶->读取指针<uintptr_t>(Uworld + 0x30); 
        if (!Uleve) { usleep(1000); continue; }
		
    	uintptr_t 对象阵列 = DecryptActorsArray(Uleve, 0xA0, 0x448);
		if (!对象阵列) { 对象阵列 = DecryptActorsArray(Uleve, 0xA0, 0x488); }
        if (!对象阵列) { usleep(1000); continue; }

		uintptr_t Arrayaddr = 落叶->读取指针<uintptr_t>(对象阵列);
		数组 = Arrayaddr;
		if(Arrayaddr >= 0x10000000000 || Arrayaddr < 0x10000) { continue; }

		int Count = 落叶->读取指针<int>(对象阵列 + 0x8);
		if(Count <=0 || Count >=2000) { continue; }
		
        // --- UPDATE LOCAL PLAYER & MATRIX ---
        uintptr_t GameInstance = 落叶->读取指针<uintptr_t>(Uworld + 0x470);
        uintptr_t LocalPlayers = 落叶->读取指针<uintptr_t>(GameInstance + 0x48);
        uintptr_t LocalPlayer = 落叶->读取指针<uintptr_t>(LocalPlayers);
        uintptr_t PlayerController = 落叶->读取指针<uintptr_t>(LocalPlayer + 0x30);
        if (!PlayerController) { usleep(1000); continue; }

        // Ambil Matrix dari Camera Manager
		uintptr_t CameraMgr = 落叶->读取指针<uintptr_t>(PlayerController + 0x548);
        if (CameraMgr) {
            uintptr_t povAddr = CameraMgr + 0x10A0 + 0x10;
            // MATRIX OFFSET: Pastikan offset matriks sesuai dump lu
            落叶->读取指针(povAddr, &TempRead.matrix, 16 * 4); 
            落叶->读取指针(povAddr, &matrix, 16 * 4);
            TempRead.Fov = 落叶->读取指针<float>(povAddr + 0x24);
        }
	
        // Ambil Oneself (Pawn)
		oneself = 落叶->读取指针<uintptr_t>(PlayerController + 0x528);
        if (!oneself) { usleep(1000); continue; }

        // --- UPDATE LOCAL PLAYER STATS ---
		MyTeam = 落叶->读取指针<int>(oneself + 0x998); // AUE_CHAR_TEAM_ID
		uintptr_t selfRoot = 落叶->读取指针<uintptr_t>(oneself + 0x208);
		落叶->读取指针(selfRoot + 0x1E4, &TempRead.MyPos, 12); // USCENE_RELATIVE_LOCATION
		
		TempRead.IsAiming = 落叶->读取指针<int>(oneself + 0xFE1); 
        TempRead.IsFiring = 落叶->读取指针<int>(oneself + 0x15A8);	
		
		for (int i = 0; i < Count; i ++)
		{		
			uintptr_t Objaddr = 落叶->读取指针<uintptr_t>(Arrayaddr + 8 * i);
            if (!Objaddr) continue;
			
			float magic = 落叶->读取指针<float>(Objaddr + 0x26c8);
			if (magic != 479.5 && magic != -1.0f) { continue; }
			
			uintptr_t object = 落叶->读取指针<uintptr_t>(Objaddr + 0x208);
			if (object <= 0xffff || object == 0 || object <= 0x10000000 || object % 4 != 0 || object >= 0x10000000000)
            	continue;
				
			auto *Players = &TempRead.mPlayerArray.mPlayer[AddrCount];
			
			落叶->读取指针(object + 0x1E4, &Players->Pos, 12);
			if (Players->Pos.x == 0 || Players->Pos.y == 0) { continue; }
			
			int State = 落叶->读取指针<int>(Objaddr + 0xd34);
        	if (State == 1048576 || State == 1048592) { continue; }
        
        	Players->TeamID = 落叶->读取指针<int>(Objaddr + 0x998);
        	if (Players->TeamID == MyTeam || Players->TeamID < 1) { continue; }
            
        	float MinHealth = 落叶->读取指针<float>(Objaddr + 0xE60);
        	float MaxHealth = 落叶->读取指针<float>(Objaddr + 0xE64);
        	Players->Health = (MinHealth / MaxHealth) * 100;
        	if (Players->Health > 100 || Players->Health <= 0) { continue; }
				
			if (落叶->读取指针<uintptr_t>(Objaddr + 0x1c0)) {
            	落叶->读取指针(落叶->读取指针<uintptr_t>(Objaddr + 0xd60) + 0xb0, &Players->Predict, sizeof(Players->Predict));
      	 	} else {
            	落叶->读取指针(object + 0x200, &Players->Predict, sizeof(Players->Predict));
        	}              	
			
			GetDistance(Players->Pos, TempRead.MyPos, &Players->Distance);
			WorldToScreen(&Players->ScreenPos, &camera, &Players->w, Players->Pos);			
			Players->camera = camera;
			
			Players->IsBot = 落叶->读取指针<int>(Objaddr + 0xA59);
			if(DrawIo[9] && Players->IsBot == 1) { continue; }
			
			getUTF8(Players->PlayerName, 落叶->读取指针<uintptr_t>(Objaddr + 0x960));
			
			Mesh = 落叶->读取指针<uintptr_t>(Objaddr + 0x510);
            if (Mesh <= 0x10000) { continue; }
        	
        	Human = Mesh + 0x270;
        	Bone = 落叶->读取指针<uintptr_t>(Mesh + 0xCE0);
			if (Bone <= 0x10000) { continue; }
			
			FTransform meshtrans = getBone(Human);
            FMatrix c2wMatrix = TransformToMatrix(meshtrans);		
			
            FTransform headtrans = getBone(Bone + 6 * 48);
            FMatrix boneMatrix = TransformToMatrix(headtrans);
            Players->Head.Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
            Players->Head.Pos.z += 7;
            Players->Head.ScreenPos = WorldToScreen(Players->Head.Pos, matrix, camera);
			
            FTransform chesttrans = getBone(Bone + 5 * 48);
            FMatrix boneMatrix1 = TransformToMatrix(chesttrans);
            Players->Chest.Pos = MarixToVector(MatrixMulti(boneMatrix1, c2wMatrix));
            Players->Chest.ScreenPos = WorldToScreen(Players->Chest.Pos, matrix, camera);
            
            FTransform pelvistrans = getBone(Bone + 1 * 48);
            FMatrix boneMatrix2 = TransformToMatrix(pelvistrans);
            Players->Pelvis.Pos = MarixToVector(MatrixMulti(boneMatrix2, c2wMatrix));
            Players->Pelvis.ScreenPos = WorldToScreen(Players->Pelvis.Pos, matrix, camera);
			
            FTransform lshtrans = getBone(Bone + 12 * 48);
            FMatrix boneMatrix3 = TransformToMatrix(lshtrans);
            Players->Left_Shoulder.Pos = MarixToVector(MatrixMulti(boneMatrix3, c2wMatrix));
            Players->Left_Shoulder.ScreenPos = WorldToScreen(Players->Left_Shoulder.Pos, matrix, camera);
			
            FTransform rshtrans = getBone(Bone + 33 * 48);
            FMatrix boneMatrix4 = TransformToMatrix(rshtrans);
            Players->Right_Shoulder.Pos = MarixToVector(MatrixMulti(boneMatrix4, c2wMatrix));
            Players->Right_Shoulder.ScreenPos = WorldToScreen(Players->Left_Shoulder.Pos, matrix, camera);
			
            FTransform lelbtrans = getBone(Bone + 13 * 48);
            FMatrix boneMatrix5 = TransformToMatrix(lelbtrans);
            Players->Left_Elbow.Pos = MarixToVector(MatrixMulti(boneMatrix5, c2wMatrix));
            Players->Left_Elbow.ScreenPos = WorldToScreen(Players->Left_Elbow.Pos, matrix, camera);
			
            FTransform relbtrans = getBone(Bone + 34 * 48);
            FMatrix boneMatrix6 = TransformToMatrix(relbtrans);
            Players->Right_Elbow.Pos = MarixToVector(MatrixMulti(boneMatrix6, c2wMatrix));
            Players->Right_Elbow.ScreenPos = WorldToScreen(Players->Right_Elbow.Pos, matrix, camera);
			
            FTransform lwtrans = getBone(Bone + 64 * 48);
            FMatrix boneMatrix7 = TransformToMatrix(lwtrans);
            Players->Left_Wrist.Pos = MarixToVector(MatrixMulti(boneMatrix7, c2wMatrix));
            Players->Left_Wrist.ScreenPos = WorldToScreen(Players->Left_Wrist.Pos, matrix, camera);
			
            FTransform rwtrans = getBone(Bone + 63 * 48);
            FMatrix boneMatrix8 = TransformToMatrix(rwtrans);
            Players->Right_Wrist.Pos = MarixToVector(MatrixMulti(boneMatrix8, c2wMatrix));
            Players->Right_Wrist.ScreenPos = WorldToScreen(Players->Right_Wrist.Pos, matrix, camera);
			
            FTransform Llshtrans = getBone(Bone + 53 * 48);
            FMatrix boneMatrix9 = TransformToMatrix(Llshtrans);
            Players->Left_Thigh.Pos = MarixToVector(MatrixMulti(boneMatrix9, c2wMatrix));
            Players->Left_Thigh.ScreenPos = WorldToScreen(Players->Left_Thigh.Pos, matrix, camera);
			
            FTransform Lrshtrans = getBone(Bone + 57 * 48);
            FMatrix boneMatrix10 = TransformToMatrix(Lrshtrans);
            Players->Right_Thigh.Pos = MarixToVector(MatrixMulti(boneMatrix10, c2wMatrix));
            Players->Right_Thigh.ScreenPos = WorldToScreen(Players->Right_Thigh.Pos, matrix, camera);
			
            FTransform Llelbtrans = getBone(Bone + 54 * 48);
            FMatrix boneMatrix11 = TransformToMatrix(Llelbtrans);
            Players->Left_Knee.Pos = MarixToVector(MatrixMulti(boneMatrix11, c2wMatrix));
            Players->Left_Knee.ScreenPos = WorldToScreen(Players->Left_Knee.Pos, matrix, camera);
			
            FTransform Lrelbtrans = getBone(Bone + 58 * 48);
            FMatrix boneMatrix12 = TransformToMatrix(Lrelbtrans);
            Players->Right_Knee.Pos = MarixToVector(MatrixMulti(boneMatrix12, c2wMatrix));
            Players->Right_Knee.ScreenPos = WorldToScreen(Players->Right_Knee.Pos, matrix, camera);
			
            FTransform Llwtrans = getBone(Bone + 55 * 48);
            FMatrix boneMatrix13 = TransformToMatrix(Llwtrans);
            Players->Left_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix13, c2wMatrix));
            Players->Left_Ankle.ScreenPos = WorldToScreen(Players->Left_Ankle.Pos, matrix, camera);
			
            FTransform Lrwtrans = getBone(Bone + 59 * 48);
            FMatrix boneMatrix14 = TransformToMatrix(Lrwtrans);
            Players->Right_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix14, c2wMatrix));
            Players->Right_Ankle.ScreenPos = WorldToScreen(Players->Right_Ankle.Pos, matrix, camera);
			
			AddrCount ++;
		}
		usleep(200);
	}
}