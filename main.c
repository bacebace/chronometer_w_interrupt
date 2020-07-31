//roteiro 8
//exception number: 15 (SysTick)	- pagina 220 do manual
//SysTick: 0xE000E010-0xE000E0FF
//0xE000ED04 ICSR		Interrupt Control State Register
//0xE000E010 SYST_CSR	SysTick Control and Status Register: Controls the system timer and provides status data
//0xE000E014 SYST_RVR	SysTick Reload Value Register: Sets or reads the reload value of the SYST_CVR register
//0xE000E018 SYST_CVR	SysTick Current Value Register: Reads or clears the current counter value
//0xE000E01C SYST_CALIB	SysTick Current Value Register: Reads the calibration value and parameters for SysTick

#include "derivative.h" /* include peripheral declarations */
#include "delay.h"
#include "lcd.h"
#include "uart.h"
#define SET_BIT(reg, idx)	reg = (reg | (1 << idx)) //seta o bit idx do registrador reg


// SYST_CSR		bit16: se eh 0, o timer nao acabou de contar; se eh 1, acabou
//				bit2: se eh 0, usa outro clk; se eh 1, usa clk do processador
//				bit1: se eh 0, nao afeta o status de excecao; se eh 1, deixa o status de excecao pendente
//				bit0: se eh 0, contador esta desligado; se eh 1, esta ligado

// SYST_RVR		bits 23..0: valor que sera colocado quando o contador atingir 0

// SYST_CVR		bits 23..0: contem o valor atual do contador

// SYST_CALIB	bit31: se eh 0, clk de referencia esta implementado; se eh 1, nao esta
//				bit30: se eh 0, 10ms de calibracao eh exato; se eh 1, eh inexato (por causa da freq do clk)
//				bits 23..0:  Optionally, holds a reload value to be used for 10ms (100Hz) timing, subject to system clock
//							skew errors. If this field is zero, the calibration value is not known.

int cc = 0, ss=0, mm=0, hh=0;
char v[15] = "00:00:00:00"; //hh:mm:ss:cc		[v0][v1]:[v3][v4]:[v6][v7]:[v9][v10]

void SysTick_Handler() {
	cc++;
}

void hora(){
	if(hh<23){
		hh++;
	}
	else{
		hh = 0;
	}
	v[1] = 48 + hh%10;	//capta a unidade
	v[0] = 48 + hh/10;	//capta a dezena
}

void minuto(){
	if(mm<59){
		mm++;
	}
	else{
		mm = 0;
		hora();
	}
	v[4] = 48 + mm%10;	//capta a unidade
	v[3] = 48 + mm/10;	//capta a dezena
}

void segundo(){
	if(ss<59){
		ss++;
	}
	else{
		ss = 0;
		minuto();
	}
	v[7] = 48 + ss%10;	//capta a unidade
	v[6] = 48 + ss/10;	//capta a dezena
}

void centesimo(){
	delay(3900);
	if(cc<99){
		cc++;
	}
	else{
		cc = 0;
		segundo();
	}
	SET_BIT(GPIOE_PTOR,23);	//para ver no osciloscopio
	v[10] = 48 + cc%10;	//capta a unidade
	v[9] = 48 + cc/10;	//capta a dezena
}

void f_reset(int flagR) {	//[v0][v1]:[v3][v4]:[v6][v7]:[v9][v10]
	if (flagR != 0) {	//reseta
		v[0]='0';
		v[1]='0';
		v[3]='0';
		v[4]='0';
		v[6]='0';
		v[7]='0';
		v[9]='0';
		v[10]='0';
		cc = 0;
		ss = 0;
		mm = 0;
		hh = 0;
	}
}

void f_stop(int flagS, int flagR) {
	f_reset(flagR);
	int n = 0;
	if (flagS == 0)	//cronometro esta contando
		centesimo();

	if (flagS != 0)	//cronometro esta parado
		for (n=0;v[n]!='\0';n++){
			v[n] = v[n];
		}
}

