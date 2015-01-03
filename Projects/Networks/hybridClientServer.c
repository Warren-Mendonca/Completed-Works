/********************************************************************/
//Author :Warren Alexander Mendonca
//UBIT NAME:wmendonc Person number: 50134835
//Person number: 
/********************************************************************/
#include<stdio.h> //printf	
#include<stdlib.h> //for exit(0);
#include <unistd.h>
#include <sys/types.h> 
#include<sys/time.h>
#include <netinet/in.h>
#include<string.h> //strcmp,memset
#include<sys/socket.h>
#include<errno.h> //For errno - the error number ********** test if required
#include<netdb.h> //hostent
#include<arpa/inet.h> //For itoa and intop
#include<signal.h> 

#define LENGTH 100
#define SERVERPORT 9292


struct serverList
{
	int id;
	char hostname[1023];
	char ip_addr[100];
	int port_num;
	int sock_fd;
};

struct clientList
{
	int id;
	char hostname[1023];
	char ip_addr[100];
	int port_num;
	int sock_fd;
};

struct stats
{
	int id;
	char host1[25];
	char host2[25];
	double upTime;
	int numUp;
	double dwTime;
	int numDw;	
};
static int clientSockFd;
static int connectSockFd;

static int regFlag;
static int client_socket[4]; //Used to hold SD values of connected clients
static char *token[30];
static char myhost[1024]; 
char ip_addr[100];
int port_num;
int bytesRead;
int host_id;

void getIp() 
{
     /*Variable declaration*/
	char* GoogleIp = "8.8.8.8";
    	int Port = 53;
    	struct sockaddr_in serv;
    	struct sockaddr_in name;
    	int sock, err;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
    		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
    		exit(1);
	}
     	else 
	      printf("Obtaining socket descriptor successfully.\n");
	
	memset(&serv, 0, sizeof(serv));
    
    	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(GoogleIp);
    	serv.sin_port = htons(Port);

    	err = connect(sock, (struct sockaddr *) &serv, sizeof(serv));
    	if (sock == -1) {
    		fprintf(stderr, "ERROR: Failed to Connect. (errno = %d)\n", errno);
    		exit(1);
	}
     	else 
	      printf("Connected successfully.\n");
	
    	socklen_t namelen = sizeof(name);
    	
	err = getsockname(sock, (struct sockaddr *) &name, &namelen);
    	if (err == -1) {
    		fprintf(stderr, "ERROR: Failed to getsockname. (errno = %d)\n", errno);
    		exit(1);
	}
     	else 
	      printf("Getsock name successful.\n");


    	inet_ntop(AF_INET, &name.sin_addr, ip_addr, 99);
    	//IP ADDRESS
	printf("IP address:%s\n", ip_addr);


    	close(sock);
}

void creator(){
	printf("\nUBIT name : wmendonc UBemail: wmendonc@buffalo.edu\n");
	printf("\nI have read and understood the course academic integrity policy at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");
	printf("\n");
}

void help(){
	printf("\n CREATOR: DISPLAY CREATOR INFORMATION");
	printf("\n MYIP: DISPLAY IP ADDRESS OF THIS PROCESS");
	printf("\n MYPORT : DISPLAY PORT ON WHICH THIS MACHINE IS LISTENING FOR CONNECTIONS");
	printf("\n REGISTER <server ip> <port> : USED BY CLIENT TO REGISTER WITH SERVER");
	printf("\n CONNECT <destination> <port> : CONNECT TO A CLIENT PEER");
	printf("\n LIST : DISPLAY A LIST OF ALL ACTIVE CONECTIONS");
	printf("\n TERMINATE <connection id> : TERMINATE CONNECTION AND MENTION CONNECTION ID");
	printf("\n EXIT : TERMINATE ALL CONNECTIONS");
	printf("\n UPLOAD <connection_id> <file_name> : UPLOAD A FILE TO THE MENTIONED CONNECTIONS ID");
	printf("\n DOWNLOAD <connection_id1> <file_name1> <connection_id2> <file_name2> <connection_id3> <file_name3> : DOWNLOAD FILES FROM MENTIONED SERVERS\nNOTE: FILE CAN BE DOWNLOADED FROM 1,2 or 3 SERVERS");
	printf("\n STATISTICS : FOR SERVER:GETS UPDATES STATISTICS FOR ALL SERVERS\nFOR CLIENT : DISPLAY CLIENT STATISTICS");
	printf("\n");
}

int regClient(int socket_desc, char **argv){
	/********************************************************************************************/
	/*****************************************Client specific***********************************/
	/********************************************************************************************/
	int err,i,max=0,j;
	struct sockaddr_in server;
	char message[2000];
	char *p;
	
	//printf("\n\nValue of serveripand port %s %s\n\n", argv[1], argv[2]);
	
	
	struct serverList l[5]; // Structure holding list values
	server.sin_addr.s_addr = inet_addr(argv[1]);
    	server.sin_family = AF_INET;
    	server.sin_port = htons(atoi(argv[2]));
	/*
	for (i=0;i<4;i++){
	
		if(max<client_socket[i])
		{
			max=client_socket[i];
			clientSockFd=max+1;
			//printf("Value of sock fd in loop %d",clientSockFd);
		}
		
	}*/
	
	
	/********************************************************************/	
	/********** Populate the list with inital value for server **********/
	/********************************************************************/
	for (i=0;i<5;i++){
		l[i].id=0;
		strcpy(l[i].hostname,"--");
		strcpy(l[i].ip_addr,"--");
		l[i].port_num=0;
		l[i].sock_fd=0;
	}	
	

	//Create socket
    	clientSockFd = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    	if (clientSockFd == -1)
    	{
    	    printf("Could not create socket");
    	}
	
	printf("Inside register\n");
	//Connect to remote server
	err=connect(clientSockFd , (struct sockaddr *)&server , sizeof(server));
    	if (err < 0)
    	   	    fprintf(stderr, "ERROR: Failed to Connect. (errno = %d)\n", errno);
    	else{
    		printf("Connection to server sucessful\n");	
		printf("\n");	
		
		//printf("Value of portnum %d",port_num);
		sprintf(message, "%d", port_num);
		//printf  ("Port num to be sent is : %s\n", message);
		printf("\n");
		
		err=send(clientSockFd , message , strlen(message) , 0);
		if( err < 0)
    		   fprintf(stderr, "ERROR: Failed to Send. (errno = %d)\n", errno);

    		else 
			printf("Send port number to server suceeded\n");

		err = recv(clientSockFd, message, 2000,0);
      	if (err < 0) 
			fprintf(stderr, "ERROR: Failed to receive for checking activity on server connected socket. (errno = %d)\n", errno);
      	else {
			message[err]='\0';
      		//printf("\nMessage is %s\n",message);
			printf("\n");
			if(strcmp(message,"DUP")==0)
				clientSockFd=-1;
			else if(strcmp(message,"FUL")==0)
				clientSockFd=-1;
			else{
				//Add clienSockFd to list of sockets
				for (i = 0; i < 4; i++)
           			{
           				//if position is empty
           				if( client_socket[i] == -1 )
           				{
               				client_socket[i] = clientSockFd;
						//printf("Client  socket fd added at %d after register",i);
           	     				break;
           				}
           			}
				//Tokenize received message
				i = 0;
  				p = strtok (message," ");  
  				while (p != NULL)
  				{
    					token[i++] = p;
  					p = strtok (NULL, " ");
  				}
				token[i]='\0';
				//printf("Done tokenizing");
				i=0;
				while(token[i]!=NULL){
					//printf("\nvalue of token %d is %s",i,token[i]);
					i++;
				}
				
				i=0;
				//Add to list structure
				for (j=0;j<5;j++){
					l[j].id=atoi(token[i++]); // string to int
					strcpy(l[j].hostname,token[i++]);
					strcpy(l[j].ip_addr, token[i++]);
					l[j].port_num=atoi(token[i++]);
					if (token[i]==NULL)
					break;
				}
				i=0; //Re-declaration;
				printf("ID\tHOSTNAME\tIPADDRESS\tPORT\n\n");
				for (i=0;i<5;i++){
					printf("\n%d\t%s\t%s\t%d\t",l[i].id,l[i].hostname,l[i].ip_addr,l[i].port_num);
				}
			}
		}		
		
		
	}
	return(clientSockFd);
}


