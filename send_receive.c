/*
 * cmd.c:
 *	prova i comandi con la scheda tiva
 *
 *
 *<robotics.itismattei@gmail.com>
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringSerial.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

/// for usleep()
#include <unistd.h>


int main ()
{
  int fd, checksum = 0, stato = 0 ;
  FILE *fp;
  int count = 48, conteggio = 0;
  unsigned int nextTime ;

  printf("Sto iniziando\n");
  if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
  {
    printf("Unable to open serial device. \n");
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  /// apre il file di testo
  fp = fopen("dati.txt", "a");
  if (fp == NULL)
    printf("Errore di apertura del file \n");

  fclose(fp);

  nextTime = millis () + 500 ;

while(1){
  fp = fopen("dati.txt", "a");
  switch(stato){

  case 0:
  /// invio del comando F: F0 check *
  /// il comando e' 70 - 0 - 70 xor 0 xor $A9 - 42
  /// oppure         F - 0 - F  xor 0 xor $A9 - *
    checksum = 'F' ^ 0 ^ 0xA9;
      printf ("\nOut: %c: ", 'F') ;
      fflush (stdout) ;
      //// invia su seriale
      serialPutchar (fd, 'F') ;
      serialPutchar (fd, 0);
      serialPutchar (fd, checksum);
      serialPutchar (fd, '*');
      fprintf(fp, "\nF %d: ", checksum);
      stato = 1;
  break;

  /// invio del comando B (indietro)
  case 1:
    checksum = 'B' ^ 0 ^ 0xA9;
      printf ("\nOut: %c: ", 'B') ;
      fflush (stdout) ;
      //// INVIA su seriale
      serialPutchar (fd, 'B') ;
      serialPutchar (fd, 0);
      serialPutchar (fd, checksum);
      serialPutchar (fd, '*');
      fprintf(fp, "\nB %d: ", checksum);
      stato = 2;
  break;

  /// invio del comando 'S' (STOP)
  case 2:
    checksum = 'S' ^ 0 ^ 0xA9;
      printf ("\nOut: %c: ", 'S') ;
      fflush (stdout) ;
      /// invia su seriale
      serialPutchar (fd, 'S') ;
      serialPutchar (fd, 0);
      serialPutchar (fd, checksum);
      serialPutchar (fd, '*');
      fprintf(fp, "\nS %d: ", checksum);
      stato = 3;
  break;

  case 3:
      checksum = 'D' ^ 1 ^ 0xA9;
      printf ("\nOut: %c: %c", 'D', '1') ;
      fflush (stdout) ;
      /// invia su seriale
      serialPutchar (fd, 'D') ;
      serialPutchar (fd, 1);
      serialPutchar (fd, checksum);
      serialPutchar (fd, '*');
      fprintf(fp, "\nD1 %d: ", checksum);
      stato = 4;
  break;

  case 4:
      checksum = 'D' ^ 3 ^ 0xA9;
      printf ("\nOut: %c: %c", 'D', '3') ;
      fflush (stdout) ;
      /// invia su seriale
      serialPutchar (fd, 'D') ;
      serialPutchar (fd, 3);
      serialPutchar (fd, checksum);
      serialPutchar (fd, '*');
      fprintf(fp, "\nD3 %d: ", checksum);
      stato = 0;
  break;
  
  default:
      stato = 0;
  break; 

/* if (conteggio > 16){
      while (serialDataAvail (fd)){
        //printf("ricevuto \n");
        //printf (" -> %3d", serialGetchar (fd)) ;
        printf("%c", serialGetchar(fd));
        fflush (stdout) ;
        //printf("\n");	
      }
      /// reset di conteggio: aspetta altri 16 secondi
      conteggio = 0;
      printf("\n");
    }
  */
     } /// fine dello SWITCH 
  
   ///attesa di 150 ms
  delay(150);
  /// occorre leggere la risposta. 
  while (serialDataAvail(fd)){
   int dato_ric =  serialGetchar(fd);	  
   printf (" -> %3d ", dato_ric) ;
   fflush (stdout) ;
   fprintf(fp, "%d - ", dato_ric);
  }
  fprintf(fp, "#\n");
  fclose(fp);
  sleep(1);
 
  }
 
  return 0 ;
}
