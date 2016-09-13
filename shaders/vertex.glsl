/*
Title: 3D Cameras
File Name: vertex.glsl
Copyright ? 2016
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


#version 400 core

// Vertex attribute for position
layout(location = 0) in vec2 in_position;

// uniform will contain the world matrix.
uniform mat3 worldMatrix;
uniform mat3 cameraView;

void main(void)
{
	//transform the vector
	vec3 worldPosition = worldMatrix * vec3(in_position, 1);
	
	vec3 viewPosition = cameraView * worldPosition;

	// output the transformed vector
	gl_Position = vec4(viewPosition, 1);
}