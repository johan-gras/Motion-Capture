#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include "base_opengl.h"
#include "server.h"

#define SIZE_WINDOW 530
#define RANGE_GUM 2000
#define TRAIL_SIZE 40

char presse,calc;
int x_gum, y_gum, x_acc, y_acc;
int reference_x = 0, reference_y = 0;
int x_trail[TRAIL_SIZE] = {0}, y_trail[TRAIL_SIZE] = {0};


/* affiche la chaine fmt a partir des coordonnées x,y*/
void draw_text(float x, float y, const char *fmt, ...)
{
    char        buf[1024];            //Holds Our String
    char        *text = buf;
    va_list        ap;                // Pointer To List Of Arguments
    
    if (fmt == NULL)                // If There's No Text
        return;                    // Do Nothing
    
    va_start(ap, fmt);                // Parses The String For Variables
    vsprintf(text, fmt, ap);            // And Converts Symbols To Actual Numbers
    va_end(ap);                    // Results Are Stored In Text
    
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    while (*text)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);
    
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* *************************************************** */
/* main */
int main(int argc,char **argv) {
    /* initialisation du serveur */
    init_serveur();
    
    /* initilisation de glut et creation de la fenetre */
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(200,200);
    glutInitWindowSize(SIZE_WINDOW,SIZE_WINDOW);
    glutCreateWindow("Kohonen");
    
    /* initialisation OpenGL */
    glClearColor(0,0,0,0);
    glColor3f(1,1,1);
    glEnable(GL_DEPTH_TEST);
    
    /* enregistrement des fonctions de rappel */
    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    
    /* Entree dans la boucle principale */
    glutMainLoop();
    stop_serveur();
    return 0;
}


/* fonction d'affichage appelée a chaque refresh*/
void affichage() {
    /* effacement de l'image avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Text
    glColor3f(1.0,1.0,1.0);
    draw_text(-0.9,0.9,"x %d y %d", x_gum-reference_x, y_gum-reference_y);
    
    // Gum
    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3f(1.0,0.0,0.0);
    glVertex2f((double)(x_gum-reference_x)/RANGE_GUM, (double)(y_gum-reference_y)/RANGE_GUM);
    glEnd();
    
    // Trail gum
    glPointSize(2);
    glBegin(GL_POINTS);
    glColor3f(0.0,1.0,0.0);
    for (int k = 0 ; k < TRAIL_SIZE ; k++){
        glVertex2f((double)(x_trail[k]-reference_x)/RANGE_GUM, (double)(y_trail[k]-reference_y)/RANGE_GUM);
    }
    glEnd();
    
    // Gum acc
    glPointSize(4);
    glBegin(GL_POINTS);
    glColor3f(0.0,0.0,1.0);
    glVertex2f((double)x_acc/64, (double)y_acc/64);
    glEnd();
    
    // Grid
    glBegin(GL_LINES);
    glColor3f(0.75f, 0.75f, 0.75f);
    for(int i=-10;i<=10;i++)
    {
        glVertex2f((float)i/10,(float)-1);
        glVertex2f((float)i/10,(float)1);
        
        glVertex2f((float)-1,(float)i/10);
        glVertex2f((float)1,(float)i/10);
    }
    glEnd();
    
    glFlush();
    glutSwapBuffers();
}

// Calculs
void idle(){
    // Trail array shift
    int similarity = 0;
    for (int k = TRAIL_SIZE-1; k >= 1 ; k--){
        x_trail[k]=x_trail[k-1];
        y_trail[k]=y_trail[k-1];
        
        if (x_trail[k] == x_gum && y_trail[k] == y_gum)
            similarity++;
    }
    x_trail[0] = x_gum;
    y_trail[0] = y_gum;
    
    // Reset reference
    if (similarity == TRAIL_SIZE-1){
        reference_x = x_gum;
        reference_y = y_gum;
    }
    
    get_data(&x_gum, &y_gum, &x_acc, &y_acc);
    glutPostRedisplay();
}


void clavier(unsigned char touche,int x,int y) {
    switch(touche) {
        case 'q': /* la touche 'q' permet de quitter le programme */
            exit(0);
    }
} /* clavier */


void reshape(int x,int y) {
    if (x<y)
        glViewport(0,(y-x)/2,x,x);
    else
        glViewport((x-y)/2,0,y,y);
} /* reshape */
