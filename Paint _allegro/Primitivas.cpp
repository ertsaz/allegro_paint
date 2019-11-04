#include "Formas.h"
#include <ctype.h>
#include <vector>
#include <list>
#define ALLEGRO_UNSTABLE
#define RADIUS       5

struct Example {
	ALLEGRO_DISPLAY* sVentana;
	ALLEGRO_FONT* fuente;
	ALLEGRO_FONT* fuentebmp;
	ALLEGRO_EVENT_QUEUE* colaevento;
	ALLEGRO_BITMAP* dbuf;
	ALLEGRO_COLOR     bg;
	ALLEGRO_COLOR     fg;
};

static struct Example ini;

struct vf2d
{
	float x;
	float y;
};

vf2d vOffset = { 0.0f, 0.0f };
vf2d vStartPan = { 0.0f, 0.0f };
float fScale = 10.0f;

// Convert coordinates from World Space --> Screen Space
void WorldToScreen(const float v_x, const float v_y, int& nScreenX, int& nScreenY)
{
	nScreenX = (int)((v_x - vOffset.x) * fScale);
	nScreenY = (int)((v_y - vOffset.y) * fScale);
}

// Convert coordinates from Screen Space --> World Space
void ScreenToWorld(int nScreenX, int nScreenY, vf2d& v)
{
	v.x = (float)(nScreenX) / fScale + vOffset.x;
	v.y = (float)(nScreenY) / fScale + vOffset.y;
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

	/*fundo = al_load_bitmap("img/icons8.png");
	if (!fundo) {

		destroy();
		return 0;
	}*/

	/*flecha_titulo = al_load_bitmap("img/arrow.png");
	if (!flecha_titulo) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"La imagen no se pudo cargar", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}*/

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

	/*timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		al_show_native_message_box(NULL, "ERRO",
			"Se produjo el siguiente error y el programa finalizará:",
			"Error al crear el temporizador", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		destroy();
		return 0;
	}*/

	al_register_event_source(ini.colaevento, al_get_display_event_source(ini.sVentana));
	al_register_event_source(ini.colaevento, al_get_mouse_event_source());
	al_register_event_source(ini.colaevento, al_get_keyboard_event_source());
	/*al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	al_start_timer(timer);*/

	return 1;
}

struct sFormas;

// Define a node
struct sPuntos
{
	sFormas* tipo_forma;
	vf2d pos;
};


// Clase BASE define la interfaz para todas las formas.
struct sFormas
{
	// Shapes are defined by the placment of nodes
	std::vector<sPuntos> vecPuntos;
	int nMaxPunto = 0;

	// The colour of the shape
	ALLEGRO_COLOR col = DARK_CYAN;

	// All shapes share word to screen transformation
	// coefficients, so share them staically
	static float fWorldScale;
	static vf2d vWorldOffset;

	// Convert coordinates from World Space --> Screen Space
	void MundoAScreen(const vf2d& v, int& nScreenX, int& nScreenY)
	{
		nScreenX = (int)((v.x - vWorldOffset.x) * fWorldScale);
		nScreenY = (int)((v.y - vWorldOffset.y) * fWorldScale);
	}

	// This is a PURE function, which makes this class abstract. A sub-class
	// of this class must provide an implementation of this function by
	// overriding it
	virtual void DibujarForma() = 0;

	// Shapes are defined by nodes, the shape is responsible
	// for issuing nodes that get placed by the user. The shape may
	// change depending on how many nodes have been placed. Once the
	// maximum number of nodes for a shape have been placed, it returns
	// nullptr
	sPuntos* OptenerPunto(const vf2d& p)
	{
		if (vecPuntos.size() == nMaxPunto)
			return nullptr; // Shape is complete so no new nodes to be issued

		// else create new node and add to shapes node vector
		sPuntos n;
		n.tipo_forma = this;
		n.pos = p;
		vecPuntos.push_back(n);

		// Beware! - This normally is bad! But see sub classes
		return &vecPuntos[vecPuntos.size() - 1];
	}

	// Test to see if supplied coordinate exists at same location
	// as any of the nodes for this shape. Return a pointer to that
	// node if it does
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

	// Draw all of the nodes that define this shape so far
	void DibujarPuntos()
	{
		int i = 0;
		for (auto& n : vecPuntos)
		{
			i++;
			int sx, sy;
			MundoAScreen(n.pos, sx, sy);
			al_draw_filled_circle(sx, sy, RADIUS/1.5, S_ORANGE);
			//circulo(RADIUS, sx, sy, MODER_ORANGE);
		}
	}
};

// We must provide an implementation of our static variables
float sFormas::fWorldScale = 1.0f;
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

		// Note the radius is also scaled so it is drawn appropriately
		circulo(fRadius * fWorldScale,sx, sy, col);
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
			// Can only draw line from first to second
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
			// Can only draw line from first to second
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[1].pos, ex, ey);
			linea(sx, sy, ex, ey, col, 0xFF00FF00);
		}

		if (vecPuntos.size() == 3)
		{
			// dibuja línea de primera punto a segunda punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[1].pos, ex, ey);
			linea(sx, sy, ex, ey, col, 0xFF00FF00);

			// dibuja línea de segunda punto a tercer punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);
			MundoAScreen(vecPuntos[2].pos, ex, ey);
			linea(sx, sy, ex, ey, col, 0xFF00FF00);


			float dx = abs(vecPuntos[0].pos.x - vecPuntos[1].pos.x);
			float dy = abs(vecPuntos[0].pos.y - vecPuntos[1].pos.y);
			float fRadiusx = sqrt(pow(dx, 2) + pow(dy, 2));
			float dx2 = abs(vecPuntos[0].pos.x - vecPuntos[2].pos.x);
			float dy2 = abs(vecPuntos[0].pos.y - vecPuntos[2].pos.y);
			float fRadiusy = sqrt(pow(dx2, 2) + pow(dy2, 2));
			// dibuja línea de tercer punto a primer punto
			MundoAScreen(vecPuntos[0].pos, sx, sy);

			elipse((int)(fRadiusx* fWorldScale), (int)(fRadiusy* fWorldScale), sx, sy, col);
		}
	}
};

