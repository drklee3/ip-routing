/*****************************
 * COEN 146, Lab5, Link-State
 *****************************/
/*	Simon Liu 
 *	Coen 146 Lab Friday 2:15pm
 */


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>		//need library for srand()
#include <unistd.h>		//need library for sleep()
#include <limits.h>		//need library for INT_MAX
#define HOST_NUMBER 4		//assume the largest amount of machines are 4


/* Globals */
typedef struct{
	char name[50];
	char ip[50];
	int port;
}MACHINE;

//allow threads access to important information 
int host_number;
int machine_id;

MACHINE linux_machines[HOST_NUMBER];		//machine informations
int matrix[HOST_NUMBER][HOST_NUMBER];		//cost table

pthread_mutex_t lock;

/*	printTable
 *	Summery: Print out the cost table.
 */
void printTable(){
	int i;
	pthread_mutex_lock(&lock);
	for(i = 0; i < HOST_NUMBER; ++i)	//print out the entire given table even if there isn't all the machine running
		printf("%d\t%d\t%d\t%d\n", matrix[i][0], matrix[i][1], matrix[i][2], matrix[i][3]);
	pthread_mutex_unlock(&lock);
}

/*	minDistance
 *	Summery: Find the next vertix find the shortest distance.
 */
int minDistance(int *dist, int *done){	
	int min = INT_MAX;
	int min_pos, i;
	for(i = 0; i < host_number; ++i){		//only find the vertix of the running machines
		if(done[i] == 0 && dist[i] <= min){	//only use the vertix if it hasn't found the shortest path from source
			min = dist[i];
			min_pos = i;
		}
	}
	return min_pos;					//return the shortest vertix we should look at
}

/*	linkState
 *	Summery: Should run Dijkstra's Algorithm every 10~20 secs. Prints out the least cost from current machine to other machines. 
 */
void* linkState(void* arg){
	int i, j, min_loc;
	int dist[host_number];
	int done[host_number];
	while(1){
		sleep(10+(rand()%11));
		//Reset for Dijkstra
		for(i = 0; i < host_number; ++i){
			dist[i] = INT_MAX;
			done[i] = 0;
		}
		//Start with the current machine's vertix
		dist[machine_id] = 0;
		//Run Dijkstra's Algorithm 
		for(i = 0; i < host_number-1; ++i){
			min_loc = minDistance(dist, done);
			done[min_loc] = 1;			//Mark as done found shortest path to the vertix
			pthread_mutex_lock(&lock);		//need to lock mutex to access cost table
			for(j = 0; j < host_number; ++j){	//find all the shortest path from this vertix to other vertices
				if((done[j] == 0) && matrix[min_loc][j] && (dist[min_loc] != INT_MAX) && (dist[min_loc] + matrix[min_loc][j] < dist[j]))
					dist[j] = dist[min_loc] + matrix[min_loc][j];
			}
			pthread_mutex_unlock(&lock);		//unlock for other threads to access cost table
		}
		printf("\nLeast cost from machine %d to:\n", machine_id);
		for(i = 0; i < host_number; ++i){
			if(i != machine_id)
				printf("machine %d is %d\n", i , dist[i]);
		}
	}
			
}

/*	recieveInfo
 *	Summery: recieve cost table updates from other machines then output the new cost table
 */
void* receiveInfo(void* arg){
	int sock;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	//establish UDP server
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(linux_machines[machine_id].port);			//set the right port from the machine table
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);					//don't care where its sent from 
	memset((char *)serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
	addr_size = sizeof(serverStorage);
	
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("socket error\n");
	}
	if(bind(sock,(struct sockaddr *)&serverAddr, sizeof(serverAddr)) !=0){
		printf("bind error\n");
	}	
	//Start recieve messages sent to this ip & port 
	int new_cost[3]; 	//revceive message as an array
	while(1){
		recvfrom(sock,new_cost, sizeof(new_cost), 0, (struct sockaddr *)&serverStorage, &addr_size);
		pthread_mutex_lock(&lock);				//lock mutex to edit cost table
		matrix[new_cost[0]][new_cost[1]] = new_cost[2];
		matrix[new_cost[1]][new_cost[0]] = new_cost[2];
		pthread_mutex_unlock(&lock);				//unlock mutex to allow other threads' access
		printf("\nNew Cost Table:\n");
		printTable();
	}
}
/***********
 *  main
 ***********/				//Compile:	gcc -o lab5 lab5.c -lpthread