int connectToClient(char **argv, struct serverList l[5]){
	/********************************************************************************************/
	/*****************************************Client specific***********************************/
	/********************************************************************************************/
	printf("\n\n\n INSIDE Connect \n\n\n");
	int err,i,max=0,j;
	struct sockaddr_in peer;
	char message[2000];
	char *p;
	
	for (j=1;j<5;j++){
					
		if(strcmp(argv[1],l[j].hostname)==0){
			argv[1]=l[j].ip_addr;
			break;
		}
	}
	//printf("\n\nValue of server/ip and port %s %s\n\n", argv[1], argv[2]);
	
	//printf("value of port = %d from htons atoi",atoi(argv[2]));
	
	peer.sin_addr.s_addr = inet_addr(argv[1]);
    	peer.sin_family = AF_INET;
    	peer.sin_port = htons(atoi(argv[2]));
	
	
	//Create socket
    	connectSockFd = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    	if (connectSockFd == -1)
    	{
    	    printf("Could not create socket");
    	}
	//printf("Value of connect sock fd =%d",connectSockFd);

	//Connect to remote server
	err=connect(connectSockFd , (struct sockaddr *)&peer , sizeof(peer));
    	if (err < 0)
    	   	    fprintf(stderr, "ERROR: Failed to Connect. (errno = %d)\n", errno);
    	else{
    		printf("Connection to peer sucessful\n");	
		printf("\n");	
		
		//printf("Value of portnum %d",port_num);
		sprintf(message, "%d", port_num);
		//printf  ("Port num to be sent is : %s\n", message);
		printf("\n");
		
		err=send(connectSockFd , message , strlen(message) , 0);
		if( err < 0)
    		   fprintf(stderr, "ERROR: Failed to Send. (errno = %d)\n", errno);

    		else 
			printf("Send port number to server suceeded\n");

		err = recv(connectSockFd, message, 2000,0);
      	if (err < 0) 
			fprintf(stderr, "ERROR: Failed to receive for checking activity on server connected socket. (errno = %d)\n", errno);
      	else {
			message[err]='\0';
      		//printf("\nMessage is %s\n",message);
			printf("\n");
			if(strcmp(message,"DUP")==0)
				connectSockFd=-1;
			else if(strcmp(message,"FUL")==0)
				connectSockFd=-1;
			else if(strcmp(message,"SUC")==0){
				//Add connectSockFd to list of sockets
				for (i = 0; i < 4; i++)
           			{
           				//if position is empty
           				if( client_socket[i] == -1 )
           				{
               				client_socket[i] = connectSockFd;
						printf("Peer  socket fd added at %d after connect success",i);
           	     				break;
           				}
           			}//end for

				return(connectSockFd);

			}//end success
		
		}//end no message error else		
		
			
	}
	return(-1);	
}



