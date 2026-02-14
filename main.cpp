#include <GL/glut.h>
#include "Scene.h"
#include <iostream>

void displayCallback() {
    Scene::display();
}

void reshapeCallback(int w, int h) {
    Scene::reshape(w, h);
}

void keyboardCallback(unsigned char key, int x, int y) {
    Scene::handleKeyboard(key, x, y);
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Village Simulator - Terrain & Buildings");

    Scene::init();

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);


    glutTimerFunc(0, Scene::update, 0);

    glutMainLoop();
    return 0;
}
