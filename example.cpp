#include "font.h"
#include <GL/freeglut.h>


Freetype* font = NULL;


void on_display (void)
{
   /* draw begin */
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity ();

   /* glfreetype invoke */
   font->draw_text(0.0f, 0.0f, DEFAUL_TEXT_COLOR, L"Hellow, world! Эй, вы!");

   /* draw end */
   glutSwapBuffers ();
}


int main(int argc, char *argv[])
{
    /* setup GLUT */
    glutInit (&argc, argv);
    glutInitContextVersion(1, 3);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("glfreetype example");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutDisplayFunc(on_display);

    /* setup OpenGL */
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_MODELVIEW);

    /* glfreetype invoke */
    font = new Freetype("LiberationSans-Regular.ttf");

    /* run */
    glutMainLoop();

    return 0;
}
