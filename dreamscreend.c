#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <math.h>

int red = -1;
int green;
int blue;

char *rgb_convert(double r, double g, double b) {
	r = r/255;
	g = g/255;
	b = b/255;

	double hue;
	double sat;
	double val;

	double max, min, delta;

	double X, Y, Z;
	double x, y;

	char *values = malloc(sizeof(char)*40);
	
	/* CALCULATE HSV */
	// determine Cmax
	if (r >= b && r >= g) 
		max = r;
	else if (g >= r && g >= b)
		max = g;
	else
		max = b;
	
	// determine Cmin
	if (r <= b && r <= g)
		min = r;
	else if (g <= r && g <= b)
		min = g;
	else
		min = b;

	// delta
	delta = max - min;

	// calculate hue
	if (delta == 0)
		hue = 0;
	else if (max == r)
		hue = ((int)(60*(((g-b)/delta)+6)))%360;
	else if (max == g)
		hue = ((int)(60*(((b-r)/delta) +2)))%360;
	else
		hue = ((int)(60*(((r-g)/delta)+4)))%360;

	// calculate saturation
	if (max == 0)
		sat = 0;
	else
		sat = delta/max;

	// calculate value
	val = max;

	/* CALCULATE XY */	
	if (r > 0.04045)
		r = (double)pow((r + 0.055) / (1.0 + 0.055), 2.4);
	else
		r = (double)(r / 12.92);
	
	if (g > 0.04045)
		g = (double)pow((g + 0.055) / (1.0 + 0.005), 2.4);
	else
		g = (double)(g / 12.92);

	if (b > 0.04045)
		b = (double)pow((b + 0.055) / (1.0 + 0.005), 2.4);
	else
		b = (double)(b / 12.92);

	X = (double) (r * 0.649926 + g * 0.103455 + b * 0.197109);
	Y = (double) (r * 0.234327 + g * 0.743075 + b * 0.022598);
	Z = (double) (r * 0.000000 + g * 0.053077 + b * 1.035763);

	x = X / (X + Y + Z);
	y = Y / (X + Y + Z);


	sprintf(values, "%0.0f %0.3f %0.3f %0.04f %0.04f", hue, sat, val, x, y);
	return values;
}

static size_t supress_response(void *buffer, size_t size, size_t nmemb, void *userp) {
	   return size * nmemb;
}

void use_api_put(char *url, char *json) {
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;
	
	curl = curl_easy_init();
	if(curl) {
		headers = curl_slist_append(headers, "Content-Type: application/json");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, supress_response);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);
	return;
}
bool volatile keep_running = true;

/* Signal callback */
void exit_handle(int sig) {
    keep_running = false;
}

/* CRC8 calculation */
unsigned char calcCRC8(unsigned char *packet) {
	unsigned char crcTable[] = {0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3};
	unsigned char crc = 0x00;
	int i, size;
	size = packet[1];
	
	for(i = 0; i <= size; i++){
		crc = crcTable[(packet[i] ^ crc) & 0xFF];
	}
	return crc;
}

/* packet assemblage */
void assemble_packet(unsigned char packet[], unsigned char prefix[], unsigned char upper_command, unsigned char lower_command){
	memcpy(packet, prefix, 5);
	packet[5] = upper_command;
	packet[6] = lower_command;
	packet[7] = calcCRC8(packet);
}


void assemble_packet_rgb(unsigned char packet[], unsigned char prefix[], unsigned char red, unsigned char green, unsigned char blue) {
	memcpy(packet, prefix, 5);
	packet[1] = 0x08;
	packet[5] = 0x05;
	packet[6] = red;
	packet[7] = green;
	packet[8] = blue;
	packet[9] = calcCRC8(packet);
}

