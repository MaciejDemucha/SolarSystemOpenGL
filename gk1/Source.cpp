
#define GLEW_STATIC
#define FREEGLUT_STATIC
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdlib.h>    
#include <time.h> 
#include <math.h> 
#include <iostream>
#include <Gl/freeglut.h>
#include <string>
//#include <gl/gl.h>
//#include <gl/glut.h>


typedef float point3[3];

const int N = 100; //liczba przedziałow
int model = 1;		//zmienna okreslajaca jaki model należy wyświetlic
bool rotate = false; //zmienna pomocnicza informująca czy należy włączyć rotację obiektu
GLuint texture; // przechowywanie tekstury

float sunR = 0.8;
float mercuryR = 0.05;
float wenusR = 0.2;
float earthR = 0.15;
float marsR = 0.1;
float jupiterR = 0.5;
float saturnR = 0.4;
float uraniusR = 0.3;
float neptunR = 0.3;

float mercuryOrbit = 1.0;
float wenusOrbit = 1.38;
float earthOrbit = 1.81;
float marsOrbit = 2.16;
float jupiterOrbit = 3.42;
float saturnOrbit = 4.73;
float uraniusOrbit = 7.3;
float neptunOrbit = 10.89;

float mercurySpeed = 0.16;
float wenusSpeed = 0.15;
float earthSpeed = 0.11;
float marsSpeed = 0.12;
float jupiterSpeed = 0.1;
float saturnSpeed = 0.09;
float uraniusSpeed = 0.08;
float neptunSpeed = 0.06;

static GLfloat thetaYAxis = 0.0;   // kąt obrotu obiektu
static GLfloat thetaXAxis = 0.0;   // kąt obrotu obiektu
static GLfloat pix2angle;     // przelicznik pikseli na stopnie

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciśnięto żadnego klawisza
							   // 1 - naciśnięty zostać lewy klawisz


static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą
									  // i poprzednią kursora myszy

static int y_pos_old = 0;       // poprzednia pozycja kursora myszy

static int delta_y = 0;        // różnica pomiędzy pozycją bieżącą
									  // i poprzednią kursora myszy

static GLfloat radius = 14.0;	//promien sfery po której porusza się obserwator

static GLfloat theta, phi = 0.0;	//kąt azymutu i elewacji
static GLfloat delta_theta = 0.0;
static GLfloat delta_phi = 0.0;
static GLfloat old_theta = 0.0;
static GLfloat old_phi = 0.0;

static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
static GLfloat center[] = { 0.0, 0.0, 0.0 };
static GLfloat up[] = { 0.0, 1.0, 0.0 };


static GLfloat thetaMercury = 0.0;
static GLfloat thetaWenus = 0.0;
static GLfloat thetaEarth = 0.0;
static GLfloat thetaMars = 0.0;
static GLfloat thetaJupiter = 0.0;
static GLfloat thetaSaturn = 0.0;
static GLfloat thetaUranius = 0.0;
static GLfloat thetaNeptun = 0.0;

/*************************************************************************************/


GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{

	// Struktura dla nagłówka pliku  TGA

#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;

	// Wartości domyślne zwracane w przypadku błędu

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;

	// Przeczytanie nagłówka pliku 


	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	// Odczytanie szerokości, wysokości i głębi obrazu

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

	// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	// Obliczenie rozmiaru bufora w pamięci


	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	// Alokacja pamięci dla danych obrazu

	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}

	// Ustawienie formatu OpenGL


	switch (sDepth)

	{

	case 3:

		*ImFormat = GL_BGR_EXT;

		*ImComponents = GL_RGB8;

		break;

	case 4:

		*ImFormat = GL_BGRA_EXT;

		*ImComponents = GL_RGBA8;

		break;

	case 1:

		*ImFormat = GL_LUMINANCE;

		*ImComponents = GL_LUMINANCE8;

		break;

	};



	fclose(pFile);



	return pbitsperpixel;

}

