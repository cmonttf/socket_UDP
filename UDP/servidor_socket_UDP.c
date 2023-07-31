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

int puerto_id;

/**********************************************************/
/* función catch que captura una interrupción             */
/**********************************************************/
void catch(int sig)
{
	printf("***Señal: %d atrapada!\n", sig);
  printf("***Cerrando servicio ...\n");
  close(coneccion_id);
  close(puerto_id);
  printf("***Servicio cerrado.\n");
  exit(EXIT_SUCCESS);
}


/**********************************************************/
/* función MAIN                                           */
/* Orden Parametros: Puerto                               */
/**********************************************************/

int main(int argc, char *argv[]) {
  socklen_t destino_tam;
  struct sockaddr_in origen_dir, destino_dir;
  char mensaje_entrada[MAX_TAM_MENSAJE], mensaje_salida[MAX_TAM_MENSAJE];
  int recibidos, enviados; //bytes recibidos

  if (argc != 2) {
    printf("\n\nEl número de parámetros es incorrecto\n");
    printf("Use: %s <puerto>\n\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // Creamos el socket del servidor
	puerto_id = socket(AF_INET, SOCK_DGRAM,0);
	if (puerto_id == -1) {
    printf("ERROR: El socket del servidor no se ha creado correctamente!\n");
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina servidora
  origen_dir.sin_family = AF_INET;
  origen_dir.sin_port = htons(atoi(argv[1]));
  origen_dir.sin_addr.s_addr = htonl(INADDR_ANY);

  //Asigna una direccion local al socket
	if( bind(puerto_id, (struct sockaddr*)&origen_dir, sizeof(origen_dir)) == -1) {
    printf("ERROR al unir el socket a la dirección de la máquina servidora\n");
    close(puerto_id);
    exit(EXIT_FAILURE);
  }



  signal(SIGINT, &catch);

    printf("\n***Servidor ACTIVO escuchando en el puerto: %s ...\n",argv[1]);  

    do {
    //Quedo a la espera de algun mensaje
    destino_tam=sizeof(destino_dir);
    recibidos= recvfrom(puerto_id, mensaje_entrada, MAX_TAM_MENSAJE, 0, (struct sockaddr*)&destino_dir, &destino_tam);
    if(recibidos <0) {
      printf("ERROR de recvfrom() \n");
      exit(EXIT_FAILURE);
    } else
    {
    printf("***Servidor recibe dato del cliente: %d.\n",destino_dir.sin_addr.s_addr);
	printf("<<Client envía >>: %s\n", mensaje_entrada);
    }
      //Envia el mensaje al cliente
	sprintf(mensaje_salida, "El mensaje recibido fue --- %s ---.",mensaje_entrada);

    enviados = sendto(puerto_id, strcat(mensaje_salida,"\0"), MAX_TAM_MENSAJE, 0, (struct sockaddr*)&destino_dir, destino_tam);
		  if (enviados<0) {
        printf("Error en sendto() \n");
        exit(EXIT_SUCCESS);
      } else
        printf("<<Server replica>>: %s\n", mensaje_salida);
    }while(1);    
    

  //Cierra el servidor
  printf("***Cerrando servicio ...\n");
  close(puerto_id);
  printf("***Servicio cerrado.\n");
  exit(EXIT_SUCCESS);
}