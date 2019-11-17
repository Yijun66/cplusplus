

#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h> 

class CPoint2D
{
public:
	float x, y;
};
void swapCP(CPoint2D *p1, CPoint2D *p2)
{
	CPoint2D tmp;
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}
void swapCodes(GLubyte *p1, GLubyte *p2)
{
	GLubyte tmp;
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}

class CRect
{
public:
	float xmin, ymin, xmax, ymax;

	float width(void) { return xmax - xmin; }
	float height(void) { return ymax - ymin; }
	void normalize(void);
	void draw(GLenum mode);
};

void CRect::normalize(void)
{
	float ftemp;
	if (xmin > xmax)
	{
		ftemp = xmin; xmin = xmax; xmax = ftemp;
	}
	if (ymin > ymax)
	{
		ftemp = ymin; ymin = ymax; ymax = ftemp;
	}
}

void CRect::draw(GLenum mode)
{
	glBegin(mode);
	glVertex2f(xmin, ymin);
	glVertex2f(xmax, ymin);
	glVertex2f(xmax, ymax);
	glVertex2f(xmin, ymax);
	glEnd();
}
void MPLineDraw(int x0, int y0, int x1, int y1)
{
	int x = x0, y = y0;
	int a = y0 - y1;
	int b = x1 - x0;
	int cx = (b >= 0 ? 1 : (b = -b, -1));
	int cy = (a <= 0 ? 1 : (a = -a, -1));

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	int d, d1, d2;
	if (-a <= b) // 斜率绝对值 <= 1
	{
		d = a + a + b;
		d1 = a + a;
		d2 = a + a + b + b;
		while (x != x1)
		{
			if (d < 0)
			{
				y += cy;
				d += d2;
			}
			else
			{
				d += d1;
			}
			x += cx;
			glBegin(GL_POINTS);
			glVertex2i(x, y);
			glEnd();
		}
	}
	else // 斜率绝对值 > 1
	{
		d = a + b + b;
		d1 = b + b;
		d2 = a + a + b + b;
		if (d < 0)
		{
			d += d1;
		}
		else
		{
			x += cx;
			d += d2;
		}
		y += cy;
		glBegin(GL_POINTS);
		glVertex2i(x, y);
		glEnd();
	}
}
#define PI 3.14159265359

int running_state = 0;


float scene_size = 1000.0;

CRect clip_rect;	
CRect window;	
CRect viewport;		
const GLint winLeftBitCode = 0x1;
const GLint winRightBitCode = 0x2;
const GLint winBottomBitCode = 0x4;
const GLint winTopBitCode = 0x8;
int pw_width, pw_height;

void set_window(CRect *cw)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(cw->xmin, cw->xmax, cw->ymin, cw->ymax);
}

void set_viewport(CRect *vp)
{
	glViewport(vp->xmin, vp->ymin,
		vp->width(), vp->height());
}

inline GLint roundx(const GLfloat a)
{
	return GLint(a + 0.5);
}

inline GLint inside(GLint code)
{
	return GLint(!code);
}
inline GLint reject(GLint code1, GLint code2)
{
	return GLint(code1&code2);
}
inline GLint accept(GLint code1, GLint code2)
{
	return GLint(!(code1 | code2));
}
GLubyte encode(CPoint2D pt)
{
	GLubyte code = 0x00;
	if (pt.x < clip_rect.xmin)
		code |= winLeftBitCode;
	if (pt.x > clip_rect.xmax)
		code |= winRightBitCode;
	if (pt.y < clip_rect.ymin)
		code |= winBottomBitCode;
	if (pt.y > clip_rect.ymax)
		code |= winTopBitCode;
	return code;
}

bool line_clipping(CPoint2D p1, CPoint2D p2, CRect *cw,
	CPoint2D *q1, CPoint2D *q2)
	
