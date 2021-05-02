#include <glad.h>
#include <glfw3.h>
#include <stdio.h>ç
#include <windows.h>
#include <iostream>
#include "esfera.h"
#include "lecturaShader_0_9.h"
//Movimiento
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ARADIANES 0.0174

void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 720;
unsigned int SCR_HEIGHT = 720;

extern float vertices_esfera[];
extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

//VAO
unsigned int VAO;
unsigned int VAOEsfera;
unsigned int VAOCubo;
unsigned int VAOCuadrado;
unsigned int VAOCuboIn;
//Angulo plano
float angulo;
//Variables para el moviminto de la grua
float velocidad;
float ang_giro;
int camara = 1;
float grados = 25.0;
glm::mat4 transform;
glm::mat4 transformtemp;
int fondo;

unsigned int texturaActual = 0;
unsigned int texturaPiscina[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned int texturaSuelo = 0;
unsigned int texturaObra = 0;

typedef struct {
	float px, py, pz;			//posicion inicial
	float angulo_trans;			//angulo giro x
	float angulo_trans_2;		//angulo giro z
	float sx, sy, sz;			//escalado en los ejes
	unsigned int listarender;	//VAO
	float colx, coly, colz;     //Color
	int articulacion;
	unsigned int textura;
	int isbase;
}objeto;

objeto base =   { 0 ,0 ,0.15 ,0 ,0 ,.3  ,.2  ,.2  ,0 ,1.0 ,1.0 ,0.0, 0, 0, 1 };
objeto baseA1 = { 0 ,0 ,0.10 ,0 ,0 ,.07 ,.07 ,.07 ,0 ,0.0 ,1.0 ,1.0, 1, 0, 0 };
objeto base1 =  { 0 ,0 ,0.10 ,0 ,0 ,.05 ,.05 ,.3  ,0 ,1.0 ,1.0 ,0.0, 0, 0, 0 };
objeto baseA2 = { 0 ,0 ,0.15 ,0 ,0 ,.05 ,.05 ,.05 ,0 ,0.0 ,1.0 ,1.0, 1, 0, 0 };
objeto base2 =  { 0 ,0 ,0.11 ,0 ,0 ,.05 ,.05 ,.3  ,0 ,1.0 ,1.0 ,0.0, 0, 0, 0 };
objeto foco =   { 0 ,0 ,0.25 ,0 ,0 ,.05 ,.05 ,.05 ,0 ,0.0 ,1.0 ,0.0, 0, 0, 0 };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
 
void myCamara() {

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);

	glm::mat4 projection;
	projection = glm::mat4();
	projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 5.0f);
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void terceraPersona(float posx, float posy, float posz, float angulo_base) {
	angulo = 0;
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		glm::vec3(posx - 0.5 * cos(angulo_base * ARADIANES), posy - 0.5 * sin(angulo_base * ARADIANES), posz + 0.4),
		glm::vec3(posx + 10 * cos(angulo_base * ARADIANES), posy + 10 * sin(angulo_base * ARADIANES), posz),
		glm::vec3(0.0, 0.0, 1.0));
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, posx - 0.5 * cos(angulo_base * ARADIANES), posy - 0.5 * sin(angulo_base * ARADIANES), posz + 0.4);

	glm::mat4 projection;
	projection = glm::mat4();
	projection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5.0f);
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void primeraPersona(float posx, float posy, float posz, float angulo_base) {
	angulo = 0;
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		glm::vec3(posx + 0.5 * cos(angulo_base * ARADIANES), posy + 0.5 * sin(angulo_base * ARADIANES), posz + 0.4),
		glm::vec3(posx + 10 * cos(angulo_base * ARADIANES), posy + 10 * sin(angulo_base * ARADIANES), posz),
		glm::vec3(0.0, 0.0, 1.0));
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, posx + 0.5 * cos(angulo_base * ARADIANES), posy + 0.5 * sin(angulo_base * ARADIANES), posz + 0.4);

	glm::mat4 projection;
	projection = glm::mat4();
	projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5.0f);
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

