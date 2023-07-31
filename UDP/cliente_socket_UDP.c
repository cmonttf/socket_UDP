/*SERVIDOR*/
/********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define MAX_TAM_MENSAJE 50 //Numero de caracteres maximo del mensaje

char mensaje[MAX_TAM_MENSAJE];

/**********************************************************/
/* función catch que captura una interrupción             */
/**********************************************************/
void catch(int sig)
{
	strcpy(mensaje,"terminar();");
}

/**********************************************************/
/* función MAIN                                           */
/* Orden Parametros: IP destino, puerto                   */
/**********************************************************/
int main(int argc, char *argv[]) {
	int puerto_id;
	struct sockaddr_in origen_dir, destino_dir;
    socklen_t destino_tam;
	char respuesta[MAX_TAM_MENSAJE]; mensaje[MAX_TAM_MENSAJE];
    int recibidos, enviados; //bytes recibidos y enviados


 	if (argc != 3) {
    printf("\n\nEl número de parámetros es incorrecto\n");
    printf("Use: %s <IP_servidor> <puerto>\n\n",argv[0]);
    exit(EXIT_FAILURE);
  }

	// Creamos el socket del cliente
	puerto_id = socket(AF_INET,SOCK_DGRAM,0);
	if (puerto_id == -1) {
    printf("ERROR: El socket del cliente no se ha creado correctamente!\n");
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina cliente
    origen_dir.sin_family = AF_INET;
	origen_dir.sin_port = htons(0); 								//Asigna un puerto disponible dela máquina
	origen_dir.sin_addr.s_addr = htonl(INADDR_ANY); //Asigna una IP de la máquina

	//Asigna una direccion local al socket
	if( bind(puerto_id, (struct sockaddr*)&origen_dir, sizeof(origen_dir)) == -1) {
    printf("ERROR al unir el socket a la dirección de la máquina cliente\n");
    close(puerto_id);
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina servidora
	destino_dir.sin_family = AF_INET;
	destino_dir.sin_addr.s_addr = inet_addr(argv[1]);
	destino_dir.sin_port = htons(atoi(argv[2]));



 signal(SIGINT, &catch);

  do {

    //Envía el mensaje
    printf("<<Client>>: ");
    scanf("%s",mensaje);
    enviados = sendto(puerto_id, mensaje, MAX_TAM_MENSAJE, 0, (struct sockaddr*)&destino_dir, sizeof(destino_dir));

    if(enviados<0) {
      printf("ERROR en sento() \n");
      close(puerto_id);
      exit(EXIT_FAILURE);
    } else {
      printf("***Cliente envía: %s a: %s en puerto: %s \n",mensaje, argv[1], argv[2]);
    }

	  //Recibe la respuesta
    destino_tam=sizeof(destino_dir);
    recibidos = recvfrom(puerto_id, respuesta, sizeof(respuesta), 0, (struct sockaddr*)&destino_dir, &destino_tam);
    if(recibidos<0) {
      printf("ERROR en recvfrom() \n");
      close(puerto_id);
      exit(EXIT_FAILURE);
    } else {
      printf("<<Server>>: %s\n", respuesta);
    }

    //Se cierra la conexión (socket)
    if(strcmp(mensaje,"terminar();") == 0) {
      printf("***Cliente terminó conección con servidor.\n");
      close(puerto_id);
      exit(EXIT_FAILURE);
    }

  } while(1);
	//Se cierra la conexión (socket)
  printf("\nCliente termina.\n");
	close(puerto_id);
  exit(EXIT_SUCCESS);
}