//Código fonto do jogo Korno's Crawler

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include <iostream>
#include "map.h"
#include "character.h"
#include "monster.h"
#include <iostream>
using namespace std;


/* Declaração de Variáveis Globais */

//Variáveis de definição de ângulo
int angulo1 = 0, angulo2 = 0, angulo3 = 0;
//Variáveis que definem a posição da câmera
int posx, posy, posz;
//Variável que define para onde a câmera olha
int oy, ox, oz;
//Variável que definem qual eixo estará na vertical do monitor
int lx, ly,lz;
double rotation, inclination;
int botao,estado;
//Variável que definee o tipo da câmera
int tipoCam = 1;
double t = 0;

//Variaveis para contagem de FPS
int initial_time = time(NULL), final_time, frame_count = 0;

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif

//Protótipos das funções
void Inicializa();
void Display();
void Mouse(int botao, int estado, int x, int y);
void keyboard (unsigned char key, int x, int y);
void TeclasEspeciais (int key, int x, int y);
void HUD();
void drawHUD();
void timer(int);
void buildFrame();
void createGame();

//Função que movimenta a câmera
void moveCamera(){
   posx = 30*sin(t);
   posz = 30*cos(t);
}

bool enableMouseMovement = false;
void UpdateCameraCenter(bool CheckValue) {
  if(CheckValue) {
    if(rotation >= 360)
      rotation -= 360;
    if(rotation < 0)
      rotation += 360;
    
    if(inclination <= 45)
      inclination = 45;
    if(inclination >= 135)
      inclination = 135;
  }
  
  ox = (sin(DEG_TO_RAD * inclination) * sin(DEG_TO_RAD * rotation) + posx);
  oy = (sin(DEG_TO_RAD * inclination) * cos(DEG_TO_RAD * rotation) + posy);
  oz = (cos(DEG_TO_RAD * inclination) + posz);
}

//Função que ajusta a câmera
void adjustCamera(){
   //Camera 2D --
   if(tipoCam > 0){ 
      enableMouseMovement = false;
      //Define a posição da câmera
      posx = mainChar->charx; //charx
      posy = 9;
      posz = mainChar->charz; //charz

      //Define para onde a lente da câmera estará apontada
      ox = mainChar->charx; //charx
      oy = 0;
      oz = mainChar->charz; //charz

      //Define eixo que vai estar a horizontal no monitor
      lx = 1;
      ly = 0;
      lz = 0;
   }
   //Camera 3D --
   else if(tipoCam < 0){
      posx = mainChar->charx;
      posz = mainChar->charz;
      posy = 2;
      lx = 0;
      ly = 1;
      lz = 0;
      oy = 2;
   }
}

//Função que cria o Frame
void buildFrame() {
   //Constroi o mapa
   buildPhase();

   //Posiciona o jogador
   buildMainChar();

   //Realiza o movimento dos monstros
   moveMonsters();

   //Posiciona os monstros
   buildMonsters();
}

//Função de criação do HUD
void HUD(){
   //glBindTexture(GL_TEXTURE_2D, HUDtex);
   /*glColor3f(1.0, 1.0, 1.0);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,1.0); glVertex2f(0.05, 0.05);
   glVertex2f(1.0, 1.0);  glVertex2f(0.3, 0.05);
   glVertex2f(1.0, 0.0);  glVertex2f(0.3, 0.15);
   glVertex2f(0.0, 0.0);  glVertex2f(0.05, 0.15);
   glEnd();*/
}

void drawHUD(){

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0.0, 1.0, 1.0, 0.0);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   HUD();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
}

void timer(int){
   glutPostRedisplay();
   glutTimerFunc(1000/60,timer,0);
}

//Função que cria as variáveis do jogo
void createGame() {
   //Inicializa (ou cria uma nova) fase com salas e paredes
   createPhase();

   //Cria novos monstros
   createMonsters();
}

//Função qeu configura o display
void Display() {
   
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LINE_SMOOTH);
   glEnable(GL_POLYGON_SMOOTH); 
   glEnable(GL_SMOOTH);
   glEnable(GL_BLEND);
   
   // Inicializa parâmetros de rendering
   // Define a cor de fundo da janela de visualização como preta
   glClearColor(0.0, 0.0, 0.0, 0.0); 
   
   //glMatrixMode() - define qual matriz será alterada
	//SEMPRE defina o tipo de apresentação na matriz PROJECTION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

   if (tipoCam > 0) {
		//Define a projeção como ortogonal
      glOrtho(-10, 10, -10, 10, -10, 10);
	} else {
      //Define a projeção como perspectiva
		gluPerspective(45,1,1,150);

	}
   
   //glMatrixMode() define qual matriz será alterada
	//SEMPRE defina a câmera (ortogonal ou perspectiva) na matriz MODELVIEW (onde o desenho ocorrerá)
	glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   //Chamada para ajustar a câmera de acordo com a visão atual
   adjustCamera();    

   //Define a pos da câmera, para onde olha e qual eixo está na vertical
   gluLookAt(posx, posy, posz, ox, oy, oz, lx, ly, lz);

   //Limpa um buffer particular ou combinações de buffers, onde buffer é uma área de armazenamento para informações da imagem
	//Nesse caso está limpando os buffers para suportarem animações
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   //Chamada para Função que desenha o objeto/cena...
   //----------------------------------------------------------------
   buildFrame();
   //----------------------------------------------------------------   
   //Executa a cena
	//SwapBuffers dá suporte para mais de um buffer, permitindo execução de animações sem cintilações
   drawHUD();
	glutSwapBuffers(); 

   /*frame_count++;
   final_time = time(NULL);
   if(final_time - initial_time > 0){
      cout << "FPS: " << frame_count / (final_time - initial_time)  << endl;
      frame_count = 0;
      initial_time = final_time;
   }
      */
}

