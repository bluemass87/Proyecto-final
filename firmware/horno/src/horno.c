/*
===============================================================================
 Name        : horno.c
 Author      : Elián Hanisch, Gastón Riera y Rodrigo Oliver
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

/* includes del proyecto */
#include "init.h"
#include "motor.h"
#include "320240.h"
#include "grafico.h"
#include "delay.h"
#include "pwm.h"
#include "adc.h"


/* mensaje de inicio para mandar por el UART */
static char mensaje_inicio[] =
		"\r\n"
		"Proyecto Final Horno Dental\r\n"
		"===========================\r\n"
		"\r\n";
static char mensaje_menu[] = "Controles:\r\n"
							 " 'c' para iniciar/detener la captura continua.\r\n"
							 " 'm' para capturar N muestras.\r\n"
							 " 'i' para poner en marcha el motor.\r\n"
							 " 'p' para detener el motor.\r\n"
							 " '+' para aumentar la velocidad del motor.\r\n"
							 " '-' para disminuir la velocidad del motor.\r\n"
							 " 'l' para cambiar el sentido de giro.\r\n";


int main(void) {
	uint8_t charUART;
	int i;

#if defined (__USE_LPCOPEN)
#if !defined(NO_BOARD_LIB)
    // Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
#endif
#endif

    /* código del horno empieza aquí */

    Horno_Init();

    DEBUGOUT(mensaje_inicio);
   	DEBUGOUT(mensaje_menu);

   	Horno_Display_Test();
   	Horno_grafico_digito(160, 60, 0);

	while(1){
    	charUART = DEBUGIN();
    	if (charUART == 'm') {
    		adc_enabled = true;
    		adc_continue = false;
    		horno_adc.valor_n = 0;
    		Board_LED_Set(0, true);
    		while(adc_enabled) {}
    		for (i=0; i < NUM_MUESTRAS_CAPTURA; i++) {
    			DEBUGOUT("%10d, %4d\r\n", i, muestras[i]);
    		}
    		Board_LED_Set(0, false);
    	} else if (charUART == 'c') {
    		adc_enabled = !adc_enabled;
    		adc_continue = true;
    		if (!adc_enabled) {
    			DEBUGOUT("Conversión detenida.\r\n");
    		} else {
    			horno_adc.valor_n = 0;
    			horno_adc.th_suma = 0;
    			horno_adc.th_cantidad = 0;
    			horno_adc.lm_suma = 0;
    			horno_adc.lm_cantidad = 0;
    		}
    	} else if (charUART == 'i') {
    		Horno_motor_marcha(horno_motor.periodo);
    		DEBUGOUT("Motor encendido - periodo %dms\n", horno_motor.periodo);
    	} else if (charUART == 'p') {
    		Horno_motor_detener();
    		DEBUGOUT("Motor detenido\n");
    	} else if (charUART == '+') {
    		Horno_motor_marcha(horno_motor.periodo + 500);
    		DEBUGOUT("Motor periodo %dms\n", horno_motor.periodo);
    	} else if (charUART == '-') {
    		Horno_motor_marcha(horno_motor.periodo - 500);
    		DEBUGOUT("Motor periodo %dms\n", horno_motor.periodo);
    	} else if (charUART == 'l') {
    		Horno_motor_ascender(!horno_motor.ascender);
    		DEBUGOUT("Motor sentido ascender %d\n", horno_motor.ascender);
    	} else if (charUART == 'h') {
    		DEBUGOUT(mensaje_menu);
    	}
    }

    return 0;
}
