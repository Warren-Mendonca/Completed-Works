#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 0

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
  char data[20];
  };

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
   int seqnum;
   int acknum;
   int checksum;
   char payload[20];
    };



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* Statistics 
 * Do NOT change the name/declaration of these variables
 * You need to set the value of these variables appropriately within your code.
 * */
int A_application = 0;
int A_transport = 0;
int B_application = 0;
int B_transport = 0;

/* Globals 
 * Do NOT change the name/declaration of these variables
 * They are set to zero here. You will need to set them (except WINSIZE) to some proper values.
 * */
float TIMEOUT = 50.0; 
int WINSIZE;         //This is supplied as cmd-line parameter; You will need to read this value but do NOT modify it's value; 
int SND_BUFSIZE = 100; //Sender's Buffer size
int RCV_BUFSIZE = 100; //Receiver's Buffer size

/* called from layer 5, passed the data to be sent to other side */


/***************************************************************/
/************* Print structure value ***************************/
/*************************************************************8*/
void printVal(struct pkt *pack){
	
	/*Variable declaration*/
	int i;
	
	/*Code start*/
	printf("\nThe value of seq num = %d \n",(*pack).seqnum);
	printf("The value of ack num = %d \n",(*pack).acknum);
	printf("The value of checksum = %d \n",(*pack).checksum);
	printf("The payload values are :  \n");
	for (i=0;i<20;i++){
		printf("%c",(*pack).payload[i]);

	}
}

/***************************************************************/
/*************** Calculate Checksum ****************************/
/***************************************************************/

int checkSum(struct pkt *pack){
	/*Variable declaration*/
        int i;
	int checkSum;

	/*Code Start*/
	
	checkSum=(*pack).acknum+(*pack).seqnum;

	for (i=0;i<20;i++){
		checkSum+=(int)(*pack).payload[i];
	}
	printf(" \nValue for checksum for current packet = %d\n",checkSum);
	
	return checkSum;

}

int base=0;
int nextseqnum=0;
static int seqnum=0;
static int acknum=0;
static int prevseq=-1;
static int expack=-1;
static int sbufSt=0;

static int msgnum=0;

static struct pkt window[1000];
	

static struct msg sbuf[100];
static struct pkt pack;


void A_output(message)
  struct msg message;
{
	
	/***********************Variable Declaration***********************/
	int i,j;
	
	/*********************Print Message********************************/
	A_application++;
	printf("\n\nA_OUTPUT called value of A_application = %d",A_application);
	
	
	if (nextseqnum<base+WINSIZE){
		for (i=0;i<20;i++){
			//	printf("%c",message.data[i]);
		}
	
			
		bzero(pack.payload,20);
		for (i=0;i<20;i++){
			pack.payload[i]=message.data[i];
		}
		
		printf("\nA_OUTPUT Message in payload is  %s\n",pack.payload);
		pack.seqnum=nextseqnum;

		pack.acknum=0;
        
		/*Calculate checksum for current message*/
		pack.checksum=checkSum(&pack);
		/*check if values are correct*/
		//printVal(&pack);
		
		
		
		/*Add packet to window*/
		for (i=base;i<base+WINSIZE;i++){
			if (window[i].seqnum==-1){
				for(j=0;j<20;j++){
					window[i].seqnum=pack.seqnum;
					window[i].acknum=0;
					window[i].checksum=pack.checksum;
					window[i].payload[j]=pack.payload[j];
				}
				printVal(&window[i]);
				break;
			}
		}
		
		

		/*Send to B*/
		tolayer3(0,pack);
		A_transport++;
		printf("\n A_OUTPUT A sent data over link value of a transport = %d",A_transport);
		
		if (nextseqnum==base){
			/*Start the timer*/
			//printf("\n****************START TIMER FOR PACKET %d in A_OUTPUT******************\n",pack.seqnum);
			starttimer(0,TIMEOUT);
		}
		
		nextseqnum++;
		
	}

	else{
		printf("A_OUTPUT A send window is full...\n Checking if we can buffer message...");
		if(msgnum==SND_BUFSIZE){
			printf("\nSENDER message buffer full, cannot send any more. Program must terminate");
			exit(1);
		}

		else{
			printf("\nA_OUTPUT SENDER buffer not yet full, buffering message");
			for (i=0;i<20;i++){
				sbuf[msgnum].data[i]=message.data[i];
			}
			msgnum++;
		}
	}
	
	bzero(message.data,20);

}

void B_output(message)  /* need be completed only for extra credit */
  struct msg message;
{
	

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
	int first=0;
	int i,j;
	printf("**************************************A_INPUT CALLED*************************************");

