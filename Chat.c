#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <wait.h>


void error(const char *msg)
{
perror(msg);
exit(1);
}
//********************************Parte Cliente*********************************

int Cliente(int puertoCliente,char *IpCliente)
{
  int sockfd; 
  char DatosEnviados[1024]; 
  struct hostent *host; /*un puntero a una estructura del tipo hostent, en la cual se define un equipo host en Internet, tales como el nombre oficial del host, el tipo de dirección que se devuelve; actualmente, siempre AF_INET, la longitud, en bytes, de la dirección.*/
  struct sockaddr_in server_addr; /*contendrá la dirección del servidor al que nos queremos conectar*/
  host = gethostbyname(IpCliente); /*contiene el nombre de un host a través de Internet*/
  sockfd = socket(AF_INET, SOCK_STREAM, 0); /*Identificador de socket. Se utilizará para conectarse, recibir conexiones, enviar y recibir datos, etc*/
		
	if (sockfd < 0)
		error("ERROR de apertura de socket");  
	
	server_addr.sin_family = AF_INET; /*Toma el valor AF_INET (tipo de dominio)*/
	server_addr.sin_port = htons(puertoCliente); /*Representa el número de puerto*/
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	
	/*La función bzero() establece todos los valores en un búfer a cero. Tiene dos
	argumentos, el primero es un puntero a la memoria intermedia y el segundo es el
	tamaño del búfer*/ 
	
	bzero(&(server_addr.sin_zero),8);
	
	/*La función connect es llamada por el cliente para establecer una conexión con el
	servidor. Toma tres argumentos, el descriptor de fichero socket, la dirección de la
	máquina a la que quiere conectarse (incluyendo el número de puerto), y el tamaño de
	esta dirección.*/
	  
    if (connect(sockfd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) 
		error("ERROR de conexión");
			  
  	
 while(1) //Ciclo que envia datos 
        {  
			printf("\n \E[1;36mYo\E[0;36m (Escriba chao para cerrar la conversación): "); //El printf sale con color el cual es cyan
			fflush(stdout); //Limpia el buffer 
			gets(DatosEnviados); // Captura el dato
           
		   if (strcmp(DatosEnviados, "chao") != 0 || strcmp(DatosEnviados, "Chao") != 0) // si envía chao entonces se cierra la conversación
			   send(sockfd,DatosEnviados,strlen(DatosEnviados), 0); 
			
		   else
		   {                                                 // Si es5 el array adios manda el datos y cierra el socket
			   send(sockfd,DatosEnviados,strlen(DatosEnviados), 0);  
			   close(sockfd);
			   break;
		   }
       }
       return 0;
}

//********************************Parte Servidor*********************************+
int Server(int puerto)
{
	int sockfd, newsockfd; 
	int Bytes_Recibidos; 
	char Datos_Recibidos[1024];
//	struct hostent *host; // Estructura hostent que contiene informacion 
	// socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int sin_size; 
	//int clilen; 
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR de apertura de socket");
		
	
		
	//bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(puerto);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero),8);
	
	/*Recibe el identificador de socket devuelto por la función socket(),*/
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	error("ERROR en la conexión");
	
	/* habilita al socket para poder recibir conexiones, el 5 indica el número máximo de conexiones en la cola de entrada de conexiones*/
	
	listen(sockfd,5); 
		
		printf("Esperando la conexión en el puerto  %i   \n ", puerto);
	
	while(1)
	{	
		//sin_size = sizeof(struct sockaddr_in);
		//clilen = sizeof(struct sockfddr_in);
	 sin_size = sizeof(struct sockaddr_in);
	 
	 /* devuelve un nuevo identificador de socket que será utilizado para la comunicación con el cliente que se ha conectado*/
	 /*recibe el identificador de socket habilitado para recibir conexiones, puntero a una referencia a la dirección 
	 * del cliente en el otro extremo de la conexión y el tamaño de esa estructura*/
		 
	 newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,&sin_size); //Acepta la cone 
     //printf("\nSe estableció la conexión");

			
			//printf("Introduzca el mensaje(yo):");
			 while (1)// Ciclo que permite la recrpción de datos 
            {  
				Bytes_Recibidos = recv(newsockfd,Datos_Recibidos,1024,0); // Recibe los bytes de red 
				Datos_Recibidos[Bytes_Recibidos] = '\0'; 

				if (strcmp(Datos_Recibidos, "chao") == 0 || strcmp(Datos_Recibidos , "Chao") == 0)  // Verifica que lo recibido es un chao
				{
					close(newsockfd);
					break;
				}

				printf("\n \e[1;35m Amigo= \e[1;35m %s \n " , Datos_Recibidos); //Escribe en color morado
				fflush(stdout); //Limpia el Buffer
          }
          break;
      }       
	  close(sockfd); // Cierra sockets
      return 0;
} 
		
