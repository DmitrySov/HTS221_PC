/*
 * usart0.c
 *
 * Created: 02.03.2020 16:53:12
 *  Author: ivandk
 */ 


/*_____ I N C L U D E S ____________________________________________________*/


#include <stdint.h>
#include <stdbool.h>

#include "usart0.h"
#include "modbus.h"
//#include "conf_uart_serial.h"


/*_____ M A C R O S ________________________________________________________*/

static UART_HandleTypeDef huart4;

/*_____ D E F I N I T I O N ________________________________________________*/

#define USART0_BUF_SIZE		74


/*_____ D E C L A R A T I O N ______________________________________________*/

uint8_t usart0_buffer[64];									//	буфер для приемо-передатчика USART0;
uint8_t *usart0_buffer_tx;									//	указатель на буфер приемо-передатчика;

uint8_t usart0_tx_n;										//	число отправленных байт;
uint8_t usart0_rx_n;										//	число принятых байт;

volatile bool usart0_modbus_timeout = false;				//	статус завершения приема данных;

/***********************************************************************************
 * Function:		usart0_init
 *
 *----------------------------------------------------------------------------------
 *
 * description:		Настраивает приемо-передатчик USART в соотвествии с параметрами,
 *					записанными в файле "config / conf_uart_serial.h", а также выводы
 *					микроконтроллера, определенные в файле "main.c". 
 *
 * parameters:		-
 * 
 * on return:		-
 *
 *************************************************************************************/

void usart0_init(void)	
{
	usart0_rx_n = 0;
	usart0_tx_n = 0;
	usart0_buffer_tx = usart0_buffer;
	
	const sam_usart_opt_t usart_console_settings = {
		USART_SERIAL_BAUDRATE,
		USART_SERIAL_CHAR_LENGTH,
		USART_SERIAL_PARITY,
		USART_SERIAL_STOP_BIT,
		US_MR_CHMODE_NORMAL
	};
		
	/*ioport_set_pin_dir(RS485_RTS, IOPORT_DIR_OUTPUT);										//	установка микросхемы RS-485 на прием;
	ioport_set_pin_level(RS485_RTS, 0);

	ioport_set_pin_dir(RS485_RX, IOPORT_DIR_INPUT);											//	настройка выводов USART0;
	ioport_set_pin_mode(RS485_RX, IOPORT_MODE_MUX_A);
	ioport_disable_pin(RS485_RX);

	ioport_set_pin_dir(RS485_TX, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(RS485_TX, IOPORT_MODE_MUX_A);
	ioport_disable_pin(RS485_TX);
	
	sysclk_enable_peripheral_clock(USART_SERIAL_ID);										//	включение тактирования USART0;
	usart_init_rs232(USART_SERIAL, &usart_console_settings, sysclk_get_main_hz());			//	настройка USART в режиме RS-232;
	usart_enable_tx(USART_SERIAL);															//	включение приемника и передатчика;
	usart_enable_rx(USART_SERIAL);
	
	usart_set_rx_timeout(USART_SERIAL, 20);													//	настройка и включение таймаута приемника (проверить!);
	usart_start_rx_timeout(USART_SERIAL);

	usart_enable_interrupt(USART_SERIAL, US_IER_RXRDY | US_IER_TIMEOUT);					//	включение прерываний от приемника и по таймауту;
	NVIC_EnableIRQ(USART0_IRQn);*/
}

/***********************************************************************************
 * Function:		usart0_modbus_timeout_get
 *
 *----------------------------------------------------------------------------------
 *
 * description:		
 *
 * parameters:		-
 * 
 * on return:		-
 *
 *************************************************************************************/

volatile bool usart0_modbus_timeout_get(void)
{
	volatile bool b;
	
	NVIC_DisableIRQ(USART0_IRQn);								//	атомарный доступ;
	b = usart0_modbus_timeout;
	usart0_modbus_timeout = false;
	NVIC_EnableIRQ(USART0_IRQn);								
	
	return b;
}

/***********************************************************************************
 * Function:		usart0_modbus_frame_get
 *
 *----------------------------------------------------------------------------------
 *
 * description:		
 *
 * parameters:		-
 * 
 * on return:		-
 *
 *************************************************************************************/

