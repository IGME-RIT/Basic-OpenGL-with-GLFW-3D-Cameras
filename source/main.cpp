/*
Title: 3D Cameras
File Name: main.cpp
Copyright � 2016
Author: David Erbelding
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "../header/shape.h"
#include "../header/transform3d.h"
#include "../header/shader.h"
#include <iostream>

Shape* square;

// The transform being used to draw our shape
Transform3D transform;

// These shader objects wrap the functionality of loading and compiling shaders from files.
Shader vertexShader;
Shader fragmentShader;

// GL index for shader program
GLuint shaderProgram;

// Index of the world matrix in the vertex shader.
GLuint worldMatrixUniform;

// Index of the camera matrix in the vertex shader.
GLuint cameraMatrixUniform;

// Here we store the position, of the camera.
Transform3D cameraPosition;

// Store the current dimensions of the viewport.
glm::vec2 viewportDimensions = glm::vec2(800, 600);

// Store the current mouse position.
glm::vec2 mousePosition;

// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
    viewportDimensions = glm::vec2(width, height);
}

// This will get called when the mouse moves.
void mouseMoveCallback(GLFWwindow *window, GLdouble mouseX, GLdouble mouseY)
{
    mousePosition = glm::vec2(mouseX, mouseY);
}

int main(int argc, char **argv)
{
	// Initializes the GLFW library
	glfwInit();

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(viewportDimensions.x, viewportDimensions.y, "3D Cameras", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//set resize callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);

    glfwSetCursorPosCallback(window, mouseMoveCallback);

	// Initializes the glew library
	glewInit();


	// Indices for square (-1, -1)[2] to (1, 1)[1]
	// [0]------[1]
	//	|		 |
	//	|		 |
	//	|		 |
	// [2]------[3]

	// Create square vertex data.
	std::vector<glm::vec3> vertices;
	vertices.push_back(glm::vec3(-1, 1, 0));
	vertices.push_back(glm::vec3(1, 1, 0));
	vertices.push_back(glm::vec3(-1, -1, 0));
	vertices.push_back(glm::vec3(1, -1, 0));

	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(1);


	// Create shape object
	square = new Shape(vertices, indices);
	


    // In OpenGL, the Z-Axis points out of the screen.
	transform.SetPosition(glm::vec3(.25, .25, -1));
    



	// SHADER STUFF
	
	// 
    std::string vertexShaderCode =
        "#version 400 core \n"

        // vertex position attribute
        "layout(location = 0) in vec3 in_position;"

        // uniform variables
        "uniform mat4 worldMatrix;"
        "uniform mat4 cameraView;"

		"void main(void)"
		"{"
			// Multiply the position by the world matrix to convert from local to world space.
            "vec4 worldPosition = worldMatrix * vec4(in_position, 1);"
            
            // Now, we multiply by the view matrix to get everything in view space.
            "vec4 viewPosition = cameraView * worldPosition;"
            
			// output the transformed vector as a vec4.
			"gl_Position = viewPosition;"
		"}";



	// Compile the vertex shader.
	vertexShader.InitFromString(vertexShaderCode, GL_VERTEX_SHADER);

	// Load and compile the fragment shader.
	fragmentShader.InitFromFile("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);





	// Create a shader program.
	shaderProgram = glCreateProgram();
	
	// Attach the vertex and fragment shaders to our program.
	vertexShader.AttachTo(shaderProgram);
	fragmentShader.AttachTo(shaderProgram);

	// Build shader program.
	glLinkProgram(shaderProgram);

	// After the program has been linked, we can ask it where it put our world matrix and camera matrix
	worldMatrixUniform = glGetUniformLocation(shaderProgram, "worldMatrix");
    cameraMatrixUniform = glGetUniformLocation(shaderProgram, "cameraView");


    std::cout << "Use WASD to move, and the mouse to look around." << std::endl;
    std::cout << "Press escape to exit" << std::endl;




	// Main Loop
	while (!glfwWindowShouldClose(window))
	{

        // Exit when escape is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }

        // Calculate delta time.
        float dt = glfwGetTime();
        // Reset the timer.
        glfwSetTime(0);


        // Get the distance from the center of the screen that the mouse has moved
        glm::vec2 mouseMovement = mousePosition - (viewportDimensions / 2.0f);

        // Calculate the horizontal view angle
        float yaw = cameraPosition.Rotation().y;
        yaw += (int)mouseMovement.x * .001f;

        // Calculate the vertical view angle
        float pitch = cameraPosition.Rotation().x;
        pitch -= (int)mouseMovement.y * .001f;

        // Clamp the camera from looking up over 90 degrees.
        float halfpi = 3.1416 / 2.f;
        if (pitch < -halfpi) pitch = -halfpi;
        else if (pitch > halfpi) pitch = halfpi;

        // Set the new rotation of the camera.
        cameraPosition.SetRotation(glm::vec3(pitch, yaw, 0));
        

        // Move the cursor to the center of the screen
        glfwSetCursorPos(window, viewportDimensions.x/2, viewportDimensions.y/2);


        // Here we get some input, and use it to move the camera
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition.Translate(cameraPosition.GetForward() * 5.0f * dt);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPosition.Translate(cameraPosition.GetRight() * -5.0f * dt);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition.Translate(cameraPosition.GetForward() * -5.0f * dt);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPosition.Translate(cameraPosition.GetRight() * 5.0f * dt);
        }


        // rotate square
        transform.RotateY(1.0f * dt);



        // Cameras use a transformations matrix just like other renderable objects.
        // When we multiply by a world matrix, we are moving an object from local space to world space.
        // When using a camera, we do the exact opposite. We move everything else from world space into camera local space.
        // To do this we make a matrix that does the inverse of what a world matrix does.
        glm::mat4 viewMatrix = cameraPosition.GetInverseMatrix();

        // A projection matrix converts objects from view space into 2d screen space.
        // Orthographic projection just flattens everything, in a straight line, basically removing the z dimension.
        /*
        
                        +----+
            +----+      | * .|
            | * .|      +----+
            +----+
        
        */
        // Orthographic projection is done in two steps.
        // First we move everything
        float near = 1; // the nearest distance we will render anything
        float far = 10; // the furthest distance we will render anything.
        float width = 1; // width of the view in world space (usually maps directly to screen size)
        float height = viewportDimensions.y / viewportDimensions.x; // height of the view in world space (usually maps directly to screen size)
        
        glm::mat4 orthographicProjection = glm::mat4(
            2/width, 0, 0, 0,                 // scale things down so that they fit within the screen space (-1 to 1).
            0, 2/height, 0, 0,                // same thing for y axis.
            0, 0, 2/(near-far), 0,            // same thing for z axis. OpenGL defines z as pointing out of the screen, so we have to negate it.
            0, 0, (far+near)/(near-far), 1    // translate everything along the z axis so that -1 is the closest thing to us, and 1 is the furthest away.
            );

        // Note: This process defines what will end up within the cube (-1, -1, -1) to (1, 1, 1).
        // Anything outside the cube will be be "clipped" by the gpu.




        // Perspective projection expands outward from the camera getting wider, and making things that are far away look smaller.
        /*
                    +-----------+
                    |           |
                    | O         |
        +----+      |         o |
        |*  .|      |           |
        +----+      +-----------+

        */
        // We do this by converting our coordinates into 
        glm::mat4 perspectiveProjection = glm::mat4(
            2/width, 0, 0, 0,                   // scale width down to fit in unit cube
            0, 2/height, 0, 0,                  // scale height
            0, 0, -(far+near)/(far-near), -1,   // scale depth, -1 converts our coordinates into homogeneous coordinates, which we need to keep our angles
            0, 0, (2*near*far)/(near-far), 1    // translate everything so that
            );

        // Compose view and projection into one matrix to send to the gpu
        glm::mat4 viewProjection = perspectiveProjection * viewMatrix;




        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 0.0);

		// Set the current shader program.
		glUseProgram(shaderProgram);

        // Send the camera matrix to the shader
        glUniformMatrix4fv(cameraMatrixUniform, 1, GL_FALSE, &(viewProjection[0][0]));
		
		// Draw using the worldMatrixUniform
		square->Draw(transform.GetMatrix(), worldMatrixUniform);



		// Stop using the shader program.
		glUseProgram(0);

		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();

	}

	// Free memory from shader program and individual shaders
	glDeleteProgram(shaderProgram);


	// Free memory from shape object
	delete square;

	// Free GLFW memory.
	glfwTerminate();


	// End of Program.
	return 0;
}
