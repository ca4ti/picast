#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>

char usageid[] =
{
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x29,
	0x1E,
	0x1F,
	0x20,
	0x21,
	0x22,
	0x23,
	0x24,
	0x25,
	0x26,
	0x27,
	0x2D,
	0x2E,
	0x2A,
	0x2B,
	0x14,
	0x1A,
	0x08,
	0x15,
	0x17,
	0x1C,
	0x18,
	0x0C,
	0x12,
	0x13,
	0x2F,
	0x30,
	0x28,
	0x00,
	0x04,
	0x16,
	0x07,
	0x09,
	0x0A,
	0x0B,
	0x0D,
	0x0E,
	0x0F,
	0x33,
	0x34,
	0x35,
	0x00,
	0x31,
	0x1D,
	0x1B,
	0x06,
	0x19,
	0x05,
	0x11,
	0x10,
	0x36,
	0x37,
	0x38,
	0x00,
	0x55,
	0x00,
	0x2C,
	0x39,
	0x3A,
	0x3B,
	0x3C,
	0x3D,
	0x3E,
	0x3F,
	0x40,
	0x41,
	0x42,
	0x43,
	0x53,
	0x47,
	0x5F,
	0x60,
	0x61,
	0x56,
	0x5C,
	0x5D,
	0x5E,
	0x57,
	0x59,
	0x5A,
	0x5B,
	0x62,
	0x63,
	0x00,
	0x00,
	0x00,
	0x44,
	0x45,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x58,
	0x00,
	0x54,
	0x46,
	0x00,
	0x00,
	0x4A,
	0x52,
	0x4B,
	0x50,
	0x4F,
	0x4D,
	0x51,
	0x4E,
	0x49,
	0x4C,
	0x00,
	0x7F,
	0x81,
	0x80,
	0x00,
	0x00,
	0x00,
	0x48 };

char mouseinput[] = {
	0x00,0x01,
	0x00,0x0c,
	0x01,0x01,
	0x00,0x00,
	0x03,0x00,
	0x00,0x00
};

char keyboardinput[] = {
	0x00,0x01,
	0x00,0x0c,
	0x01,0x00,
	0x00,0x00,
	0x03,0x00,
	0x00,0x00
};
/*
char reportdescriptor[55] = {
	0x01,0x01,
	0x01,0x00,
	50,
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x02,                    // USAGE (Mouse)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x09, 0x01,                    //   USAGE (Pointer)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x05, 0x09,                    //     USAGE_PAGE (Button)
	0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
	0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
	0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
	0x95, 0x03,                    //     REPORT_COUNT (3)
	0x75, 0x01,                    //     REPORT_SIZE (1)
	0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	0x95, 0x01,                    //     REPORT_COUNT (1)
	0x75, 0x05,                    //     REPORT_SIZE (5)
	0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
	0x09, 0x30,                    //     USAGE (X)
	0x09, 0x31,                    //     USAGE (Y)
	0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
	0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
	0x75, 0x08,                    //     REPORT_SIZE (8)
	0x95, 0x02,                    //     REPORT_COUNT (2)
	0x81, 0x06,                    //     INPUT (Data,Var,Rel)
	0xc0,                          //   END_COLLECTION
	0xc0                           // END_COLLECTION
};*/


