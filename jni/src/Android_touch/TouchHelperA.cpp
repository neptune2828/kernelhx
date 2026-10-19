#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <time.h>
#include <draw.h>
#include "imgui.h"
#include "TouchHelper.h"

// ==========================================
// VARIABEL GLOBAL (JANGAN DIHAPUS)
// ==========================================
int slot = 9;
int origfd, fd;
int TouchReso[2];
pthread_t TouchLoop;
TouchFinger Finger[10];
input_event event[128];
int TouchSize[2], ScreeSize[2];

Vec2 Point;

void Upload()
{
    int32_t tmpCnt = 0;
	memset(event, 0, sizeof(struct input_event) * 128);
    for (int i = 0; i <= 10; i++)
    {
        if (Finger[i].isDown)
        {
            event[tmpCnt].type = EV_ABS;
            event[tmpCnt].code = ABS_X;
            event[tmpCnt].value = Finger[i].x;
            tmpCnt++;
            event[tmpCnt].type = EV_ABS;
            event[tmpCnt].code = ABS_Y;
            event[tmpCnt].value = Finger[i].y;
	        tmpCnt++;
            event[tmpCnt].type = EV_ABS;
            event[tmpCnt].code = ABS_MT_POSITION_X;
            event[tmpCnt].value = Finger[i].x;
            tmpCnt++;
            event[tmpCnt].type = EV_ABS;
            event[tmpCnt].code = ABS_MT_POSITION_Y;
            event[tmpCnt].value = Finger[i].y;
            tmpCnt++;
            event[tmpCnt].type = EV_SYN;
            event[tmpCnt].code = SYN_MT_REPORT;
            event[tmpCnt].value = 0;
            tmpCnt++;
        }
	}
	if (tmpCnt == 0)
	{
		event[tmpCnt].type = EV_SYN;
		event[tmpCnt].code = SYN_MT_REPORT;
		event[tmpCnt].value = 0;
		tmpCnt++;
	}
	event[tmpCnt].type = EV_SYN;
	event[tmpCnt].code = SYN_REPORT;
	event[tmpCnt].value = 0;
	tmpCnt++;
	write(fd, event, sizeof(struct input_event) * tmpCnt);
}

Vec2 RotatePoint(Vec2 Rotate, int32_t Width, int32_t Heiht)
{
    if (Orientation == 0) {
        return Vec2(Rotate.x, Rotate.y);
    }
    Vec2 Point(Rotate.x, Rotate.y);
    if (Orientation == 3) {
        Point.x = (float)Heiht - Rotate.y;
        Point.y = Rotate.x;
    } else if (Orientation == 2) {
        Point.x = (float)Width - Rotate.x;
        Point.y = (float)Heiht - Rotate.y;
    } else if (Orientation == 1) {
        Point.x = Rotate.y;
        Point.y = (float)Width - Rotate.x;
    }
    return Point;
}

void *TypeA(void*)
{
    int latest = 0;
	float x_now = 0, y_now = 0;
	struct input_event ie;
	struct input_event key;
    int rd;
    
    // PERBAIKAN: read > 0 biar kalau gagal (-1) gak masuk loop
	while ((rd = read(origfd, &ie, sizeof(struct input_event))) > 0)
	{
        if (ie.code == ABS_MT_SLOT) {
            latest = ie.value;
            // PERBAIKAN: Cegah out-of-bounds kalau latest ngawur
            if (latest < 0 || latest >= 10) latest = 0; 
        }
        if (ie.code == ABS_MT_TRACKING_ID) {
            if (ie.value == -1) {
                Finger[latest].isDown = false;
				ImGui::GetIO().MouseDown[0] = Finger[latest].isDown;
            } else {
                Finger[latest].isDown = true;
				ImGui::GetIO().MouseDown[0] = Finger[latest].isDown;
            }
        }
        if (ie.code == ABS_MT_POSITION_X) {
            Finger[latest].x = ie.value;
			x_now = Finger[latest].x;
        }
        if (ie.code == ABS_MT_POSITION_Y) {
            Finger[latest].y = ie.value;
			y_now = Finger[latest].y;
        }
        if (ie.code == SYN_REPORT) {
            Upload();
        }
        if (ie.code == KEY_APPSELECT) {
            key.type = ie.type;
            key.code = ie.code;
            key.value = ie.value;
            write(fd, &key, sizeof(key));
        }
        if (ie.code == KEY_BACK) {
            key.type = ie.type;
            key.code = ie.code;
            key.value = ie.value;
            write(fd, &key, sizeof(key));
        }
		Point = RotatePoint(Vec2(x_now, y_now), TouchSize[0], TouchSize[1]);            
        ImGui::GetIO().MousePos = ImVec2(Point.x / TouchReso[0], Point.y / TouchReso[1]);	
	}
    printf("[TOUCH] Thread TypeA berhenti (read <= 0)\n");
	return 0;
}

int GetEventCount()
{
    DIR *dir = opendir("/dev/input/");
    dirent *ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, "event"))
            count++;
    }
    return count;
}