//******************************************Funciones para realizar la funcionalidad de agregar amigos*******************************************

//Contiene los datos del contacto

struct NodosLista{
	
   char nombre[20];
   char ip[20];
   char puerto[20];
   struct NodosLista *siguiente; //puntero al siguiente elemento
   
};

struct NodosLista *PrimerNodo, *UltimoNodo; //Puntero al primer y último elemento de la lista

//Función que pone a NULL el primer y último puntero para que no apunten a ningún lugar
	
void inicializacion(){
	
	PrimerNodo=NULL; //a nulo el puntero inicial
	UltimoNodo=NULL; //a nulo el puntero al último elemento
	
	}

//Función que inserta un elemento al final de la lista
	
void insertar(char Nombre[20], char Ip[20], char Puerto[20]){
	
	struct NodosLista *nuevo; //se define el nuevo nodo para almacenar los datos que vayamos a insertar en la lista
	
	//reservamos memoria
	
	nuevo=(struct NodosLista*)malloc(sizeof(struct NodosLista));
	
	if (nuevo==NULL)
		printf("No hay memoria disponible\n");
		
	else{
		strcpy(nuevo->nombre,Nombre); //se añade el parámetro Nombre recibido en la función al campo nombre del puntero nuevo
		strcpy(nuevo->ip,Ip);
		strcpy(nuevo->puerto,Puerto);
		nuevo->siguiente=NULL; //el puntero nuevo debe apuntar a nulo
		
		}

	if (PrimerNodo==NULL){ //si está nulo es porque es el primer elemento que vamos a insertar en la lista
		
		PrimerNodo=nuevo;
		UltimoNodo=nuevo; //el nuevo nodo es al mismo tiempo el primero  el último de la lista
		}

	else{ //si no es el primero ya había al menos uno antes
		 
		UltimoNodo->siguiente=nuevo; //el puntero último ahora apunta al nuevo nodo
		UltimoNodo=nuevo; //nuevo ahora es el último de la lista
		}
		
	}
	
//Imprime los elementos de la lista (los structs)
	
void imprimir(){
	
	struct NodosLista *aux; //recorre cada elemento de la lista
	aux=PrimerNodo;
	
	while(aux!=NULL){
		
		printf("Nombre: %s\nIP: %s\nPuerto: %s\n",aux->nombre,aux->ip,aux->puerto);
		printf("***************************************************************\n");
		aux=aux->siguiente;
		
		}
	
	
	}
	
//función que busca un contacto en la lista e imprime sus respectivos datos
	
struct NodosLista *busqueda(char contacto[20]){
	
	struct NodosLista *aux; //recorrerá cada elemento de la lista
	aux=PrimerNodo; //se ubica en el primer nodo
	
	
	if(aux==NULL)
		printf("No hay elementos en la lista");
		
	else{
		
		while(aux!=NULL){
			
			int resultado=strcmp(aux->nombre,contacto); //compara el aux->nombre con el nombre que recibió como parámetro y almacena un entero como resultado
			
			if(resultado==0){ //si da cero quiere decir que el nombre ingresado si se encuentra en la lista de contactos

				//printf("El contacto si se encuentra\n");
				printf("Los datos del contacto son: ");
				printf("\n");
				printf("\n");
				printf("Nombre: %s\nIP: %s\nPuerto: %s\n",aux->nombre,aux->ip,aux->puerto);
				return aux;
			}
				
			aux=aux->siguiente;	//sigue con el siguiente elemento de la lista
			
		}
		aux=NULL; //ya aquí comparó todos los elementos de la lista con el nombre ingresado, lo cual indica que el contacto no se encontraba en esta
	}
	printf("Esta persona no se encuentra en su lista de contactos\n");
	return aux;
	}
	
