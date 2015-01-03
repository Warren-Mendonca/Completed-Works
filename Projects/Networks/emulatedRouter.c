/**
 * @wmendonc_assignment3
 * @author  Warren Mendonca <wmendonc@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/global.h"
#include "../include/logger.h"
#include <unistd.h>
#include <sys/types.h> 
#include<sys/time.h>
#include <netinet/in.h>
#include<string.h> //strcmp,memset
#include<sys/socket.h>
#include<errno.h> //For errno - the error number ********** test if required
#include<netdb.h> //hostent
#include<arpa/inet.h> //For itoa and intop

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log();

	/*Clear LOGFILE and DUMPFILE*/
	fclose(fopen(LOGFILE, "w"));
	fclose(fopen(DUMPFILE, "wb"));

	/*Start Here*/


	/*variables*/
	int i,j;
	int rInt;
	int numNeigh=0;
	FILE *fr;
	int rdCount=0;	
	static int packets;

	//Flag to check if the program has run an update or disable(stores server id)
	static int dbFlag=0;
	static int upFlag=0;


	//Variables from file	
	uint16_t numUp;
	int numNb;
	uint16_t serverId;
	
	/************************************************************/
	/*********Structure for maintaining routing table************/
	/************************************************************/
	
	struct neigh{
		int timeOutCt;
		int packTrue;
		int serverId;	//dest server
		int nextServer; //next hop
		char ip[100];
		int serverPort;
		uint16_t cost;
		int nbFlag;
		int onePack;
		uint16_t iCost;

	};

	struct neigh nb[5];
	for (i=0;i<5;i++){
		nb[i].timeOutCt=0;
		nb[i].packTrue=0;
		nb[i].serverId=i+1;
		nb[i].nbFlag=0;
		nb[i].onePack=0;
		nb[i].cost=65535;
		nb[i].nextServer=-1;
			
	}

	/************************************************************/
	/****Routing packet information data structure***************/
	/************************************************************/

	struct rPack{
		
		uint16_t serverId;
		uint16_t cost;
		uint16_t serverPort;
		uint8_t ipOne;
		uint8_t ipTwo;
		uint8_t ipThree;
		uint8_t ipFour;
	};
		
	//Create packets equal to number of nodes
	struct rPack packet[5];


			
	//Initialize the values to -1
			
	for(i=0;i<5;i++){
		packet[i].serverId=-1;
		packet[i].cost=65535;
		packet[i].serverPort=-1;
		packet[i].ipOne=-1;
		packet[i].ipTwo=-1;
		packet[i].ipThree=-1;
		packet[i].ipFour=-1;
		//printf("cost = %d",packet[i].cost);
	}			


	/*Code start */
	rInt=atoi(argv[4]);
	printf("\nInterval entered: %d\n",rInt);	

	/*retreive topology filename*/
	// Open the file
	char *filename=argv[2];
						
	fr = fopen(filename,"r"); 
	if (!fr) 
	{
		perror("Failed to open file, Please enter filename with full path\n");
	}

	
	//Use get line to get line
	size_t buffer_size = 80;
    	char *buffer = malloc(buffer_size * sizeof(char));
	
	
	// read each line and retreive information from topology file

	int line_number = 0;
	while(-1 != getline(&buffer, &buffer_size, fr))
	{
   	     	printf("%d: %s", ++line_number, buffer);
		//Get number of servers

		if (line_number==1){
			numUp=atoi(buffer);
		}
		
		

		//Get number of neighbours

		else if(line_number==2){
			numNb=atoi(buffer);
		}
		
		//Get the ip and port for all servers

		else if((line_number>2)&&(line_number<=numUp+2)){
			
			


			//Retrieve server information from the next lines in file
			char *p;
  			char *array[10];
			int index=line_number-3;
			
			i=0;
			
			
			p = strtok (buffer," ");
			
			while (p != NULL)
  			{
    				array[i++] = p;
  				p = strtok (NULL, " ");
  			}  
  			
			packet[index].serverId=atoi(array[0]);
			packet[index].serverPort=atoi(array[2]);
			for(i=0;i<5;i++){
				if(nb[i].serverId==atoi(array[0])){
					strcpy(nb[i].ip,array[1]);
					nb[i].serverPort=atoi(array[2]);
				}
			}
	
			//printf("server id =%d and server port =%d\n",packet[index].serverId,packet[index].serverPort);
			//Tokenize IP and save in a 4 byte char array

			i=0;
			char *pip;
			//char *arrayip[10]
			//printf("\n Array 1 value :: %s\n",array[1]);
			pip = strtok (array[1],".");
			
	
			while (pip != NULL)
  			{
    				//printf("Token %s",pip);
				if(i==0){
					packet[index].ipOne=atoi(pip);
				}
				if(i==1){
					packet[index].ipTwo=atoi(pip);
				}
				if(i==2){
					packet[index].ipThree=atoi(pip);
				}
				if(i==3){
					packet[index].ipFour=atoi(pip);
				}
				pip = strtok (NULL, ".");
				
				i++;
				
  			}
			

		}

		//Get server id and cost to neigbbours

		else {
			char *pc;
  			char *token[10];
			
			i=0;
			pc = strtok (buffer," ");
			
			while (pc != NULL)
  			{
    				token[i++] = pc;
  				pc = strtok (NULL, " ");
  			}
			
			//Get server id for current server
			serverId=atoi(token[0]);
			printf("Value of server id :: %d \n",serverId);
			//Get cost for neighbours
			for(i=0;i<numUp;i++){
				if(packet[i].serverId==atoi(token[1])){
					packet[i].cost=atoi(token[2]);

					//Mark server as neighbour and store cost, and make next hop as the server
					for(j=0;j<5;j++){
						if(nb[j].serverId==atoi(token[1])){
							nb[j].nbFlag=1;
							nb[j].nextServer=nb[j].serverId;
							nb[j].cost=atoi(token[2]);
							nb[j].iCost=nb[j].cost;		
							numNeigh++;
						}
					}
					
				}
				else if (packet[i].serverId==serverId){
					packet[i].cost=0;
				}
			}
				
		}

    	}
	for(i=0;i<numUp;i++){
		if(nb[i].serverId==serverId){
			nb[i].cost=0;
			nb[i].nextServer=serverId;
		}
	}
	
	printf("\n Value of num up = %d\n",numUp);

	/************************************************************/
	/**** Nb Routing packet information data structure **********/
	/************************************************************/
	//Holds server and cost
	struct info{
			uint16_t serverId;
			uint16_t cost;
	};

	
	struct nbRPack{
		uint16_t rServerId;
		struct info rIn[numUp];		
	
	};	

	struct nbRPack nbPack[numNeigh];	
	
	for(j=0;j<numNeigh;j++){
		nbPack[j].rServerId=0;			
	}
	int k,l;

	//Initialize values for neighbour routing tables

	for (i=0;i<5;i++){
		if(nb[i].nbFlag==1){
			for(j=0;j<numNeigh;j++){
				if(nbPack[j].rServerId==0){
					nbPack[j].rServerId=nb[i].serverId;
					for(k=0;k<numUp;k++){
						nbPack[j].rIn[k].serverId=k+1;
						if(nbPack[j].rIn[k].serverId==nbPack[j].rServerId){ 
							nbPack[j].rIn[k].cost=0; 						//Cost 0 as cost to self
						}
						else if(nbPack[j].rIn[k].serverId==serverId){
							for(l=0;l<numUp;l++){
								if(nb[l].serverId==nbPack[j].rServerId){
									nbPack[j].rIn[k].cost=nb[l].cost;		//Cost as cost of link from topology 
								}
							}
						}
						else{
							nbPack[j].rIn[k].cost=65535; 					//Else infinity till we get a packet from this node
						}
					}
					break;
				}
			}
		}
	}

	//Print value in nb routing table structure 
	for(j=0;j<numNeigh;j++){
		printf("Neighbour in neibour routing table :: %d\n",nbPack[j].rServerId);			
	}
	




	//printf("Value of serverid and ip in nb astructure\n");

	//for(i=0;i<5;i++){
		//printf("\nValue of server id:: %d Value of ip %s\n",nb[i].serverId,nb[i].ip);
	//}

	/************************************************************************/
	/******************************* Print Packet ***************************/
	/************************************************************************/
	printf("\nPacket values ::::\n");

	for (i=0;i<numUp;i++){
		printf("\n---------------------------------------------------------\n");
		printf("Server id :: %d Server cost :: %d server Port :: %d",packet[i].serverId,packet[i].cost,packet[i].serverPort);
		printf("\nIp address = %d.%d.%d.%d\n",packet[i].ipOne,packet[i].ipTwo,packet[i].ipThree,packet[i].ipFour);	
	}
	

	
    	fflush(stdout);

	
	// finished
    	fclose(fr);
    	free(buffer);

	/**************************************************************************/
	/******************************Start server logic**************************/
	/**************************************************************************/
	int sockfd,err;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
   	char mesg[1000];
	int thisServerPort;
	char thisServerIp[10];
	char temp[10];
	fd_set readfds;
	struct timeval tv;
	char command[100];

	//Message buffer size
	int mSize;

	mSize=(12*numUp)+8;
	
	//create message buffer of exact length
	char message[mSize];	

   	printf("Size of message buffer =%d",sizeof(message));

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if (sockfd==-1){
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
        	exit(1);
	}

	//retrieve this servers details

	for (i=0;i<5;i++){
		if(packet[i].serverId==serverId){
			thisServerPort=packet[i].serverPort;
			bzero(thisServerIp,0);
			bzero(temp,0);
			sprintf(temp,"%d",packet[i].ipOne);
			strcpy(thisServerIp,temp);
			strcat(thisServerIp,".");
			sprintf(temp,"%d",packet[i].ipTwo);
			strcat(thisServerIp,temp);
			strcat(thisServerIp,".");
			sprintf(temp,"%d",packet[i].ipThree);
			strcat(thisServerIp,temp);
			strcat(thisServerIp,".");
			sprintf(temp,"%d",packet[i].ipFour);
			strcat(thisServerIp,temp);
			//strcat(thisServerIp," "); //Remove comment only for checking value
								 //ipton requires no blank space at end
			
		}
	}
	
	//printf("\n\n This server IP :: %s This server Port :: %d",thisServerIp,thisServerPort);	
	
   	bzero(&servaddr,sizeof(servaddr));
   	servaddr.sin_family = AF_INET;

	
	err=inet_pton(AF_INET,thisServerIp,&(servaddr.sin_addr));
	if(err==0){
		printf("invalid IP address\n");
	}
	else if (err==-1){
		printf("Error with error no %d",errno);
	}

   	//servaddr.sin_addr.s_addr=inet_addr(thisServerIp);
   	servaddr.sin_port=htons(thisServerPort);
   	
	err=bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	if (err == -1) {
     		fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
     		exit(1);
     	}
	else{
		printf("\nObtaining bind successful.\n");
	}

	
	tv.tv_sec=rInt;
	tv.tv_usec=0;
	static int memLoc=8;
		
	while (1) {
		FD_ZERO(&readfds);          /* initialize the fd set */
	    	FD_SET(sockfd, &readfds); /* add socket fd */
	    	FD_SET(0, &readfds);        /* add stdin fd (0) */
		err=select(sockfd+1, &readfds, 0, 0, &tv);
		if (err < 0) {
	     		error("ERROR in select");
	    	}		
		if (FD_ISSET(0, &readfds)) {
			printf("READING from stdin\n");
			gets(command);
			
			char *p;
  			char *array[10];
			for (i=0;i<10;i++){
				array[i]=NULL;
			}
			i = 0;
  			p = strtok (command," ");  
  			
			while (p != NULL)
  			{
    				array[i++] = p;
  				p = strtok (NULL, " ");
  			}
			/******************************************************************************************************/
			/****************************       disable COMMAND    ************************************************/
			/******************************************************************************************************/
			
			if ((strcmp(array[0],"disable")==0)||(strcmp(array[0],"DISABLE")==0)){
				
				int isNb=0;
				if(array[1]!=NULL){
					
					uint16_t dServerId=atoi(array[1]);
					for(i=0;i<numUp;i++){
						if(nb[i].serverId==dServerId){
							isNb=nb[i].nbFlag;
						}
					}
					if(isNb!=0){
						dbFlag=dServerId;
					
						for(i=0;i<numUp;i++){
							if(nb[i].serverId==dServerId){
								nb[i].nbFlag=0;
								nb[i].cost=65535;
								nb[i].iCost=65535;
								
								nb[i].nextServer=-1;
								
							}
							if(packet[i].serverId==dServerId){
								packet[i].cost=65535;
							}
						}
						//Reset cost value for all neighbours
						for(i=0;i<numUp;i++){
							if(nb[i].nbFlag==1){
								nb[i].cost=nb[i].iCost;
								nb[i].nextServer=nb[i].serverId;
		
								for(j=0;j<numUp;j++){
									if(packet[j].serverId==nb[i].serverId){
										packet[i].cost=nb[i].cost;
									}
								}
							}
		
						}
						//Mark routing table entries for this entry as infinite
						for(i=0;i<numNeigh;i++){
							if(nbPack[i].rServerId==dServerId){
								for(j=0;j<numUp;j++){
									nbPack[i].rIn[j].cost=65535;
								}
							}
						}
		
						/*************************************************************************/
						/**************************** Apply formula ******************************/
						/*************************************************************************/
						uint16_t cost1;
						uint16_t cost2;
						static uint16_t cost;
						uint16_t possNext;
						uint16_t defNext;
						
						int check;
			
						for(i=0;i<numUp;i++){
						
						
						
							if(nb[i].serverId!=serverId){
								
								printf("Iteration for destination server id %d\n",nb[i].serverId);
								printf("-----------------------------------------\n");	
								//Initialize cost for this iteration
								cost=65535;
		
								for(j=0;j<numNeigh;j++){
									possNext=nbPack[j].rServerId;
									
								
									for(k=0;k<numUp;k++){
										if(nb[k].serverId==nbPack[j].rServerId){
											cost1=nb[k].cost;
											//printf("Initial cost to server %d from this server ::cost1 %d\n",nbPack[j].rServerId,cost1);
											break;	
										}
									}
									for(k=0;k<numUp;k++){
										if(nbPack[j].rIn[k].serverId==nb[i].serverId){
											cost2=nbPack[j].rIn[k].cost;
											//printf("Cost from server %d to %d = %d\n",nbPack[j].rServerId,nb[i].serverId,cost2);
											break;
										}
									}
								
									if((cost2!=65535)&&(cost1!=65535)){			//If no path from server to destination and if cost to this server is not marked as infinite
										check=cost1+cost2;
										if (check>65535){
											cost1=65535;					//Count reaches infinity cannot assign greater value to uint_16
										}
										else{
											cost1=cost1+cost2; 				//add cost to reach this sever to cost from this server to destination
										}
										//printf("Total cost from this server to %d plus cost from that server to = %d\n",nb[i].serverId,cost1);	
		
									}
									else{
										cost1=65535;		//If the received data is also inf that means it cannot reach this destination
									}
			
									//Find if this path or previous one was shorter
									if(cost<cost1){
											printf("No change in path\n");
									}
									else if (cost>cost1){
										printf("Change in path for server Id %d!!\n",nb[i].serverId);
										//printf("Possinle New cost is %d and possible next hop is %d \n",cost1,possNext);
										defNext=possNext;
										cost=cost1;
						
									}
									else {
										//Same no change required ***************************** MAY HAVE TO CHANGE WHEN UPDATE TO INFINITY
									}
							
								}
								for(j=0;j<numUp;j++){
									if((nb[j].serverId==nb[i].serverId)&&(nb[j].cost!=cost)){
										nb[j].cost=cost;
										nb[j].nextServer=defNext;
									}
									if((packet[j].serverId==nb[i].serverId)&&(packet[j].cost!=cost)){
										packet[j].cost=cost;
									}
								}
							} 
						}
						//Check if any link has lost its route completely
						for(i=0;i<numUp;i++){
							if((nb[i].cost==65535)&&(nb[i].nbFlag!=1)){	//Check if it has completely lost path to some server mark next server as -1
								nb[i].nextServer=-1;
							}
						}
						cse4589_print_and_log("%s:SUCCESS\n",array[0]);
					}
					else{
						char err[100]="BAD COMMAND ENTERED :: Please ensure value of link to be disabled is a neighbour";
				
						cse4589_print_and_log("%s:%s\n",array[0],err);
						fflush(stdout);	
				
					}
				}
				else{
					char err[100]="BAD COMMAND ENTERED :: Please enter value of link to be disabled";
				
					cse4589_print_and_log("%s:%s\n",array[0],err);
					fflush(stdout);	
				}
				

			}
			/******************************************************************************************************/
			/*******************************      UPDATE COMMAND    ************************************************/
			/******************************************************************************************************/
			
			else if ((strcmp(array[0],"update")==0)||(strcmp(array[0],"UPDATE")==0)){
				
				
				if((array[1]!=NULL)&&(array[2]!=NULL)&&(array[3]!=NULL)){
					
					uint16_t dServerId=atoi(array[2]);
					upFlag=atoi(array[2]);
					int nwCost=0;	
				
					if(strcmp(array[3],"inf")==0){
						printf("User entered inf as update value");
						nwCost=65535;			
					}
					else{
						nwCost=atoi(array[3]);			
					}
					if(nwCost!=0){
						for(i=0;i<numUp;i++){
							if(nb[i].serverId==dServerId){
								nb[i].cost=nwCost;
								nb[i].iCost=nwCost;			//modify initial cost of the link as the new value as well
								nb[i].nextServer=dServerId;
								
							}
							if(packet[i].serverId==dServerId){
								packet[i].cost=nwCost;
							}
						}
						//Reset cost value for all neighbours other than the modified one
						for(i=0;i<numUp;i++){
							if((nb[i].nbFlag==1)&&(nb[i].serverId!=dServerId)){
								nb[i].cost=nb[i].iCost;
								nb[i].nextServer=nb[i].serverId;
			
								for(j=0;j<numUp;j++){
									if(packet[j].serverId==nb[i].serverId){
										packet[i].cost=nb[i].cost;
									}
								}
							}
		
						}
						//Mark routing table entries for the edited server entry as new cost and the rest as initial value
						for(i=0;i<numNeigh;i++){
							if(nbPack[i].rServerId==dServerId){
								for(j=0;j<numUp;j++){
									if(nbPack[i].rIn[j].serverId==serverId){
										nbPack[i].rIn[j].cost=nwCost;
									}
								}
							}
						}
	
						/*************************************************************************/
						/**************************** Apply formula ******************************/
						/*************************************************************************/
						uint16_t cost1;
						uint16_t cost2;
						static uint16_t cost;
						uint16_t possNext;
						uint16_t defNext;
					
						int check;
		
						for(i=0;i<numUp;i++){
							
							if(nb[i].serverId!=serverId){
							
								printf("Iteration for destination server id %d\n",nb[i].serverId);
								printf("-----------------------------------------\n");	
								//Initialize cost for this iteration
								cost=65535;
			
								for(j=0;j<numNeigh;j++){
									possNext=nbPack[j].rServerId;
									
									
									for(k=0;k<numUp;k++){
										if(nb[k].serverId==nbPack[j].rServerId){
											cost1=nb[k].cost;
											//printf("Initial cost to server %d from this server ::cost1 %d\n",nbPack[j].rServerId,cost1);
											break;	
										}
									}
									for(k=0;k<numUp;k++){
										if(nbPack[j].rIn[k].serverId==nb[i].serverId){
											cost2=nbPack[j].rIn[k].cost;
											//printf("Cost from server %d to %d = %d\n",nbPack[j].rServerId,nb[i].serverId,cost2);
											break;
										}
									}
								
									if((cost2!=65535)&&(cost1!=65535)){			//If no path from server to destination and if cost to this server is not marked as infinite
										check=cost1+cost2;
										if (check>65535){
											cost1=65535;					//Count reaches infinity cannot assign greater value to uint_16
										}
										else{
											cost1=cost1+cost2; 				//add cost to reach this sever to cost from this server to destination
										}
										//printf("Total cost from this server to %d plus cost from that server to = %d\n",nb[i].serverId,cost1);	
		
									}
									else{
										cost1=65535;		//If the received data is also inf that means it cannot reach this destination
									}
			
									//Find if this path or previous one was shorter
									if(cost<cost1){
											printf("No change in path\n");
									}
									else if (cost>cost1){
										printf("Change in path for server Id %d!!\n",nb[i].serverId);
										//printf("Possinle New cost is %d and possible next hop is %d \n",cost1,possNext);
										defNext=possNext;
										cost=cost1;
						
									}
									else {
										
									}
							
								}
								for(j=0;j<numUp;j++){
									if((nb[j].serverId==nb[i].serverId)&&(nb[j].cost!=cost)){
										nb[j].cost=cost;
										nb[j].nextServer=defNext;
									}
									if((packet[j].serverId==nb[i].serverId)&&(packet[j].cost!=cost)){
										packet[j].cost=cost;
									}
								}
							} 
						}
						//Check if any link has lost its route completely
						for(i=0;i<numUp;i++){
							if((nb[i].cost==65535)&&(nb[i].nbFlag!=1)){		//Check if it has completely lost path to some server mark next server as -1
								nb[i].nextServer=-1;		
							}
						}
					
			
						cse4589_print_and_log("%s:SUCCESS\n",array[0]);
					}
					else{
						char err[100]="BAD COMMAND ENTERED :: Zero is an invalid link cost";
					
						cse4589_print_and_log("%s:%s\n",array[0],err);
						fflush(stdout);
					}
				}

				else{
					char err[100]="BAD COMMAND ENTERED :: Please ensure all inputs are entered";
				
					cse4589_print_and_log("%s:%s\n",array[0],err);
					fflush(stdout);	
				}
			}

			/******************************************************************************************************/
			/*******************************      CRASH COMMAND    ************************************************/
			/******************************************************************************************************/
			
			else if ((strcmp(array[0],"crash")==0)||(strcmp(array[0],"CRASH")==0)){
				for (i=0;i<numUp;i++){
					nb[i].nbFlag=0;		//Stop sending updates on all links an infinitely loop
				}
				cse4589_print_and_log("%s:SUCCESS\n",array[0]);
				for(i=1;i>0;i++){
					//do nothing
				}
				
			}		
			/******************************************************************************************************/
			/****************************       display COMMAND    ************************************************/
			/******************************************************************************************************/
			
			else if ((strcmp(array[0],"display")==0)||(strcmp(array[0],"DISPLAY")==0)){
				
				//Print routing table
				printf("Destination\tNext Hop\tCost\n");
				for(i=0;i<numUp;i++){
					cse4589_print_and_log("%-15d%-15d%-15d\n",nb[i].serverId,nb[i].nextServer,nb[i].cost);

				}
				cse4589_print_and_log("%s:SUCCESS\n",array[0]);
				
				fflush(stdout);
					
			}

			/**************************************************************************/
			/***************************STEP COMMAND***********************************/
			/**************************************************************************/
			else if ((strcmp(array[0],"step")==0)||(strcmp(array[0],"STEP")==0)){
				
				//Create routing message
				bzero(message,mSize);
				
				uint16_t temp;
				temp=htons(numUp);
				memcpy(message,&temp,2);
				for(i=0;i<5;i++){
					if(packet[i].serverId==serverId){
						temp=htons(packet[i].serverPort);
						memcpy(message+2,&temp,2);	
						memcpy(message+4,&(packet[i].ipOne),1);	
						memcpy(message+5,&(packet[i].ipTwo),1);	
						memcpy(message+6,&(packet[i].ipThree),1);	
						memcpy(message+7,&(packet[i].ipFour),1);	
					}
				}
				
				//Add server details in sorted order
				for(i=0;i<5;i++){
					for(j=0;j<5;j++){
						if(packet[j].serverId==i+1){
							memcpy(message+memLoc,&(packet[i].ipOne),1);
							memLoc++;
							memcpy(message+memLoc,&(packet[i].ipTwo),1);
							memLoc++;
							memcpy(message+memLoc,&(packet[i].ipThree),1);
							memLoc++;
							memcpy(message+memLoc,&(packet[i].ipFour),1);
							memLoc++;
							temp=htons(packet[i].serverPort);
							memcpy(message+memLoc,&temp,2);
							memLoc=memLoc+4; //Taking into consideration 2 bytes for padding
							temp=htons(packet[i].serverId);
							memcpy(message+memLoc,&temp,2);
							memLoc=memLoc+2;
							temp=htons(packet[i].cost);
							memcpy(message+memLoc,&temp,2);
							memLoc=memLoc+2; 
						}
					}
				}
			
				uint16_t tem;

				memcpy(&tem,message,2);
				//printf("\nValue of retrieved mesage = %d\n",tem);
				fflush(stdout);
				int numbytes;
				int clsock;
			
				for(i=0;i<5;i++){
					if(nb[i].nbFlag==1){
						//Set client details
						clsock=socket(AF_INET,SOCK_DGRAM,0);
						if(clsock==-1){
							printf("\nFailed to create client socket\n");
						}
						cliaddr.sin_family = AF_INET;
						cliaddr.sin_port = htons(nb[i].serverPort);
						err=inet_pton(AF_INET,nb[i].ip,&(cliaddr.sin_addr));
						if(err==0){
							printf("\nInvalid IP address for client\n");
						}
						else if (err==-1){
							printf("Error with error no %d",errno);
						}
	
						//SEND update   						
						numbytes=sendto(clsock,message,mSize,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
						if(numbytes<0){
							printf("\nError sending to %d",nb[i].serverId);
							printf("Error with error no %d",errno);
						}
						else{
							printf("\nNum bytes sent to server id %d :: %d",nb[i].serverId,numbytes);
						}
	
					}
				}

				//Reset Timer and memory location tracker
				memLoc=8;
				tv.tv_sec=rInt;
				tv.tv_usec=0;
				cse4589_print_and_log("%s:SUCCESS\n",array[0]);
				fflush(stdout);	
			}
			/******************************************************************************************************/
			/****************************       dump COMMAND    ************************************************/
			/******************************************************************************************************/
			
			else if ((strcmp(array[0],"dump")==0)||(strcmp(array[0],"DUMP")==0)){
				
			
				//Create routing message
				bzero(message,mSize);
				uint16_t temp;
				temp=htons(numUp);
				memcpy(message,&temp,2);
				for(i=0;i<5;i++){
					if(packet[i].serverId==serverId){
						temp=htons(packet[i].serverPort);
						memcpy(message+2,&temp,2);	
						memcpy(message+4,&(packet[i].ipOne),1);	
						memcpy(message+5,&(packet[i].ipTwo),1);	
						memcpy(message+6,&(packet[i].ipThree),1);	
						memcpy(message+7,&(packet[i].ipFour),1);	

					}
				}
			
				//Add server details in sorted order
				for(i=0;i<5;i++){
					for(j=0;j<5;j++){
						if(packet[j].serverId==i+1){
							memcpy(message+memLoc,&(packet[i].ipOne),1);
							memLoc++;
							memcpy(message+memLoc,&(packet[i].ipTwo),1);
							memLoc++;
							memcpy(message+memLoc,&(packet[i].ipThree),1);
							memLoc++;
							memcpy(message+memLoc,&(packet[i].ipFour),1);
							memLoc++;
							temp=htons(packet[i].serverPort);
							memcpy(message+memLoc,&temp,2);
							memLoc=memLoc+4; //Taking into consideration 2 bytes for padding
							temp=htons(packet[i].serverId);
							memcpy(message+memLoc,&temp,2);
							memLoc=memLoc+2;
							temp=htons(packet[i].cost);
							memcpy(message+memLoc,&temp,2);
							memLoc=memLoc+2; 
						}
					}
				}

				cse4589_dump_packet(message,mSize);
				memLoc=8;
				cse4589_print_and_log("%s:SUCCESS\n",array[0]);
					
			}

			/******************************************************************************************************/
			/****************************       packets COMMAND    ************************************************/
			/******************************************************************************************************/
			
			else if ((strcmp(array[0],"packets")==0)||(strcmp(array[0],"PACKETS")==0)){
				
				cse4589_print_and_log("\nThe number of packets received by this router = %d\n",packets);
				packets=0;
				cse4589_print_and_log("%s:SUCCESS\n",array[0]);
				
			}
			
			/******************************************************************************************************/
			/****************************       Academic integrity COMMAND    ************************************************/
			/******************************************************************************************************/
			
			else if ((strcmp(array[0],"academic_integrity")==0)||(strcmp(array[0],"ACADEMIC_INTEGRITY")==0)){
				cse4589_print_and_log("%s:SUCCESS\n",array[0]);
				
				
				cse4589_print_and_log("I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");

				fflush(stdout);
			}
			/******************************************************************************************************/
			/********************************       BAD COMMAND    ************************************************/
			/******************************************************************************************************/
			
			else {
				char err[100]="BAD COMMAND ENTERED :: Please check if the command is valid";
				
				cse4589_print_and_log("%s:%s\n",array[0],err);
				
			}
			
		}
		else if (FD_ISSET(sockfd, &readfds)) { 
			printf("Something arrived on listener\n");
			int numbytes;
			len = sizeof(cliaddr);
      		uint16_t rNumUp;			//Received Number of update fields
			uint16_t rServerPort;			//Received server port
			uint8_t rIpOne;
			uint8_t rIpTwo;
			uint8_t rIpThree;
			uint8_t rIpFour;
			uint16_t rServerId;
			uint16_t temp;
			
			struct rPack rpacket[5]; 	//Structure to maintain received packet
			bzero(message,mSize);

			//Receive message
			numbytes = recvfrom(sockfd,message,mSize,0,(struct sockaddr *)&cliaddr,&len);
      		if(numbytes<0){
				printf("Error receiving packet");
			}
			else{
				printf("\nNum bytes Received :: %d",numbytes);
			}
			
			//Get packet structure from the received message
			memcpy(&temp,message,2);
			rNumUp=ntohs(temp);

			memcpy(&temp,message+2,2);	
			rServerPort=ntohs(temp);

			memcpy(&(rIpOne),message+4,1);	
			memcpy(&(rIpTwo),message+5,1);	
			memcpy(&(rIpThree),message+6,1);	
			memcpy(&(rIpFour),message+7,1);	
			
			
			printf("\nReceived message details::\n");
			printf("Num updates::%d Server port = %d IP address = %d.%d.%d.%d\n",rNumUp,rServerPort,rIpOne,rIpTwo,rIpThree,rIpFour);


			
			//Add server details in sorted order
			for(i=0;i<5;i++){
				memcpy(&(rpacket[i].ipOne),message+memLoc,1);
				memLoc++;
				memcpy(&(rpacket[i].ipTwo),message+memLoc,1);
				memLoc++;
				memcpy(&(rpacket[i].ipThree),message+memLoc,1);
				memLoc++;
				memcpy(&(rpacket[i].ipFour),message+memLoc,1);
				memLoc++;
				memcpy(&temp,message+memLoc,2);
				rpacket[i].serverPort=ntohs(temp);
				memLoc=memLoc+4; //Taking into consideration 2 bytes for padding
				memcpy(&temp,message+memLoc,2);
				rpacket[i].serverId=ntohs(temp);
				memLoc=memLoc+2; 
				memcpy(&temp,message+memLoc,2);
				rpacket[i].cost=ntohs(temp);
				memLoc=memLoc+2; 
				
			}
			
			printf("\nPrinting updates received :: \n");
			for(i=0;i<5;i++){
				if(rpacket[i].cost==0){
					rServerId=rpacket[i].serverId;
				}
				printf("Server Id %d, Server Port %d Server Ip %d.%d.%d.%d Cost to server %d \n",rpacket[i].serverId,rpacket[i].serverPort,rpacket[i].ipOne,rpacket[i].ipTwo,rpacket[i].ipThree,rpacket[i].ipFour,rpacket[i].cost);
			}
			
			int z;
			int tmVal;
			for(z=0;z<numUp;z++){
				if((nb[z].serverId==rServerId)&&(nb[z].nbFlag==1)&&(dbFlag==0)&&(upFlag==0)){
					/*************increment packets and set packet received flag****************/
					packets++;
				
					for(i=0;i<numUp;i++){
						if(nb[i].serverId==rServerId){
							nb[i].packTrue=1;
							tmVal=nb[i].timeOutCt;
							nb[i].timeOutCt=0;
							nb[i].onePack=1;			//Server has sent at least one packet
						}
					}
					/***************************************************************************/
		
					/***********************************************************************/
					cse4589_print_and_log("RECEIVED A MESSAGE FROM SERVER %d\n",rServerId);	
	
				
					for(i=0;i<numUp;i++){
						cse4589_print_and_log("%-15d%-15d\n",rpacket[i].serverId,rpacket[i].cost);
					}
	
					/***********************************************************************/
					//Put received value in neighbour routing info table
					for(i=0;i<numNeigh;i++){
						if(nbPack[i].rServerId==rServerId){
							for(j=0;j<numUp;j++){
								nbPack[i].rIn[j].serverId=rpacket[j].serverId;
								nbPack[i].rIn[j].cost=rpacket[j].cost;
							}	
						}
					}
					//Check if neighbour has modified cost between us meaning it now takes a new route
					int nbCost;
					int nbPackCost;
					for(i=0;i<numUp;i++){
						if(nb[i].serverId==rServerId){
							nbCost=nb[i].cost;
						}
					}
					for(i=0;i<numNeigh;i++){
						if(nbPack[i].rServerId==rServerId){
							for(j=0;j<numUp;j++){
								if(nbPack[i].rIn[j].serverId==serverId){
									nbPackCost=nbPack[i].rIn[j].cost;
								}
							}
						}				
					}
					if(nbCost!=nbPackCost){
						//printf("Value of nb cost %d value of nb Pack cost %d\n",nbCost,nbPackCost);
						//printf("************************************************************\n");
						printf("Neighbour cost change detected!.Set to received cost");
						for(i=0;i<numUp;i++){
							if(nb[i].serverId==rServerId){
								nb[i].cost=nb[i].iCost;
							}
						}
					}

					//Print neighbour routing tables stored here
					for(i=0;i<numNeigh;i++){
						printf("Routing table for %d\n",nbPack[i].rServerId);
						for(j=0;j<numUp;j++){
							printf("ServerId %d Cost %d\n",nbPack[i].rIn[j].serverId,nbPack[i].rIn[j].cost);
						}
					}

					/*************************************************************************/
					/**************************** Apply formula ******************************/
					/*************************************************************************/
					uint16_t cost1;
					uint16_t cost2;
					static uint16_t cost;
					uint16_t possNext;
					uint16_t defNext;
				
					int check;
		
					for(i=0;i<numUp;i++){
						
					
					
						if(nb[i].serverId!=serverId){
						
							printf("Iteration for destination server id %d\n",nb[i].serverId);
							printf("-----------------------------------------\n");	
							//Initialize cost for this iteration
							cost=65535;
	
							for(j=0;j<numNeigh;j++){
								possNext=nbPack[j].rServerId;
								
								
								for(k=0;k<numUp;k++){
									if(tmVal>=3){
										if(nbPack[j].rIn[k].serverId==serverId){
											cost1=nbPack[j].rIn[k].cost;
											//printf("Initial cost to server %d from this server ::cost1 %d\n",nbPack[j].rServerId,cost1);
											break;
										}
									}
									else{
										if(nb[k].serverId==nbPack[j].rServerId){
											cost1=nb[k].cost;
											//printf("Initial cost to server %d from this server ::cost1 %d\n",nbPack[j].rServerId,cost1);
											break;
										}	
									}
								}
								for(k=0;k<numUp;k++){
									if(nbPack[j].rIn[k].serverId==nb[i].serverId){
										cost2=nbPack[j].rIn[k].cost;
										//printf("Cost from server %d to %d = %d\n",nbPack[j].rServerId,nb[i].serverId,cost2);
										break;
									}
								}
							
								if((cost2!=65535)&&(cost1!=65535)){		//If no path from server to destination and if cost to this server is not marked as infinite
									check=cost1+cost2;
									if (check>65535){
										cost1=65535;
									}
									else{
										cost1=cost1+cost2; 				//add cost to reach this sever to cost from this server to destination
									}
									//printf("Total cost from this server to %d plus cost from that server to = %d\n",nb[i].serverId,cost1);
	
								}
								else{
									cost1=65535;		//If the received data is also inf that means it cannot reach this destination
								}
		
								//Find if this path or previous one was shorter
								if(cost<cost1){
										printf("No change in path\n");
								}
								else if (cost>cost1){
									printf("Change in path for server Id %d!!\n",nb[i].serverId);
									//printf("Possinle New cost is %d and possible next hop is %d \n",cost1,possNext);
									defNext=possNext;
									cost=cost1;
				
								}
								else {
									
									//if(cost2!=0){				//This is some new path and not the same server
										printf("Change in path for server Id %d!!\n",nb[i].serverId);
										//printf("Possinle New cost is %d and possible next hop is %d \n",cost1,possNext);
										defNext=possNext;
										cost=cost1;
									//}
									for (k=0;k<numUp;k++){		//New path may already have an entry in table
										if((nb[k].serverId==nbPack[j].rServerId)&&(nb[k].nbFlag==1)){
											defNext=nb[k].nextServer;
										}
									}
									
								}
							
							}
							for(j=0;j<numUp;j++){
								if((nb[j].serverId==nb[i].serverId)){
									nb[j].cost=cost;
									nb[j].nextServer=defNext;
								}
								if((packet[j].serverId==nb[i].serverId)){
									packet[j].cost=cost;
								}
							}
						} 
					}
					//Check if any link has lost its route completely or if a next hop is infinite
					for(i=0;i<numUp;i++){
						if((nb[i].cost==65535)&&(nb[i].nbFlag!=1)){	//Check if it has completely lost path to some server mark next server as -1
							nb[i].nextServer=-1;
						}
						else if((nb[i].cost==65535)&&(nb[i].nbFlag==1)){
							nb[i].nextServer=nb[i].serverId;
						}
					}
						
				}
			}
			fflush(stdout);
			memLoc=8;
			if(dbFlag!=0){
				/*************increment packets and set packet received flag****************/
				packets++;
			
				for(i=0;i<numUp;i++){
					if(nb[i].serverId==rServerId){
						nb[i].packTrue=1;
						nb[i].timeOutCt=0;
						nb[i].onePack=1;			//Server has sent at least one packet
					}
				}
				/***************************************************************************/
				/***********************************************************************/
				cse4589_print_and_log("RECEIVED A MESSAGE FROM SERVER %d\n",rServerId);	
				
				for(i=0;i<numUp;i++){
					cse4589_print_and_log("%-15d%-15d\n",rpacket[i].serverId,rpacket[i].cost);
				}
				/***********************************************************************/
	
			}
			if(upFlag!=0){
				/*************increment packets and set packet received flag****************/
				packets++;
			
				for(i=0;i<numUp;i++){
					if(nb[i].serverId==rServerId){
						nb[i].packTrue=1;
						nb[i].timeOutCt=0;
						nb[i].onePack=1;			//Server has sent at least one packet
					}
				}
				/***************************************************************************/
	
				/***********************************************************************/
				cse4589_print_and_log("RECEIVED A MESSAGE FROM SERVER %d\n",rServerId);	
				
				for(i=0;i<numUp;i++){
					cse4589_print_and_log("%-15d%-15d\n",rpacket[i].serverId,rpacket[i].cost);
				}
				/***********************************************************************/
	
			}
			

		}
		else if(err==0){
			printf("Timeout occured :: Sending updates\n");
			printf("-----------------------------------\n");

			
			/***********************************************************************************************/
			/*****Check if updates were received for all 'active' neighbours if not increment timeout Ct****/
			/***********************************************************************************************/

			for(i=0;i<numUp;i++){
				if((nb[i].nbFlag==1)&&(nb[i].onePack==1)){			// For all entries who are neighbours an those who have sent at least one packet
					if(nb[i].packTrue==0){	
						nb[i].timeOutCt++;	//Increment if no packet received
					}
					else if(nb[i].packTrue==1){
						nb[i].packTrue=0;	//Reset the flag if update received
					}
				}
				
			}

			for(i=0;i<numUp;i++){
				if(nb[i].nbFlag==1){
					//If timeout Ct is 3 then mark cost as infinity and next hop as -1
					if(nb[i].timeOutCt>=3){
						printf("\nNo update received from %d\n",nb[i].serverId);
						printf("------------------------------\n");
						nb[i].cost=65535;
						nb[i].iCost=65535;	
						nb[i].nextServer=-1;
						nb[i].nbFlag=0; 

						//change value in packet structure
						for(j=0;j<numUp;j++){
							if(packet[j].serverId==nb[i].serverId){
								packet[j].cost=65535;
							}
						}
												
						//reset neighbour link costs
						for(k=0;k<numUp;k++){
							if((nb[k].nbFlag==1)&&(nb[k].cost!=65535)){
								nb[k].cost=nb[k].iCost;
								nb[k].nextServer=nb[k].serverId;
								
								//Reset in packets structure
								for(l=0;l<numUp;l++){
									if(packet[l].serverId==nb[k].serverId){
										packet[l].cost=nb[k].cost;
									}
								}
							}
						
						}
						
						//Mark Entries in neighbour routing info table for this server as infinite
						for(j=0;j<numNeigh;j++){
							if(nbPack[j].rServerId==nb[i].serverId){
								for(k=0;k<numUp;k++){
									nbPack[j].rIn[k].cost=65535;
								}
							}
						}

					}
				}
			}

			/*************************************************************************/
			/**************************** Apply formula ******************************/
			/*************************************************************************/
			uint16_t cost1;
			uint16_t cost2;
			static uint16_t cost;
			uint16_t possNext;
			uint16_t defNext;
			
			int check;

			for(i=0;i<numUp;i++){
				
				
				
				if(nb[i].serverId!=serverId){
					
					printf("Iteration for destination server id %d\n",nb[i].serverId);
					printf("-----------------------------------------\n");	
					//Initialize cost for this iteration
					cost=65535;

					for(j=0;j<numNeigh;j++){
						possNext=nbPack[j].rServerId;
						
						
						for(k=0;k<numUp;k++){
							if(nb[k].serverId==nbPack[j].rServerId){
								cost1=nb[k].cost;
								//printf("Initial cost to server %d from this server ::cost1 %d\n",nbPack[j].rServerId,cost1);
								break;	
							}
						}
						for(k=0;k<numUp;k++){
							if(nbPack[j].rIn[k].serverId==nb[i].serverId){
								cost2=nbPack[j].rIn[k].cost;
								//printf("Cost from server %d to %d = %d\n",nbPack[j].rServerId,nb[i].serverId,cost2);
								break;
							}
						}
						
						if((cost2!=65535)&&(cost1!=65535)){		//If no path from server to destination and if cost to this server is not marked as infinite
							check=cost1+cost2;
							if (check>65535){
								cost1=65535;
							}
							else{
								cost1=cost1+cost2; 				//add cost to reach this sever to cost from this server to destination
							}				
							//printf("Total cost from this server to %d plus cost from that server to = %d\n",nb[i].serverId,cost1);

						}
						else{
							cost1=65535;		//If the received data is also inf that means it cannot reach this destination
						}

						//Find if this path or previous one was shorter
						if(cost<cost1){
								printf("No change in path\n");
						}
						else if (cost>cost1){
							printf("Change in path for server Id %d!!\n",nb[i].serverId);
							//printf("Possinle New cost is %d and possible next hop is %d \n",cost1,possNext);
							defNext=possNext;
							cost=cost1;
			
						}
						else {
							for (k=0;k<numUp;k++){
								if((nb[k].serverId==nbPack[j].rServerId)&&(nb[k].nbFlag==1)){
									defNext=nb[k].nextServer;
								}
							}
						}
					
					}
					for(j=0;j<numUp;j++){
						if((nb[j].serverId==nb[i].serverId)&&(nb[j].cost!=cost)){
							nb[j].cost=cost;
							nb[j].nextServer=defNext;
						}
						if((packet[j].serverId==nb[i].serverId)&&(packet[j].cost!=cost)){
							packet[j].cost=cost;
						}
					}
				} 
			}
	
			//Check if any link has lost its route completely
			for(i=0;i<numUp;i++){
				if((nb[i].cost==65535)&&(nb[i].nbFlag!=1)){	//Check if it has completely lost path to some server mark next server as -1
					nb[i].nextServer=-1;
				}
				else if((nb[i].cost==65535)&&(nb[i].nbFlag==1)){
					nb[i].nextServer=nb[i].serverId;
				}
			}
						
			/**************************************************************************/
			/***************************Send message***********************************/
			/**************************************************************************/
			//Create routing message
			bzero(message,mSize);
			
			uint16_t temp;
			temp=htons(numUp);
			memcpy(message,&temp,2);
			for(i=0;i<5;i++){
				if(packet[i].serverId==serverId){
					temp=htons(packet[i].serverPort);
					memcpy(message+2,&temp,2);	
					memcpy(message+4,&(packet[i].ipOne),1);	
					memcpy(message+5,&(packet[i].ipTwo),1);	
					memcpy(message+6,&(packet[i].ipThree),1);	
					memcpy(message+7,&(packet[i].ipFour),1);	
				}
			}
			
			//Add server details in sorted order
			for(i=0;i<5;i++){
				for(j=0;j<5;j++){
					if(packet[j].serverId==i+1){
						memcpy(message+memLoc,&(packet[i].ipOne),1);
						memLoc++;
						memcpy(message+memLoc,&(packet[i].ipTwo),1);
						memLoc++;
						memcpy(message+memLoc,&(packet[i].ipThree),1);
						memLoc++;
						memcpy(message+memLoc,&(packet[i].ipFour),1);
						memLoc++;
						temp=htons(packet[i].serverPort);
						memcpy(message+memLoc,&temp,2);
						memLoc=memLoc+4; //Taking into consideration 2 bytes for padding
						temp=htons(packet[i].serverId);
						memcpy(message+memLoc,&temp,2);
						memLoc=memLoc+2;
						temp=htons(packet[i].cost);
						memcpy(message+memLoc,&temp,2);
						memLoc=memLoc+2; 
					}
				}
			}
			
			uint16_t tem;

			memcpy(&tem,message,2);
			//printf("\nValue of retrieved mesage = %d\n",tem);
			fflush(stdout);
			int numbytes;
			int clsock;
			
			for(i=0;i<5;i++){
				if(nb[i].nbFlag==1){
					//Set client details
					clsock=socket(AF_INET,SOCK_DGRAM,0);
					if(clsock==-1){
						printf("\nFailed to create client socket\n");
					}
					cliaddr.sin_family = AF_INET;
					cliaddr.sin_port = htons(nb[i].serverPort);
					err=inet_pton(AF_INET,nb[i].ip,&(cliaddr.sin_addr));
					if(err==0){
						printf("\nInvalid IP address for client\n");
					}
					else if (err==-1){
						printf("Error with error no %d",errno);
					}

					//SEND update   						
					numbytes=sendto(clsock,message,mSize,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					if(numbytes<0){
						printf("\nError sending to %d",nb[i].serverId);
						printf("Error with error no %d",errno);
					}
					else{
						printf("\nNum bytes sent to server id %d :: %d",nb[i].serverId,numbytes);
					}

				}
			}

			//Reset Timer and memory location tracker
			memLoc=8;
			tv.tv_sec=rInt;
			tv.tv_usec=0;
			fflush(stdout);
			if(upFlag!=0){
				
				upFlag=0;
			}
			if(dbFlag!=0){
				dbFlag=0;
			}
		}

	}



	return 0;
}