//Ejes
void dibujaEjes() {
	unsigned int VBO, EBO;
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		//Vertices          //Colores
		0.0f, 0.0f, 0.0f,	 1.0f, 1.0f, 1.0f,  // 0
		.5f, 0.0f, 0.0f,	 1.0f, 0.0f, 0.0f, //x
		0.0f, .5f, 0.0f,	 0.0f, 1.0f, 0.0f,// y
		0.0f, .5f, 0.0f,	 0.0f, 0.0f, 1.0f, // z  
		.5f , .5f, 0.5f,	1.0f, 1.0f, 1.0f // 1,1,1 bueno realmente la mitad
	};
	unsigned int indices[] = {  // empieza desde cero
		0, 1,
		0, 2,
		0, 3,
		0, 4
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position Color

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

}

//Dibujamos la esfera
void dibujaEsfera() {
	unsigned int VBO;

	glGenVertexArrays(1, &VAOEsfera);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAOEsfera);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	//Normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	//Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}

//Dibujamos el cuadrado
void dibujaCuadrado() {
	unsigned int VBO, EBO;

	
	float vertices[] = {
		 0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
	    -0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,

		-0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
		 0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		 0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
	};


	glGenVertexArrays(1, &VAOCuadrado);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCuadrado);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

}

int dibujaCubo() {
	unsigned int VBO, EBO;


	float vertices[] = {
		//CARA DELANTERA
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		 0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,

		 0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,

		//CARA TRASERA
		 0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		 0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,

		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		 0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,

		//CARA DERECHA
		 0.5, -0.5, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		 0.5, -0.5, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		 0.5, 0.5, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,

		 0.5, 0.5, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		 0.5, 0.5, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		 0.5, -0.5, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,

		//CARA IZQUIERDA
		-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,

		-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,

		//CARA SUPERIOR
		 0.5, -0.5, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		-0.5, 0.5, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,

		-0.5, 0.5, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
		 0.5, 0.5, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		 0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,

		//CARA INFERIOR
		-0.5f,-0.5f,-0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
		 0.5f,-0.5f,-0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
		 0.5f,0.5f,-0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,

		 0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		-0.5f,0.5f,-0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		-0.5f,-0.5f,-0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
	};

	glGenVertexArrays(1, &VAOCubo);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCubo);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//position Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return(VAOCubo);

}

int dibujaCuboInvertido() {
	unsigned int VBO, EBO;


	float vertices[] = {
		//CARA DELANTERA
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		 0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		 0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,

		 0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		-0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,

		//CARA TRASERA
		 0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,		1.0f, 1.0f,

		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		 0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,

		//CARA DERECHA
		 0.5, -0.5, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		 0.5, -0.5, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		 0.5, 0.5, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,

		 0.5, 0.5, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		 0.5, 0.5, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		 0.5, -0.5, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,

		 //CARA IZQUIERDA
		 -0.5f, -0.5f, 0.5f,	-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		 -0.5f, 0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,

		 -0.5f, 0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		 -0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		 -0.5f, -0.5f, 0.5f,	-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,

		 /*
		 //CARA SUPERIOR
		  0.5, -0.5, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		 -0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		 -0.5, 0.5, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,

		 -0.5, 0.5, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
		  0.5, 0.5, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		  0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		  */

		  //CARA INFERIOR
		  -0.5f,-0.5f,-0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
		   0.5f,-0.5f,-0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
		   0.5f, 0.5f,-0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f,

		   0.5f, 0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		  -0.5f,0.5f,-0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		  -0.5f,-0.5f,-0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
	};


	glGenVertexArrays(1, &VAOCuboIn);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCuboIn);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//position Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return(VAOCubo);

}