void setTexture(const char* filename)
{
	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;
	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga

	pBytes = LoadTGAImage(filename, &ImWidth, &ImHeight, &ImComponents, &ImFormat);


	// Zdefiniowanie tekstury 2-D

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	//Zwolnienie pamięci

	free(pBytes);

	// Włączenie mechanizmu teksturowania

	glEnable(GL_TEXTURE_2D);

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Określenie sposobu nakładania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}



/*************************************************************************************/

void drawCircle(float cx, float cz, float r, int num_segments)
{
	float theta = 3.1415926 * 2 / float(num_segments);
	float tangetial_factor = tanf(theta);//calculate the tangential factor 

	float radial_factor = cosf(theta);//calculate the radial factor 

	float x = r;//we start at angle = 0 

	float z = 0;
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	for (int ii = 0; ii < num_segments; ii++)
	{
		glVertex3f(x + cx, 0, z + cz);

		//calculate the tangential vector 
		//remember, the radial vector is (x, y) 
		//to get the tangential vector we flip those coordinates and negate one of them 

		float tx = -z;
		float ty = x;

		//add the tangential vector 

		x += tx * tangetial_factor;
		z += ty * tangetial_factor;

		//correct using the radial factor 

		x *= radial_factor;
		z *= radial_factor;
	}
	glEnd();
}


// Funkcja rysująca osie układu współrzędnych

void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// początek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// początek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  początek i koniec obrazu osi y
	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();

}

void drawSphere( double r, int lats, int longs, const char* textureName) {
	
	setTexture(textureName);
	GLUquadric* quad;
	glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na biały
	quad = gluNewQuadric();
	gluQuadricTexture(quad, true);
	gluSphere(quad, r, lats, longs);
}