void f_list(int flagL, int flagS, int flagR) {
	if (flagL == 0) { //nao lista
		//limpa a tela e joga o cursor para esquerda
		putchar_UART0(0x1B);
		puts_UART0("[2J");
		putchar_UART0(0x1B);
		puts_UART0("[H");
		
		f_stop(flagS, flagR);
		puts_UART0(v); //escreve no terminal
		puts_LCD(v); //escreve no LCD
	}

	if (flagL != 0) { //lista	
		puts_UART0("\n\r");	//pula uma linha e poe o cursor no canto esquerdo
		f_stop(flagS, flagR);
		delay(1250);
		puts_UART0(v); //escreve no terminal
		puts_LCD(v); //escreve no LCD
	}
}

int f_display (int flagD, int flagL, int flagS, int flagR) {
	if (flagD == 0) {	//nao mostra no terminal
		//limpa terminal
		putchar_UART0(0x1B);
		puts_UART0("[2J");
		putchar_UART0(0x1B);
		puts_UART0("[H");
		
		f_stop(flagS, flagR);
		delay(2370);
		puts_LCD(v); //escreve no LCD
	}
	
	if (flagD != 0)	//mostra no terminal
		f_list(flagL, flagS, flagR);
	
	flagR=0;
	return(flagR);
	
}

int main(){
	//inicializacoes de GPIOs, LCD e UART0
	init_GPIO();
	init_GPIO_LCD();
	init_LCD();
	clear_LCD();
	init_UART0();
	
	//ajusta a posicao do LCD
	setpos_LCD(1, 0);
	
	//limpa a tela e joga para o canto superior esquerdo
	putchar_UART0(0x1B);
	puts_UART0("[2J");
	putchar_UART0(0x1B);
	puts_UART0("[H");

	int flagS, flagD, flagL, flagR;
	flagS = 1;		//quando eh 0, conta;					quando eh 1, nao conta
	flagD = 1;		//quando eh 0, nao mostra no terminal;	quando eh 1, mostra no terminal
	flagL = 0;		//quando eh 0, nao lista;				quando eh 1, lista
	flagR = 0;		//quando eh 0, nao reseta;				quando eh 1, reseta
	char cmm;

	puts_UART0(v);
	
	//tabela de comandos				dec							hex
	//s ou S: stop						115(s),83(S)				0x73,0x53
	//d ou D: display (terminal)		100(d),68					0x64,0x44
	//l uo L: list						108(l),76(L)				0x6C,0x4C
	//r ou R: reset						114(r),82(R)				0x72,0x52
	//ESC: sai do programa				27							0x1B
	
	while(1) {
		cmm = getchar_UART0();
		
		//abaixo verifico se algum comando foi feito e acendo a respectiva flag
		if (cmm == 0x72 || cmm == 0x52) {	//se comando for r ou R
			if (flagR==0)
				flagR = 1;
			else
				flagR = 0;
		}
		
		if (cmm == 0x73 || cmm == 0x53) {	//se comando for s ou S
			if (flagS==0)
				flagS = 1;
			else 
			flagS = 0;
		}
		if (cmm == 0x64 || cmm == 0x44) {	//se comando for d ou D
			if (flagD==0)
				flagD = 1;
			else 
				flagD = 0;
		}
		
		if (cmm == 0x6C || cmm == 0x4C) {	//se comando for l ou L
			if (flagL==0)
				flagL = 1;
			else
				flagL = 0;
		}
		
		if (cmm == 0x1B) {	//se comando for ESC
			putchar_UART0(0x1B);
			puts_UART0("[2J");
			putchar_UART0(0x1B);
			puts_UART0("[H");
			clear_LCD();
			break;
		}
		
		//abaixo verifico flags e tomo as acoes
		flagR = f_display (flagD, flagL, flagS, flagR);
				
	} //fim do laco while

	return 0;
}