void openGlInit() {
	//Habilito aqui el depth en vez de arriba para los usuarios de linux y mac mejor arriba
	//Incializaciones varias
	glClearDepth(1.0f); //Valor z-buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // valor limpieza buffer color
	glEnable(GL_DEPTH_TEST); // z-buffer
	//glEnable(GL_CULL_FACE); //ocultacion caras back
	glEnable(GL_TEXTURE_2D);
	glCullFace(GL_BACK);

}

glm::vec3 nuevaPos(glm::mat4 transfm) {

	return glm::vec3(transfm * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

//Cargamos las texturas
int Carga_Texturas(const char* nombre) {

	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	unsigned char* data = stbi_load(nombre, &width, &height, &nrChannels, 0);
	std::cout << nrChannels << std::endl;

	if (data) {
		int format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 2)
			format = GL_RG;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
			
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	}

	else texture = 0;

	stbi_image_free(data);

	return texture;
}

//Secuencia de frames
void frames() {

	double tiempo = glfwGetTime();
	tiempo *= 100;
	int t = (int)tiempo;
	//cada segundo
	if (t % 100 == 0)
	{
		for (int i = 0; i < 16; i++)
		{
			if (texturaActual == texturaPiscina[i])
			{
				if (texturaActual != texturaPiscina[15])
				{
					texturaActual = texturaPiscina[i + 1];
				}
				else
				{
					texturaActual = texturaPiscina[0];
				}
				break;
			}
		}
	}
}

//Inicializamos las texturas
void inicializarTexturas() {
	
	texturaPiscina[0] = Carga_Texturas("../Textures/caust00.png");
	texturaPiscina[1] = Carga_Texturas("../Textures/caust01.png");
	texturaPiscina[2] = Carga_Texturas("../Textures/caust02.png");
	texturaPiscina[3] = Carga_Texturas("../Textures/caust03.png");
	texturaPiscina[4] = Carga_Texturas("../Textures/caust04.png");
	texturaPiscina[5] = Carga_Texturas("../Textures/caust05.png");
	texturaPiscina[7] = Carga_Texturas("../Textures/caust06.png");
	texturaPiscina[6] = Carga_Texturas("../Textures/caust07.png");
	texturaPiscina[8] = Carga_Texturas("../Textures/caust08.png");
	texturaPiscina[9] = Carga_Texturas("../Textures/caust09.png");
	texturaPiscina[10] = Carga_Texturas("../Textures/caust10.png");
	texturaPiscina[11] = Carga_Texturas("../Textures/caust11.png");
	texturaPiscina[12] = Carga_Texturas("../Textures/caust12.png");
	texturaPiscina[13] = Carga_Texturas("../Textures/caust13.png");
	texturaPiscina[14] = Carga_Texturas("../Textures/caust14.png");
	texturaPiscina[15] = Carga_Texturas("../Textures/caust15.png");
	
	texturaSuelo = Carga_Texturas("../Textures/hierba.jpg");
	texturaObra = Carga_Texturas("../Textures/Obra.jpg");
	base.textura = base1.textura = base2.textura = Carga_Texturas("../Textures/brazos.jpg");
	baseA1.textura = baseA2.textura = foco.textura = Carga_Texturas("../Textures/metal.jpg");
	texturaActual = texturaPiscina[0];

}

//Función que dibuja la piscina
void dibujaPiscina(int transformLoc) {
	unsigned int colorLocSuelo = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLocSuelo, 0.0, 0.0, 1.0);
	//PISCINA
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glActiveTexture(GL_TEXTURE0);
	frames();
	glBindTexture(GL_TEXTURE_2D, texturaActual);
	glm::mat4 transform1;
	transform1 = glm::mat4();
	transform1 = glm::translate(transform1, glm::vec3(-1.0f, 0.0f, -0.44f));
	transform1 = glm::scale(transform1, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform1));
	
	glBindVertexArray(VAOCuadrado);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//Función que dibuja el suelo
