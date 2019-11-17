#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

struct Point
{
	float x, y;
	Point(int a = 0, int b = 0)
	{
		x = a;
		y = b;
	}
};
struct Bian_list
{
	float dx, jx;
	int ymax, ymin;
	int sp;
};
struct Huo_list
{
	int num;
	Bian_list * next[10];
};
struct Xb
{
	float x[10];
	int num;
};

void InitiateHuo_list(Huo_list * H)
{
	H->num = 0;
	H->next[0] = NULL;
}
void InsertHuo_list(Huo_list * H, Bian_list * b_list)
{
	H->next[H->num] = b_list;
	H->num++;
}
void Deleteb_list(Huo_list * H, int j)
{
	int i;
	for (i = j; i < H->num; i++)
	{
		H->next[i] = H->next[i + 1];
	}
	H->num--;
}
void pai_xuHuo_list(Xb * xb)
{
	float t;
	int i=0, j;
	int n = xb->num;
	while (i < n - 1)
	{
		t = xb->x[i + 1];
		j = i;
		while (j > -1 && t < xb->x[j])
		{
			xb->x[j + 1] = xb->x[j];
			j--;
		}
		xb->x[j + 1] = t;
		i = i + 1;
	}
}

void ScanLine(Point * Polygon_point, int num)
{
	int i=0, j, ymax, ymin, d;
	ymax = Polygon_point[1].y;
	ymin = Polygon_point[1].y;
	d = ymax - ymin;
	Bian_list * b_list = (Bian_list *)malloc(num * sizeof(Bian_list));
	while ( i < num )
	{
		if (Polygon_point[i].y > ymax)
			ymax = Polygon_point[i].y;
		if (Polygon_point[i].y < ymin)
			ymin = Polygon_point[i].y;
		i++;
	}
    i = 0;
	while (i < num)
	{
		if (Polygon_point[i].y < Polygon_point[i + 1].y)
			b_list[i].jx = Polygon_point[i].x;
		else
			b_list[i].jx = Polygon_point[i + 1].x;
		if (!(Polygon_point[i].y ==Polygon_point[i + 1].y))
		{
			b_list[i].dx = (float)(Polygon_point[i].x - Polygon_point[i + 1].x) /
				(Polygon_point[i].y - Polygon_point[i + 1].y);
			b_list[i].sp = 0;
		}
		else
		{
			b_list[i].dx = (Polygon_point[i].x > Polygon_point[i + 1].x) ? Polygon_point[i].x : Polygon_point[i + 1].x;
			b_list[i].jx = (Polygon_point[i].x < Polygon_point[i + 1].x) ? Polygon_point[i].x : Polygon_point[i + 1].x;
			b_list[i].sp = 1;
		}
		b_list[i].ymax = (Polygon_point[i].y > Polygon_point[i + 1].y) ? Polygon_point[i].y : Polygon_point[i + 1].y;
		b_list[i].ymin = (Polygon_point[i].y < Polygon_point[i + 1].y) ? Polygon_point[i].y : Polygon_point[i + 1].y;
		i++;
}
	Xb xb;
	Huo_list * h_list = new(Huo_list);
	h_list->num = 0;
	i = ymin;
	while(i < ymax)
	{   int n = 0;
		xb.num = 0;
		for (j = 0; j < num - 1; j++)
			if (i == b_list[j].ymin)
			{
				InsertHuo_list(h_list, &b_list[j]);
			}
		
		while (n < h_list->num)
		{
			if (h_list->next[n]->sp)
			{
				xb.x[xb.num] = h_list->next[n]->jx;
				xb.num++;
				xb.x[xb.num] = h_list->next[n]->dx;
				xb.num++;
			}
			else
			{
				xb.x[xb.num] = h_list->next[n]->jx + h_list->next[n]->dx * (i - h_list->next[n]->ymin);
				xb.num++;
			}
			n++;
		}
		pai_xuHuo_list(&xb);
		int tc = 0;
		for (j = 0; j < xb.num; j++, j++)
		{
			int x, x1, xr;
			x1 = xb.x[j];
			xr = xb.x[j + 1];
			x = x1;
			while (x <= xr)
			{
				glColor3f(0.0, 0.0, 0.0);
				glVertex2i(x, i);
				x++;
			}
		}
		if (i == ymin)
			i--;
		i++;
		for (j = 0; j < h_list->num; j++)
			if (i == h_list->next[j]->ymax)
			{
				Deleteb_list(h_list, j);
			}
		if (i == ymin)
			i++;
		
	}
     i++;
}
void LineDDA(int x0, int y0, int x1, int y1)

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

	void ScanLineSegment()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		Point spt[8];
		spt[0] = Point(280, 280);
		spt[1] = Point(320, 350);
		spt[2] = Point(275, 280);
		spt[3] = Point(230, 280);
		spt[4] = Point(245, 260);
		spt[5] = Point(250, 254);
		spt[6] = Point(235, 245);
		spt[7] = Point(230, 230);
		ScanLine(spt, 8);
		glEnd();
		glFlush();
	}
	void main(int argc, char * argv[])
	{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowPosition(50, 100);
		glutInitWindowSize(500, 500);
		glutCreateWindow("扫描填充算法");
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0.0, 500.0, 0.0, 500.0);
		glutDisplayFunc(ScanLineSegment);
		glutMainLoop();
	}