int Execute(const char *cmd, char *result)
{
    FILE *ptr;
    int iRet = -1;
    char buf_ps[1024], ps[1024];
    strcpy(ps, cmd);
    if ((ptr = popen(ps, "r")) != nullptr)
    {
        while (fgets(buf_ps, sizeof(buf_ps), ptr) != nullptr) {
            strcat(result, buf_ps);
            if (strlen(result) > 1000) {
                break;
            }
        }
        pclose(ptr);
        ptr = nullptr;
        iRet = 0;
    } else {
        iRet = -1;
    }
    return iRet;
}

int GetEventId()
{
    char resu[4096], temp[128];
    int EventCount = GetEventCount();
    for (int i = 0; i < EventCount; i++)
    {
        memset(resu, 0, sizeof resu);
        sprintf(temp, "getevent -i /dev/input/event%d", i);
        Execute(temp, resu);
        if (strstr(resu, "ABS (0003)") != nullptr) {
            return i;
        }
    }
    return 0;
}

char *RandStr(char *str, const int len)
{
    int i;
    srand(time(NULL));
    for (i = 0; i < len; ++i)
    {
        switch ((rand() % 3))
        {
            case 1:
                str[i] = 'A' + rand() % 26;
            break;
            case 2:
                str[i] = 'a' + rand() % 26;
            break;
            default:
                str[i] = '0' + rand() % 10;
            break;
        }
    }
    str[++i] = '\0';
    return str;
}

void Touch_Init()
{
	char origI[256];
    ImGuiIO *Io = &ImGui::GetIO();
    
	sprintf(origI, "/dev/input/event%d", GetEventId());
	origfd = open(origI, O_RDWR);
	struct input_absinfo absX, absY;
	ioctl(origfd, EVIOCGABS(ABS_MT_POSITION_X), &absX);
	ioctl(origfd, EVIOCGABS(ABS_MT_POSITION_Y), &absY);
    
    TouchSize[0] = absX.maximum + 1;
    TouchSize[1] = absY.maximum + 1;
	
    ScreeSize[0] = screen_x;
	ScreeSize[1] = screen_y;
    if (ScreeSize[0] > ScreeSize[1]) {
        ScreeSize[1] = ScreeSize[0];
        ScreeSize[0] = ScreeSize[1];
	}
    TouchReso[0] = TouchSize[0] / ScreeSize[0];
    TouchReso[1] = TouchSize[1] / ScreeSize[1];
    if (TouchReso[0] == 0 || TouchReso[1] == 0) {
        TouchReso[0] = 1;
        TouchReso[1] = 1;
    }
    
	struct uinput_user_dev ui_dev;
	int uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (uinp_fd == 0) {
		printf("无法打开uinput,请检查是否有Root权限, %s\n", strerror(errno));
		return;
	}
    
    ioctl(uinp_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_ABS);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_X);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_SYN);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinp_fd, UI_SET_KEYBIT, KEY_BACK);
	ioctl(uinp_fd, UI_SET_KEYBIT, KEY_APPSELECT);
    
    char TouchName[15];
	memset(&ui_dev, 0, sizeof(ui_dev));
	strncpy(ui_dev.name, RandStr(TouchName, 10), UINPUT_MAX_NAME_SIZE);
	ui_dev.id.bustype = BUS_USB;
	ui_dev.id.vendor = rand() % 15;
    ui_dev.id.product = rand() % 15;
    ui_dev.id.version = rand() % 15;
    
    ui_dev.absmin[ABS_X] = 0;
    ui_dev.absmax[ABS_X] = absX.maximum;
    ui_dev.absmin[ABS_Y] = 0;
	ui_dev.absmax[ABS_Y] = absY.maximum;
	ui_dev.absmin[ABS_MT_POSITION_X] = 0;
	ui_dev.absmax[ABS_MT_POSITION_X] = absX.maximum;
	ui_dev.absmin[ABS_MT_POSITION_Y] = 0;
	ui_dev.absmax[ABS_MT_POSITION_Y] = absY.maximum;
    
	write(uinp_fd, &ui_dev, sizeof(ui_dev));
	if (ioctl(uinp_fd, UI_DEV_CREATE)) {
		printf("无法创建uinput设备\n");
		return;
	}
    
	fd = uinp_fd;
	if (ioctl(origfd, EVIOCGRAB, 1)) {
		printf("触摸句柄错误\n");
		return;
	}
    
	pthread_create(&TouchLoop, NULL, TypeA, NULL);
}

void Touch_Up(int s)
{
    Finger[slot].isDown = false;
    Upload();
}

void Touch_Down(int s, int x, int y)
{
	Finger[slot].x = x * TouchReso[0];
	Finger[slot].y = y * TouchReso[1];
	Finger[slot].isDown = true;
	Upload();
}

void Touch_Move(int s, int x, int y)
{
	Finger[slot].x = x * TouchReso[0];
	Finger[slot].y = y * TouchReso[1];
	Finger[slot].isDown = true;
	Upload();
}