modbus_frame_t usart0_modbus_frame_get(void)
{
	modbus_frame_t frame;

	frame.buffer = usart0_buffer;						//	сохраняем указатель на принятые данные;
	frame.n = usart0_rx_n;								//	сохраняем число байт;
	usart0_rx_n = 0;

	return frame;
}

/***********************************************************************************
 * Function:		USART0_Handler 
 *
 *----------------------------------------------------------------------------------
 *
 * description:		Обработчик прерывания USART0. Осуществляет прием и передачу данных
 *					по RS-485.
 *
 * parameters:		-
 * 
 * on return:		-
 *
 *************************************************************************************/

void USART0_Handler(void)
{
	uint32_t dw_status;																//	статус приемо-передатчика;
	uint32_t us_irq_mask;															//	статус прерываний;
	uint32_t received_byte;															//	переменная для полученных данных;
//	static uint8_t i=0;
	
	dw_status = usart_get_status(USART_SERIAL);										//	сохраняем статусный регистр UART_CSR;
	us_irq_mask = usart_get_interrupt_mask(USART_SERIAL);							//	сохраняем регистр прерываний UART_IMR;
	
	if ((dw_status & US_CSR_RXRDY) && (us_irq_mask & US_IMR_RXRDY))					//	"если прерывание вызвано приемом символа...";
	{
		usart_read(USART_SERIAL, &received_byte);										//	чтение символа;
		if (usart0_rx_n < USART0_BUF_SIZE)
		{
			usart0_buffer[usart0_rx_n++] = received_byte;									//	сохраняем принятый байт;
		}
	}
	if ((dw_status & US_CSR_TIMEOUT) && (us_irq_mask & US_IMR_TIMEOUT))				//	"если прерывание вызвано таймаутом, ...";
	{
		usart0_modbus_timeout = true;													//	выставляем флаг об окончании приема данных;
		usart_start_rx_timeout(USART_SERIAL);											//	сбрасываем аппаратный флаг таймаута;
	}
	if ((dw_status & US_CSR_TXRDY) && (us_irq_mask & US_IMR_TXRDY))					//	"если прерывние вызвано окончанием передачи символа, ...";
	{
		if (usart0_tx_n == 0)															//	"если все байты переданы, ...";
		{
			usart_disable_interrupt(USART_SERIAL, US_CSR_TXRDY);							//	отключение прерываний по окончании передачи символа;
			usart_enable_interrupt(USART_SERIAL, US_CSR_TXEMPTY);							//	включение прерываний по заверешении всей передачи;
		}
		else
		{
			USART_SERIAL -> US_THR = US_THR_TXCHR(*(usart0_buffer_tx++));					//	передаем следующий байт;
			usart0_tx_n--;
		}
	}
	if ((dw_status & US_CSR_TXEMPTY) && (us_irq_mask & US_IMR_TXEMPTY))				//	"если прерывание вызвано окончанием передачи, ...";
	{
		usart0_rx_n = 0;																//	обнуление счетчиков;
		usart0_tx_n = 0;
		usart0_buffer_tx = usart0_buffer;
		ioport_set_pin_level(RS485_RTS, 0);												//	установка RS-485 на прием;
		usart_disable_interrupt(USART_SERIAL, US_CSR_TXEMPTY);							//  отключение прерываний по заверешении передачи;
		usart_enable_interrupt(USART_SERIAL, US_IER_RXRDY | US_IER_TIMEOUT);			//	включение прерываний по окончанию приема и таймауту;
	}
}

/***********************************************************************************
 * Function:		usart0_transmit
 *
 *----------------------------------------------------------------------------------
 *
 * description:		
 *
 * parameters:		*p - указатель на  начало буфера передачи;
 *					n - количество байт для передачи;
 * 
 * on return:		-
 *
 *************************************************************************************/

void usart0_transmit(uint8_t *buffer, uint8_t n)
{
	usart0_tx_n = n;
	usart0_buffer_tx = buffer;
	
	HAL_UART_Transmit(&huart4, usart0_buffer_tx, usart0_tx_n, 1000);

	/*ioport_set_pin_level(RS485_RTS, 1);												//  настройка RS-485 на передачу;
	usart_disable_interrupt(USART_SERIAL, US_IER_RXRDY | US_IER_TIMEOUT);			//  отключение прерываний от приемника и по превышения таймаута;
	usart_enable_interrupt(USART_SERIAL, US_CSR_TXRDY);								//  включение прерываний от передатчика;*/
}

