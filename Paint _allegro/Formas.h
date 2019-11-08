#pragma once
# include <allegro5\allegro.h>
# include <allegro5\allegro_image.h>
# include <allegro5\allegro_native_dialog.h>
# include <allegro5\allegro_font.h>
# include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_opengl.h>
#include <math.h>
#include <stdio.h>

enum eforma
{
	NADA, LINEA_DDA, LINEA_B, RECTANGULO, CIRCULO, ELIPSE, TRIANGULO
};

#define BLANCO al_map_rgb(255, 255, 255)
#define NEGRO al_map_rgb(0, 0, 0)
#define V_DARK al_map_rgb(14, 14, 15)
#define DARK_BLUE al_map_rgb(0, 0, 139)
#define M_ORANGE al_map_rgb(179, 106, 48)
#define S_ORANGE al_map_rgb(200, 76, 8)
#define DARK_CYAN al_map_rgb(91, 162, 114)
#define GRAYI al_map_rgb(197, 200, 197)
#define DARK_BLUET al_map_rgb(124, 117, 163)

// Funcion para colocar un Pixel en la pantalla
// Se colorea automaticamente dependiendo del main
void Pixel(int x, int y, const ALLEGRO_COLOR col);

// Funcion para trazar una Linea dpor DDA 
void lineaDDA(int x0, int y0, int x1, int y1, const ALLEGRO_COLOR col);

// Funcion para trazar una linea por Bresenham
void linea(int x0, int y0, int x1, int y1, const ALLEGRO_COLOR col = BLANCO, uint32_t pattern = 0xFFFFFFFF);

// Funcion que crea un rectangulo, se necesita dos Puntos
void rectangulo(int x, int y, int w, int h, const ALLEGRO_COLOR col);

// Funcion que crea un circulo por Bresenham
// Nesecita un Radio
void circulo(int r, int xc, int yc, const ALLEGRO_COLOR col);

// Funcion que crea un elipse por Bresenham se nesecita el centro(xc,yc)
// Nesecita un Radio_x y un Radio_y
void elipse(int rx, int ry, int xc, int yc, const ALLEGRO_COLOR col);

// Funcion que crea un triangulo, se nesecita tres puntos 
// Se cierra automaticamente
void trgiso(int x0, int y0, int x1, int y1, const ALLEGRO_COLOR col);

// Funcion para colocar un Pixel en la pantalla
// Se colorea automaticamente dependiendo del main
void Pixel(int x, int y, const ALLEGRO_COLOR col) {
	al_put_pixel(x, y, col);
}

// Funcion para trazar una linea por Bresenham
void linea(int x0, int y0, int x1, int y1, const ALLEGRO_COLOR col, uint32_t pattern) {
	int x, y, dx, dy, p, sigX, sigY;

	auto rol = [&](void)
	{
		pattern = (pattern << 1) | (pattern >> 31);
		return pattern & 1;
	};

	dx = (x1 - x0);
	dy = (y1 - y0);
	/* determinar que punto usar para empezar, cual para terminar */
	if (dy < 0) {
		dy = -dy;
		sigY = -1;
	}
	else {
		sigY = 1;
	}


	if (dx < 0) {
		dx = -dx;
		sigX = -1;
	}
	else {
		sigX = 1;
	}


	x = x0;
	y = y0;

	/* se cicla hasta llegar al extremo de la linea */
	if (dx > dy) {
		p = 2 * dy - dx;;
		while (x != x1) {
			x = x + sigX;
			if (p < 0) {
				p = p + (2 * dy);
			}
			else {
				y = y + sigY;
				p = p + (2 * (dy - dx));
			}
			if (rol()) Pixel(x, y, col);

		}
	}
	else {
		p = 2 * dx - dy;
		while (y != y1) {
			y = y + sigY;
			if (p < 0) {
				p = p + 2 * dx;
			}
			else {
				x = x + sigX;
				p = p + 2 * (dx - dy);
			}
			if (rol()) Pixel(x, y, col);
		}
	}
}

// Funcion que crea un rectangulo, se necesita dos Puntos
void rectangulo(int x, int y, int w, int h, const ALLEGRO_COLOR col) {

	linea(x, y, x + w, y, col);
	linea(x + w, y, x + w, y + h, col);
	linea(x + w, y + h, x, y + h, col);
	linea(x, y + h, x, y, col);
}

