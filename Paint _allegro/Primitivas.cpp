#include "Formas.h"
#include <ctype.h>
#include <vector>
#include <list>
#define ALLEGRO_UNSTABLE
#define RADIUS       5

struct Example {
	ALLEGRO_DISPLAY* sVentana;
	ALLEGRO_FONT* fuente;
	ALLEGRO_EVENT_QUEUE* colaevento;
	ALLEGRO_BITMAP* icono1;
	ALLEGRO_BITMAP* icono2;
	ALLEGRO_BITMAP* icono3;
	ALLEGRO_BITMAP* icono4;
	ALLEGRO_BITMAP* icono5;
	ALLEGRO_BITMAP* icono6;
};

static struct Example ini;

// Se define una estructura para un punto
struct vf2d
{
	float x;
	float y;
};

vf2d vOffset = { 0.0f, 0.0f };
vf2d vIniPan = { 0.0f, 0.0f };
float fEscala = 5.0f;

// Convierte coordenadas de World Space --> Screen Space
void WorldToScreen(const float v_x, const float v_y, int& nScreenX, int& nScreenY)
{
	nScreenX = (int)((v_x - vOffset.x) * fEscala);
	nScreenY = (int)((v_y - vOffset.y) * fEscala);
}

// Convierte coordenadas de Screen Space --> World Space
void ScreenToWorld(int nScreenX, int nScreenY, vf2d& v)
{
	v.x = (float)(nScreenX) / fEscala + vOffset.x;
	v.y = (float)(nScreenY) / fEscala + vOffset.y;
}

void destroy() {
	if (ini.sVentana)
		al_destroy_display(ini.sVentana);
	if (ini.fuente)
		al_destroy_font(ini.fuente);
	if (ini.colaevento)
		al_destroy_event_queue(ini.colaevento);

}

int ini_allegro(int nAnc, int nAlt, const char* tTitulo) {
	if (!al_init()) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al inicializar allegro", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return 0;
	}


	ini.sVentana = al_create_display(nAnc, nAlt);
	if (!ini.sVentana) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al crear Ventana", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}
	al_set_window_title(ini.sVentana, tTitulo);

	al_init_primitives_addon();

	if (!al_init_image_addon()) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al inicializar el complemento de imagen", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}

	ini.icono1 = al_load_bitmap("img/barra1.png");
	ini.icono2 = al_load_bitmap("img/linea.png");
	ini.icono3 = al_load_bitmap("img/linea.png");
	ini.icono4 = al_load_bitmap("img/linea.png");
	ini.icono5 = al_load_bitmap("img/linea.png");
	ini.icono6 = al_load_bitmap("img/linea.png");
	if (!ini.icono1 || !ini.icono2 || !ini.icono3 || !ini.icono4 || !ini.icono5 || !ini.icono6) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"La imagen no se pudo cargar", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}

	al_init_font_addon();
	if (!al_init_ttf_addon()) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al inicializar el complemento allegro_ttf", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}


	ini.fuente = al_load_ttf_font("arial.ttf", 18, 0);
	if (!ini.fuente) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al cargar la vecPuntos", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}

	if (!al_install_keyboard()) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al inicializar el teclado", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}

	if (!al_install_mouse()) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al inicializar el mouse", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}

	ini.colaevento = al_create_event_queue();
	if (!ini.colaevento) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al crear la cola de eventose", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}

	al_register_event_source(ini.colaevento, al_get_display_event_source(ini.sVentana));
	al_register_event_source(ini.colaevento, al_get_mouse_event_source());
	al_register_event_source(ini.colaevento, al_get_keyboard_event_source());

	return 1;
}

struct sFormas;

// Define un punto
struct sPuntos
{
	sFormas* tipo_forma;
	vf2d pos;
};

// Clase BASE define la interfaz para todas las formas.
struct sFormas
{
	//  Se declaran los puntos para las formas
	std::vector<sPuntos> vecPuntos;
	int nMaxPunto = 0;

	// Color de la forma
	ALLEGRO_COLOR col = DARK_CYAN;
	// Escala de las formas
	static float fWorldEscala;
	// compensacion
	static vf2d vWorldOffset;

	// Convertir coordenadas de World Space --> Screen Space
	void MundoAScreen(const vf2d& v, int& nScreenX, int& nScreenY)
	{
		nScreenX = (int)((v.x - vWorldOffset.x) * fWorldEscala);
		nScreenY = (int)((v.y - vWorldOffset.y) * fWorldEscala);
	}

	// Funcion que permite llamar al las de las 
	// sub-clases que heredan.
	virtual void DibujarForma() = 0;