//Función que obtiene la ip del contacto al que se quiere enviar un mensaje

char *busqueda_ip(char contacto[20]){
	
	struct NodosLista *aux; //recorrerá cada elemento de la lista
	aux=PrimerNodo; //se ubica en el primer nodo
	
	
	if(aux==NULL)
		printf("No hay elementos en la lista");
		
	else{
		
		while(aux!=NULL){
			
			int resultado=strcmp(aux->nombre,contacto); //compara el aux->nombre con el nombre que recibió como parámetro y almacena un entero como resultado
			
			if(resultado==0){ //si da cero quiere decir que el nombre ingresado si se encuentra en la lista de contactos

				//printf("El contacto si se encuentra\n");
				//printf("Los datos del contacto son: ");
				printf("\n");
				//printf("Nombre: %s\nIP: %s\nPuerto: %s\n",aux->nombre,aux->ip,aux->puerto);
				return aux->ip;
			}
				
			aux=aux->siguiente;	//sigue con el siguiente elemento de la lista
			
		}
		aux=NULL; //ya aquí comparó todos los elementos de la lista con el nombre ingresado, lo cual indica que el contacto no se encontraba en esta
	}
	printf("Esta persona no se encuentra en su lista de contactos\n");
	return NULL;
	}
	
//*********************************************************************************************************

//Función que devielve el puerto del cliente al que se quiere enviar un mensaje

char *busqueda_puerto(char contacto[20]){
	
	struct NodosLista *aux; //recorrerá cada elemento de la lista
	aux=PrimerNodo; //se ubica en el primer nodo
	
	
	if(aux==NULL)
		printf("No hay elementos en la lista");
		
	else{
		
		while(aux!=NULL){
			
			int resultado=strcmp(aux->nombre,contacto); //compara el aux->nombre con el nombre que recibió como parámetro y almacena un entero como resultado
			
			if(resultado==0){ //si da cero quiere decir que el nombre ingresado si se encuentra en la lista de contactos

				//printf("El contacto si se encuentra\n");
				//printf("Los datos del contacto son: ");
				printf("\n");
				//printf("Nombre: %s\nIP: %s\nPuerto: %s\n",aux->nombre,aux->ip,aux->puerto);
				return aux->puerto;
			}
				
			aux=aux->siguiente;	//sigue con el siguiente elemento de la lista
			
		}
		aux=NULL; //ya aquí comparó todos los elementos de la lista con el nombre ingresado, lo cual indica que el contacto no se encontraba en esta
	}
	printf("Esta persona no se encuentra en su lista de contactos\n");
	return NULL;
	}
	
	
//Al agregar un amigo se llama a esta función para escribir los datos en el

void escribir_archivo()
{

	FILE *Archivo,*archivo_puertos;
	char Nombre[256];
	char ip[256];
	char puerto[256];
	

	
	//Archivo que contendrá los contactos con su respectivo nombre, dirección ip y número de puerto

	Archivo= fopen("Amigos.txt","a");
	archivo_puertos= fopen ("Archivo_Configuracion.txt", "a");
	
	if (Archivo==NULL) {
		
		printf( "No se pudo abrir el archivo\n" );
		exit( 1 );
	}

		printf( "Por favor ingrese los siguientes datos del contacto: \n" );
		printf("\n");
		printf( "Nombre: \n" );
		scanf("%s", Nombre); //obtiene el nombre ingresado por el usuario mediante el stdin (teclado)
		fputs(Nombre, Archivo); //escribe el nombre en el archivo
		fputs(",", Archivo);
		printf("\n");
		printf( "Dirección IP: \n" );
		scanf("%s", ip);
		printf("\n");
		fputs(ip, Archivo);
		fputs(",", Archivo);	
		printf("Número de puerto: \n" );	
		scanf("%s", puerto);
		fputs(puerto, Archivo);
		fputs("\n", Archivo);
		fputs(puerto,archivo_puertos); //se coloca de una vez el puerto que irá en el archivo de configuración
		fputs("\n", archivo_puertos);
	

		if (fclose(Archivo)!=0)
			printf( "Problemas al cerrar el Archivo\n" );
	
}

