/* Nokia 5110 LCD AVR Library example
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 * Custom char code by Marcelo Cohen - 2021
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "nokia5110.h"

#define MAX_X 83 // Largura máxima do jogo
#define MAX_Y 47 // Altura máxima do jogo

uint8_t xb1 = 93; // cima
uint8_t xb2 = 83;

uint8_t xb3 = 73;
uint8_t xb4 = 63;

uint8_t xb5 = 53; // cima
uint8_t xb6 = 43;

uint8_t xb7 = 33;
uint8_t xb8 = 23;

uint8_t up = 0;
uint8_t y = 20;
uint8_t x = 20;
uint8_t clock = 0;
uint16_t timer = 0;
uint8_t score = 0;
uint8_t game = 0;
char msg[30];
int on = 1;

// uint8_t glyph[] =

void timer1_init()
{
    // Configura o Timer1 para operar em modo CTC
    TCCR1B |= (1 << WGM12);

    // Configura a fonte de clock do Timer1 com o prescaler de 256
    TCCR1B |= (1 << CS12);
    OCR1A = 6250; // 0.06 s

    TIMSK1 |= (1 << OCIE1A);

    // Habilita as interrupções globais
    sei();
}

int main(void)
{
    DDRB &= ~(1 << PB0); // start
    DDRB &= ~(1 << PD7); // up

    cli();
    timer1_init();
    nokia_lcd_init();

    nokia_lcd_clear();

    while (1)
    {
        if (PINB & (1 << PB0))
        {
            while (PINB & (1 << PB0))
            {
            }
            game = 1;
            clock = 0;
            timer = 0;
            y = 25;

            while (game)
            {
                if (PINB & (1 << PB0))
                {
                    while (PINB & (1 << PB0))
                    {
                    }
                    game = 0;
                    while (1)
                    {
                        if (PINB & (1 << PB0))
                        {
                            while (PINB & (1 << PB0))
                            {
                            }
                            game = 1;
                            break;
                        }
                    }
                }

                if (PIND & (1 << PD7))
                {
                    up = 1;
                    while (PIND & (1 << PD7))
                    {
                    }
                }
            }
        }
        nokia_lcd_clear();
        nokia_lcd_drawline(0, 0, 83, 0);
        nokia_lcd_drawline(0, 0, 0, 47);
        nokia_lcd_drawline(0, 47, 83, 47);
        nokia_lcd_drawline(83, 0, 83, 47);
        nokia_lcd_set_cursor(10, 15);
        if (on)
        {
            sprintf(msg, "PRESS START");
            nokia_lcd_write_string(msg, 1);
        }

        if (score != 0)
        {
            nokia_lcd_set_cursor(10, 24);
            sprintf(msg, "BEST %d", score);
            nokia_lcd_write_string(msg, 1);
        }

        nokia_lcd_render();
    }
}

void endgame()
{
    if (timer > score)
        score = timer;

    xb1 = 93;
    xb2 = 83;

    xb3 = 73;
    xb4 = 63;

    xb5 = 53;
    xb6 = 43;

    xb7 = 33;
    xb8 = 23;

    on = 1;
    clock = 0;
}
void checkup(uint8_t *x1, uint8_t *x2)
{
    if (*x1 < 1)
    {
        *x2 = 83;
        *x1 = 93;
    }

    if (*x1 > 83)
    {
        nokia_lcd_drawrect(*x2, 10, 83, 20);
    }
    else if (*x2 < 1)
    {
        *x2 = 1;
        nokia_lcd_drawrect(*x2, 10, *x1, 20);
    }
    else
    {
        nokia_lcd_drawrect(*x2, 10, *x1, 20);
    }
}

void checkdown(uint8_t *x1, uint8_t *x2)
{
    if (*x1 < 1)
    {
        *x2 = 83;
        *x1 = 93;
    }

    if (*x1 > 83)
    {
        nokia_lcd_drawrect(*x2, 47, 83, 37);
    }
    else if (*x2 < 1)
    {
        *x2 = 1;
        nokia_lcd_drawrect(*x2, 47, *x1, 37);
    }
    else
    {
        nokia_lcd_drawrect(*x2, 47, *x1, 37);
    }
}

void checkbirdUp(uint8_t *x1, uint8_t *x2)
{
    if ((x <= *x1 && x >= *x2) || ((x + 5) <= *x1 && (x + 5) >= *x2) || ((x - 5) <= *x1 && (x - 5) >= *x2))
    {
        if ((y - 4) < 20)
        {
            game = 0;
            endgame();
        }
    }
}

void checkbirdDown(uint8_t *x1, uint8_t *x2)
{
    if ((x <= *x1 && x >= *x2) || ((x + 5) <= *x1 && (x + 5) >= *x2) || ((x - 5) <= *x1 && (x - 5) >= *x2))
    {
        if ((y + 4) > 37)
        {
            game = 0;
            endgame();
        }
    }
}

ISR(TIMER1_COMPA_vect)
{
    TCNT1 = 0;

    if (game)
    {
        clock++;
        if (clock >= 10)
        {
            clock = 0;
            timer++;
        }
        if (up)
        {
            y -= 3;
            if ((y - 4) < 12)
            {
                game = 0;
                endgame();
                up = 0;
            }
            else
            {

                up = 0;
            }
        }
        else
        {
            y += 1;
            if ((y + 4) > 46)
            {
                game = 0;
                endgame();
                up = 0;
            }
        }

        xb1 -= 1;
        xb2 -= 1;

        xb3 -= 1;
        xb4 -= 1;

        xb5 -= 1;
        xb6 -= 1;

        xb7 -= 1;
        xb8 -= 1;

        nokia_lcd_clear();
        nokia_lcd_drawline(0, 0, 83, 0);
        nokia_lcd_drawline(0, 0, 0, 47);
        nokia_lcd_drawline(0, 47, 83, 47);
        nokia_lcd_drawline(83, 0, 83, 47);
        nokia_lcd_set_cursor(65, 2);

        sprintf(msg, "%0d", timer);
        nokia_lcd_write_string(msg, 1);
        nokia_lcd_drawline(0, 10, 83, 10);

        checkup(&xb1, &xb2);
        checkdown(&xb3, &xb4);
        checkup(&xb5, &xb6);
        checkdown(&xb7, &xb8);

        checkbirdUp(&xb1, &xb2);
        checkbirdDown(&xb3, &xb4);
        checkbirdUp(&xb5, &xb6);
        checkbirdDown(&xb7, &xb8);
        if (game)
        {
            nokia_lcd_drawcircle(x, y, 5);
        }

        nokia_lcd_render();
    }
    else
    {
        clock++;
        if (clock >=8)
        {
            if (on)
            {
                on = 0;
            }
            else
            {
                on = 1;
            }
            clock = 0;
        }
    }
}