{
	
	GLubyte code1, code2;
	int done = false, plotLine = false;
	GLfloat m;/*
	printf("%f~%f~%f~%f===\n", p1.x, p1.y, p2.x, p2.y);
	printf("%f~%f~%f~%f===\n", window.xmax,window.xmin,window.ymax,window.ymin);
	printf("%f~%f~%f~%f===\n", clip_rect.xmax, clip_rect.xmin, clip_rect.ymax, clip_rect.ymin);*/
	while (!done)
	{
		code1 = encode(p1);
		code2 = encode(p2);
		if (accept(code1, code2))
		{
			done = true;
			plotLine = true;
		}
		else if (reject(code1, code2))
		{
			done = true;
		}
		else
		{
			if (inside(code1))
			{
				swapCP(&p1, &p2);
				swapCodes(&code1, &code2);
			}
			if (p2.x != p1.x)
				m = (p2.y - p1.y) / (p2.x - p1.x);
			if (code1&winLeftBitCode)
			{
				p1.y += (clip_rect.xmin - p1.x)*m;
				p1.x = clip_rect.xmin;
			}
			else if (code1&winRightBitCode)
			{
				p1.y += (clip_rect.xmax - p1.x)*m;
				p1.x = clip_rect.xmax;
			}
			else if (code1&winBottomBitCode)
			{
				if (p2.x != p1.x)
					p1.x += (clip_rect.ymin - p1.y) / m;
				p1.y = clip_rect.ymin;
			}
			else if (code1&winTopBitCode)
			{
				if (p2.x != p1.x)
					p1.x += (clip_rect.ymax - p1.y) / m;
				p1.y = clip_rect.ymax;
			}
		}
	}
	if (plotLine)
	{
		q1->x = roundx(p1.x);
		q1->y = roundx(p1.y);
		q2->x = roundx(p2.x);
		q2->y = roundx(p2.y);
	}
	return (bool)plotLine;
}

void Translaterect(int dx, int dy)
{
	clip_rect.xmin += dx;
	clip_rect.xmax += dx;
	clip_rect.ymin += dy;
	clip_rect.ymax += dy;
}
void MidpointLine(int x0, int y0, int x1, int y1)
{
	if ((x0 != x1) && (y0 != y1))
	{
		int a, b, delta1, delta2, d, x, y;
		float k = (float)(y1 - y0) / (float)(x1 - x0);
		if (k >= 0 && k <= 1)
		{
			a = y0 - y1;
			b = x1 - x0;
			d = 2 * a + b;
			delta1 = 2 * a;
			delta2 = 2 * (a + b);
			x = x0;
			y = y0;
			glVertex2i(x, y);
			while (x < x1)
			{
				if (d < 0)
				{
					x++;
					y++;
					d += delta2;
				}
				else
				{
					x++;
					d += delta1;
				}
				glVertex2i(x, y);
			}
		}
		else //if(k>1)
		{
			a = y0 - y1;
			b = x1 - x0;
			d = a + 2 * b;
			delta1 = 2 * (a + b);
			delta2 = 2 * b;
			x = x0;
			y = y0;
			glVertex2i(x, y);
			while (y < y1)
			{
				if (d < 0)
				{
					y++;
					d += delta2;
				}
				else
				{
					y++;
					x++;
					d += delta1;
				}
				glVertex2i(x, y);
			}
		}
	}
	else
	{
		int min, d;
		if (x0 == x1)
		{
			int x = x0, y;
			y = (y0 <= y1) ? y0 : y1;
			d = fabs((double)(y0 - y1));
			while (d >= 0)
			{
				glVertex2i(x, y);
				y++;
				d--;
			}
		}
		if (y0 == y1)
		{
			int x, y = y0;
			x = (x0 <= x1) ? x0 : x1;
			d = fabs((double)(x0 - x1));
			while (d >= 0)
			{
				glVertex2i(x, y);
				x++;
				d--;
			}
		}
	}
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_LINE_STIPPLE);
}


void display()
{
	int i;
	CPoint2D p1, p2, q1, q2;
	double a, r;

	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	MPLineDraw(100, 10, 200, 40);
	glBegin(GL_POINTS);
	MidpointLine(150, 70, 220, 180);
	glEnd();
	MPLineDraw(150, 100, 250,50);
	glPopMatrix();
	



	glutSwapBuffers();
}


void reshape(int w, int h)
{

	pw_width = w;
	pw_height = h;
	viewport.xmin = 0;
	viewport.xmax = w;
	viewport.ymin = 0;
	viewport.ymax = h;
	set_viewport(&viewport);
	window.xmin = -0.6*scene_size;
	window.xmax = 0.6*scene_size;
	window.ymin = -0.6*scene_size;
	window.ymax = 0.6*scene_size;
	set_window(&window);	 
	clip_rect.xmin = 0.5*window.xmin;
	clip_rect.xmax = 0.5*window.xmax;
	clip_rect.ymin = 0.5*window.ymin;
	clip_rect.ymax = 0.5*window.ymax;
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
	}
}

void special_key(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		Translaterect(-5.0, 0.0);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		Translaterect(5.0, 0.0);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		Translaterect(0.0, -5.0);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		Translaterect(0.0, 5.0);
		glutPostRedisplay();
		break;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Test 2D Clipping and Viewing");
	init();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_key);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