	printf("\nA_INPUT packet ACK num= %d exp ack num=%d\n",packet.acknum, expack+1);

 	if(checkSum(&packet)==packet.checksum){

		//printf("\nACK Packet passed checksum... \n Proceeding to check if packet has expected seq num\n");
		if (packet.acknum==expack+1){
			printf("\nA_INPUT SENDER: Correct packet received by receiver.");
			//printf("\nIncrease base and check if has sent all its packets");
			base++;
			expack++;
			
			printf("\nA_INPUT Value of base:: %d Value of next seq num:: %d\n",base, nextseqnum);


			printf("\n\n******************A_INPUT Stop timer for %d th packet*************************",packet.seqnum);
			stoptimer(0);
			
			if(base<nextseqnum){
				printf("\n****************START TIMER FOR PACKET in A_INPUT %d******************\n",++packet.seqnum);
				starttimer(0,TIMEOUT);
			}
		
			if (((msgnum-sbufSt)!=0)&&(nextseqnum<base+WINSIZE)){			//Means all initial window files are sent and files are now in buffer
				
				bzero(packet.payload,20);
				for (i=0;i<20;i++){
					packet.payload[i]=sbuf[sbufSt].data[i];
				}
				sbufSt++;
				
				printf("\nMessage in payload is  %s\n",packet.payload);
				packet.seqnum=nextseqnum;

				packet.acknum=0;
        
				/*Calculate checksum for current message*/
				packet.checksum=checkSum(&packet);
				/*check if values are correct*/
				//printVal(&packet);
		
		
		
				/*Add packet to window*/
				for (i=base;i<(base+WINSIZE);i++){
					if (window[i].seqnum==-1){
						for(j=0;j<20;j++){
							window[i].seqnum=packet.seqnum;
							window[i].acknum=0;
							window[i].checksum=packet.checksum;
							window[i].payload[j]=packet.payload[j];
						}
						break;
					}
				}
	
				/* start timeout */
		           if ( base == nextseqnum )
				{
			 		starttimer(0,TIMEOUT);
			 		
	            	}
	            	/*Send to B */
	            	tolayer3(0,packet);
				A_transport++;
				printf("\n A_INPUT A sent data over link from buffer. Value of A Transport = %d",A_transport);

				nextseqnum++;

				
			}
			
		}
		else if(packet.acknum>(expack+1)){
			printf("\nA_INPUT: Receiver has replied with a value larger than expack. Receiver ack was lost and new one has arrived\n");
			printf("\nA_INPUT: Old VALUE of base and exp ack :: %d %d \n",base,expack+1);
			expack=packet.acknum;
			base=expack+1;
			
			printf("\nA_INPUT: NEW VALUE of base and exp ack :: %d %d \n",base,expack+1);
			
			printf("\n\n******************A_INPUT Stop timer *************************");
			stoptimer(0);
			
			if(base<nextseqnum){
				printf("\n****************START TIMER FOR LAST UNACKed PACKET in A_INPUT %d******************\n",base);
				starttimer(0,TIMEOUT);
			}

		}
		else if (packet.acknum<(expack+1)){
				printf("\nDuplicate previous ack received. Ignore...");
			
		}

	}
	else{
		printf("\nA_INPUT: Received packet has checksum issues. Do nothing...\n");
	}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	int i,j;
	printf("**********A_TIMER_Timeout for packet %d has occured:::Go back and resend packets",expack+1);
	
	printf("\nA_TIMERStart Timer for ::: %d ",expack+1);
	starttimer(0,TIMEOUT);
	
	base=expack+1;
	nextseqnum=base;

	for (i=base;i<(base+WINSIZE);i++){
		printf("Value of seqnum in window:: %d",window[i].seqnum);
		if(window[i].seqnum!=-1){
			printf("\nA_TIMER : value of seqnum and message of resent packet is :: %d \n",window[i].seqnum);
			for (j=0;j<20;j++){
				printf("%c",window[i].payload[j]);
			}

			tolayer3(0,window[i]);
			A_transport++;
			printf("\n A_TIMER A sent data over link value of a transport = %d",A_transport);
		
			nextseqnum++;
		}
	}
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	//Init values of window seq num to -1
	int i;
	for (i=0;i<1000;i++){
		window[i].seqnum=-1;
	}

}