//Función que lee los datos de los contactos y los almacena en una lista

struct NodosLista *leer_archivo(){
	
	struct NodosLista *datos;
	inicializacion();
	datos=(struct NodosLista*)malloc(sizeof(struct NodosLista));
	FILE* archivotxt;
	char cadenaTxt[100]; //linea del txt
	char* separadorLineas;
	archivotxt = fopen ("Amigos.txt", "r");
	
	
	
	while (fscanf (archivotxt, "%s", cadenaTxt) != EOF) {
		separadorLineas = strtok (cadenaTxt,","); //strtok separa linea de texto en partes, segun el separador definido, este es el primer token de cada línea
		strcpy (datos->nombre, separadorLineas);
		separadorLineas = strtok (NULL, ","); //siguiente token
		strcpy (datos->ip, separadorLineas);
		separadorLineas = strtok (NULL, ",");
		strcpy (datos->puerto, separadorLineas);
		insertar(datos->nombre,datos->ip,datos->puerto);
		
	}
	printf("La lista de contactos es:\n");
	printf("\n");
	imprimir();
	return datos;
	fclose (archivotxt); 

}

struct NodosLista *leer_archivo2(){
	
	struct NodosLista *datos;
	inicializacion();
	datos=(struct NodosLista*)malloc(sizeof(struct NodosLista));
	FILE* archivotxt;
	char cadenaTxt[100]; //linea del txt
	char* separadorLineas;
	archivotxt = fopen ("Amigos.txt", "r");
	
	
	
	while (fscanf (archivotxt, "%s", cadenaTxt) != EOF) {
		separadorLineas = strtok (cadenaTxt,","); //strtok separa linea de texto en partes, segun el separador definido, este es el primer token de cada línea
		strcpy (datos->nombre, separadorLineas);
		separadorLineas = strtok (NULL, ","); //siguiente token
		strcpy (datos->ip, separadorLineas);
		separadorLineas = strtok (NULL, ",");
		strcpy (datos->puerto, separadorLineas);
		insertar(datos->nombre,datos->ip,datos->puerto);
		
	}
	return datos;
	fclose (archivotxt); 

}

//*************************************Funciones para realizar el archivo de configuración****************************************************


struct NodoPuerto{
	
   char Puerto[20];
   struct NodoPuerto *siguiente; //puntero al siguiente elemento
   
};

struct NodoPuerto *PrimerNodo_puerto, *UltimoNodo_puerto; //Puntero al primer y último elemento de la lista

//Función que pone a NULL el primer y último puntero para que no apunten a ningún lugar
	
void inicializacion_puerto(){
	
	PrimerNodo_puerto=NULL; //a nulo el puntero inicial
	UltimoNodo_puerto=NULL; //a nulo el puntero al último elemento
	
	}

//Función que inserta un elemento al final de la lista, esta lista es la que almacenará los datos del archivo de configuración
	
void insertar_puerto(char Puerto[20]){
	
	struct NodoPuerto *nuevo; //se define el nuevo nodo para almacenar los datos que vayamos a insertar en la lista
	
	//reservamos memoria
	
	nuevo=(struct NodoPuerto*)malloc(sizeof(struct NodoPuerto));
	
	if (nuevo==NULL)
		printf("No hay memoria disponible\n");
		
	else{
		strcpy(nuevo->Puerto,Puerto);
		nuevo->siguiente=NULL; //el puntero nuevo debe apuntar a nulo
		
		}

	if (PrimerNodo_puerto==NULL){ //si está nulo es porque es el primer elemento que vamos a insertar en la lista
		
		PrimerNodo_puerto=nuevo;
		UltimoNodo_puerto=nuevo; //el nuevo nodo es al mismo tiempo el primero  el último de la lista
		}

	else{ //si no es el primero ya había al menos uno antes
		 
		UltimoNodo_puerto->siguiente=nuevo; //el puntero último ahora apunta al nuevo nodo
		UltimoNodo_puerto=nuevo; //nuevo ahora es el último de la lista
		}
		
	}
	
	