void dibujasuelo(int transformLoc) {
	//Color suelo
	unsigned int colorLocSuelo = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLocSuelo, 0.5, 0.5, 0.5);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturaSuelo);

	float i, j;
	float escalasuelo = 10;

	for (i = -2; i <= 2; i += (1 / escalasuelo)) {
		for (j = -2; j <= 2; j += (1 / escalasuelo)) {
			transform = glm::mat4();
			transform = glm::rotate(transform, (float)(angulo * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
			transform = glm::translate(transform, glm::vec3(i, j, 0.0f));
			transform = glm::scale(transform, glm::vec3((1 / escalasuelo), (1 / escalasuelo), (1 / escalasuelo)));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
			glBindVertexArray(VAOCuadrado);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}

//Dibujamos la base de la grua
void dibujabase(int transformLoc, int colorLoBase) {
	//glUniform3f(colorLoBase, base.colx, base.coly, base.colz);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, base.textura);

	transform = glm::mat4();
	transform = glm::rotate(transform, (float)(angulo * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::translate(transform, glm::vec3(base.px, base.py, base.pz));
	transform = glm::rotate(transform, (float)(base.angulo_trans * ARADIANES), glm::vec3(0.0f, 0.0f, 1.0f));
	transformtemp = transform;
	transform = glm::scale(transform, glm::vec3(base.sx, base.sy, base.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(base.listarender);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

//Dibujamos las articulaciones de la grua
void dibujaArtBra(objeto parte, int transformLoc, int colorLoBase) {

	//glUniform3f(colorLoBase, parte.colx, parte.coly, parte.colz);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, parte.textura);

	transform = glm::mat4();
	transform = transformtemp;
	transform = glm::translate(transform, glm::vec3(parte.px, parte.py, parte.pz));
	if (parte.articulacion == 1) {
		transform = glm::rotate(transform, (float)(parte.angulo_trans * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, (float)(parte.angulo_trans_2 * ARADIANES), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	transformtemp = transform;
	transform = glm::scale(transform, glm::vec3(parte.sx, parte.sy, parte.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(parte.listarender);
	if (parte.articulacion == 1) {
		glDrawArrays(GL_TRIANGLES, 0, 1080);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

//Dibujamos el foco de iluinación
void dibujaFoco(int transformLoc) {
	//Foco
	transform = glm::mat4(); // identity matrix
	transform = transformtemp;
	transform = glm::translate(transform, glm::vec3(foco.px, foco.py, foco.pz));
	transformtemp = transform;
	transform = glm::scale(transform, glm::vec3(foco.sx, foco.sy, foco.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
}

int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creo la ventana													
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "'Z,X,C'->Cambiar vista, 'L,K'-> mover la camara, 'T,F,G,H'-> mover brazo arriba, 'A,D,S,W'-> mover brazo abajo", NULL, NULL);
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	glfwSetKeyCallback(window, keyCallback);
	openGlInit();

	//generarShader();
	shaderProgram = setShaders("shader.vert", "shader.frag");

	glUniform1i(glGetUniformLocation(shaderProgram, "textura1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "textura2"), 1);

	//dibujaEjes();
	dibujaCuadrado();
	dibujaCubo();
	dibujaCuboInvertido();
	dibujaEsfera();
	
	base.listarender = base1.listarender = base2.listarender = fondo = VAOCubo;
	baseA1.listarender = baseA2.listarender = VAOEsfera;
	fondo = VAOCuboIn;

	inicializarTexturas();
	// Lazo de la ventana mientras no la cierre
	glUseProgram(shaderProgram);
	// -----------
	while (!glfwWindowShouldClose(window)){
		// input
		// -----
		processInput(window);
		// render
		// ------
		glClearColor(0.0f, 0.5f, 1.0f, 1.0f); //Borro el Buffer the la ventana
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Diferentes tipos de camara
		if (camara == 1) {
			myCamara();
		}
		if (camara == 2) {
			terceraPersona(base.px, base.py, base.pz, base.angulo_trans);
		}
		if (camara == 3) {
			primeraPersona(base.px, base.py, base.pz, base.angulo_trans);
		}

		//Dibujamos en GL_FILL
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		//Transformaciones
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		//Luz
		unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		

		//Dibujamos el suelo
		dibujasuelo(transformLoc);

		//Movimiento de la base con aceleración
		base.px += velocidad * cos(base.angulo_trans * ARADIANES);
		base.py += velocidad * sin(base.angulo_trans * ARADIANES);

		//Color para la base
		unsigned int colorLoGrua = glGetUniformLocation(shaderProgram, "objectColor");

		//Base
		dibujabase(transformLoc, colorLoGrua);
		//Primera articulación
		dibujaArtBra(baseA1, transformLoc, colorLoGrua);
		//Primer brazo
		dibujaArtBra(base1, transformLoc, colorLoGrua);
		//Segunda articulación
		dibujaArtBra(baseA2, transformLoc, colorLoGrua);
		//Segundo brazo
		dibujaArtBra(base2, transformLoc, colorLoGrua);
		//Dibujamos el foco
		dibujaFoco(transformLoc);

		//Fondo
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaObra);
		glDisable(GL_TEXTURE_2D);
		glm::mat4 transform1;
		transform1 = glm::mat4();
		transform1 = glm::translate(transform1, glm::vec3(0.0f, 0.0f, 0.0f));
		transform1 = glm::scale(transform1, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform1));
		glBindVertexArray(fondo);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glEnable(GL_TEXTURE_2D);

		//Dibujamos la piscina
		dibujaPiscina(transformLoc);

		unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		glm::vec3 pos_linterna = nuevaPos(transform);
		glUniform3f(lightPosLoc, pos_linterna.x, pos_linterna.y, pos_linterna.z);

		float spotCutoffLoc = glGetUniformLocation(shaderProgram, "spotCutoff");
		glUniform1f(spotCutoffLoc, grados);

		// glfw: swap 
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

		glDeleteVertexArrays(1, &VAO);
		glfwTerminate();
		return 0;
	}

void processInput(GLFWwindow* window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	//glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::cout << key << std::endl;

	//Primera arti
	if (key == 65 && baseA1.angulo_trans >= -85) { //A
		baseA1.angulo_trans--;
	}
	if (key == 83 && baseA1.angulo_trans_2 >= -90) { //S
		baseA1.angulo_trans_2--;
	}
	if (key == 87 && baseA1.angulo_trans_2 <= 90) { //W
		baseA1.angulo_trans_2++;
	}
	if (key == 68 && baseA1.angulo_trans <= 85) { //D
		baseA1.angulo_trans++;
	}
	//Segunda arti
	if (key == 84) { //T
		baseA2.angulo_trans_2++;
	}
	if (key == 71) { //G
		baseA2.angulo_trans_2--;
	}
	if (key == 70) { //F
		baseA2.angulo_trans--;
	}
	if (key == 72) { //H
		baseA2.angulo_trans++;
	}
	//Plano
	if (key == 75) { //K
		angulo++;
	}
	if (key == 76) { //L
		angulo--;
	}

	//Movimiento de la base (<^>)
	if (key == 262) { //HACIA LA DERECHA
		base.angulo_trans--;
	}
	if (key == 263) { //HACIA LA IZQUIERDA
		base.angulo_trans++;
	}
	if (key == 265) { //HACIA LA DERECHA
		velocidad += 0.001;
	}
	if (key == 264) { //HACIA LA IZQUIERDA
		velocidad -= 0.001;
	}

	//Elegir camara
	//Cámara externa
	if (key == 90) { //Z
		camara = 1;
	}
	//Tercera persona
	if (key == 88) { //X
		camara = 2;
	}
	//Primera persona
	if (key == 67) { //C
		camara = 3;
	}
}
