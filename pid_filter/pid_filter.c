#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pid_filter.h"



/**  
* Simple DVB PID filter with find the specified pid in the specified file, creating a new file containing only packets of that pid *
* No other processing is dont with the file.                                                                                       *
* Can filter boh 204/188 packet sized streams                                                                                      *
* Author :Jonathan Bell                                                                                                            *
* Date :17 Feb 2022                                                                                                                *
* Usage : ./filter -p <pid> -f <input.ts>                                                                                          *
* Build : gcc -o pid_filter ./pid_filter.c                                                                                         *
* @result = New Pid file                                                                                                           *
* TODO: Make output possible to stdout by input switch for pipe operation                                                          *
*/ 


/** 
   Show Help 
*/

void show_help(void)
{
  printf("To use this application please use the following format ./pid_filter -p=<pid> -f ./bbc.ts n");
}



/** 
filter pid output to a new file, called pid.ts
*/

int filter_pid(int pid,int start,int packet_size, FILE *fp)
{
  int ret = -1;
  char filename[8] = {0};
  FILE *of = NULL;
  char *buffer;
  unsigned short second_short = 0; 
  unsigned short matching_pid = 0;
 
  if(!fp)
    {
      return(ret);
    }

  buffer = malloc((sizeof(char) * packet_size));

  if(buffer)
    {
      sprintf(filename,"%d.pid",pid); 
      of = fopen(filename,"w");

      if(of)
	{
	   fseek(fp,0,SEEK_SET);
	  /* set file pointer to start */
	  fseek(fp,start,SEEK_SET);

	 
	  while (fread(buffer,sizeof(*buffer),packet_size, fp) == packet_size)
	    {
	      if(buffer[0] == 0x47)
		{
		  
		  second_short = (unsigned char)(buffer[1] << 8) + (unsigned char)(buffer[2]);
		  matching_pid = second_short & PID_MASK;
		  
		  if((unsigned short) pid == matching_pid)
		    {
		      printf("matching pid = 0x%x\n",matching_pid);
		      printf("Sync Byte: 0x%x\n",(unsigned char) *buffer);
		      printf("PID: 0x%x\n", matching_pid);
		      printf("TEI: 0x%x\n", (second_short & TEI_MASK) >> 8);
		      printf("PUSI:0x%x\n", (second_short & PUSI_MASK) >>8);
		      printf("TRP:0x%x\n",(unsigned char) (second_short & TR_MASK) >> 8);
		      ret += fwrite(buffer, sizeof(unsigned char), packet_size, of);	
		    }
		}
	    }
	}
    }
  
  fclose(of);
  
  return ret;
  
}


/** 
   Find packet size in file
*/

int find_packet_size(FILE *fp,int *packet_start)
{
  int packet_size = -1;
  char sample[PS_SAMPLE];
  int size = 0;
  int i = 0;
  
  size = fread(sample,sizeof(char),PS_SAMPLE,fp);
  if(size != PS_SAMPLE)
    {
      /*Consider sample size maybe to small to judge, default to 188 */
      /* filtering over say 10 packets maybe better, as transport streams tend to be very large */
      packet_size = 188;
    }

  for (i = 0; i<PS_SAMPLE;i++)
    {
      if(sample[i] == 0x47)
	{
	  *packet_start= i;
	  /* packet start indicator found */
	  if(PS_SAMPLE - i > 204)
	    {
	      if(sample[i + 204] == 0x47)
		{
		  /* small sample check, suspect 204 packet size */
		  packet_size = 204;
		  
		}
	      else if(sample[i + 188] == 0x47)
		{
		  /*small sample check, suspect 188 packet size */
		  packet_size = 188;
		}
	    }
	  
	}
    }
    
  return packet_size;
}

/** 
   Simple DVB packet filter                                                         
   Takes a DVB transport stream and filters the main packets    
   for analysis of the contents of a transport stream, stripping 
   requested pid 
*/

int main(int argc,char **argv)
{

  int c = 0;
  int pid = 0;
  int packet_size = 0;
  char *pid_val = NULL;
  char *filename = NULL;
  FILE *fp = NULL;
  int start = 0;

  
  while((c = getopt(argc,argv,"udp:f:")) != -1)
    {
      switch(c)
	{
	case 'p':
	  pid_val = optarg;
	  pid = atoi(pid_val);
	  break;
	case 'f':
	  filename=optarg;
	  break;
	  /* decide at later time to disable prints and push result out to standout, for pipe operation */
	case 'd':
	  break;
	case 'u':
	  break;
	case '?':
	case ':':
	default:
	  show_help();
	  return (-1);   
	}
    }
  
  printf("File to open: %s\n",filename);
  printf("Pid to filter: %d\n",pid);
  
  fp = fopen(filename,"r");
  if(fp != NULL)
    {
      /* ts packets are generally 204 or 188 bytes depending on the transport */
      /* block size filtered should be a multiple of this */
      packet_size = find_packet_size(fp,&start);
      printf("Packet offset = %d\n",start);
      printf("Packet size = %d\n",packet_size);
      
      filter_pid(pid,start,packet_size,fp);
      fclose(fp);
    }
   
  return (0);
  
}