/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{

	++B_transport;
	printf("\nB_INPUT called value of B transport= %d",B_transport);
	int i;

	if (checkSum(&packet)==packet.checksum){

		printf("\nB_INPUT Packet passed checksum\nMessage received is :");
		for (i=0;i<20;i++){
			printf("%c",packet.payload[i]);
		}

		
		if (packet.seqnum==(prevseq+1)){
			/*Deliver Packet to upper layer*/
			printf("\n\n B_INPUT Packet seqnum = prev seq num+1!!!! deliver to application");
			
			//printf("B_INPUT Initial prev seqnum value %d",prevseq);
			tolayer5(1,packet.payload);
			++B_application;
			printf("\n B_INPUT Message sent to application layer value of B application = %d\n",B_application);
			packet.acknum=packet.seqnum;
			packet.checksum=checkSum(&packet);
			prevseq=packet.seqnum;

			printf("B_INPUT preseqnum after message sent %d",prevseq);
			printf("\n\nSend ACK to A input");
			/*Respond to A with same packet*/
			tolayer3(1,packet);
		
		}
		else if(packet.seqnum<=prevseq){
			printf("\n\n B_INPUT Sequence number is the same as previous packet\nPacket is a duplicate::::Discarding Packet....\n\n");
		}
		else if(packet.seqnum>(prevseq+1)){
			printf("\n\n B_INPUT Sequence number is the not as expected packet sequence number\nResend last proper packet sequence number:: %d",prevseq);
			packet.acknum=prevseq;
			printf("\n\nSend ACK to A input");
			/*Respond to A with this packet*/
			tolayer3(1,packet);
		
		}
		
		
	}
	else{
		printf("\n B_INPUT Packet has some checksum issue\n");
		printf("\n Set ack to previous correctly received packet");

		/*set negative acknowledgement and send to A*/

		packet.acknum=prevseq;
		printf("\n\nSend ACK to A input");
		tolayer3(1,packet);

	}

	
}

/* called when B's timer goes off */
void B_timerinterrupt()
{
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event {
   float evtime;           /* event time */
   int evtype;             /* event type code */
   int eventity;           /* entity where event occurs */
   struct pkt *pktptr;     /* ptr to packet (if any) assoc w/ this event */
   struct event *prev;
   struct event *next;
 };
struct event *evlist = NULL;   /* the event list */

//forward declarations
void init();
void generate_next_arrival();
void insertevent(struct event*);

/* possible events: */
#define  TIMER_INTERRUPT 0  
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define   A    0
#define   B    1



int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */ 
int nsimmax = 0;           /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob = 0.0;	   /* probability that a packet is dropped */
float corruptprob = 0.0;   /* probability that one bit is packet is flipped */
float lambda = 0.0; 	   /* arrival rate of messages from layer 5 */
int ntolayer3 = 0; 	   /* number sent into layer 3 */
int nlost = 0; 	  	   /* number lost in media */
int ncorrupt = 0; 	   /* number corrupted by media*/

/**
 * Checks if the array pointed to by input holds a valid number.
 *
 * @param  input char* to the array holding the value.
 * @return TRUE or FALSE
 */
int isNumber(char *input)
{
    while (*input){
        if (!isdigit(*input))
            return 0;
        else
            input += 1;
    }

    return 1;
}

int main(int argc, char **argv)
{
   struct event *eventptr;
   struct msg  msg2give;
   struct pkt  pkt2give;
   
   int i,j;
   char c;

   int opt;
   int seed;

   //Check for number of arguments
   if(argc != 5){
   	fprintf(stderr, "Missing arguments\n");
	printf("Usage: %s -s SEED -w WINDOWSIZE\n", argv[0]);
   	return -1;
   }

   /* 
    * Parse the arguments 
    * http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html 
    */
   while((opt = getopt(argc, argv,"s:w:")) != -1){
       switch (opt){
           case 's':   if(!isNumber(optarg)){
                           fprintf(stderr, "Invalid value for -s\n");
                           return -1;
                       }
                       seed = atoi(optarg);
                       break;

           case 'w':   if(!isNumber(optarg)){
                           fprintf(stderr, "Invalid value for -w\n");
                           return -1;
                       }
                       WINSIZE = atoi(optarg);
                       break;

           case '?':   break;

           default:    printf("Usage: %s -s SEED -w WINDOWSIZE\n", argv[0]);
                       return -1;
       }
   }
  
   init(seed);
   A_init();
   B_init();
   
   while (1) {
        eventptr = evlist;            /* get next event to simulate */
        if (eventptr==NULL)
           goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist!=NULL)
           evlist->prev=NULL;
        if (TRACE>=2) {
           printf("\nEVENT time: %f,",eventptr->evtime);
           printf("  type: %d",eventptr->evtype);
           if (eventptr->evtype==0)
	       printf(", timerinterrupt  ");
             else if (eventptr->evtype==1)
               printf(", fromlayer5 ");
             else
	     printf(", fromlayer3 ");
           printf(" entity: %d\n",eventptr->eventity);
           }
        time = eventptr->evtime;        /* update time to next event time */
        if (nsim==nsimmax)
	  break;                        /* all done with simulation */
        if (eventptr->evtype == FROM_LAYER5 ) {
            generate_next_arrival();   /* set up future arrival */
            /* fill in msg to give with string of same letter */    
            j = nsim % 26; 
            for (i=0; i<20; i++)  
               msg2give.data[i] = 97 + j;
            if (TRACE>2) {
               printf("          MAINLOOP: data given to student: ");
                 for (i=0; i<20; i++) 
                  printf("%c", msg2give.data[i]);
               printf("\n");
	     }
            nsim++;
            if (eventptr->eventity == A) 
               A_output(msg2give);  
             else
               B_output(msg2give);  
            }
          else if (eventptr->evtype ==  FROM_LAYER3) {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i=0; i<20; i++)  
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
	    if (eventptr->eventity ==A)      /* deliver packet by calling */
   	       A_input(pkt2give);            /* appropriate entity */
            else
   	       B_input(pkt2give);
	    free(eventptr->pktptr);          /* free the memory for packet */
            }
          else if (eventptr->evtype ==  TIMER_INTERRUPT) {
            if (eventptr->eventity == A) 
	       A_timerinterrupt();
             else
	       B_timerinterrupt();
             }
          else  {
	     printf("INTERNAL PANIC: unknown event type \n");
             }
        free(eventptr);
        }