void server (char type, char **argv){

	/*Variable delaration*/
	char command[1024]; //For user command
	int size=3; //For size of list structure bound to 3 
	int i=0,j=0; //Counter variable
	int n,dupFlag;
	int fullFlag;
	int termSock;
	//char type=*argv[1];
	int argc=0;
	char buf[2000]; /* message buffer */
	char temp[10];
	fd_set readfds;
	int sock,clsock;     // Master socket descriptors
	
	int sd_child;		//socket descriptor to handle connections already established
	int max_sd;		//Used in select to handle multiple connections

	
	int yes=1;        // for setsockopt() SO_REUSEADDR	
	int gai_result; // For result of getaddrinfo()
	char hostname[1024]; //For hostname
	hostname[1023] = '\0';
	
	//Initialize child socket list to zero
	//Value 3 is the maximum connections for this server (total 4)
	for (i = 0; i < 4; i++) 
    	{
    	    client_socket[i] = -1;
    	}
      
			
	/*Structures*/
	struct serverList l[5]; // Structure holding list values
	struct stats ss[30],sc[30];
	struct addrinfo hints, *info, *p; //addrinfo structure
	struct sockaddr_in *sa, sin; //IPV4
	struct hostent *he;
	struct sockaddr_in6 *sa6; // IPV6
		

	printf("\n\nThis is a : %c\n", type); // check for command line entered
	
	gethostname(hostname, 1023);
	printf("Hostname: %s\n", hostname);


	/*Using modern getaddrinfo and sockaddr*/
	/*set hints parameters to call specific addrinfo*/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	/*getaddrinfo + standard error check*/
	/****************check with second parameter null********************************************/
	if ((gai_result = getaddrinfo(hostname, argv[2], &hints, &info)) != 0) {
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
	    exit(1);
	}
	
	
	/********************************************************************/	
	/********** Populate the list with inital value for server **********/
	/********************************************************************/
	for (i=0;i<5;i++){
		l[i].id=0;
		strcpy(l[i].hostname,"--");
		strcpy(l[i].ip_addr,"--");
		l[i].port_num=0;
		l[i].sock_fd=0;
	}	
	//Set id of server
	l[0].id=1;

	/********************************************************************/	
	/********** Populate the list with inital value for stats ***********/
	/********************************************************************/
	
	for (i=0;i<30;i++){
		//stats server
		ss[i].id=0;
		strcpy(ss[i].host1,"--");
		strcpy(ss[i].host2,"--");
		ss[i].upTime=0;
		ss[i].numUp=0;
		ss[i].dwTime=0;
		ss[i].numDw=0;	
		
		//stats client
		sc[i].id=0;
		strcpy(sc[i].host1,"--");
		strcpy(sc[i].host2,"--");
		sc[i].upTime=0;
		sc[i].numUp=0;
		sc[i].dwTime=0;
		sc[i].numDw=0;	

	}

	/* loop through all the results and connect to the first we can */
	for(p = info; p != NULL; p = p->ai_next) {
						
		

		/******************************************************************************************************/
		/******************************** Get the Socket file descriptor **************************************/
		/******************************************************************************************************/
		int err;
		sock = socket(p->ai_family, p->ai_socktype, IPPROTO_TCP);
     		if (sock == -1) {
    			fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
        		exit(1);
		}
        	else 
		       printf("[Server] Obtaining socket descriptor successfully.\n");


		//lose the pesky "address already in use" error message
   		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		
		/* Bind a Port */
		err=bind(sock, p->ai_addr, p->ai_addrlen);
		if (err == -1) {
     			fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
        		exit(1);
     		}
		else{
			printf("[Server] Obtaining bind successful.\n");
		}
		
		err=listen(sock, 5);
     		if (err == -1) {
			perror("listen");
			exit(3);
		}
		else{
			printf("[Server] listening sucessfully");
		}
		
		while (1) {
				
	    		/* 
	     		* select: Has the user typed something to stdin or 
	     		* has a connection request arrived?
	     		*/
	    		FD_ZERO(&readfds);          /* initialize the fd set */
	    		FD_SET(sock, &readfds); /* add socket fd */
	    		FD_SET(0, &readfds);        /* add stdin fd (0) */
	    		char *p;
  			char *array[3];
  			//printf("value of sock %d",sock);
			dupFlag=0;
			fullFlag=0;

			max_sd=sock;
			
			//add child sockets to set :: reference:http://www.binarytides.com/multiple-socket-connections-fdset-select-linux/
        		//Value 3 is the maximum number of clients (total 4) 
			for ( i = 0 ; i < 4 ; i++) 
        		{
            		//socket descriptor
            		sd_child = client_socket[i];
             
            		//if valid socket descriptor then add to read list
            		if(sd_child > 0)
                		FD_SET( sd_child , &readfds);
             
            		//highest file descriptor number, need it for the select function
           			if(sd_child > max_sd)
           	     		max_sd = sd_child;
        		}
	
			if (select(max_sd+1, &readfds, 0, 0, 0) < 0) {
	     	 		error("ERROR in select");
	    		}

	 		/* if the user has entered a command, process it */
    			if (FD_ISSET(0, &readfds)) {
     				printf("\nReading from stdin\n");
                     
				gets(command);
				//printf("%s",command);
				i = 0;
  				p = strtok (command," ");  
  				
				while (p != NULL)
  				{
    					array[i++] = p;
  					p = strtok (NULL, " ");
  				}
					
				/******************************************************************************************************/
				/****************************       CREATOR COMMAND    ************************************************/
				/******************************************************************************************************/
			
				if (strcmp(array[0],"CREATOR")==0){
					creator();	
				}
			
				/******************************************************************************************************/
				/****************************          HELP COMMAND    ************************************************/
				/******************************************************************************************************/
			
				else if (strcmp(array[0],"HELP")==0){
					help();
				}

				/******************************************************************************************************/
				/**************************** Print IP Address ********************************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"MYIP")==0)
				getIp();
			
				/******************************************************************************************************/
				/********* Store and print value of the port this process is listening to as passed by user ***********/
				/******************************************************************************************************/
				else if (strcmp(array[0],"MYPORT")==0){
					int len = sizeof(struct sockaddr);
					if (getsockname(sock, (struct sockaddr *)&sin, &len) == -1)
					    	perror("getsockname");
					else
						port_num=ntohs(sin.sin_port);
					    	printf("Port number:%d\n",port_num); //PORT NUMBER
				}
				/******************************************************************************************************/
				/******************************** Output for exit command *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"EXIT")==0){
					
					printf("\nServer must not exit\n");	
				}
				/******************************************************************************************************/
				/********************************************* STATISTICS *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"STATISTICS")==0){
					
					for (i = 0; i < 4; i++)
        				{
						if (client_socket[i]!=-1){
							clsock=client_socket[i];
							bzero(buf,2000);
							strcpy(buf,"STA");
							err=send(clsock, buf , strlen(buf) , 0);
							if( err < 0)
    		  						fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    							else{
								bzero(buf,2000);
								sleep(2);
								n = recv(clsock, buf, 2000,0);
      							if (n < 0) 
									fprintf(stderr, "ERROR: Failed to receive stats list from client. (errno = %d)\n", errno);
      							else{
									if(strcmp(buf,"ERR")!=0){
										printf("\n\n\nEntries in client statistics list for sock %d are %s\n\n\n",clsock,buf);
										//Tokenize received message
										i = 0;
										p = strtok (buf," ");  
										while (p != NULL)
										{
											token[i++] = p;
											p = strtok (NULL, " ");
										}
										token[i]='\0';
										//printf("\n\nDone tokenizing\n\n");
										i=0;
										for (j=0;j<30;j++){
											sc[j].id=atoi(token[i++]);
											strcpy(sc[j].host1,token[i++]);
											strcpy(sc[j].host2, token[i++]);
											sc[j].upTime=atoi(token[i++]);
											sc[j].numUp=atoi(token[i++]);
											sc[j].dwTime=atoi(token[i++]);
											sc[j].numDw=atoi(token[i++]);
											if (token[i]==NULL)
												break;
										}
										n=0;
										for(i=0;i<30;i++){
												
											for (j=0;j<30;j++){
												if((strcmp(ss[i].host1,sc[j].host1)==0)&&(strcmp(ss[i].host2,sc[j].host2)==0)){
													//insert and break
													ss[i].upTime=((ss[i].upTime*ss[i].numUp)+(sc[j].upTime*sc[j].numUp))/(ss[i].numUp+sc[j].numUp);
													ss[i].numUp=ss[i].numUp+sc[j].numUp;
											
													ss[i].dwTime=((ss[i].dwTime*ss[i].numDw)+(sc[j].dwTime*sc[j].numDw))/(ss[i].numDw+sc[j].numDw);
													ss[i].numDw=ss[i].numDw+sc[j].numDw;
													n=1;
													sc[j].id=0;
													break;
												}
											}
										}
										n=0;
										for(i=0;i<30;i++){
											for (j=0;j<30;j++){
												if((ss[i].id==0)&&(sc[j].id!=0)){
													//insert and break
													ss[i].id=i+1;
													strcpy(ss[i].host1,sc[j].host2);
													strcpy(ss[i].host2,sc[j].host2);
													ss[i].upTime=sc[j].upTime;
													ss[i].numUp=sc[j].numUp;
												
													ss[i].dwTime=sc[j].dwTime;
													ss[i].numDw=sc[j].numDw;
													sc[j].id=0;
													n=1;
													break;
												}
											}
										
										}
										printf("HostName1\tHostName2\tTotal Uploads\tAverage Upload speed\tTotal Downloads\tAverage Down Rate\n");
										for (i=0;i<30;i++){
								
											if(ss[i].id!=0){
												printf("%s\t %s \t%d \t%f \t%d \t%f\n",ss[i].host1,ss[i].host2,ss[i].numUp,ss[i].upTime,ss[i].numDw,ss[i].dwTime);
											}
										}
									}
								}//end else for receive message	
							}//end else for send message
							
						}//close client socket not equal to -1
					}
				}
				/******************************************************************************************************/
				/******************************** Output for list command *********************************************/
				/******************************************************************************************************/
						
				else if (strcmp(array[0],"LIST")==0){
					
					printf("ID\tHOSTNAME\tIPADDRESS\tPORT\n\n");
					//ID for server
					strcpy(l[0].hostname,hostname);
					strcpy(l[0].ip_addr,ip_addr);
					l[0].port_num=port_num;
					
					i=0; //Re-declaration;
					for (i=1;i<5;i++){
						if (l[i].id!=0){
							host_id=(--l[i].id);
							strcpy(hostname,l[i].hostname);
							strcpy(ip_addr,l[i].ip_addr);
							port_num=l[i].port_num;
							printf("\n%-5d%-35s%-20s%-8d\n",host_id,hostname,ip_addr,port_num);
							fflush(stdout);
						}
						else{
							host_id=l[i].id;
							strcpy(hostname,l[i].hostname);
							strcpy(ip_addr,l[i].ip_addr);
							port_num=l[i].port_num;
							printf("\n%-5d%-35s%-20s%-8d\n",host_id,hostname,ip_addr,port_num);
							fflush(stdout);	
						}

					}		
				}
				else{
					printf("\nError : check list of commands using HELP\n");
				}
     			}

     			/* if a connection request has arrived, process it */
    			if (FD_ISSET(sock, &readfds)) {
     				printf("\nAccepted connection: reading request");
       							
				struct sockaddr_in client;
				int lenc=sizeof(client);
				printf("before accept");
      			clsock = accept(sock,(struct sockaddr*)&client, (socklen_t*)&lenc);
      			if (clsock < 0) 
				error("ERROR on accept");
      			
				printf("connection accepted\n");
				printf("\n");
      			bzero(buf,2000);
				//check if duplicate IP already present in list
				strcpy(buf,inet_ntoa(client.sin_addr));
				//printf("\n\nValue of the ip address from client is :%s\n\n",buf);
				//CHeck for duplicate
				for (i = 0; i < 5; i++)
            		{	
					//printf("Value of ip address and buf in structure %s %s\n",l[i].ip_addr,buf);
					if ((strcmp(l[i].ip_addr,buf)==0))
					{	
						bzero(buf,2000);
						strcpy(buf,"DUP");
						err=send(clsock, buf , strlen(buf) , 0);
						close(clsock);
						printf("Duplicate connection received");
						dupFlag=1;
					}
				}
				//Check for list full
				for (i = 0; i < 5; i++)
            		{	
					//printf("Value of ip address and buf in structure %s %s\n",l[i].ip_addr,buf);
					if (l[1].id!=0&&l[2].id!=0&&l[3].id!=0&&l[4].id!=0)
					{	
						bzero(buf,2000);
						strcpy(buf,"FUL");
						err=send(clsock, buf , strlen(buf) , 0);
						close(clsock);
						printf("Server has reached max number of connections");
						fullFlag=1;
					}
				}
				if(dupFlag!=1&&fullFlag!=1){
					bzero(buf,2000);
      			
					n = recv(clsock, buf, 2000,0);
      				if (n < 0) 
						fprintf(stderr, "ERROR: Failed to receive port num. (errno = %d)\n", errno);
      				else {
      					//printf("Message is %s",buf);
						printf("\n");
					}
				
					//inform user of socket number - used in send and receive commands
            			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , clsock , inet_ntoa(client.sin_addr) , ntohs(client.sin_port));
        	
					//add new socket to array of sockets
            			for (i = 0; i < 4; i++)
            			{
            	    			//if position is empty
                				if( client_socket[i] == -1 )
                				{
                    				client_socket[i] = clsock;
                    				break;
                				}
            			}
					//Add to list structure
					for (j=1;j<5;j++){
						if (l[j].id==0){
							l[j].id=j+1;
							he=gethostbyaddr(&client.sin_addr, sizeof (client.sin_addr), AF_INET);
							printf("%s\n\n",he->h_name);
							strcpy(l[j].hostname,he->h_name);
							inet_ntop(AF_INET, &client.sin_addr, l[j].ip_addr, 99);
							l[j].port_num=atoi(buf); //String to integer
							l[j].sock_fd=clsock;
							break;
						}	
					}
				
					bzero(buf,2000);
					//Sending the server list to all connected
					for (i = 0; i < 5; i++)
            			{
						if (l[i].id != 0 )
						{	
							sprintf(temp,"%d",l[i].id); // convert int to string
							strcat(buf,temp);
							strcat(buf," ");
							strcat(buf,l[i].hostname);
							strcat(buf," ");	
							strcat(buf,l[i].ip_addr);
							strcat(buf," ");
							sprintf(temp,"%d",l[i].port_num); //convert int to string
							strcat(buf,temp);
							strcat(buf," ");
							//printf("\n\nMessage in buffer is %s\n\n",buf);
						}
	           	     		}
				
					// Send server clients list to all connected except itself
					for (i = 1; i < 5; i++)
            			{
						if (l[i].id != 0 )
						{
							//printf("\n\nvalue of sock FD in structure while passing %d\n\n",l[i].sock_fd);
							err=send(l[i].sock_fd, buf , strlen(buf) , 0);
								if( err < 0)
    		  							fprintf(stderr, "\nERROR: Failed to Send server list to clients. (errno = %d)\n", errno);
    							else 
								printf("Send List to client socket sucessful\n");
						}
					}
				}
     	 		}//end IF FD_ISSET
			
			//check for activity on socket other sockets
			for (i = 0; i < 4; i++)
        		{
            		clsock = client_socket[i];
              		//printf("\n\nValue of client socket of array in else =%d\n",client_socket[i]);
            		if(client_socket[i]>0){
					if (FD_ISSET( clsock, &readfds)){
                				//Check if it was for closing , and also read the incoming message
                				bzero(buf,2000);
      					//printf("Value of client sock id in client is : %d",clsock);
						n = recv(clsock, buf, 2000,0);
      					if (n < 0) 
							fprintf(stderr, "ERROR: Failed to receive for checking activity on server connected socket. (errno = %d)\n", errno);
      					
						else if (strcmp(buf,"TER")==0) {
      						//printf("\nMessage is %s\n",buf);
							printf("\n");
							
							
							//Remove from client socket list and client List
							for (i=0;i<4;i++){
								if (clsock==client_socket[i])
									client_socket[i]=-1;
								if (l[i].sock_fd==clsock){
									l[i].id=0;
									strcpy(l[i].hostname,"--");
									strcpy(l[i].ip_addr,"--");
									l[i].port_num=0;
									l[i].sock_fd=0;					
								}
							}
							err=send(clsock, buf , strlen(buf) , 0);
							if( err < 0)
    		  						fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    							else{
								printf("Socket id %d has been terminated",clsock);
								close(clsock);
								bzero(buf,2000);
								//Sending the server list to all connected
								for (i = 0; i < 5; i++)
            						{
									if (l[i].id != 0 )
									{	
										sprintf(temp,"%d",l[i].id); // convert int to string
										strcat(buf,temp);
										strcat(buf," ");
										strcat(buf,l[i].hostname);
										strcat(buf," ");	
										strcat(buf,l[i].ip_addr);
										strcat(buf," ");
										sprintf(temp,"%d",l[i].port_num); //convert int to string
										strcat(buf,temp);
										strcat(buf," ");
										//printf("\n\nMessage in buffer is %s\n\n",buf);
									}
	           	     					}//end for
					
								// Send server clients list to all connected except itself
								for (i = 1; i < 5; i++)
            						{
									if (l[i].id != 0 )
									{
										//printf("\n\nvalue of sock FD in structure while passing %d\n\n",l[i].sock_fd);
										err=send(l[i].sock_fd, buf , strlen(buf) , 0);
										if( err < 0)
    		  									fprintf(stderr, "\nERROR: Failed to Send server list to clients. (errno = %d)\n", errno);
    										else 
											printf("Send to client socket sucessful\n");
									}// end l[i] check
								}//end for
							}//end send error check
						}//else if message is TER
					}//end if FD_ISSET loop
				}//if client_socket loop
			}//for loop
		}//end while    
		
	}//end for loop which goes through all results
 
	freeaddrinfo(info);
}