int main(int argc, char **argv)
{
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		printf("socket failed!");
		exit(1);
	}

	struct sockaddr_in serveraddr;



	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("192.168.101.80");
	serveraddr.sin_port = htons(50000);

	int flag = 1;

	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));

	if (connect(fd, (struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		perror("connect failed!");
		exit(1);
	}

	


	Display *d;
	int s;
	Window w;
	XEvent e;

	d = XOpenDisplay(NULL);
	if (d == NULL)
	{
		printf("Cannot open display\n");
		exit(1);
	}
	s = DefaultScreen(d);

	/*XSetWindowAttributes wa;
	wa.override_redirect = True;
	w = XCreateWindow(d, RootWindow(d, s), 0, 0, 1280, 720, 1, CopyFromParent, CopyFromParent,
		CopyFromParent, CWOverrideRedirect, &wa);
	*/
	w = XCreateSimpleWindow(d, RootWindow(d, s), 0, 0, 800, 550, 1, BlackPixel(d, s), WhitePixel(d, s));




	Atom delWindow = XInternAtom( d, "WM_DELETE_WINDOW", 0 );
	XSetWMProtocols(d , w, &delWindow, 1);

	XSelectInput(d, w, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask  );

	XMapWindow(d, w);

	//XGrabKeyboard(d, DefaultRootWindow(d), True, GrabModeAsync, GrabModeAsync, CurrentTime);
	//XGrabPointer(d, DefaultRootWindow(d), True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync,
		//GrabModeAsync, None, None, CurrentTime);

	XWarpPointer(d, None, DefaultRootWindow(d), 0, 0, 0, 0, 400, 400);

	int oldx = 0, oldy = 0;
	int warp = 0;
	while(1) 
	{





		XNextEvent(d, &e);
		if (e.type == Expose)
		{
			;
		}
		else if (e.type == ClientMessage)
			break;
		else if (e.type == KeyPress)
		{
			int keyin = e.xkey.keycode;
			if (keyin == 37)
				keyboardinput[9] |= 1;
			else if (keyin == 50)
				keyboardinput[9] |= 1 << 1;
			else if (keyin == 64)
				keyboardinput[9] |= 1 << 2;
			else if (keyin == 133)
				keyboardinput[9] |= 1 << 3;
			else if (keyin == 105)
				keyboardinput[9] |= 1 << 4;
			else if (keyin == 62)
				keyboardinput[9] |= 1 << 5;
			else if (keyin == 108)
				keyboardinput[9] |= 1 << 6;
			else if (keyin == 135)
				keyboardinput[9] |= 1 << 7;
			else if (keyin < 128)
				keyboardinput[11] = usageid[keyin];
			else
				keyboardinput[11] = 0;
			printf("KeyPress:%d\n", keyin);



			printf("send:%d\n", send(fd, keyboardinput, sizeof(keyboardinput), 0));


		}
		else if (e.type == KeyRelease)
		{
			int keyin = e.xkey.keycode;
			if (keyin == 37)
				keyboardinput[9] &= ~1;
			else if (keyin == 50)
				keyboardinput[9] &= ~(1 << 1);
			else if (keyin == 64)
				keyboardinput[9] &= ~(1 << 2);
			else if (keyin == 133)
				keyboardinput[9] &= ~(1 << 3);
			else if (keyin == 105)
				keyboardinput[9] &= ~(1 << 4);
			else if (keyin == 62)
				keyboardinput[9] &= ~(1 << 5);
			else if (keyin == 108)
				keyboardinput[9] &= ~(1 << 6);
			else if (keyin == 135)
				keyboardinput[9] &= ~(1 << 7);
			else
				keyboardinput[11] = 0;
			printf("KeyRelease:%d\n", keyin);
			printf("send:%d\n", send(fd, keyboardinput, sizeof(keyboardinput), 0));
		}
		else if (e.type == ButtonPress)
		{
			int buttonnum = e.xbutton.button;
			printf("ButtonPress:%d\n", buttonnum);

			char mask;
			switch (buttonnum)
			{
			case 1:
				mask = 1;
				break;
			case 3:
				mask = 2;
				break;
			case 2:
				mask = 4;
				break;
			default:
				mask = 0;
			}
			mouseinput[9] |= mask;
			mouseinput[10] = 0;
			mouseinput[11] = 0;
			printf("send:%d\n", send(fd, mouseinput, sizeof(mouseinput), 0));



		}
		else if (e.type == ButtonRelease)
		{
			int buttonnum = e.xbutton.button;
			printf("ButtonRelease:%d\n", buttonnum);

			char mask;
			switch (buttonnum)
			{
			case 1:
				mask = 1;
				break;
			case 3:
				mask = 2;
				break;
			case 2:
				mask = 4;
				break;
			default:
				mask = 0;
			}
			mask = ~mask;
			mouseinput[9] &= mask;
			mouseinput[10] = 0;
			mouseinput[11] = 0;
			printf("send:%d\n", send(fd, mouseinput, sizeof(mouseinput), 0));
		}
		else if (e.type == MotionNotify)
		{
			int newx = e.xmotion.x;
			int newy = e.xmotion.y;
			int xdiff = newx - oldx;
			int ydiff = newy - oldy;
			oldx = newx;
			oldy = newy;

			printf("test:%d\n", e.xmotion.send_event);
			printf("test2:%d\n", e.xmotion.state);
			
			if (warp == 1)
			{
				warp = 0;
				continue;
			}
			if (newx < 128 || newx > 800-128 || newy < 128 || newy > 550-128)
			{
				XWarpPointer(d, None, DefaultRootWindow(d), 0, 0, 0, 0, 400, 300);
				warp = 1;
			}

			printf("MotionNotify:%d,%d\n", newx, newy);


			






			if (xdiff > 127)
				mouseinput[10] = 127;
			else if (xdiff < -128)
				mouseinput[10] = -128;
			else
				mouseinput[10] = xdiff;

			if (ydiff > 127)
				mouseinput[11] = 127;
			else if (ydiff < -128)
				mouseinput[11] = -128;
			else
				mouseinput[11] = ydiff;
			//printf("xdiff:%d,ydiff:%d\n", xdiff, ydiff);
			//for (int i = 0; i < sizeof(mouseinput); i++)
				//printf("%d,",mouseinput[i]);
			printf("send:%d\n", send(fd, mouseinput, sizeof(mouseinput), 0));

		}


	}
	close(fd);

	XDestroyWindow(d, w);

	XCloseDisplay(d);

   return 0;
 }

 