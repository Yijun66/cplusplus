#include<iostream>
#include<gl/glut.h>
#include<vector>
using namespace std;
int result[6][2] = { { 22, 11 },{ 50, 80 },{ 80, 15 },{ 90, 80 },{ 100, 30 },{ 100, 10 } };
int re[4][2] = { { 20, 120 },{ 120, 120 },{ 120, 20 },{ 20, 20 } };
enum mark { L, B, R, T };
struct P {
	int x;
	int y;
	int judge;
};
struct node {
	int data[2];
	int judge;
	node* next;
	node* pre;
};
void addNode(node *& now, int num1, int num2, int ju) {
	node *p = now;
	node *q = NULL;
	q = new node;
	q->data[0] = num1;
	q->data[1] = num2;
	q->judge = ju;
	q->pre = p;
	p->next = q;
	now = q;
}
void insertNode(node *&one, node *&two, const P Three, bool change) {
	node* tem = new node;
	tem = new node;
	tem->data[0] = Three.x;
	tem->data[1] = Three.y;
	tem->judge = Three.judge;
	tem->next = two;
	two->pre = tem;
	tem->pre = one;
	one->next = tem;
	if (change) {
		two = tem;
	}
	else {
		one = tem;
	}
}
void initNode(node *&head, node *&rec) {
	head = new node;
	rec = new node;
	head->data[0] = rec->data[0] = 0;
	head->data[1] = rec->data[1] = 0;
	head->next = rec->next = NULL;
	head->pre = rec->pre = NULL;

	node *p = head;
	//记录多边形顶点
	for (int i = 0; i < 6; ++i) {
		addNode(p, result[i][0], result[i][1], 0);
	}
	p->next = head;
	head->pre = p;
	p = rec;
	for (int i = 0; i < 4; ++i) {
		addNode(p, re[i][0], re[i][1], 0);
	}
	p->next = rec;
	rec->pre = p;
}
int cross(const node* one, const node* two, const int edg) {
	int result = 0;
	switch (edg) {
	case T:
		if ((one->data[1] > 120) && (two->data[1] < 120) || (one->data[1] < 120) && (two->data[1] > 120)) {
			result = 1;
		}
		break;
	case R:
		if ((one->data[0] > 120) && (two->data[0] < 120) || (one->data[0] < 120) && (two->data[0] > 120)) {
			result = 1;
		}
		break;
	case B:
		if ((one->data[1] > 20) && (two->data[1] < 20) || (one->data[1] < 20) && (two->data[1] > 20)) {
			result = 1;
		}
		break;
	case L:
		if ((one->data[0] > 20) && (two->data[0] < 20) || (one->data[0] < 20) && (two->data[0] > 20)) {
			result = 1;
		}
		break;
	default:
		break;
	}
	return result;
}

void testPoint(vector<P> &Three, node *&first, node *&head, int edg, float xl, float xr, float yb, float yt) {
	node* p = first;
	node* q = p->next;
	P tem;
	while (p != head) {
		int x = 0;
		int y = 0;
		if (q == head) {
			q = q->next;
		}
		float k = 0;
		if (q->data[0] - p->data[0] != 0) {
			k = (q->data[1] - p->data[1])*1.0 / (q->data[0] - p->data[0]);
		}
		switch (edg) {
		case L:
			if (cross(p, q, L)) {
				x = xl;
				y = p->data[1] + (x - p->data[0]) * k;
				tem.x = x;
				tem.y = y;
				tem.judge = 1;
				Three.push_back(tem);
			}
			break;
		case R:
			if (cross(p, q, R)) {
				x = xr;
				y = p->data[1] + (x - p->data[0])*k;
				tem.x = x;
				tem.y = y;
				tem.judge = 1;
				Three.push_back(tem);
			}
			break;
		case B:
			if (cross(p, q, B)) {
				if (k == 0) {    
					if (p->data[1] != q->data[1]) {
						y = yb;
						x = p->data[0];
					}
					else {
						break;
					}
				}
				if (k != 0) {
					y = yb;
					x = p->data[0] + (y - p->data[1]) / k;
				}
				tem.x = x;
				tem.y = y;
				tem.judge = 1;
				Three.push_back(tem);
			}
			break;
		case T:
			if (cross(p, q, T)) {
				if (k == 0) {
					if (p->data[1] != q->data[1]) {
						y = yb;
						x = p->data[0];
					}
					else {
						break;
					}
				}
				if (k != 0) {
					y = yt;
					x = p->data[0] + (y - p->data[1]) / k;
				}
				tem.x = x;
				tem.y = y;
				tem.judge = 1;
				Three.push_back(tem);
			}
			break;
		default:
			break;
		}
		p = p->next;
		q = p->next;
	}
}
void getPoint(vector<P> &Three, node* &head, float xl, float xr, float yb, float yt) {
	node* p = head->next;
	for (int i = 0; i < 4; ++i) {
		testPoint(Three, p, head, i, xl, xr, yb, yt);
	}
}

