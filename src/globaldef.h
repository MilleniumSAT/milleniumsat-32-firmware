#ifndef _GLOBALDEF_H
#define _GLOBALDEF_H

#include "globalvar.h"

/*Definição*/
#define RXD2 16
#define TXD2 17
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 60
#define V_FIRMWARE 9
#define EQUIPE 01
#define ON_IDLE 1
#define WORKING 2
#define WIFI 3
#define LOOKING_FOR_WIFI 4
#define ledPino 15
#define ledPino2 2
#define NUMERO_MAXIMO_PACOTES 200


/*STATEMACHINE*/
#define STATE_0  0
#define STATE_1  1
#define STATE_2  2
#define STATE_3  3
#define STATE_4  4

/*STATUS DE ERRO*/
#define SUCESSO  1
#define AGUARDA  3
#define ERRO  0
#define FLAG_ATIVADA 1
#define FLAG_DESATIVADA 0

/*TIPOS*/
#define TIPO_STRING "STRING"

/*DEBUG*/
#define SERIAL_DEBUG 1
#define MQTT_DEBUG 2
#define SOCKET_DEBUG 3
#define POST_DEBUG 4
#define SEM_TOPICO "SEM_TOPICO"
#define SERVIDOR "https://pkg-receiver-millenium.vercel.app/api/newData?pacote="
#define SERVIDOR_HOMOLOGACAO "http://ptsv2.com/t/ijfka-1650200607/post"
#define PORTA_SOCKET 1883
#define SERVIDOR_SOCKET ".."
#define TEMPO_DELAY_ENTRE_PACOTES 180
#define TEMPO_DELAY_WDT 8

/*LED_STATE*/
#define OBTENDO_SENSORES 1
#define ENVIANDO 10
#define HIBERNANDO 50

/*SERIAL*/
#define VEL_SERIAL_DEBUG 115200
#define VEL_SERIAL_2 9600

/*SENSORES*/


#endif