//Funkcja odpowiadająca za efekt obrotu obiektu
void rotateObject()
{
	if (rotate) {

		thetaMercury += mercurySpeed;
		if (thetaMercury > 360.0) thetaMercury -= 360.0;

		thetaWenus += wenusSpeed;
		if (thetaWenus > 360.0) thetaWenus -= 360.0;

		thetaEarth += earthSpeed;
		if (thetaEarth > 360.0) thetaEarth -= 360.0;

		thetaMars += marsSpeed;
		if (thetaMars > 360.0) thetaMars -= 360.0;

		thetaJupiter += jupiterSpeed;
		if (thetaJupiter > 360.0) thetaJupiter -= 360.0;

		thetaSaturn += saturnSpeed;
		if (thetaSaturn > 360.0) thetaSaturn -= 360.0;

		thetaUranius += uraniusSpeed;
		if (thetaUranius > 360.0) thetaUranius -= 360.0;

		thetaNeptun += neptunSpeed;
		if (thetaNeptun > 360.0) thetaNeptun -= 360.0;


		glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
	}
}

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

	gluLookAt(viewer[0], viewer[1], viewer[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	// Zdefiniowanie położenia obserwatora

	Axes();
	// Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej


	if (status == 1)                     // jeśli lewy klawisz myszy wcięnięty
	{
		theta += delta_theta * pix2angle;    // modyfikacja kąta obrotu o kat proporcjonalny
		phi += delta_phi * pix2angle;		 // do różnicy położeń kursora myszy
	}

	if (status == 2)                     // jeśli lewy klawisz myszy wcięnięty
	{
		radius += delta_y * pix2angle;
	}

	viewer[0] = radius * cos(theta) * cos(phi);
	viewer[1] = radius * sin(phi);
	viewer[2] = radius * sin(theta) * cos(phi);
	
	
	drawCircle(0, 0, mercuryOrbit, 360);
	
	drawCircle(0, 0, wenusOrbit, 360);

	drawCircle(0, 0, earthOrbit, 360);

	drawCircle(0, 0, marsOrbit, 360);
	
	drawCircle(0, 0, jupiterOrbit, 360);
	
	drawCircle(0, 0, saturnOrbit, 360);
	
	drawCircle(0, 0, uraniusOrbit, 360);
	
	drawCircle(0, 0, neptunOrbit, 360);

	glPushMatrix();
	drawSphere( sunR, 100, 100, "textures/sun.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(mercuryOrbit * cos(thetaMercury), 0, mercuryOrbit * sin(thetaMercury));
	drawSphere( mercuryR, 20, 20, "textures/mercury.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(wenusOrbit * cos(thetaWenus), 0, wenusOrbit * sin(thetaWenus));
	drawSphere( wenusR, 20, 20, "textures/venus.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(earthOrbit * cos(thetaEarth), 0, earthOrbit * sin(thetaEarth));
	drawSphere( earthR, 20, 20, "textures/earth.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(marsOrbit * cos(thetaMars), 0, marsOrbit * sin(thetaMars));
	drawSphere( marsR, 20, 20, "textures/mars.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(jupiterOrbit * cos(thetaJupiter), 0, jupiterOrbit * sin(thetaJupiter));
	drawSphere( jupiterR, 20, 20, "textures/jupiter.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(saturnOrbit * cos(thetaSaturn), 0, saturnOrbit * sin(thetaSaturn));
	drawSphere( saturnR, 20, 20, "textures/saturn.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(uraniusOrbit * cos(thetaUranius), 0, uraniusOrbit * sin(thetaUranius));
	drawSphere( uraniusR, 20, 20, "textures/uranus.tga");
	glPopMatrix();

	glPushMatrix();
	glTranslatef(neptunOrbit * cos(thetaNeptun), 0, neptunOrbit * sin(thetaNeptun));
	drawSphere( neptunR, 20, 20, "textures/neptune.tga");
	glPopMatrix();

	glFlush();
	// Przekazanie poleceń rysujących do wykonania

	glutSwapBuffers();
}

//zmiana modelu rysowanego jajka
void keys(unsigned char key, int x, int y)
{
	
	//rotacja
	
	if (key == 'r') rotate = !rotate;

	RenderScene(); // przerysowanie obrazu sceny
}

//komunikacja z uzytkownikiem
void menu() {
	
	std::cout << "r - rotacja \n";
	std::cout << "LPM - obracanie kamera \n";
	std::cout << "PPM - zoom \n";
}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania

/*************************************************************************************/
// Funkcja "monitoruje" położenie kursora myszy i ustawia wartości odpowiednich
// zmiennych globalnych

void Motion(GLsizei x, GLsizei y)
{
	
	//x_pos_old = x;            

	delta_y = y - y_pos_old;
	y_pos_old = y;

	
	GLfloat i = 0.1;

	delta_theta = (x - old_theta) * i;
	old_theta = x;

	delta_phi = (y - old_phi) * i;
	old_phi = y;

	glutPostRedisplay();     // przerysowanie obrazu sceny
}

/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia wartości odpowiednich zmiennych globalnych

void Mouse(int btn, int state, int x, int y)
{


	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		        // przypisanie aktualnie odczytanej pozycji kursora
							 // jako pozycji poprzedniej

		y_pos_old = y;
		old_theta = x;
		old_phi = y;
		status = 1;          // wcięnięty został lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		       // przypisanie aktualnie odczytanej pozycji kursora
							 // jako pozycji poprzedniej
		y_pos_old = y;
		status = 2;          // wcięnięty został prawy klawisz myszy
	}
	else

		status = 0;          // nie został wcięnięty żaden klawisz
}


void MyInit(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcący (wypełnienia okna) ustawiono na czarny

	// Zmienne dla obrazu tekstury

	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	// Teksturowanie będzie prowadzone tyko po jednej stronie ściany

	glEnable(GL_CULL_FACE);


	// Włączenie mechanizmu teksturowania

	glEnable(GL_TEXTURE_2D);

	// Ustalenie trybu teksturowania

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Określenie sposobu nakładania tekstur

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLfloat light_position[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	

}

/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bieżącej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkości okna okna widoku (viewport) w zależności
	// relacji pomiędzy wysokością i szerokością okna

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli



void main(int argc, char** argv)
{

	menu();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Układ słoneczny");

	glutKeyboardFunc(keys);

	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy

	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Bedzie ona wywoływana za każdym razem
	// gdy zajdzie potrzba przeryswania okna 
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// zazmiany rozmiaru okna      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania powierzchni niewidocznych

	glutIdleFunc(rotateObject);

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT


}

/*************************************************************************************/