void imprimir_puerto(){
	
	struct NodoPuerto *aux; //recorre cada elemento de la lista
	aux=PrimerNodo_puerto;
	
	while(aux!=NULL){
		
		printf("Puerto: %s\n",aux->Puerto);
		printf("***************************************************************\n");
		aux=aux->siguiente;
		
		}
	
	}
	
//*********************************************************************************************************
	
	
//Función que lee el archivo de configuración, el puerto que recibe lo compara con el que está en el archivo y si coinciden entonces quiere decir
//que si est´a en el archivo de configuración por lo tanto se establecerá la conexión a ese puerto

int leer_archivo_puerto(int puerto){
	
	struct NodoPuerto *datos;
	inicializacion_puerto();
	datos=(struct NodoPuerto*)malloc(sizeof(struct NodoPuerto));
	int conversion;
	
	FILE* archivotxt;
    char linea[5];   
    archivotxt=fopen("Archivo_Configuracion.txt", "r"); 
      
    while (fgets(linea, 256, archivotxt) != NULL)
    {
		  	strcpy (datos->Puerto, linea);
		  	insertar_puerto(datos->Puerto);
		  	conversion=atoi(linea);
		  	
		  	if (conversion==puerto){
				return conversion;
			}
				
      }
   
	//printf("La lista de puertos es:\n");
	//printf("\n");
	//imprimir_puerto();
	return 0;
	fclose (archivotxt);   
  
}

//++++++++++++++++++++++++++++++++++++++++++ Main+++++++++++++++++++++++++++++++++++++++++++++++++++++




int main(){
	
	int opc; 
	int PuertoServer,PuertoCliente=0,opcion; // Variable para almacenar el pueto de cliente y el del servidor
    char *PuertoS;
    char *IpCliente;
    char contacto[20];
    
    printf("*****Bienvenido al sistema de Chat*****\n");
    printf("\n");
	printf("¿Qué desea realizar:\n");
	printf("\n");
	printf("1.Entrar\n");
	printf("\n");
	printf("2.Salir\n");
	printf("\n");
	scanf("%d", &opc);
	switch(opc){
		
		case 1:
		
	printf("¿Qué desea realizar:\n");
	printf("\n");
	printf("1.Agregar un amigo\n");
	printf("\n");
	printf("2.Ver la lista de contactos\n");
	printf("\n");
	printf("3.Buscar un contacto\n");
	printf("\n");
	printf("4.Enviar un mensaje\n");
	printf("\n");
	printf("Por favor digite el número de la opción que desea realizar\n");
	printf("\n");
	scanf("%d", &opcion);
    
    switch(opcion)
    {
		
			case 1:
			
				escribir_archivo();
				main(); 
				break;
				
			case 2:
				
				leer_archivo();
					main(); 
				break;
			
			case 3:
			
				printf("Por favor ingrese el contacto que desea buscar");
				printf("\n");
				printf("\n");
				scanf("%s", contacto);
				leer_archivo2();
				printf("\n");
				busqueda(contacto);
					main(); 
				break;
				
			case 4:
			
				printf("Digite el nombre del contacto al que desea enviar un mensaje:\n");
				printf("\n");
				scanf("%s", contacto);
				leer_archivo2();
				PuertoS=busqueda_puerto(contacto);
				PuertoServer=atoi(PuertoS);
				IpCliente=busqueda_ip(contacto);
				PuertoCliente=leer_archivo_puerto(PuertoServer); 
				
				if (PuertoCliente!=0){
					 
					int idProceso;
					idProceso=fork(); // Se lleva acabo la duplicación de procesos mediante el fock
				
					if(idProceso==0 )					 
						Cliente(PuertoCliente,IpCliente); // LLamada a la función cliente con los parametros de Puerto y IP
					 
					else 
						Server(PuertoServer); // Llamada a la función servidor con los parametros Puerto y Nombre
					
					
				}
				
				
				else{
					printf("El puerto no se encuentra en el archivo de configuración");
					}
					
				main(); 	
			default:
				printf("Ha ingresado un opción no valido\n");
				break;
}	
	
	case 2:
		printf("Ha salido del sistema de chat"); 
		exit(1); 
		break; 
		
		default:
			printf("Ha ingresado una opción no valido\n");
			break;				
   
           
    }
			
	 return 0; 	
}	
   
// FIn del menu