terminate:
	//Do NOT change any of the following printfs
	printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n",time,nsim);
	
	printf("\n");
	printf("Protocol: GBN\n");
	printf("[PA2]%d packets sent from the Application Layer of Sender A[/PA2]\n", A_application);
	printf("[PA2]%d packets sent from the Transport Layer of Sender A[/PA2]\n", A_transport);
	printf("[PA2]%d packets received at the Transport layer of Receiver B[/PA2]\n", B_transport);
	printf("[PA2]%d packets received at the Application layer of Receiver B[/PA2]\n", B_application);
	printf("[PA2]Total time: %f time units[/PA2]\n", time);
	printf("[PA2]Throughput: %f packets/time units[/PA2]\n", B_application/time);
	return 0;
}



void init(int seed)                         /* initialize the simulator */
{
  int i;
  float sum, avg;
  float jimsrand();
  
  
   printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n");
   printf("Enter the number of messages to simulate: ");
   scanf("%d",&nsimmax);
   printf("Enter  packet loss probability [enter 0.0 for no loss]:");
   scanf("%f",&lossprob);
   printf("Enter packet corruption probability [0.0 for no corruption]:");
   scanf("%f",&corruptprob);
   printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
   scanf("%f",&lambda);
   printf("Enter TRACE:");
   scanf("%d",&TRACE);

   srand(seed);              /* init random number generator */
   sum = 0.0;                /* test random number generator for students */
   for (i=0; i<1000; i++)
      sum=sum+jimsrand();    /* jimsrand() should be uniform in [0,1] */
   avg = sum/1000.0;
   if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n" ); 
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit(0);
    }

   ntolayer3 = 0;
   nlost = 0;
   ncorrupt = 0;

   time=0.0;                    /* initialize time to 0.0 */
   generate_next_arrival();     /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand() 
{
  double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
  float x;                   /* individual students may need to change mmm */ 
  x = rand()/mmm;            /* x should be uniform in [0,1] */
  return(x);
}  

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/
 
void generate_next_arrival()
{
   double x,log(),ceil();
   struct event *evptr;
    //char *malloc();
   float ttime;
   int tempint;

   if (TRACE>2)
       printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");
 
   x = lambda*jimsrand()*2;  /* x is uniform on [0,2*lambda] */
                             /* having mean of lambda        */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + x;
   evptr->evtype =  FROM_LAYER5;
   if (BIDIRECTIONAL && (jimsrand()>0.5) )
      evptr->eventity = B;
    else
      evptr->eventity = A;
   insertevent(evptr);
} 


void insertevent(p)
   struct event *p;
{
   struct event *q,*qold;

   if (TRACE>2) {
      printf("            INSERTEVENT: time is %lf\n",time);
      printf("            INSERTEVENT: future time will be %lf\n",p->evtime); 
      }
   q = evlist;     /* q points to header of list in which p struct inserted */
   if (q==NULL) {   /* list is empty */
        evlist=p;
        p->next=NULL;
        p->prev=NULL;
        }
     else {
        for (qold = q; q !=NULL && p->evtime > q->evtime; q=q->next)
              qold=q; 
        if (q==NULL) {   /* end of list */
             qold->next = p;
             p->prev = qold;
             p->next = NULL;
             }
           else if (q==evlist) { /* front of list */
             p->next=evlist;
             p->prev=NULL;
             p->next->prev=p;
             evlist = p;
             }
           else {     /* middle of list */
             p->next=q;
             p->prev=q->prev;
             q->prev->next=p;
             q->prev=p;
             }
         }
}