int dream() {
	int i, sockfd, p;
	struct sockaddr_in serveraddr;
	struct hostent *server;
	unsigned char packet[8];
	unsigned char packet_rgb[10];
	struct sigaction act;
	
	int choice;	
	char *hostname;
	char *addresses[] = { "192.168.1.37", "192.168.1.161", "192.168.1.104" };
	int members = 3;
	int portno = 8888;

	unsigned char prefix[] = { 0xFC, 0x06, 0x01, 0x11, 0x03 };
	unsigned char special[] = { 0xFC, 0x05, 0xFF, 0x30, 0x01 };

	// Commands
	unsigned char mode = 0x01;
	unsigned char brightness = 0x02;
	unsigned char ambient_color = 0x05;
	unsigned char ambient_mode_type = 0x08;
	unsigned char ambient_scene = 0x0D;
	unsigned char input = 0x20;
	
	// Mode Payloads
	unsigned char mode_sleep = 0x00;
	unsigned char mode_video = 0x01;
	unsigned char mode_music = 0x02;
	unsigned char mode_ambient = 0x03;
	

	// Brightness Payload
	unsigned char brightness_value = 0x0A;
	
	// Ambient Scenes
	unsigned char test = 0x04;
	
	// Options for Input
	unsigned char input_hdmi_1 = 0x00;
	unsigned char input_hdmi_2 = 0x01;
	unsigned char input_hdmi_3 = 0x02;


	printf("Enter option number: ");
	if(scanf("%d", &choice) == 0)
		return -1;
       
	switch(choice) {
		case 1:
			assemble_packet(packet, prefix, mode, mode_sleep);
			break;
		case 2:
			assemble_packet(packet, prefix, mode, mode_video);
			break;
		case 3:
			assemble_packet(packet, prefix, mode, mode_music);
			break;
		case 4:
			assemble_packet(packet, prefix, mode, mode_ambient);
			break;
		case 5:
			assemble_packet(packet, prefix, input, input_hdmi_1);
			break;
		case 6:
			assemble_packet(packet, prefix, input, input_hdmi_2);
			break;
		case 7:
			assemble_packet(packet, prefix, input, input_hdmi_3);
			break;
		case 8:
			brightness_value = brightness_value + 10;
			if (brightness_value > 100)
				brightness_value = 10;
			assemble_packet(packet, prefix, brightness, brightness_value);
			break;
		case 9:
			if (brightness_value >= 20)
				brightness_value = brightness_value - 10;
			else
				brightness_value = 100;
			assemble_packet(packet, prefix, brightness, brightness_value);
			break;
		case 10:
			assemble_packet(packet, prefix, mode, mode_ambient);
			printf("Enter RGB Values: ");
			scanf("%d %d %d", &red, &green, &blue);
			assemble_packet_rgb(packet_rgb, prefix, red, green, blue);
			break;  
		case 11:
			assemble_packet(packet, special, 0x0A, 0x2A);
		default:
			bzero(packet, sizeof(packet));
        }

	for (i = 0; i < members; i++) {
	hostname = addresses[i];
	act.sa_handler = exit_handle;
	sigemptyset (&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT,  &act, 0);
	sigaction(SIGTERM, &act, 0);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("ERROR: cannot open socket");
		exit(EXIT_FAILURE);
	}
	
	/* get Dreamscreen DNS entry */
	server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr,"ERROR: no such host as %s\n", hostname);
		exit(EXIT_FAILURE);
	}

	/* build Dreamscreen address */
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
	(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);

	/* send packet to Dreamscreen */
	p = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr));	
	if (choice == 10) 
		p = sendto(sockfd, packet_rgb, sizeof(packet_rgb), 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr));	
	if (p < 0)
		perror("ERROR: in sendto");
	fflush(stdout);
	close(sockfd);
	}
	return 0;
}


int main(int argc, char **argv) {

	char json_buffer[1000];
	char *nanoleaf_url = "http://192.168.1.32:16021/api/v1/Wl2aru89oF8d7eket1cqqsv0fyEcgYhc/state";
	char *hue_url = "http://192.168.1.249/api/mIi8OwbG-lHvUx7SdXEBSK2aILxguUNQOUK1Cayl/groups/1/action";

	double h, s, v, x, y;	
	int shift;
	char *values;
	dream();
	if (red != -1) {
		values = rgb_convert(red, green, blue);
		sscanf(values, "%lf %lf %lf %lf %lf", &h, &s, &v, &x, &y);
		//sprintf(json_buffer, "{\"on\":true, \"sat\":%0.0f, \"bri\":%0.0f, \"xy\":[%0.4f, %0.4f]}", (s*255)-1, (v*255)-1, x, y);
		sprintf(json_buffer, "{\"on\":true, \"sat\":%0.0f, \"bri\":%0.0f, \"hue\":%0.0f}", (s*255)-1, (v*255)-1, (h/360)*65535);
		use_api_put(hue_url, json_buffer);
		sprintf(json_buffer, "{\"on\": {\"value\":true}, \"hue\":{\"value\":%0.0f}, \"sat\":{\"value\":%0.0f}, \"brightness\":{\"value\":%0.0f}}", h, s*100, v*100);
		use_api_put(nanoleaf_url, json_buffer);
	}
	
	return 0;
}