void client (char type, char **argv){

	/*Variable delaration*/
	char command[1024]; //For user command
	int size=3; //For size of list structure bound to 3 
	int i=0,j=0,k=0; //Counter variable
	int n;
	int argc=0;
	char buf[2000]; /* message buffer */
	fd_set readfds;
	int sock,clsock;     // listening socket descriptor
	int termSock;

	//char *token[25];
	char temp[100];
	int yes=1;        // for setsockopt() SO_REUSEADDR	
	int gai_result; // For result of getaddrinfo()
	char hostname[1024]; //For hostname
	hostname[1023] = '\0';
	
	int dupFlag;
	int fullFlag;
	int sd_child;		//socket descriptor to handle connections already established
	int max_sd;		//Used in select to handle multiple connections
	
	/********* FOR UPLOAD *********/
	char path[100] = "./"; // Added some space for array[2]
	FILE *fu;
	int blockSize, upSock;
	char upbuf[LENGTH];
	char fr_name[100];
	FILE *fr;
	int write_sz;
	struct timeval begin,end;
	double elapsedTime;
	double rate;
	
	/********************DOWNLOAD*************************/
	int dwSock;
	
	/*Structures*/
	struct serverList l[5]; // Structure holding list values
	struct clientList c[4];
	struct stats s[30];
	struct addrinfo hints, *info, *p; //addrinfo structure
	struct sockaddr_in *sa, sin; //IPV4
	struct hostent *he;


	struct sockaddr_in6 *sa6; // IPV6
		
	//Initialize child socket list to zero
	//Value 3 is the maximum connections for this server (total 4)
	for (i = 0; i < 4; i++) 
    	{
    	    client_socket[i] = -1;
    	}
      
	
	printf("\n\nThis is a : %c\n", type); // check for command line entered
	
	gethostname(hostname, 1023);
	printf("Hostname: %s\n", hostname);
	strcpy(myhost,hostname);

	/*Using modern getaddrinfo and sockaddr*/
	/*set hints parameters to call specific addrinfo*/
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	/*getaddrinfo + standard error check*/
	/****************check with second parameter null********************************************/
	if ((gai_result = getaddrinfo(hostname, argv[2], &hints, &info)) != 0) {
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
	    exit(1);
	}
	
	
	/********************************************************************/	
	/********** Populate the list with inital value for client **********/
	/********************************************************************/
	
	for (i=0;i<4;i++){
		c[i].id=0;
		strcpy(c[i].hostname,"--");
		strcpy(c[i].ip_addr,"--");
		c[i].port_num=0;
		c[i].sock_fd=0;
	}	
	
	/********************************************************************/	
	/********** Populate the list with inital value for server **********/
	/********************************************************************/
	
	for (i=0;i<5;i++){
		l[i].id=0;
		strcpy(l[i].hostname,"--");
		strcpy(l[i].ip_addr,"--");
		l[i].port_num=0;
		l[i].sock_fd=0;
	}
	
	/********************************************************************/	
	/********** Populate the list with inital value for stats ***********/
	/********************************************************************/
	
	for (i=0;i<30;i++){
		s[i].id=0;
		strcpy(s[i].host1,"--");
		strcpy(s[i].host2,"--");
		s[i].upTime=0;
		s[i].numUp=0;
		s[i].dwTime=0;
		s[i].numDw=0;	

	}

	/* loop through all the results and connect to the first we can */
	for(p = info; p != NULL; p = p->ai_next) {
						
		

		/******************************************************************************************************/
		/******************************** Get the Socket file descriptor **************************************/
		/******************************************************************************************************/
		int err;
		sock = socket(p->ai_family, p->ai_socktype, IPPROTO_TCP);
     		if (sock == -1) {
    			fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
        		exit(1);
		}
        	else 
		       printf("[Client] Obtaining socket descriptor successfully.\n");


		//lose the pesky "address already in use" error message
   		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		
		/* Bind a Port */
		err=bind(sock, p->ai_addr, p->ai_addrlen);
		if (err == -1) {
     			fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
        		exit(1);
     		}
		else{
			printf("[Client] Obtaining bind successful.\n");
		}
		
		err=listen(sock, 5);
     		if (err == -1) {
			perror("listen");
			exit(3);
		}
		else{
			printf("[Client] listening sucessfully");
		}

		while (1) {
				
	    		/* 
	     		* select: Has the user typed something to stdin or 
	     		* has a connection request arrived?
	     		*/
	    		FD_ZERO(&readfds);          /* initialize the fd set */
	    		FD_SET(sock, &readfds); /* add socket fd */
	    		FD_SET(0, &readfds);        /* add stdin fd (0) */
			dupFlag=0;
			fullFlag=0;

			//add child sockets to set
        		for ( i = 0 ; i < 4 ; i++)
        		{
            		//socket descriptor
            		sd_child = client_socket[i];
             
            		//if valid socket descriptor then add to read list
            		if(sd_child > 0)
                			FD_SET( sd_child , &readfds);
             
            		//highest file descriptor number, need it for the select function
           			if(sd_child > max_sd)
           		     		max_sd = sd_child;
        		}
			char *p;
  			char *array[10];
			//initialize values for array
			for (i=0;i<10;i++){
				array[i]="-";
			}
  			
			if (select(max_sd+1, &readfds, 0, 0, 0) < 0) {
	     	 		error("ERROR in select");
	    		}

	 		/* if the user has entered a command, process it */
    			if (FD_ISSET(0, &readfds)) {
     				printf("\nReading from stdin\n");
                     
				gets(command);
				
				i = 0;
  				p = strtok (command," ");  
  				
				while (p != NULL)
  				{
    					array[i++] = p;
  					p = strtok (NULL, " ");
  				}
					
				/******************************************************************************************************/
				/****************************       CREATOR COMMAND    ************************************************/
				/******************************************************************************************************/
			
				if (strcmp(array[0],"CREATOR")==0){
					creator();	
				}
			
				/******************************************************************************************************/
				/****************************          HELP COMMAND    ************************************************/
				/******************************************************************************************************/
			
				else if (strcmp(array[0],"HELP")==0){
					help();
				}

				/******************************************************************************************************/
				/**************************** Print IP Address ********************************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"MYIP")==0)
				getIp();
			
				/******************************************************************************************************/
				/********* Store and print value of the port this process is listening to as passed by user ***********/
				/******************************************************************************************************/
				else if (strcmp(array[0],"MYPORT")==0){
					int len = sizeof(struct sockaddr);
					if (getsockname(sock, (struct sockaddr *)&sin, &len) == -1)
					    	perror("getsockname");
					else
						port_num=ntohs(sin.sin_port);
					    	printf("Port number:%d\n",port_num); //PORT NUMBER
				}
				/******************************************************************************************************/
				/***************************************** REGISTER ***************************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"REGISTER")==0){
					 
	
					regFlag=1;
					clientSockFd=regClient(sock,array);
					i=0;
					//Add to list structure
					for (j=0;j<5;j++){
						l[j].id=atoi(token[i++]); // string to int
						strcpy(l[j].hostname,token[i++]);
						strcpy(l[j].ip_addr, token[i++]);
						l[j].port_num=atoi(token[i++]);
						if (token[i]==NULL)
						break;
					}
						
					//Set server details in client list on first connect
					c[0].id=1;
					strcpy(c[0].hostname,l[0].hostname);
					strcpy(c[0].ip_addr,l[0].ip_addr);
					c[0].port_num=l[0].port_num;
					
					if(clientSockFd>0)
					c[0].sock_fd=clientSockFd;
					//printf("\n\nvalue of clientsock in register block = %d\n\n",clientSockFd);
					
				}
				/******************************************************************************************************/
				/************************************************ CONNECT *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"CONNECT")==0){
					n=-1;
					for (j=1;j<5;j++){
					
						if(strcmp(array[1],l[j].hostname)==0){
								array[1]=l[j].ip_addr;
								break;
						}
					}
					
					for (j=1;j<5;j++){
						if ((strcmp(array[1],l[j].ip_addr)==0)&&(l[j].port_num=atoi(array[2]))&&strcmp(ip_addr,l[j].ip_addr)!=0){
							n=1;
							strcpy(hostname,l[j].hostname);
							strcpy(ip_addr,l[j].ip_addr);
							port_num=l[j].port_num;
							break;
						}
					}
					if (n!=1)
						printf("Values with connect are incorrect please enter valid combination of port number and hostname/ip address. Please ensure that you do not try to connect with server or self");
					else{
						for (i=0;i<4;i++){
							if (c[i].id==0){
								n=1;
								break;
							}
						}
						if (n=1){
							connectSockFd=connectToClient(array,l);
							//printf("\n\n\nValue of Connect sockFD after return %d\n\n\n",connectSockFd);
							if (connectSockFd>0){
								for (i=0;i<4;i++){
									
									if (c[i].id==0){
										c[i].id=i+1;
										strcpy(c[i].hostname,hostname);
										strcpy(c[i].ip_addr,ip_addr);
										c[i].port_num=port_num;
										c[i].sock_fd=connectSockFd;
										break;
									}//end if for empty position in client list
	
								}//end for for insert into client list
	
							}//end if for socket>0
						}//end if for inner n=1
						else
							perror("Client has already registered with max number of peers");
					}//end else for n=1
					
				}
				/******************************************************=************************************************/
				/********************************************** TERMINATE *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"TERMINATE")==0){
					//printf("value of n: %s\n\n",array[1]);
					
					n=atoi(array[1]);
					//printf("value of n: %d\n\n",n);
					j=-1;
					for (i=0;i<4;i++){
						if (n==c[i].id){
							termSock=c[i].sock_fd;
							//printf("Value of term sock in terminate is %d\n\n", termSock);
							j=1;
							break;
						}
					}
					if (j!=1)
						printf("Please check if input is present in the List displayed in list command");
					else{
						bzero(buf,2000);
						strcpy(buf,"TER");
						err=send(termSock, buf , strlen(buf) , 0);
						if( err < 0)
    		  					fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    						else{ 
							n = recv(termSock, buf, 2000,0);
      						if (n < 0) 
								fprintf(stderr, "ERROR: Failed to receive for termination confirmation on peer terminated socket. (errno = %d)\n", errno);
      						else {
								//Remove from client socket list and client List
								for (i=0;i<4;i++){
									if (termSock==client_socket[i])
										client_socket[i]=-1;
									if (c[i].sock_fd==termSock){
										c[i].id=0;
										strcpy(c[i].hostname,"--");
										strcpy(c[i].ip_addr,"--");
										c[i].port_num=0;
										c[i].sock_fd=0;
									}//close structure if					
								}//close for

							close(termSock);

							}//close recv else
						}
					}

				}

				/*****************************************************=************************************************/
				/*************************************************** EXIT *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"EXIT")==0){
					//printf("value of n: %s\n\n",array[1]);
					
					
					j=-1;
					k=0;
					//run for everyone
					for (k=0;k<4;k++){
						if (c[k].sock_fd!=0){
							n=c[k].sock_fd;
							//printf("\n\n\nvalue of n= %d\n\n",n);
							bzero(buf,2000);
							strcpy(buf,"TER");
							err=send(n, buf , strlen(buf) , 0);
							if( err < 0)
	    		  					fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
	    						else{ 
								err = recv(n, buf, 2000,0);
	      						if (err < 0) 
									fprintf(stderr, "ERROR: Failed to receive for termination confirmation on peer terminated socket. (errno = %d)\n", errno);
	      						else {
									printf("\nreceived confirmation for socket number %d",n);
									//Remove from client socket list and client List
									for (i=0;i<4;i++){
										if (n==client_socket[i])
											client_socket[i]=-1;
										if (c[i].sock_fd==n){
											c[i].id=0;
											strcpy(c[i].hostname,"--");
											strcpy(c[i].ip_addr,"--");
											c[i].port_num=0;
											c[i].sock_fd=0;
										}//close structure if					
									}//close for
									close(n);
								}//close recv else
							}//close send else
						}//close check for if list holds value other than -1
					}//end for run for everyone

					
					
				}
				/******************************************************************************************************/
				/************************************************* UPLOAD *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"UPLOAD")==0){
					for(i=0;i<10;i++){
						printf("\nValues = %s\n",array[i]);
					}
					n=atoi(array[1]);
					//printf("value of n: %d\n\n",n);
					j=-1;
					for (i=0;i<4;i++){
						if (n==c[i].id){
							upSock=c[i].sock_fd;
							strcpy(hostname,c[i].hostname);
							//printf("Value of up sock in upload is %d\n\n", upSock);
							j=1;
							break;
						}
					}
					if (j!=1)
						printf("Please check if input is present in the List displayed in list command");
					else{
						bzero(buf,2000);
						strcpy(buf,"UPL");
						err=send(upSock, buf , strlen(buf) , 0);
						if( err < 0)
    		  					fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    						else{
							bzero(buf,2000);
							strcpy(buf,array[2]);
							sleep(2);
							err=send(upSock, buf , strlen(buf) , 0);
							if( err < 0)
    		  						fprintf(stderr, "\nERROR: Failed to send file name. (errno = %d)\n", errno);
    							else{
								//receive confirmation of file name
								//n = recv(upSock, buf, 2000,0);
								printf("\n\nConfirmation received proceeding to send, value in client buf %s\n\n",buf);
								sleep(2);
								bzero(path, 100);
								strcpy(path,"./");
								strcat(path,array[2]);
	
								// Open the file
						
								fu = fopen(path,"r"); 
								if (!fu) 
								{
    									perror("Failed to open file\n");
    			
								}
								bzero(upbuf,LENGTH);
								i=0;
								gettimeofday(&begin, NULL);
								while((blockSize=fread(upbuf, sizeof (char),LENGTH, fu))>0){
									//printf("\n\nbytes read %d",blockSize);
									i=i+blockSize;
									if(send(upSock, upbuf, blockSize, 0) < 0)
		    							{
		        							fprintf(stderr, "ERROR: Failed to send file. (errno = %d)\n", errno);
		       	 						break;
									}
									printf("Value sent in upbuf %s",upbuf);
		 	   						bzero(upbuf, LENGTH);
									sleep(1);
								}//end while block size >0

								gettimeofday(&end, NULL);
							
								elapsedTime =  (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
								rate=i/elapsedTime;
								printf("\n\n\n Tx %s -> %s, File Size: %d Bytes, Time Taken %f seconds, Tx Rate = %f bits/sec\n\n\n",myhost, hostname,i,elapsedTime,rate);

								printf("\n\nOk File %s from Client was Sent!\n", array[2]);
								n=0;
								//printf("\nValue of hostname= %s first slot in stasts = %s\n",hostname,s[0].host2);
								// Insert into stats table
								for (i=0;i<30;i++){
									if(strcmp(s[i].host2,hostname)==0){
										//insert and break
										s[i].upTime=((s[i].upTime*s[i].numUp)+rate)/(s[i].numUp+1);
										s[i].numUp=s[i].numUp+1;
										n=1;
										break;
									}
								}
								if (n!=1){
									// Insert new entry into stats table
									for (i=0;i<30;i++){
										if(s[i].id==0){
											//insert and break
											s[i].id=i+1;
											strcpy(s[i].host1,myhost);
											strcpy(s[i].host2,hostname);
											s[i].upTime=rate;
											s[i].numUp=1;
											n=1;
											break;
										}
									}
								}
								
							}//end else for send filename
						}//end else condition for message UPL
					}//end j=1 else condition	
				}
				/******************************************************************************************************/
				/*********************************************** DOWNLOAD *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"DOWNLOAD")==0){
					for(i=0;i<10;i++){
						printf("\nValues = %s\n",array[i]);
					}
					for (i=1;i<10;i=i+2){
						
						//printf("\nValue of i in this iteration %d\n",i);
						if ((strcmp(array[i],"-")!=0)&&(strcmp(array[i+1],"-")!=0)){
							
							n=atoi(array[i]);
							
							//printf("value of n: %d\n\n",n);
							j=-1;
							for (i=0;i<4;i++){
								if (n==c[i].id){
									dwSock=c[i].sock_fd;
									strcpy(hostname,c[i].hostname);
									//printf("Value of up sock in upload is %d\n\n", dwSock);
									j=1;
									break;
								}
							}			
							if (j!=1)
								printf("Please check if input is present in the List displayed in list command");
							else{
								bzero(buf,2000);
								strcpy(buf,"DWL");
								err=send(dwSock, buf , strlen(buf) , 0);
								if( err < 0)
    		  							fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    								else{
									bzero(buf,2000);
									strcpy(buf,array[i+1]); //Set file name
									sleep(5);
									err=send(dwSock, buf , strlen(buf) , 0);
									if( err < 0)
    		  								fprintf(stderr, "\nERROR: Failed to send file name. (errno = %d)\n", errno);
    									else{
										bzero(fr_name, 100);
										strcpy(fr_name,"./");
										strcat(fr_name,buf);
										strcat(fr_name,"_");
										strcat(fr_name,hostname);
										printf("Value in buffer %s",buf);

										printf("\n\nfilename is %s",fr_name);
	
										fr = fopen(fr_name, "ab+");
										if(fr == NULL)
											printf("File %s Cannot be opened file on server.\n", fr_name);
										else
										{	
										
											bzero(upbuf, LENGTH); 
											blockSize = 0;
											i=0;
											gettimeofday(&begin, NULL);
											while((blockSize = recv(dwSock, upbuf, LENGTH, 0)) > 0) 
											{	
												i=i+blockSize;
												
												//printf("Value of block size and write_sz %d %d",blockSize,write_sz);
												//printf("Value received in upbuf %s",upbuf);
															
			    									write_sz = fwrite(upbuf, sizeof (char), blockSize, fr);
												if(write_sz < 0)
			    									{
			        									error("File write failed on recepient.\n");
			    									}
												if (blockSize == 0 || blockSize != LENGTH) 
												{
													break;
												}	
												bzero(upbuf, LENGTH);
												
											}
											gettimeofday(&end, NULL);
											elapsedTime =  (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
											
											rate=i/elapsedTime;
											printf("\n\n\n Rx %s -> %s, File Size: %d Bytes, Time Taken %f seconds, Rx Rate = %f bits/sec\n\n\n",hostname,myhost,i,elapsedTime,rate);
											
											if(blockSize < 0)
		    									{
												fprintf(stderr, "recv() failed due to errno = %d\n", errno);
        										}
											printf("Ok received from client!\n");
											fclose(fr);
											//printf("\nValue of hostname= %s first slot in stasts = %s\n",hostname,s[0].host2);
											
											// Insert into stats table
											for (i=0;i<30;i++){
												if(strcmp(s[i].host2,hostname)==0){
													//insert and break
													s[i].dwTime=((s[i].dwTime*s[i].numDw)+rate)/(s[i].numDw+1);
													s[i].numDw=s[i].numDw+1;
													n=1;
													break;
												}
											}
											if (n!=1){
												// Insert new entry into stats table
												for (i=0;i<30;i++){
													if(s[i].id==0){
														//insert and break
														s[i].id=i+1;
														strcpy(s[i].host1,myhost);
														strcpy(s[i].host2,hostname);
														s[i].dwTime=rate;
														s[i].numDw=1;
														n=1;
														break;
													}
												}
											}		
										}//end else for file null
									}//end else for sending filename
	
								}//end else for sending DWL 

							}//end else for j=1	
						
						}//if compare that values are entered
					
					}//end for getting values in array
					
				}
				/******************************************************************************************************/
				/********************************************* STATISTICS *********************************************/
				/******************************************************************************************************/
				else if (strcmp(array[0],"STATISTICS")==0){
					printf("HostName\tTotal Uploads\tAverage Upload speed\tTotal Downloads\tAverage Down Rate\n");
					for (i=0;i<30;i++){
						
						if(s[i].id!=0){
							printf("%s \t%d \t%f \t%d \t%f\n",s[i].host2,s[i].numUp,s[i].upTime,s[i].numDw,s[i].dwTime);
						}
					}			
				}
				
				/******************************************************************************************************/
				/******************************** Output for list command *********************************************/
				/******************************************************************************************************/
						
				else if (strcmp(array[0],"LIST")==0){
					printf("ID\tHOSTNAME\tIPADDRESS\tPORT\n\n");
					
					i=0; //Re-declaration;
					for (i=0;i<4;i++){
						host_id=c[i].id;
						strcpy(hostname,c[i].hostname);
						strcpy(ip_addr,c[i].ip_addr);
						port_num=c[i].port_num;
						printf("\n%-5d%-35s%-20s%-8d\n",host_id,hostname,ip_addr,port_num);
						fflush(stdout);
					}		
				}


				else{
					printf("\nError : check list of commands using HELP\n");
				}
     			}//end first if for select

     			/* if a connection request has arrived, process it */
    			if (FD_ISSET(sock, &readfds)) {
     				printf("\nAccept: wait for a connection request");
       							
				struct sockaddr_in client;
				int lenc=sizeof(client);
      			clsock = accept(sock,(struct sockaddr*)&client, (socklen_t*)&lenc);
      			if (clsock < 0) 
					error("ERROR on accept");
      			else{
					printf("connection accepted\n");
					printf("\n");
				}
      			bzero(buf,2000);
				//check if duplicate IP already present in list
				strcpy(buf,inet_ntoa(client.sin_addr));
				//printf("\n\nValue of the ip address from client is :%s\n\n",buf);
				//Check for duplicate
				for (i = 0; i < 4; i++)
            		{	
					//printf("Value of ip address and buf in structure %s %s\n",c[i].ip_addr,buf);
					if ((strcmp(c[i].ip_addr,buf)==0))
					{	
						bzero(buf,2000);
						strcpy(buf,"DUP");
						err=send(clsock, buf , strlen(buf) , 0);
						if( err < 0)
    		  					fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    						else{ 
							printf("Send duplicate message to client socket sucessful\n");
							close(clsock);
							printf("Peer Client has not added duplcate connection");
							dupFlag=1;
						}
					}
				}
				//Check for list full
				for (i = 0; i < 4; i++)
            		{	
					//printf("Value of ip address and buf in structure %s %s\n",c[i].ip_addr,buf);
					if (c[0].id!=0&&c[1].id!=0&&c[2].id!=0&&c[3].id!=0)
					{	
						bzero(buf,2000);
						strcpy(buf,"FUL");
						err=send(clsock, buf , strlen(buf) , 0);
						if( err < 0)
    		  					fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    						else{ 
							printf("Send full message to client socket sucessful\n");
							close(clsock);
							printf("Peer Client has reached max number of connections");
							fullFlag;
						}
					}
				}
				
				if(dupFlag!=1&&fullFlag!=1){
					bzero(buf,2000);
      				
					n = recv(clsock, buf, 2000,0);
      				if (n < 0) 
						fprintf(stderr, "ERROR: Failed to receive port num. (errno = %d)\n", errno);
      				else {
      					//printf("Message is %s",buf);
						printf("\n");
					}
				
					//inform user of socket number - used in send and receive commands
            			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , clsock , inet_ntoa(client.sin_addr) , ntohs(client.sin_port));
        				

					//add new socket to array of sockets
            			for (i = 0; i < 4; i++)
            			{
            	    			//if position is empty
                				if( client_socket[i] == -1 )
                				{
                    				client_socket[i] = clsock;
                    				break;
                				}
            			}
					
					//Add to list structure
					for (j=0;j<4;j++){
						if (c[j].id==0){
							c[j].id=j+1;
							he=gethostbyaddr(&client.sin_addr, sizeof (client.sin_addr), AF_INET);
							printf("%s\n\n",he->h_name);
							strcpy(c[j].hostname,he->h_name);
							inet_ntop(AF_INET, &client.sin_addr, c[j].ip_addr, 99);
							c[j].port_num=atoi(buf);
							c[j].sock_fd=clsock;
							//printf("\n\nvalue of sock FD in structure while passing %d\n\n",c[j].sock_fd);
							break;
						}//if	
					}//for
				
					bzero(buf,2000);
					strcpy(buf,"SUC");
						
					//printf("\n\nMessage in buffer is %s\n\n",buf);
					printf("\n\nCONNECTION SUCCESSFUL\n\n");
					
					err=send(clsock, buf , strlen(buf) , 0);
					if( err < 0)
    		  				fprintf(stderr, "\nERROR: Failed to send success message. (errno = %d)\n", errno);
    					else 
						printf("Send to client socket sucessful\n");
						
				}//if dupflag 
				
			}//if fdset sock
			
			else if (regFlag==1){
				//check for activity on socket other sockets
				for (i = 0; i < 4; i++)
        			{
            			clsock = client_socket[i];
					strcpy(hostname,c[i].hostname);
              			//printf("\n\nValue of client socket of array in else =%d\n",client_socket[i]);
            			if(client_socket[i]>0){
						if (FD_ISSET( clsock, &readfds)){
                				//Check if it was for closing , and also read the incoming message
                				bzero(buf,2000);
      						//printf("Value of client sock id in client is : %d",clsock);
							n = recv(clsock, buf, 2000,0);
      						if (n < 0) 
								fprintf(stderr, "ERROR: Failed to receive for checking activity on server connected socket. (errno = %d)\n", errno);
      						else if (strcmp(buf,"TER")==0) {
      							//printf("\nMessage is %s\n",buf);
								printf("\n");
								
									
								//Remove from client socket list and client List
								for (i=0;i<4;i++){
									if (clsock==client_socket[i])
										client_socket[i]=-1;
									if (c[i].sock_fd==clsock){
										c[i].id=0;
										strcpy(c[i].hostname,"--");
										strcpy(c[i].ip_addr,"--");
										c[i].port_num=0;
										c[i].sock_fd=0;					
									}
								}
								err=send(clsock, buf , strlen(buf) , 0);
								if( err < 0)
    		  							fprintf(stderr, "\nERROR: Failed to send full message. (errno = %d)\n", errno);
    								else
									printf("Socket id %d has been terminated",clsock);
									close(clsock);
							}//end else if for terminate

							else if(strcmp(buf,"UPL")==0){
								/*****************UPLOAD*********************/
								bzero(buf, 2000);
								n = recv(clsock, buf, 2000,0);
      							if (n < 0) 
									fprintf(stderr, "ERROR: Failed to receive filename. (errno = %d)\n", errno);
      							else if(n>0)
								{	//send confirmation of file name
									//sleep(2);
									//err=send(upSock, buf , strlen(buf) , 0);
									printf("Confirmation sent");
									bzero(fr_name, 100);
									strcpy(fr_name,"./");
									strcat(fr_name,buf);
									strcat(fr_name,"1+");
									printf("Value in buffer %s",buf);

									printf("\n\nfilename is %s",fr_name);

									fr = fopen(fr_name, "ab+");
									if(fr == NULL)
										printf("File %s Cannot be opened file on server.\n", fr_name);
									else
									{	
										
										bzero(upbuf, LENGTH); 
										blockSize = 0;
										i=0;
										gettimeofday(&begin, NULL);
										while((blockSize = recv(clsock, upbuf, LENGTH, 0)) > 0) 
										{	
											
											//printf("Value received in upbuf %s",upbuf);
														
			    								write_sz = fwrite(upbuf, sizeof (char), blockSize, fr);
											//printf("Value of write %d",write_sz);
											i=i+blockSize;
											//printf("Value of block size %d",blockSize);
											
											if(write_sz < 0)
			    								{
			        								error("File write failed on recepient.\n");
			    								}
											if (blockSize == 0 || blockSize != LENGTH) 
											{
												break;
											}
											bzero(upbuf, LENGTH);
											
										}
										gettimeofday(&end, NULL);
										elapsedTime =  (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
										
										rate=i/elapsedTime;
										printf("\n\n\n Rx %s -> %s, File Size: %d Bytes, Time Taken %f seconds, Rx Rate = %f bits/sec\n\n\n",hostname,myhost,i,elapsedTime,rate);
										
										if(blockSize < 0)
		    								{
											fprintf(stderr, "recv() failed due to errno = %d\n", errno);
        									}
										printf("Ok received from client!\n");
										fclose(fr);
										
									}//end else for file null
								}//end else received file name
							}//end else if for upload

							else if(strcmp(buf,"DWL")==0){
								/*****************DOWNLOAD*********************/
								bzero(buf, 2000);
								//Receive file name
								n = recv(clsock, buf, 2000,0);
      							if (n < 0) 
									fprintf(stderr, "ERROR: Failed to receive filename. (errno = %d)\n", errno);
      							else if(n>0)
								{	
									//printf("Value of recieved file name = %s",buf);
									sleep(2);
									bzero(path, 100);
									strcpy(path,"./");
									strcat(path,buf);
									
									// Open the file
						
									fu = fopen(path,"r"); 
									if (!fu) 
									{
    										perror("Failed to open file\n");
    			
									}
									bzero(upbuf,LENGTH);
									i=0;
									gettimeofday(&begin, NULL);
									while((blockSize=fread(upbuf, sizeof (char),LENGTH, fu))>0){
										//printf("\n\nbytes read %d",blockSize);
										i=i+blockSize;
										if(send(clsock, upbuf, blockSize, 0) < 0)
		    								{
		        								fprintf(stderr, "ERROR: Failed to send file. (errno = %d)\n", errno);
		       	 							break;
										}
										//printf("Value sent in upbuf %s",upbuf);
		 	   							bzero(upbuf, LENGTH);
										sleep(1);
									}//end while for send file
									gettimeofday(&end, NULL);
							
									elapsedTime =  (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
									rate=i/elapsedTime;
									printf("\n\n\n Tx %s -> %s, File Size: %d Bytes, Time Taken %f seconds, Tx Rate = %f bits/sec\n\n\n",myhost, hostname,i,elapsedTime,rate);

									printf("\n\nOk File %s from Peer was Sent!\n", array[2]);

								}//end else received file name
							}//end else if for upload

							else if (strcmp(buf,"STA")==0) {
      							printf("\n\nserver contacted for list details\n\n");
								printf("\n");
								bzero(buf,2000);
									
								//Sending the server list to all connected
								for (i = 0; i < 30; i++)
            						{
									if (s[i].id != 0 )
									{	
										sprintf(temp,"%d",s[i].id); // convert int to string
										strcat(buf,temp);
										strcat(buf," ");
										strcat(buf,s[i].host1);
										strcat(buf," ");	
										strcat(buf,s[i].host2);
										strcat(buf," ");
										sprintf(temp,"%f",s[i].upTime); // convert double to string
										strcat(buf,temp);
										strcat(buf," ");
										sprintf(temp,"%d",s[i].numUp); // convert int to string
										strcat(buf,temp);
										strcat(buf," ");
										sprintf(temp,"%f",s[i].dwTime); // convert double to string
										strcat(buf,temp);
										strcat(buf," ");
										sprintf(temp,"%d",s[i].numDw); // convert int to string
										strcat(buf,temp);
										strcat(buf," ");
										//printf("\n\nMessage in buffer is %s\n\n",buf);
									}//end sid populated check
													
	           	     					}//end for all entries in stats table
								if (s[0].id!=0)
									err=send(clsock, buf , strlen(buf) , 0);
								else{
									bzero(buf,2000);
									strcpy(buf,"ERR");
									err=send(clsock, buf , strlen(buf) , 0);

								}
							}
							else{		
									/******************ELSE LISTEN ON SOCKET FOR OTHER MESSAGES***********************/
									//printf("\nMessage is %s\n",buf);
									printf("\n");
									//Tokenize received message
									i = 0;
  									p = strtok (buf," ");  
  									while (p != NULL)
  									{
    										token[i++] = p;
  										p = strtok (NULL, " ");
  									}
									token[i]='\0';
									//printf("Done tokenizing");
									i=0;
									while(token[i]!=NULL){
										//printf("\nvalue of token %d is %s",i,token[i]);
										i++;
									}
									i=0;
									//Add to list structure
									for (j=0;j<5;j++){
										l[j].id=atoi(token[i++]);
										strcpy(l[j].hostname,token[i++]);
										strcpy(l[j].ip_addr, token[i++]);
										l[j].port_num=atoi(token[i++]);
										if (token[i]==NULL)
										break;
									}
									i=0; //Re-declaration;
									printf("ID\tHOSTNAME\tIPADDRESS\tPORT\n\n");
									for (i=0;i<5;i++){
										printf("\n%d\t%s\t%s\t%d\t",l[i].id,l[i].hostname,l[i].ip_addr,l[i].port_num);
									}
											
							}
						}
					}//if client_socket loop
				}//for loop
								
     			}//else if
		}//end while    
		
	}//end for loop which goes through all results
 
	freeaddrinfo(info);
}

void handleSigpipe(int signum)
{
   printf("SIGPIPE ignored");
   return;
}

int main(int argc[], char *argv[]){
	
	// Ignore SIGPIPE "Broken pipe" signals when socket connections are broken.
	
	signal(SIGPIPE, handleSigpipe);
	/*Variable delaration*/
	char *type=argv[1];	
	//printf("value of type = %c\n",type);

	if (strcmp(type,"s")==0){
		printf("\ninside server\n");
		server(*type, argv); 
		
	}
	else if(strcmp(type,"c")==0){
		printf("\ninside client\n");
		client(*type, argv);
	}
	else{
		printf("Error incorrect command line option\n");
	}


}