	sPuntos* OptenerPunto(const vf2d& p)
	{
		if (vecPuntos.size() == nMaxPunto)
			return nullptr; // Cuando la forma tiene todo los puntos 

		// si no se agrega mas puntos
		sPuntos n;
		n.tipo_forma = this;
		n.pos = p;
		vecPuntos.push_back(n);

		return &vecPuntos[vecPuntos.size() - 1];
	}

	// Si existe un punto en la coordenada
	// retorna la direccion del punto
	sPuntos* HitPunto(vf2d& p)
	{
		for (auto& n : vecPuntos)
		{
			float dx = n.pos.x - p.x;
			float dy = n.pos.y - p.y;
			float dd = dx * dx + dy * dy;
			if (dd < 0.01f)
				return &n;
		}

		return nullptr;
	}

	// funcion para Dibujar todos los puntos
	void DibujarPuntos()
	{
		int i = 0;
		for (auto& n : vecPuntos)
		{
			i++;
			int sx, sy;
			MundoAScreen(n.pos, sx, sy);

			al_draw_filled_circle(sx, sy, RADIUS / 1.5, S_ORANGE);// Se utilisa las de la libreria para el rellono
			//circulo(RADIUS, sx, sy, MODER_ORANGE);
		}
	}
};

// inicializacion de la Escala y la Compensacion
float sFormas::fWorldEscala = 1.0f;
vf2d sFormas::vWorldOffset = { 0,0 };

// LINEA sub clase, hereda desde sFormas
struct sLinea : public sFormas
{
	sLinea()
	{
		nMaxPunto = 2;
		vecPuntos.reserve(nMaxPunto);

	}

	void DibujarForma()
	{
		int sx, sy, ex, ey;
		MundoAScreen(vecPuntos[0].pos, sx, sy);
		MundoAScreen(vecPuntos[1].pos, ex, ey);
		linea(sx, sy, ex, ey, col);
	}
};

// RECTANGULO
struct sRec : public sFormas
{
	sRec()
	{
		nMaxPunto = 2;
		vecPuntos.reserve(nMaxPunto);
	}

	void DibujarForma()
	{
		int sx, sy, ex, ey;
		MundoAScreen(vecPuntos[0].pos, sx, sy);
		MundoAScreen(vecPuntos[1].pos, ex, ey);
		rectangulo(sx, sy, ex - sx, ey - sy, col);
	}
};

// CIRCULOS
struct sCir : public sFormas
{
	sCir()
	{
		nMaxPunto = 2;
		vecPuntos.reserve(nMaxPunto);
	}

	void DibujarForma()
	{

		float dx = abs(vecPuntos[0].pos.x - vecPuntos[1].pos.x);
		float dy = abs(vecPuntos[0].pos.y - vecPuntos[1].pos.y);
		float fRadius = sqrt(pow(dx, 2) + pow(dy, 2));
		int sx, sy, ex, ey;
		MundoAScreen(vecPuntos[0].pos, sx, sy);
		MundoAScreen(vecPuntos[1].pos, ex, ey);
		linea(sx, sy, ex, ey, col, 0xFF00FF00);

		// El radio se escala
		circulo(fRadius * fWorldEscala, sx, sy, col);
	}
};

// TRIANGULO
struct sTriangulo : public sFormas
{
	sTriangulo()
	{
		nMaxPunto = 3;
		vecPuntos.reserve(nMaxPunto);
	}

	void DibujarForma()
	{
		int sx, sy, ex, ey;

		if (vecPuntos.size() < 3)
		{
			// dibuja línea de primera punto a segunda punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[1].pos, ex, ey);
			linea(sx, sy, ex, ey, col, 0xFF00FF00);
		}

		if (vecPuntos.size() == 3)
		{
			// dibuja línea de primera punto a segunda punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[1].pos, ex, ey);
			linea(sx, sy, ex, ey, col, 0xFFFFFFFF);

			// dibuja línea de segunda punto a tercer punto
			MundoAScreen(vecPuntos[1].pos, sx, sy);
			MundoAScreen(vecPuntos[2].pos, ex, ey);
			linea(sx, sy, ex, ey, col, 0xFFFFFFFF);

			// dibuja línea de tercer punto a primer punto
			MundoAScreen(vecPuntos[2].pos, sx, sy);
			MundoAScreen(vecPuntos[0].pos, ex, ey);
			linea(sx, sy, ex, ey, col, 0xFFFFFFFF);
		}

	}
};

// ELIPSE
struct sElipse : public sFormas
{
	sElipse()
	{
		nMaxPunto = 3;
		vecPuntos.reserve(nMaxPunto);
	}