// Funcion que crea un circulo por Bresenham
// Nesecita un Radio
void circulo(int r, int xc, int yc, const ALLEGRO_COLOR col) {

	int x, y, rx2, ry2, p1, p2;

	x = 0;
	y = r;
	rx2 = pow(r, 2);
	ry2 = pow(r, 2);

	p1 = ry2 - (rx2 * r) + (0.25 * rx2);

	while ((ry2 * x) < (rx2 * y))
	{
		if (p1 < 0)
		{
			x++;
			p1 = p1 + (2 * ry2 * x) + ry2;
		}
		else
		{
			x++; y--;
			p1 = p1 + (2 * ry2 * x) - (2 * rx2 * y) + ry2;
		}
		Pixel(xc + x, yc + y, col);
		Pixel(xc - x, yc + y, col);
		Pixel(xc + x, yc - y, col);
		Pixel(xc - x, yc - y, col);
	}

	p2 = (ry2)*pow(((double)x + 0.5), 2) + (rx2)*pow(((double)y - 1.0f), 2) - ((double)rx2 * (double)ry2);

	while (y > 0)
	{
		if (p2 > 0)
		{
			y--;
			p2 = p2 - (2 * rx2 * y) + rx2;
		}
		else
		{
			x++; y--;
			p2 = p2 + (2 * ry2 * x) - (2 * rx2 * y) + rx2;
		}
		Pixel(xc + x, yc + y, col);
		Pixel(xc - x, yc + y, col);
		Pixel(xc + x, yc - y, col);
		Pixel(xc - x, yc - y, col);
	}
}

// Funcion que crea un elipse por Bresenham se nesecita el centro(xc,yc)
// Nesecita un Radio_x y un Radio_y
void elipse(int rx, int ry, int xc, int yc, const ALLEGRO_COLOR col) {

	int x, y, rx2, ry2, p1, p2;
	//int ry;

	x = 0;
	//ry = rx / 2;
	y = ry;
	rx2 = pow(rx, 2);
	ry2 = pow(ry, 2);

	p1 = ry2 - (rx2 * ry) + (0.25 * rx2);

	while ((ry2 * x) < (rx2 * y))
	{
		if (p1 < 0)
		{
			x++;
			p1 = p1 + (2 * ry2 * x) + ry2;
		}
		else
		{
			x++; y--;
			p1 = p1 + (2 * ry2 * x) - (2 * rx2 * y) + ry2;
		}
		Pixel(xc + x, yc + y, col);
		Pixel(xc - x, yc + y, col);
		Pixel(xc + x, yc - y, col);
		Pixel(xc - x, yc - y, col);
	}

	p2 = (ry2)*pow(((double)x + 0.5f), 2) + (rx2)*pow(((double)y - 1.0f), 2) - ((double)rx2 * (double)ry2);

	while (y > 0)
	{
		if (p2 > 0)
		{
			y--;
			p2 = p2 - (2 * rx2 * y) + rx2;
		}
		else
		{
			x++; y--;
			p2 = p2 + (2 * ry2 * x) - (2 * rx2 * y) + rx2;
		}
		Pixel(xc + x, yc + y, col);
		Pixel(xc - x, yc + y, col);
		Pixel(xc + x, yc - y, col);
		Pixel(xc - x, yc - y, col);
	}
}

// Funcion que crea un triangulo, se nesecita tres puntos 
// Se cierra automaticamente
void trgiso(int x0, int y0, int x1, int y1, const ALLEGRO_COLOR col) {

	int r;
	r = sqrt((pow(((double)x1 - (double)x0), 2)) + (pow(((double)y1 - (double)y0), 2))); // CAMBIALO
	linea(x0, y0, x1, y1, col);
	linea(x1, y1, x0 + r, y0, col);
	linea(x0, y0, x0 + r, y0, col);

}

// Funcion para trazar una Linea dpor DDA 
void lineaDDA(int x0, int y0, int x1, int y1, const ALLEGRO_COLOR col) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (abs(dx) > abs(dy)) {          // pendiente < 1
		float m = (float)dy / (float)dx;
		float b = y0 - m * x0;
		if (dx < 0)
			dx = -1;
		else
			dx = 1;
		while (x0 != x1) {
			x0 += dx;
			y0 = round(m * (double)x0 + b); //calcula con la ecuacion de la recta
			Pixel(x0, y0, col);
		}
	}
	else
		if (dy != 0) {                              // pendiente >= 1
			float m = (float)dx / (float)dy;
			float b = x0 - m * y0;
			if (dy < 0)
				dy = -1;
			else
				dy = 1;
			while (y0 != y1) {
				y0 += dy;
				x0 = round(m * (double)y0 + b);
				Pixel(x0, y0, col);//calcula con la ecuacion de la recta
			}
		}
}

