/*------------------------ Author: dim6ata ---------------------------------*/
/*------------------------ Asteroids Coursework 1 -------------------------------*/
/*------------------------------ References: ------------------------------------

https://www.learncpp.com/cpp-tutorial/59-random-number-generation/ - source for learning about random number generation
http://www.cplusplus.com/reference/vector/vector/erase/ - source for learning how to use vector.erase()
https://gamedev.stackexchange.com/questions/33447/aligning-bullets-position-in-asteroid-game - source for learning how to calculate the position of a projectile
 https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml - explored different options for blending */


/*------------------------ LIBRARIES IMPORT -------------------------------------*/
#include "GLUT/glut.h"//include GLUT on MAC
//#include "include\freeglut.h"//include freeglut on Windows
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <ctime>
/*------------------------ CONSTANTS DECLARATION ---------------------------------*/
#define X 0.0 //initial ship x coordinate used to centre ship
#define Y 0.0 //initial ship y coordinate used to centre ship
#define RADIUS_SHIP 1.5 //ship radius
#define PI 3.14159  //Mathematical constant used for cos and sin functions

using namespace std;

/*------------------------ GLOBAL VARIABLES DECLARATION ---------------------------------*/
GLfloat red = 1.0, green = 1.0, blue = 1.0;
GLfloat angle = 2.0 * PI;//used for asteroid and bullet drawing
GLfloat angleShip = 90.0;//used for rotating the ship
GLfloat angleBullet=0.0;//recalculated angleShip to be used for bullet launching
GLfloat angleAster = 0.0;//angle that is constantly updated by angleSpintTimer() method, so that the asteroid can rotate
GLfloat xMax = 10;//max and min coordinates used as a border for bullet when it reaches the end of the window
GLfloat yMax = 10;
GLfloat xMin = -10.0;
GLfloat yMin = -10.0;
GLfloat xBullet = 0.0;//contains current x coordinate of bullet which is updated in calcBulletX()
GLfloat yBullet = 0.0;//contains current y coordinate of bullet which is updated in calcBulletY()
GLfloat radiusBullet = 0.2;//the radius of the bullet

GLint score = 0;//keeps track of score
GLint numAsteroid = 0;//the number which contains the current count of asteroids on the screen, when zero, new selection of asteroids is redrawn

GLboolean grid = true;//shows grid when right click of the mouse is pressed and menu option 2 selected.
GLboolean bulletLaunch = false;//launches bullet upon a press of the space bar.

vector<GLfloat> vecAster;//asteroid x and y coordinates, generated in locationAster() by calling randomNumberGen() and collisionDet() to avoid clashing
vector<GLfloat> vecRadius;//radius coordinates for each asteroid, generated in radiusAster() method
vector<GLfloat> vecLines; //number of lines for each asteroid shape, generated in numLinesAster() method
vector<GLfloat> vecColour;//colours generated for each asteroid by colourAster() method, which calls randomColour() to achieve it

GLfloat currentDirection[2];//keeps the current values of ship direction, used for pointing bullets

/*------------------------ METHOD LIST ---------------------------------*/

void drawShipSequence();
void drawShip(GLfloat radius);
void drawAsterSeq();
void drawAsteroid(GLint j);
void locationAster();
GLfloat randomNumberGen(int min, int max);
GLboolean collisionDet(GLfloat x2, GLfloat x1, GLfloat y2, GLfloat y1, GLfloat r2, GLfloat r1);
void radiusAster();
void numLinesAster();
void colourAster();
GLfloat randomColour();
void asterSpinTimer(int value);
void deleteAster(GLint g);
void drawBullet();
void bulletCollision();
void calcBulletX();
void calcBulletY();
void keyboard(GLint key , GLint x, GLint y );
void createMenu(GLint id);
void printScore(string score, string result);
void drawGrid();
void drawPattern();

/*------------------------ METHOD DECLARATIONS ---------------------------------*/