int main (int argc, char *argv[])	//Execution:	Machine_ID total_Machine Cost_File Machine_File
{
	int sock, i;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;		
	pthread_mutex_init(&lock,NULL);		//start the mutex lock 
	host_number = atoi(argv[2]);
	if (argc != 5)
	{
		printf ("missing argument(s)\n");
		return 1;
	}
	
	machine_id = atoi(argv[1]);		//set the machine_id to global
		
	/* Open the host_name file */
	FILE *fpHOST = fopen(argv[4], "rb");				//host machine name get from 2nd argument 
	FILE *fpCOST = fopen(argv[3], "rb");				//cost table file 
	if(fpHOST == NULL || fpCOST == NULL){				//if one of file fails to open, close program 
		printf("Fail to open host name file!!\n");
		return 0;
	}

	/* Creating the machine & cost table */
	for(i = 0; i < host_number; ++i){
		fscanf(fpHOST, "%s\t%s\t%d\n", linux_machines[i].name, linux_machines[i].ip, &linux_machines[i].port);	//copy the data in the given host_name file
		fscanf(fpCOST, "%d\t%d\t%d\t%d\n", &matrix[i][0], &matrix[i][1], &matrix[i][2], &matrix[i][3]);
	}
	/* Test if the host table worked */
/*	for(i = 0; i < host_number; ++i)
		printf("%s\t%s\t%d\n", &linux_machines[i].name, &linux_machines[i].ip, linux_machines[i].port);	
*/	
	fclose(fpHOST);			//close both given files 
	fclose(fpCOST);
	
	srand(time(NULL));	//initialize randomization
	
	/* Start the threads for linkState & receiveInfo */
	pthread_t thr_link, thr_receive;
	pthread_create(&thr_link, NULL, linkState, NULL);
	pthread_create(&thr_receive, NULL, receiveInfo, NULL);
	
	/* Get user input */
	int cost_update[3];
	cost_update[0] = machine_id;
	printf("Machine %d Initialized\n", machine_id);	//let user know the program is running
	while(1){
		sleep(10);	//get user input every 10 sec, don't care about 2 input every 30 sec
		printf("\nWaiting for new cost to specified machines(Format: machine_ID new_cost):\n");
		scanf("%d %d", &cost_update[1], &cost_update[2]);
		pthread_mutex_lock(&lock);
		matrix[cost_update[0]][cost_update[1]] = cost_update[2];			//updates cost table
		matrix[cost_update[1]][cost_update[0]] = cost_update[2];
		pthread_mutex_unlock(&lock);
		printf("\nNew cost table:\n");
		printTable();	
		for(i=0; i < host_number; ++i){	//need to send to all the machines
			//establish udp connection with the host table to get the ip and socket
			if(machine_id != i){
				//configure ip address
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(linux_machines[i].port);				//destination port number
				inet_pton(AF_INET, linux_machines[i].ip, &serverAddr.sin_addr.s_addr);		//destination ip addresss
				memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
				addr_size = sizeof serverAddr;
				//Create UDP Socket
				if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
					printf("socket error\n");
					return 1;
				}
				sendto(sock,cost_update, sizeof(cost_update), 0, (struct sockaddr *)&serverAddr, addr_size);	//send to destination, won't wait for confimation
			}
		}
	}
	return 0;	
}