void printevlist()
{
  struct event *q;
  int i;
  printf("--------------\nEvent List Follows:\n");
  for(q = evlist; q!=NULL; q=q->next) {
    printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
    }
  printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(AorB)
int AorB;  /* A or B is trying to stop timer */
{
 struct event *q,*qold;

 if (TRACE>2)
    printf("          STOP TIMER: stopping timer at %f\n",time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
       /* remove this event */
       if (q->next==NULL && q->prev==NULL)
             evlist=NULL;         /* remove first and only event on list */
          else if (q->next==NULL) /* end of list - there is one in front */
             q->prev->next = NULL;
          else if (q==evlist) { /* front of list - there must be event after */
             q->next->prev=NULL;
             evlist = q->next;
             }
           else {     /* middle of list */
             q->next->prev = q->prev;
             q->prev->next =  q->next;
             }
       free(q);
       return;
     }
  printf("Warning: unable to cancel your timer. It wasn't running.\n");
}


void starttimer(AorB,increment)
int AorB;  /* A or B is trying to stop timer */
float increment;
{

 struct event *q;
 struct event *evptr;
 //char *malloc();

 if (TRACE>2)
    printf("          START TIMER: starting timer at %f\n",time);
 /* be nice: check to see if timer is already started, if so, then  warn */
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
   for (q=evlist; q!=NULL ; q = q->next)  
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
      printf("Warning: attempt to start a timer that is already started\n");
      return;
      }
 
/* create future event for when timer goes off */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + increment;
   evptr->evtype =  TIMER_INTERRUPT;
   evptr->eventity = AorB;
   insertevent(evptr);
} 


/************************** TOLAYER3 ***************/
void tolayer3(AorB,packet)
int AorB;  /* A or B is trying to stop timer */
struct pkt packet;
{
 struct pkt *mypktptr;
 struct event *evptr,*q;
 //char *malloc();
 float lastime, x, jimsrand();
 int i;


 ntolayer3++;

 /* simulate losses: */
 if (jimsrand() < lossprob)  {
      nlost++;
      if (TRACE>0)    
	printf("          TOLAYER3: packet being lost\n");
      return;
    }  

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */ 
 mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
 mypktptr->seqnum = packet.seqnum;
 mypktptr->acknum = packet.acknum;
 mypktptr->checksum = packet.checksum;
 for (i=0; i<20; i++)
    mypktptr->payload[i] = packet.payload[i];
 if (TRACE>2)  {
   printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
	  mypktptr->acknum,  mypktptr->checksum);
    for (i=0; i<20; i++)
        printf("%c",mypktptr->payload[i]);
    printf("\n");
   }

/* create future event for arrival of packet at the other side */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtype =  FROM_LAYER3;   /* packet will pop out from layer3 */
  evptr->eventity = (AorB+1) % 2; /* event occurs at other entity */
  evptr->pktptr = mypktptr;       /* save ptr to my copy of packet */
/* finally, compute the arrival time of packet at the other end.
   medium can not reorder, so make sure packet arrives between 1 and 10
   time units after the latest arrival time of packets
   currently in the medium on their way to the destination */
 lastime = time;
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==FROM_LAYER3  && q->eventity==evptr->eventity) ) 
      lastime = q->evtime;
 evptr->evtime =  lastime + 1 + 9*jimsrand();
 


 /* simulate corruption: */
 if (jimsrand() < corruptprob)  {
    ncorrupt++;
    if ( (x = jimsrand()) < .75)
       mypktptr->payload[0]='Z';   /* corrupt payload */
      else if (x < .875)
       mypktptr->seqnum = 999999;
      else
       mypktptr->acknum = 999999;
    if (TRACE>0)    
	printf("          TOLAYER3: packet being corrupted\n");
    }  

  if (TRACE>2)  
     printf("          TOLAYER3: scheduling arrival on other side\n");
  insertevent(evptr);
} 

void tolayer5(AorB,datasent)
  int AorB;
  char datasent[20];
{
  int i;  
  if (TRACE>2) {
     printf("          TOLAYER5: data received: ");
     for (i=0; i<20; i++)  
        printf("%c",datasent[i]);
     printf("\n");
   }
  
}