void sortPointRec(const vector<P> Three, node *& recPoint, int num) {
	node *p = recPoint->next;
	node *q = p->next;
	bool change = false;
	if (Three[0].y == 20) {
		change = true;
	}
	if (num == 2) {
		change = false;
	}
	for (int i = 0; i < Three.size(); ++i) {
		if (q == recPoint) {
			q = q->next;
		}
		if (Three[i].y == 120) {
			if ((Three[i].x <= q->data[0] && (q->data[1] == 120)) || (Three[i].x <= q->data[0] && cross(q, p, T))) {
				insertNode(p, q, Three[i], change);
			}
			else {
				p = p->next;
				q = p->next;
				i--;
			}
		}
		else if (Three[i].x == 120) {
			if ((Three[i].y >= q->data[1] && (q->data[0] == 120)) || (Three[i].y > p->data[1] && cross(p, q, R))) {
				insertNode(p, q, Three[i], change);

			}
			else {
				p = p->next;
				q = p->next;
				i--;
			}
		}
		else if (Three[i].y == 20) {
			if ((Three[i].x <= p->data[0] && (p->data[1] == 20)) || (Three[i].x <= q->data[0] && cross(p, q, B))) {
				insertNode(p, q, Three[i], change);
			}
			else {
				p = p->next;
				q = p->next;
				i--;
			}
		}
		else if (Three[i].x == 20) {
			if ((Three[i].y <= p->data[1] && (p->data[0] == 20)) || (Three[i].y >= q->data[1] && cross(p, q, L))) {
				insertNode(p, q, Three[i], change);
			}
			else {
				p = p->next;
				q = p->next;
				i--;
			}
		}
	}
}

void getArray(node *& polPoint, const vector<P> Three, node *& recPoint) {
	sortPointRec(Three, recPoint, 1);
	sortPointRec(Three, polPoint, 2);
}

void draw(node *& polPoint, node *& recPoint) {
	vector<P> Q, S;
	node *one = polPoint->next;
	node *two = recPoint->next;
	P tem;
	int c = 1;
	bool f = false, begin = true;

	while (c != 0) {
		if (one->judge == 1) {  
			tem.x = one->data[0];
			tem.y = one->data[1];
			tem.judge = 0;
			one->judge = 0;   
			S.push_back(tem);  
			f = true;
		}
		if (f) {
			tem.x = one->data[0];
			tem.y = one->data[1];
			tem.judge = 0;
			Q.push_back(tem);
		}
		if (one->judge == 2) { 
			f = false;
			while ((two->data[0] != one->data[0]) && (two->data[1] != one->data[1])) { 
				two = two->next;
			}
			while ((two->data[0] != S[0].x) && (two->data[1] != S[0].y)) {
				if (two != recPoint) {  
					tem.x = one->data[0];
					tem.y = one->data[1];
					tem.judge = 0;
					Q.push_back(tem);
				}
				two = two->next;
			}
			two = recPoint->next;
			glLineWidth(3);
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < Q.size(); ++i) {
				glVertex2f(Q[i].x, Q[i].y);
			}
			glEnd();
			Q.clear();
			S.clear();

			node *ma = polPoint->next;  
			c = 0;
			while (ma != polPoint) {
				if (ma->judge == 1) {
					c++;
				}
				ma = ma->next;
			}
		}
		one = one->next;
		if (one == polPoint) {
			one = one->next;
		}
	}
}

void drawPol() {
	node *h = new node;
	node *recPoint = new node;
	initNode(h, recPoint);
	node *p = h->next;
	glColor3f(0.f, 0.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
	while (p != h) {
		glVertex2f(p->data[0], p->data[1]);
		p = p->next;
	}
	glEnd();
	vector<P> Three;
	getPoint(Three, h, 20, 120, 20, 120);

	/*for (int i = 0; i <= c; ++i) {
	cout << "请输入第" << i << "个点的标记：" << endl;
	cin >> Three[i].judge;
	}*/
	Three[1].judge = 2;
	Three[3].judge = 2;
	getArray(h, Three, recPoint);
	glColor3f(1.f, 0.0f, 0.0f);
	draw(h, recPoint);
}
void initPlo() {
	glClear(GL_COLOR_BUFFER_BIT);

	drawPol();
	glColor3f(0.f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(20.f, 20.f);
	glVertex2f(20.f, 120.f);
	glVertex2f(120.f, 120.f);
	glVertex2f(120.f, 20.f);
	glEnd();
	glFlush();
}
void changeSize(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h) {
		glOrtho(0.0f, 200.0f, 0.0f, 200.0f * h / w, 1.0f, -1.0f);
	}
	else {
		glOrtho(0.0f, 200.0f * w / h, 0.0f, 200.0f, 1.0f, -1.0f);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void setUpRc() {
	glClearColor(1.f, 1.0f, 1.0f, 1.f);
}
void main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("WA");
	glutDisplayFunc(initPlo);
	glutReshapeFunc(changeSize);
	setUpRc();
	glutMainLoop();
}