	void DibujarForma()
	{
		int sx, sy, ex, ey;

		if (vecPuntos.size() < 3)
		{
			// dibuja línea de primera punto a segunda punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[1].pos, ex, ey);
			linea(sx, sy, ex, sy, col, 0xFF00FF00);
		}

		if (vecPuntos.size() == 3)
		{
			// dibuja línea de primera punto a segunda punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[1].pos, ex, ey);
			linea(sx, sy, ex, sy, col, 0xFF00FF00);

			// dibuja línea de segunda punto a tercer punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[2].pos, ex, ey);
			linea(sx, sy, sx, ey, col, 0xFF00FF00);

			// Radio en x y y
			// Se calcula con el triangulo rectangulo
			float dx = abs(vecPuntos[0].pos.x - vecPuntos[1].pos.x);
			float dy = abs(vecPuntos[1].pos.y - vecPuntos[1].pos.y);
			float fRadiusx = sqrt(pow(dx, 2) + pow(dy, 2));
			float dx2 = abs(vecPuntos[0].pos.x - vecPuntos[0].pos.x);
			float dy2 = abs(vecPuntos[0].pos.y - vecPuntos[2].pos.y);
			float fRadiusy = sqrt(pow(dx2, 2) + pow(dy2, 2));
			// dibuja línea de tercer punto a primer punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);

			elipse((int)(fRadiusx * fWorldEscala), (int)(fRadiusy * fWorldEscala), sx, sy, col);
		}
	}
};