/*------------------------ Reshape Function ------------------------------------*/
void reshape(GLint width, GLint height)
{
    if(height == 0) height = 1;
    GLfloat aspect = (GLfloat)width/(GLfloat)height;
    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, width, height);
    glLoadIdentity();
    //glOrtho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
    if (width <= height){//changes aspect ratio depending on which way the window is being resized
        glOrtho(-10.0, 10.0, -10.0/aspect, 10.0/aspect, -1.0, 1.0);//when width is smaller than height
    }
    else{
        glOrtho(-10.0*aspect, 10.0*aspect, -10.0, 10.0 , -1.0, 1.0);//when height is smaller than width
    }
    glMatrixMode(GL_MODELVIEW);
}
/*------------------------ Display Function ---------------------------------*/
void display(void)
{
    srand((unsigned int)(time(NULL)));//allows rand() to give different values at consequent uses.
    glClear(GL_COLOR_BUFFER_BIT);     //clears window
    
    glLoadIdentity();
    
    
    if (!grid)//Grid activates:
    {
        drawGrid();
    }
    else{//displays pattern
        drawPattern();
    }
    //generates asteroid field when empty and reinitialises veriables and vector arrays:
    if(numAsteroid==0){//runs only when the field has been cleared of asteroids
        vecAster.clear();
        vecRadius.clear();
        vecLines.clear();
        vecColour.clear();
        angleAster = 0.0;
        
        numAsteroid = 7;
        radiusAster();
        locationAster();
        colourAster();
        numLinesAster();
        
        drawAsterSeq();
        
        glutTimerFunc(0, asterSpinTimer, 1);//sets rotational motion of asteroids when they are created
    }
    else{//runs while field is full with asteroids
        drawAsterSeq();
    }
    
    if(bulletLaunch){//when space bar is pressed
        drawBullet();
    }
    
    printScore("Score: ", to_string(score)); //printing current score
    
    drawShipSequence();//draws ship
    glutPostRedisplay();
}
/*------------------------ Draw Grid Function ---------------------------------*/
void drawGrid(){
    
    glEnable(GL_BLEND);//switches on the blending option
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);//function that blends the incoming source and output
    
    glLineStipple(3, 0xAAAA); //creates a template for a stippled line use
    
    glEnable(GL_LINE_STIPPLE);//enables stippled line
    glBegin(GL_LINES);
    
    glColor3f(0.0, 1.0, 1.0);
    
    GLint numLines = 22;
    
    for(GLint i = 0; i<numLines; i+=2){ //vertical printing
        
        glVertex2f(xMin+i, yMin);
        glVertex2f(xMin+i, yMax);
    }
   
    for(GLint k = 0; k<numLines; k+=2){  //horizontal printing
        
        glVertex2f(xMin, yMin+k);
        glVertex2f(xMax, yMin+k);
    }
    glEnd();
    
    glDisable(GL_LINE_STIPPLE);//disables stippled line pattern
    glDisable(GL_BLEND);//prevents blending to be applied to items that are drawn after, such as asteroids and ship
    glutPostRedisplay();
}
/*------------------------ Draw Pattern Function ---------------------------------*/
void drawPattern(){//uses hexadecimal values to determine which pixels to write on,
        //e.g 1C(hexa) will convert to 0001 1100(binary) which means that 3 pixels will be drawn on, etc
        GLubyte peaceY[] = {
        0x00, 0x00, 0x1C, 0x01, 0xC0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x02, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x02, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x1C, 0x01, 0xC0, 0x00, 0x00, 0x00};
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
    glColor3f(0.0, 1.0, 1.0);
    glEnable(GL_POLYGON_STIPPLE);
    glRectf(xMin, yMax, xMax, yMin);//creates a rectangle in which the pattern is then displayed
    glPolygonStipple(peaceY);//displays the above pattern
    glDisable(GL_POLYGON_STIPPLE);
    glDisable(GL_BLEND);
    
    glutPostRedisplay();
    
}
/*------------------------ Ship Initialisation Sequence ---------------------------------*/
void drawShipSequence(){
    
    glPushMatrix();
    glRotatef(angleShip, 0.0, 0.0, 1.0);//rotates the ship on the z axis
    drawShip(RADIUS_SHIP);
    glPopMatrix();
    glutPostRedisplay();
    
}
/*------------------------ Draw Ship Function ---------------------------------*/
void drawShip(GLfloat radius){
    
    GLfloat x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7, x8, y8;
    x1 = cos(0 * PI / 180) * radius;
    y1 = sin(0 * PI / 180) * radius;
    x2 = cos(90 * PI / 180) * radius;
    y2 = sin(90 * PI / 180) * radius;
    x3 = cos(135 * PI / 180) * radius;
    y3 = sin(135 * PI / 180) * radius;
    x4 = cos(90 * PI / 180) * (radius/2);
    y4 = sin(90 * PI / 180) * (radius/2);
    x5 = cos(180 * PI / 180) * (radius/2);
    y5 = sin(180 * PI / 180) * (radius/2);
    x6 = cos(270 * PI / 180) * (radius/2);
    y6 = sin(270 * PI / 180) * (radius/2);
    x7 = cos(225 * PI / 180) * radius;
    y7 = sin(225 * PI / 180) * radius;
    x8 = cos(270 * PI / 180) * radius;
    y8 = sin(270 * PI / 180) * radius;
    
    glBegin(GL_POLYGON);
    
    glColor3f(0.3, 0.9, 0.9);
    glVertex2f(x1, y1);
    glColor3f(0.1, 0.6, 0.9);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
    glVertex2f(x5, y5);
    glVertex2f(x6, y6);
    glVertex2f(x7, y7);
    glVertex2f(x8, y8);
    
    glEnd();
    glFlush();
}
/*------------------------ Asteroid Creation Sequence ---------------------------------*/
void drawAsterSeq(){
    GLfloat  x, y;
    
    for(GLint j = 0; j<numAsteroid; j++){
        
        x = vecAster[j*2];
        y = vecAster[j*2+1];
        glPushMatrix();
        glTranslatef(x, y, 0.0);            //then translates it to the random position from vecAster.
        glRotatef(angleAster, 0.0, 0.0, 1.0);//then rotates it ^
        glTranslatef(0,0,0);//moves the asteroid to the centre ^
        drawAsteroid(j);
        glPopMatrix();
    }
    glutPostRedisplay();
}
/*------------------------ Drawing Individual Asteroids Function ---------------------------------*/
void drawAsteroid(GLint j)
{
    
    GLfloat radius, x1, y1;
    radius = vecRadius[j];
    
    glColor3f(vecColour[3*j], vecColour[3*j+1], vecColour[3*j+2]);
    glBegin(GL_POLYGON);
    
    //draws polygon depending on the number of lines passed in vecLines[j]
    for(GLint i = 0; i<vecLines[j]; i++){
        
        GLfloat theta = angle * (GLfloat)i / (GLfloat)vecLines[j];
        x1 = radius*cos(theta);
        y1 = radius*sin(theta);
        
        glVertex2f(x1, y1);
    }
    
    glEnd();
}
/*------------------------ Drawing Bullet Function ---------------------------------*/
void drawBullet(){
    
    glPushMatrix();
    GLfloat x1, y1;
    glColor3f(1.0, 0.5, 0.6);
    glLineWidth(1.0);
    
    //adds translation with changeable values so that the bullet can move.
    glTranslatef(xBullet, yBullet, 0);//moves bullet along xBullet yBullet coordinates which are updated in calcBulletX() and calcBulletY()
   
    glRotatef(angleShip, 0, 0, 1.0);//rotates the bullet in the direction that the ship takes.
    
    glBegin(GL_POLYGON);
    
    for(GLint i = 0; i<3; i++){//draws bullet
        
        GLfloat theta;
        
        if(i==0){
            glColor3f(1.0, 1.0, 1.0);
            theta = (angle) * (GLfloat)i / (GLfloat)3;
        }
        else if (i==1){
            glColor3f(1.0, 0.5, 0.6);
            theta = (angle) * (GLfloat)i / (GLfloat)3;
        }
        else{
            glColor3f(1.0, 0.5, 0.6);
            theta = angle*(GLfloat)i / (GLfloat)3;
        }
        x1 = radiusBullet*cos(theta);
        y1 = radiusBullet*sin(theta);
        glVertex2f(x1, y1);
    }
    
    glEnd();
    
    bulletCollision();//checks if bullet collides with any asteroids, if not then it stops it at the min/max borders.
    
    glPopMatrix();
    glutPostRedisplay();
}
/*--------------------------------------------------------------------------------------------------*/
/*------------------------ Asteroid Coordinates Generator Function ---------------------------------*/
void locationAster(){
    GLfloat x1,y1,x2, y2, rOld, rNew;
    
    GLboolean flag = true;//marks the first time we will be adding to the array.
    GLboolean flag2;
    
    for(GLint k = 0; k<numAsteroid;k++){
        if(k == -1){//checks in case k has become negative, to avoid going out of range
            k = 0;
        }
        flag2 = true;
        
        x2 = randomNumberGen(-7, 7);//generates a random number between -7 and 7.
        y2 = randomNumberGen(-7, 7);
        
        if(!collisionDet(x2, X, y2, Y, vecRadius[k], (RADIUS_SHIP+1))){//checks if the coordinates of x2, y2 do not clash with these of the ship.
            if(flag){//first time around of adding coordinates
                
                vecAster.push_back(x2);//add coordinate for x location of x
                vecAster.push_back(y2);//add coordinate for y location of y
                
                flag=false;//this will stay false until it is again time to fill in the array from empty
            }
            else{//runs when there already is one element in the vecAster array.
                for(GLint j = 0; j<k; j++){
                    
                    x1 = vecAster[2*j];
                    y1 = vecAster[2*j+1];
                    rOld = vecRadius[j];
                    rNew = vecRadius[j+1];
                    
                    if(collisionDet(x2, x1, y2, y1, rNew, (rOld+2))){//checks if asteroids would clash with each other
                        k--;
                        flag2=false;//if there is a clash this flag allows to break out of the for loop without adding the values to the list.
                        break;
                    }
                }
                if(flag2){//adds elements to vecAster. Only runs if the collision from above did not find any clashes.
                    vecAster.push_back(x2);
                    vecAster.push_back(y2);
                }
            }
        }
        else{
            k--;//takes 1 from k when velues for x2,y2 are found that clash with the ship
        }
    }
}
/*------------------------ Colour Generator Function ---------------------------------*/
GLfloat randomColour()
{
    return rand()/((GLfloat)RAND_MAX+1);//generates a number between 0.0 and 1.0
}
/*------------------------ Colour Vector Filler Function ---------------------------------*/
void colourAster(){//adds a colour for each asteroid in the game
    
    for(GLint i = 0; i<numAsteroid*3 ; i++){
        
        vecColour.push_back(randomColour());
    }
}
/*------------------------ Asteroid Radius Generator ---------------------------------*/
void radiusAster(){//adds a radius for each asteroid in the game
    GLfloat radius;
    for(GLint k = 0; k<numAsteroid;k++){
        radius = (GLfloat)(rand()%2+0.7);//generates a number between 0.7 and 1.7 to be set as radius of asteroid
        vecRadius.push_back(radius);//adds to the back of the vector list
    }
}
/*------------------------ Asteroid Shape Generator Function ---------------------------------*/
void numLinesAster(){
    GLint numLines;
    for(GLint i = 0; i < numAsteroid; i++){
        
        numLines = randomNumberGen(5, 8);//generates between 5 and 8, which would determine the shape of the asteroid
        vecLines.push_back(numLines);
    }
}
/*------------------------ Random Generator for Asteroid Vertices ---------------------------------*/
GLfloat randomNumberGen(GLint min, GLint max)
{
    return min + (GLint)((max - min + 1) * (rand() * (1.0 / (RAND_MAX + 1.0))));//generates a random number between 2 points
}
/*------------------------ Collision Detection Function ---------------------------------*/
GLboolean collisionDet(GLfloat x2, GLfloat x1, GLfloat y2, GLfloat y1, GLfloat r2, GLfloat r1){
    
    if((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)<=(r1+r2)*(r1+r2)){//circle collision detection formula
        return true;
    }
    else{
        return false;
    }
}
/*------------------------ Bullet Collision Calculator Function ---------------------------------*/
void bulletCollision()
{
    
    for(GLint i = 0; i<numAsteroid; i++){
        
        if(collisionDet(xBullet, vecAster[2*i], yBullet, vecAster[2*i+1], radiusBullet, vecRadius[i])){//if a bullet collides with an object
            
            deleteAster(i);//calls deletion method
            numAsteroid--;//decreases number of asteroids
            score +=10;//increases score
            bulletLaunch = false;//removes the current bullet
            glutTimerFunc(0, asterSpinTimer, 1);//resets the asteroid spinner
            
            break;
        }
        else if(xBullet>xMax || xBullet<xMin || yBullet<yMin || yBullet>yMax){//checks whether the bullet has reached the min/max boundaries
            bulletLaunch = false;
        }
    }
}
/*------------------------ Delete Asteroid Function ---------------------------------*/
void deleteAster(GLint g){

    if(g<vecAster.size()){
        
        vecAster.erase(vecAster.begin()+(2*g+1));
        vecAster.erase(vecAster.begin()+(2*g));
        vecLines.erase(vecLines.begin()+g);
        vecColour.erase(vecColour.begin()+(3*g+2));
        vecColour.erase(vecColour.begin()+(3*g+1));
        vecColour.erase(vecColour.begin()+(3*g));
        vecRadius.erase(vecRadius.begin()+g);
    }
}
/*------------------------ Timer for Asteroid Spinning Function ---------------------------------*/
void asterSpinTimer(int value)
{
    glClear(GL_COLOR_BUFFER_BIT);
    //angle by which the asteroids spin
    angleAster = angleAster + 5.0;
    if (angleAster>360)angleAster = 0;
    
    glutPostRedisplay();
    glutTimerFunc(0, asterSpinTimer, 1);//calls TimerFunction on tick - callback
}
/*------------------------ Bullet Movement Generator ---------------------------------*/
void bulletShooter(int value){//allows bullet to follow initial x,y trajectory and move forward
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    xBullet += cos(angleBullet)*value;
    yBullet += sin(angleBullet)*value;
    
    glutPostRedisplay();
    glutTimerFunc(5, bulletShooter, 1.8);
 
}
/*------------------------ Current X of Ship ---------------------------------*/
void calcBulletX(){//calculates current x coordinate of ship pointer
    
    currentDirection[0] = cos((angleShip)*PI/180)*RADIUS_SHIP;
}
/*------------------------ Current Y of Ship ---------------------------------*/
void calcBulletY(){//calculates current y coordinate of ship pointer
    
    currentDirection[1] = sin((angleShip)*PI/180)*RADIUS_SHIP;
}
/*------------------------ Keyboard Commands Function ---------------------------------*/
void keyboard(GLint key , GLint x, GLint y )
{
    switch (key)
    {
        case GLUT_KEY_LEFT://left key to move ship left
            
            angleShip = angleShip + 10;
            if(angleShip>=360){
                angleShip = 0;
            }
            calcBulletX();
            calcBulletY();
            
            glutPostRedisplay();
            
            break;
            
        case GLUT_KEY_RIGHT://right key to move ship right
            
            angleShip = angleShip - 5;
            if(angleShip>=360){
                angleShip = 0;
            }
            calcBulletX();
            calcBulletY();
            
            glutPostRedisplay();
            break;
            
        case 32://spacebar ascii code
       
            angleBullet = angleShip*PI/180;
            xBullet = currentDirection[0];
            yBullet = currentDirection[1];
            bulletLaunch = true;
            
            glutTimerFunc(5, bulletShooter, 1.8);
            glutPostRedisplay();
            break;
            
        case 27: // Escape key to exit
            exit (0);
            break;
    }
}
/*------------------------ Right Click Menu Function ---------------------------------*/
void createMenu(GLint id)
{
    if (id == 1)
    {
        grid = 0.0;//sets grid to false initially
    }
    else
    {
        grid = 1.0;//when grid is selected from menu, it makes it true and shows grid.
    }
    
    glutPostRedisplay();
}
/*------------------------ Display Score Function ---------------------------------*/
void printScore(string score, string result) //function used to display score
{
    glColor3f(1.0, 1.0, 0.0);
    glRasterPos2f(-9, 9);
    
    for(int j = 0; j<score.length();j++){
        
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,score[j]);//prints all characters in the word score
    }
    for (int i = 0; i < result.length(); i++){
        
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,result[i]);//prints all characters in result
    }
    glutPostRedisplay();
}
/*------------------------ Initialisation Function ---------------------------------*/
void init(void){
    glClearColor(0.4, 0.3, 0.9, 0.3);
}
/*------------------------ MAIN ---------------------------------*/
int main(int argc, char** argv)
{
    //Prints User Directions in console:
    cout<<"How To Play Asteroids:"<<endl<<endl
    <<"1. Press Left and Right Arrows To Turn"<<endl
    <<"2. Press Press Space Bar To Shoot"<<endl
    <<"3. Right Click for Menu Options"<<endl
    <<"4. Press Escape Button to Quit"<<endl;
    //initialisation and window setup
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(500, 500);
    //centers the game to the centre of the screen:
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-500)/2,(glutGet(GLUT_SCREEN_HEIGHT)-500)/2);
    glutCreateWindow("Asteroids");//creates a window for asteroids game
    init();//method that sets the background colour.
    glutSpecialFunc(keyboard);//sets up keyboard input
    glutDisplayFunc(display);//makes a call to display function, where all items are drawn.
    glutCreateMenu(createMenu); //create right click menu:
    glutAddMenuEntry("grid on", 1);
    glutAddMenuEntry("pattern on", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    glutReshapeFunc(reshape);//calls reshape() that organises screen dimensions and allows for screen resizing.
    
    glutMainLoop();
    return 0;
}
