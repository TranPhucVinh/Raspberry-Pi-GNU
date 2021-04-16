#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include<string.h>
#include<curl/curl.h>
#include<wiringPi.h>
#include<pthread.h>

int delay_value = 1000;
int buttonState = 0;

//Datatype of return value from server
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
	pthread_t thread_curl;//pointer thread for HTTP client
	pthread_t thread_blink;//pointer thread for Blink LED
	pthread_t thread_button;//pointer thread for buttons

	int iret_curl;//result of HTTP client thread
	int iret_blink;//result of Blink LED thread
	int iret_button; //result of button thread

	curl_global_init(CURL_GLOBAL_ALL);

	wiringPiSetup();

	iret_curl = pthread_create(&thread_curl,NULL,func_thread_curl,NULL);//HTTP client thread
	iret_blink = pthread_create(&thread_blink,NULL,func_thread_blink,NULL);//Blink LED thread
	iret_button = pthread_create(&thread_button, NULL, func_thread_button, NULL);//Button thread

	printf("Result of HTTP client thread: %d\n",iret_curl);
	printf("Result of blink LED thread: %d\n",iret_blink);
	printf("Result of button thread: %d \n", iret_button);

	pthread_join(thread_curl,NULL);
	pthread_join(thread_blink,NULL);
	pthread_join(thread_button, NULL);

	curl_global_cleanup();
	exit(0);
}

//HTTP client thread
void *func_thread_curl(void *ptr){
	CURL *curl_handle;
	CURLcode res;
	char message[64];
	int frame_number = 0;
	int devSerialnumber = 1989;
	while(1){
		struct MemoryStruct chunk;
		chunk.memory = malloc(1);//Size of array will increase by the received data from server
		chunk.size = 0;
		snprintf(message,64,"serialnumber=%d&frame=%d&data=%d",devSerialnumber,frame_number, buttonState);
		frame_number++;
		printf("[CURL] message: %s\n",message);
		curl_handle = curl_easy_init();
		if(curl_handle){
			//Set up URL
			curl_easy_setopt(curl_handle,CURLOPT_URL,"http://192.168.0.116//get_post/get_post.php");

			//Timeout for curl: 10 second
			curl_easy_setopt(curl_handle,CURLOPT_TIMEOUT,10L);

			//Attach WRITEFUNCTION
			curl_easy_setopt(curl_handle,CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

			//Set received data from server to variable chunk
			curl_easy_setopt(curl_handle,CURLOPT_WRITEDATA,(void *)&chunk);

			//POST method
			curl_easy_setopt(curl_handle,CURLOPT_POSTFIELDS,message);

			//Send to server
			res = curl_easy_perform(curl_handle);

			if(res != CURLE_OK){
				printf("[CURL] curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			}
			else{
				printf("[CURL] Finish sending data\n");
				printf("[CURL] Return from server: [%d bytes] %s\n",chunk.size,chunk.memory);
			}
		}

		curl_easy_cleanup(curl_handle);

		//Free allocated RAM to update chunk
		free(chunk.memory);
		sleep(15); //Slepp 15 second
	}
}

//Blink LED thread
void *func_thread_blink(void *ptr){
	pinMode(0,OUTPUT);
	while(1){
		digitalWrite(0,HIGH);
		delay(1000);
		digitalWrite(0,LOW);
		delay(1000);
	}
}

//Button thread
void *func_thread_button(void *ptr){
	pinMode(21, INPUT);

	/*
		PUD_OFF: TURN OFF PULL UP and DOWN,
		PUD_DOWN: PULL DOWN,
		PUD_UP: PULL UP
	*/
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