int main(int argc, char** argv)
{
	ALLEGRO_EVENT event;
	std::list<sFormas*> listaFormas;
	sFormas* formaTemp = nullptr;
	sPuntos* selecPunto = nullptr;
	vf2d vMouse;
	vf2d vCursor = { 0, 0 };
	float fCuadricula = 1.0f;
	bool mpres = false;
	bool bSinPunto = false;
	bool bMoverPunto = false;
	int nForma = NADA;
	bool panel = false;

	(void)argc;
	(void)argv;

	if (!ini_allegro(1200, 600, "Herramientas de Programacion Grafica"))
		return -1;

	vOffset = { (float)(-al_get_display_width(ini.sVentana) / 2) / fEscala, (float)(-al_get_display_height(ini.sVentana) / 2) / fEscala };

	while (true) {

		/*************************************************acualizacion de pantalla*******************************************************/
		al_wait_for_event(ini.colaevento, &event);
		vMouse = { (float)event.mouse.x ,(float)event.mouse.y };
		vf2d vMouseB;
		ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseB);
		vCursor.x = floorf((vMouseB.x + 0.5f) * fCuadricula);
		vCursor.y = floorf((vMouseB.y + 0.5f) * fCuadricula);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;
		if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				break;
			switch (toupper(event.keyboard.unichar)) {
			case 'S':
				if (bSinPunto)
					bSinPunto = false;
				else
					bSinPunto = true;
				break;
			case 'M':
				if (bMoverPunto)
					bMoverPunto = false;
				else
					bMoverPunto = true;
				break;
			case 'L':
				nForma = LINEA_B;
				break;
			case 'C':
				nForma = CIRCULO;
				break;
			case 'R':
				nForma = RECTANGULO;
				break;
			case 'E':
				nForma = ELIPSE;
				break;
			case 'T':
				nForma = TRIANGULO;
				break;
			}
		}
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			vCursor.x = floorf((vMouseB.x + 0.5f) * fCuadricula);
			vCursor.y = floorf((vMouseB.y + 0.5f) * fCuadricula);

			if (event.mouse.button == 1)
			{
				if (nForma == LINEA_B)
				{
					formaTemp = new sLinea();

					// primer punto
					selecPunto = formaTemp->OptenerPunto(vCursor);

					// segundo punto
					selecPunto = formaTemp->OptenerPunto(vCursor);
					nForma = NADA;
				}
				else if (nForma == CIRCULO)
				{
					formaTemp = new sCir();

					// primer punto
					selecPunto = formaTemp->OptenerPunto(vCursor);

					// segundo punto
					selecPunto = formaTemp->OptenerPunto(vCursor);
					nForma = NADA;
				}
				else if (nForma == RECTANGULO)
				{
					formaTemp = new sRec();

					// primer punto
					selecPunto = formaTemp->OptenerPunto(vCursor);

					// segundo punto
					selecPunto = formaTemp->OptenerPunto(vCursor);
					nForma = NADA;
				}
				else if (nForma == TRIANGULO)
				{
					formaTemp = new sTriangulo();

					// primer punto
					selecPunto = formaTemp->OptenerPunto(vCursor);

					// segundo punto
					selecPunto = formaTemp->OptenerPunto(vCursor);
					nForma = NADA;
				}
				else if (nForma == ELIPSE)
				{
					formaTemp = new sElipse();

					// primer punto
					selecPunto = formaTemp->OptenerPunto(vCursor);

					// segundo punto
					selecPunto = formaTemp->OptenerPunto(vCursor);
					nForma = NADA;
				}
				else if (bMoverPunto)
				{
					selecPunto = nullptr;
					for (auto& Formas : listaFormas)
					{
						selecPunto = Formas->HitPunto(vCursor);
						if (selecPunto != nullptr)
							break;
					}
				}
			}
			else if (event.mouse.button == 3)
			{
				vIniPan.x = vMouse.x;
				vIniPan.y = vMouse.y;
				mpres = true;
			}
		}
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
		{
			if (event.mouse.button == 1)
			{
				if (formaTemp != nullptr)
				{
					selecPunto = formaTemp->OptenerPunto(vMouse);
					if (selecPunto == nullptr)
					{
						formaTemp->col = GRAYI;
						listaFormas.push_back(formaTemp);
					}
				}
			}
			mpres = false;

		}
		if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {

			if (mpres)
			{
				vOffset.x -= (vMouse.x - vIniPan.x) / fEscala;
				vOffset.y -= (vMouse.y - vIniPan.y) / fEscala;
				vIniPan = vMouse;
			}
			else if (selecPunto != nullptr)
			{
				selecPunto->pos = vCursor;
			}

			if (event.mouse.x >= 0 && event.mouse.x <= al_get_bitmap_width(ini.icono1) && event.mouse.y >= 100 && event.mouse.y <= 100 + al_get_bitmap_height(ini.icono1))

			{
				panel = true;
			}


		}

		/*************************************************acualizacion de pantalla*******************************************************/
		if (al_is_event_queue_empty(ini.colaevento)) {

			al_clear_to_color(V_DARK);

			int sx, sy;
			int ex, ey;

			// Mundo
			vf2d vWorldTopLeft, vWorldBottomRight;
			ScreenToWorld(0, 0, vWorldTopLeft);
			ScreenToWorld(al_get_display_width(ini.sVentana), al_get_display_height(ini.sVentana), vWorldBottomRight);

			// Valores más allá de los límites de la pantalla
			vWorldTopLeft.x = floor(vWorldTopLeft.x);
			vWorldTopLeft.y = floor(vWorldTopLeft.y);
			vWorldBottomRight.x = ceil(vWorldBottomRight.x);
			vWorldBottomRight.y = ceil(vWorldBottomRight.y);

			WorldToScreen(0, vWorldTopLeft.y, sx, sy);
			WorldToScreen(0, vWorldBottomRight.y, ex, ey);
			linea(sx, sy, ex, ey, al_map_rgb(36, 64, 45), 0xFF00FF00);
			WorldToScreen(vWorldTopLeft.x, 0, sx, sy);
			WorldToScreen(vWorldBottomRight.x, 0, ex, ey);
			linea(sx, sy, ex, ey, al_map_rgb(36, 64, 45), 0xFF00FF00);

			// Actualizar coeficientes de traslacion de formas
			sFormas::fWorldEscala = fEscala;
			sFormas::vWorldOffset.x = vOffset.x;
			sFormas::vWorldOffset.y = vOffset.y;

			for (auto& forma : listaFormas)
			{
				forma->DibujarForma();
				if (bSinPunto)
					forma->DibujarPuntos();
			}

			if (formaTemp != nullptr)
			{
				formaTemp->DibujarForma();
				formaTemp->DibujarPuntos();
			}

			/**********************************iconos****************************************/
			al_draw_bitmap(ini.icono1, 0, 100, 0);
			if (panel)
			{
				al_draw_textf(ini.fuente, GRAYI, 200, 100, ALLEGRO_ALIGN_RIGHT, "panel");
				panel = false;
			}
			/**********************************iconos****************************************/

			WorldToScreen(vCursor.x, vCursor.y, sx, sy);
			circulo(4.0f, sx, sy, M_ORANGE);

			al_draw_textf(ini.fuente, GRAYI, 200, 20, ALLEGRO_ALIGN_RIGHT, "X: %.0f , Y: %.0f", vCursor.x, vCursor.y);
			al_set_target_backbuffer(ini.sVentana);
			al_flip_display();
		}
	}

	destroy();
	return 0;
}

