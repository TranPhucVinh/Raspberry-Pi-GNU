
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<ctype.h> 
#include<string.h>
#include<curl/curl.h> 
#include<wiringPi.h> 
#include<pthread.h>

int delay_value;
int buttonState = 0;
/*Kieu du lieu nhan phan hoi*/
struct MemoryStruct{
	char *memory;
	size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,void *userp){
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL){
		/*out of memory*/
		printf("Error: not enough memory (realloc return NULL)\n");
		return 0;
	}
	memcpy(&(mem->memory[mem->size]),contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	return realsize;
}
void *func_thread_curl(void *ptr);
void *func_thread_blink(void *ptr);
void *func_thread_button (void *ptr);

int main(void){
	pthread_t thread_curl;//Con tro luong gui du lieu
	pthread_t thread_blink;//Con tro luong chop led
	pthread_t thread_button;//Con tro luong button

	int iret_curl;//Bien ket qua thuc thi luong gui du lieu
	int iret_blink;//Bien ket qua thuc thi luong chop led
	int iret_button; //Bien ket qua thuc thi luong button

	//Khoi tao CURL toan cuc
	curl_global_init(CURL_GLOBAL_ALL);

	wiringPiSetup();

	iret_curl = pthread_create(&thread_curl,NULL,func_thread_curl,NULL);//Luong gui du lieu
	iret_blink = pthread_create(&thread_blink,NULL,func_thread_blink,NULL);//Luong chop LED
	iret_button = pthread_create(&thread_button, NULL, func_thread_button, NULL); //Luong button

	printf("Ket qua tao luong gui du lieu: %d\n",iret_curl);
	printf("Ket qua tao luong chop LED: %d\n",iret_blink);
	printf("Ket qua tao luong button: %d \n", iret_button);

	pthread_join(thread_curl,NULL);
	pthread_join(thread_blink,NULL);
	pthread_join(thread_button, NULL);

	//Xoa CURL toan cuc truoc khi thoat
	curl_global_cleanup();
	exit(0);
}

/*Ham thuc thi gui du lieu*/
void *func_thread_curl(void *ptr){
	CURL *curl_handle;//Khoi tao con tro curl
	CURLcode res;
	char message[64];
	int frame_number = 0;
	int devSerialnumber = 1989;
	while(1){
		struct MemoryStruct chunk;
		chunk.memory = malloc(1);/*kich thuoc mang se tang theo du lieu nhan duoc*/
		chunk.size = 0;
		snprintf(message,64,"serialnumber=%d&frame=%d&data=%d",devSerialnumber,frame_number, buttonState);
		frame_number++;
		printf("[CURL] message: %s\n",message);
		curl_handle = curl_easy_init();//Khoi dong con tro curl
		if(curl_handle){
			//Thiet lap duong dan len Server
			curl_easy_setopt(curl_handle,CURLOPT_URL,"http://192.168.0.27/get_post/get_post.php");
			//Thiet lap Timeout cho curl 10 giay
			curl_easy_setopt(curl_handle,CURLOPT_TIMEOUT,10L);
			//Gan ham nhan du lieu tu server
			curl_easy_setopt(curl_handle,CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
			//Dua du lieu nhan duoc vao bien chunk
			curl_easy_setopt(curl_handle,CURLOPT_WRITEDATA,(void *)&chunk);
			//Dua data vao curl
			curl_easy_setopt(curl_handle,CURLOPT_POSTFIELDS,message);
			//Gui data len server
			res = curl_easy_perform(curl_handle);
			if(res != CURLE_OK){
				printf("[CURL] curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			}
			else{
				printf("[CURL] Gui du lieu thanh cong\n");
				printf("[CURL] Phan hoi tu server: [%d bytes] %s\n",chunk.size,chunk.memory);
				char temp[16];
				int j=0;
				for (int i=3;i<chunk.size;i++){
					if(chunk.memory[i]=='#') break;
					temp[j] = chunk.memory[i];
					j++;
				}
				j = atoi(temp);
				if (j>0) delay_value = j;
				printf("[CURL] Delay value = %d \n", delay_value);
			}
		}
		/*Luon reset con tro CURL*/
		curl_easy_cleanup(curl_handle);
		/*Giai phong vung RAM da cap phat cho bien chunk*/
		free(chunk.memory);
		//Dinh ky 15 giay gui 1 lan
		sleep(15);
	}
}

/*Ham thuc thi chop LED*/
void *func_thread_blink(void *ptr){
	pinMode(0,OUTPUT);
	while(1){
		digitalWrite(0,HIGH);
		delay(delay_value);
		digitalWrite(0,LOW);
		delay(delay_value);
	}
}

void *func_thread_button(void *ptr){
	pinMode(21, INPUT);

	//PUD_OFF: TAT PULL UP.DOWN, PUD_DOWN: PULL DOWN, PUD_UP: PULL UP
	pullUpDnControl(21, PUD_DOWN);
	while(1){
		if(digitalRead(11)){
			buttonState = 1;
		}
		else {
			buttonState = 0;
		}
	//printf("Button state = %d \n", buttonState);
		delay(100);
	}
}