void Mouse(int botao, int estado, int rotation, int inclination) {
   //botao - recebe o código do botão pressionado
   //estado - recebe se está pressionado ou não
   //x, y - recebem respectivamente as posições do mouse na tela
   switch (botao) {
      case GLUT_LEFT_BUTTON:
      break;

      case GLUT_RIGHT_BUTTON:
      break;
   }

}

void keyboard (unsigned char key, int x, int y) {
   //Key - recebe o código ASCII da tecla
   //x, y - recebem as posições do mouse na tela (permite tratar os dois dispositivos)
      //Caso seja 2D

      //Passa para int a posição do personagem
      int posX = (int) mainChar->charx;
      int posZ = (int) mainChar->charz;
      switch (key) {
         case 'w':
            if (currentPhase->map[posX + 1][posZ] == 1) {
               if (mainChar->direcaox != 1) {
                  mainChar->direcaox = 1;
                  mainChar->direcaoz = 0;
               } else {
                  mainChar->charx++;   
               }
               if (tipoCam == -1)
               {
                  ox = mainChar->direcaox + mainChar->charx;
                  oz = mainChar->charz + mainChar->direcaoz;
               }
            }
            break;
         case 's':
            if (currentPhase->map[posX - 1][posZ] == 1) {
               
               if (mainChar->direcaox != -1) {
                  mainChar->direcaox = -1;
                  mainChar->direcaoz = 0;
               } else {
                  mainChar->charx--;
               }
               if (tipoCam == -1)
               {
                  ox = mainChar->direcaox + mainChar->charx;
                  oz = mainChar->charz + mainChar->direcaoz;
               }
            }
            break;
         case 'a':
            if (currentPhase->map[posX][posZ - 1] == 1) {
               if (mainChar->direcaoz != -1) {
                  mainChar->direcaox = 0;
                  mainChar->direcaoz = -1;
               } else {
                  mainChar->charz--;
               }
               if (tipoCam == -1)
               {
                  ox = mainChar->direcaox + mainChar->charx;
                  oz = mainChar->charz + mainChar->direcaoz;
               }
            }
            break;
         case 'd':
            if (currentPhase->map[posX][posZ + 1] == 1) {
               if (mainChar->direcaoz != 1) {
                  mainChar->direcaox = 0;
                  mainChar->direcaoz = 1;
               } else {
                  mainChar->charz++;
               }
               if (tipoCam == -1)
               {
                  ox = mainChar->direcaox + mainChar->charx;
                  oz = mainChar->charz + mainChar->direcaoz;
               }
            }
            break;
         case 'p':
            tipoCam *= -1;
            adjustCamera();
            break;
         default:
            break;
      }
   glutPostRedisplay();
}

void TeclasEspeciais (int key, int x, int y) {
   //Key - recebe o código ASCII da tecla
   //x, y - recebem respectivamente as posições mouse na tela (permite tratar os dois dispositivos)
   if (key==GLUT_KEY_RIGHT) {
      posx+=5; ox+=5;
   }
   else if (key==GLUT_KEY_PAGE_UP) {
      posy+=5; oy+=5;
   }
   else if (key==GLUT_KEY_UP) {
      posz-=5; oz-=5;
   }
   else if (key==GLUT_KEY_LEFT) { 
      posx-=5; ox-=5;
   }
   else if (key==GLUT_KEY_PAGE_DOWN) {
      posy-=5; oy-=5;
   }
   else if (key==GLUT_KEY_DOWN) {      
      posz+=5; oz+=5;
   }
   glutPostRedisplay();
}

int main(int argc,char **argv) {
   //Iniatizes glut
	glutInit(&argc, argv);

   //Seed da srand
   srand(time(NULL));
   
   //Define as características do espaço vetorial
	//Nesse caso, permite animações (sem cintilações), cores compostas por (R, G, B)
	//Buffer que permite trabalhar com profundidade e elimina faces escondidas
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

   //Define a janela
   glutInitWindowSize(800, 800);
   glutInitWindowPosition(100, 100);
   glutCreateWindow("Korno's Crawler");

   //Inicializa as variáveis da Phase 1 do jogo
   createGame();

   //Inicializa as variaveis do personagem principal
   createMainChar();

   //Mostra o Mapa
	printMap();

   glutDisplayFunc(Display);
   glutMouseFunc(Mouse);
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(TeclasEspeciais);

   glutTimerFunc(0,timer,0);

   glutMainLoop();
   
   return 0; 
}