int main(int argc, char** argv)
{
	ALLEGRO_EVENT event;
	std::list<sFormas*> listShapes;
	sFormas* formaTemp = nullptr;
	sPuntos* selecPunto = nullptr;
	vf2d vMouse;
	vf2d vCursor = { 0, 0 };
	float fGrid = 1.0f;
	float fScale = 10.0f;
	bool mdown = false;
	int nForma = NADA;

	(void)argc;
	(void)argv;

	if (!ini_allegro(1400, 800, "Herramientas de Programacion Grafica"))
		return -1;

	vOffset = { (float)(-al_get_display_width(ini.sVentana) / 2) / fScale, (float)(-al_get_display_height(ini.sVentana) / 2) / fScale };

	while (true) {

		/*************************************************acualizacion de pantalla*******************************************************/
		al_wait_for_event(ini.colaevento, &event);
		vMouse = { (float)event.mouse.x ,(float)event.mouse.y };
		vf2d vMouseB;
		ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseB);
		vCursor.x = floorf((vMouseB.x + 0.5f) * fGrid);
		vCursor.y = floorf((vMouseB.y + 0.5f) * fGrid);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;
		if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				break;
			switch (toupper(event.keyboard.unichar)) {
			case ' ':

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

			if (nForma == LINEA_B)
			{
				formaTemp = new sLinea();

				// Place first node at location of keypress
				selecPunto = formaTemp->OptenerPunto(vCursor);

				// Get Second node
				selecPunto = formaTemp->OptenerPunto(vCursor);
				nForma = NADA;
			}
			else if (nForma == CIRCULO)
			{
				formaTemp = new sCir();

				// Place first node at location of keypress
				selecPunto = formaTemp->OptenerPunto(vCursor);

				// Get Second node
				selecPunto = formaTemp->OptenerPunto(vCursor);
				nForma = NADA;
			}
			else if (nForma == RECTANGULO)
			{
				formaTemp = new sRec();

				// Place first node at location of keypress
				selecPunto = formaTemp->OptenerPunto(vCursor);

				// Get Second node
				selecPunto = formaTemp->OptenerPunto(vCursor);
				nForma = NADA;
			}
			else if (nForma == TRIANGULO)
			{
				formaTemp = new sTriangulo();

				// Place first node at location of keypress
				selecPunto = formaTemp->OptenerPunto(vCursor);

				// Get Second node
				selecPunto = formaTemp->OptenerPunto(vCursor);
				nForma = NADA;
			}
			else if (nForma == ELIPSE)
			{
				formaTemp = new sElipse();

				// Place first node at location of keypress
				selecPunto = formaTemp->OptenerPunto(vCursor);

				// Get Second node
				selecPunto = formaTemp->OptenerPunto(vCursor);
				nForma = NADA;
			}
			else if (event.mouse.button == 1){}
			else if (event.mouse.button == 2){}
			//rclick(x, y);
			else if (event.mouse.button == 3)
			{
				vStartPan.x = vMouse.x;
				vStartPan.y = vMouse.y;
				mdown = true;
			}
		}
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (formaTemp != nullptr)
			{
				selecPunto = formaTemp->OptenerPunto(vMouse);
				if (selecPunto == nullptr)
				{
					formaTemp->col = GRAYI;
					listShapes.push_back(formaTemp);
				}
			}
			mdown = false;
		}
		if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (mdown)
			{
				vOffset.x -= (vMouse.x - vStartPan.x) / fScale;
				vOffset.y -= (vMouse.y - vStartPan.y) / fScale;
				vStartPan = vMouse;
			}
			else if (selecPunto != nullptr)
			{
				selecPunto->pos = vCursor;
			}
		}
		
		/*************************************************acualizacion de pantalla*******************************************************/
		if (al_is_event_queue_empty(ini.colaevento)) {

			al_clear_to_color(al_map_rgb(14, 14, 15));

			int sx, sy;
			int ex, ey;

			// Get visible world
			vf2d vWorldTopLeft, vWorldBottomRight;
			ScreenToWorld(0, 0, vWorldTopLeft);
			ScreenToWorld(al_get_display_width(ini.sVentana), al_get_display_height(ini.sVentana), vWorldBottomRight);

			// Get values just beyond screen boundaries
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

			// Update shape translation coefficients
			sFormas::fWorldScale = fScale;
			sFormas::vWorldOffset.x = vOffset.x;
			sFormas::vWorldOffset.y = vOffset.y;
			
			for (auto& forma : listShapes)
			{
				forma->DibujarForma();
				forma->DibujarPuntos();
			}

			if (formaTemp != nullptr)
			{
				formaTemp->DibujarForma();
				formaTemp->DibujarPuntos();
			}

			al_draw_textf(ini.fuente, al_map_rgb(255, 255, 255), 200, 20, ALLEGRO_ALIGN_RIGHT, "X: %.2f , Y: %.2f", vCursor.x, vCursor.y);
			al_set_target_backbuffer(ini.sVentana);
			al_flip_display();
		}
	}

	destroy();
	return 0;
}

