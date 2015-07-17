/////////////////////////////////////////////
//
// Geometry Clip-Maps Tutorial
//
// (C) by Sven Forstmann in 2014
//
// License : MIT
// http://opensource.org/licenses/MIT
/////////////////////////////////////////////
// Mathlib included from 
// http://sourceforge.net/projects/nebuladevice/
/////////////////////////////////////////////
#include <iostream> 
#include <vector> 
#include <string> 
#include <stdio.h>
#include <glew.h>
#include <wglew.h>
#include <windows.h>
#include <mmsystem.h>
#include <GL/glut.h>
using namespace std;
#include "glsl.h"
#pragma comment(lib,"winmm.lib")
///////////////////////////////////////////
#include "Bmp.h"
#include "ogl.h"
///////////////////////////////////////////
int grid= 64;				// patch resolution
int levels=5;				// LOD levels
int width=2048,height=2048; // heightmap dimensions
///////////////////////////////////////////

void DrawScene()
{
	if ( GetAsyncKeyState(VK_ESCAPE) )  exit(0);

	POINT cursor;
	GetCursorPos(&cursor); // mouse pointer position

	bool	wireframe= GetAsyncKeyState(VK_SPACE);	// render wireframe
	bool	topdown	 = GetAsyncKeyState(VK_RETURN);	// view top-down
	float	viewangle= float(cursor.x)/5.0;
	vec3f	viewpos ( (timeGetTime()>>2)&((1<<17)-1) , -(float(cursor.y)/1000.0)* 0.1-0.01 , 0 );

	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	static int tex_heightmap=0;
	static int tex_terrain=0;

	static bool init=true;
	static Shader shader("Shader");

	static int vbo=0;
	static std::vector<float> vert;

	if(init)
	{
		/*+++++++++++++++++++++++++++++++++++++*/
		// terrain heightmap
		Bmp bmp(width,height,32);
		loopj(0,height) loopi(0,width)
		{
			float x= float(i)/float(width);
			float y= float(j)/float(height);
			float h = (sin(4*M_PI*x)+sin(4*M_PI*y)+sin(16*M_PI*x)*sin(16*M_PI*y))*0.125+0.5;
			((float*)bmp.data)[i+j*width]=h;
		}	
		//bmp.load_float("../result.f32"); // <-- use this for loading raw float map from file
		tex_heightmap = ogl_tex_new(width,height,GL_LINEAR_MIPMAP_LINEAR,GL_REPEAT,GL_LUMINANCE16F_ARB,GL_LUMINANCE,bmp.data, GL_FLOAT);
		/*+++++++++++++++++++++++++++++++++++++*/
		// terrain texture
		loopj(0,height)	loopi(0,width) loopk(0,3)
		{
			bmp.data[(i+j*width)*3+k]=i^j^(k*192);
		}
		tex_terrain = ogl_tex_new(width,height,GL_LINEAR_MIPMAP_LINEAR,GL_REPEAT,GL_RGB,GL_RGB,bmp.data, GL_UNSIGNED_BYTE);
		/*+++++++++++++++++++++++++++++++++++++*/
		// driver info
		std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;			//std::cout << "GL_EXTENSIONS: " << glGetString(GL_EXTENSIONS) << std::endl;
		std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "GLU_VERSION: " << gluGetString(GLU_VERSION) << std::endl;			//std::cout << "GLU_EXTENSIONS: " << gluGetString(GLU_EXTENSIONS) << std::endl;
		std::cout << "GLUT_API_VERSION: " << GLUT_API_VERSION << std::endl;
		/*+++++++++++++++++++++++++++++++++++++*/
		// load shaders
		shader.attach(GL_VERTEX_SHADER,"../shader/vs.txt");
		shader.attach(GL_FRAGMENT_SHADER,"../shader/frag.txt");
		shader.link();
		/*+++++++++++++++++++++++++++++++++++++*/
		// make vbo quad patch
		loopj(0,grid+1)
		loopi(0,grid+2)
		{
			loopk(0, ((i==0) ? 2 : 1) )
			{
				vert.push_back(float(i)/grid);
				vert.push_back(float(j)/grid);
				vert.push_back(0);
			}			
			++j;
			loopk(0, ((i==grid+1) ? 2 : 1) )
			{
				vert.push_back(float(i)/grid);
				vert.push_back(float(j)/grid);
				vert.push_back(0);
			}
			--j;
		}
		/*+++++++++++++++++++++++++++++++++++++*/
		glGenBuffers(1, (GLuint *)(&vbo));
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vert.size(),&vert[0], GL_DYNAMIC_DRAW_ARB );
		/*+++++++++++++++++++++++++++++++++++++*/
		init=false;
		/*+++++++++++++++++++++++++++++++++++++*/
	}
	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();

	if (topdown)
	{
		glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
		glRotatef(180,1,0,0);
		wireframe^=1;
	}
	else		 
	{
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		gluPerspective(90.0,float(vp[2])/float(vp[3]) , 0.0001, 1.0);
		glTranslatef(0,viewpos.y,0);	// set height
		glRotatef(130,1,0,0);		
		glRotatef(viewangle,0,0,1);		// set rotation
	}

	matrix44 mat;
	glGetFloatv(GL_PROJECTION_MATRIX, &mat.m[0][0]);		CHECK_GL_ERROR();
	
	// Enable VBO
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);				CHECK_GL_ERROR();
	glEnableClientState(GL_VERTEX_ARRAY);					CHECK_GL_ERROR();
	glVertexPointer  ( 3, GL_FLOAT,0, (char *) 0);			CHECK_GL_ERROR();

	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, tex_heightmap);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB( GL_TEXTURE1 );
	glBindTexture(GL_TEXTURE_2D, tex_terrain);

	// Triangle Mesh
	shader.begin();
	shader.setUniform1i("tex_heightmap",0);
	shader.setUniform1i("tex_terrain",1);

	float sxy=2; // scale x/y
	shader.setUniform4f("map_position", 
		-viewpos.x/float(2*512*grid),
		-viewpos.z/float(2*512*grid),0,0);

	loopi(0,levels)
	{
		float ox=(int(viewpos.x*(1<<i))&511)/float(512*grid);
		float oy=(int(viewpos.z*(1<<i))&511)/float(512*grid);

		vec3f scale	(sxy*0.25,sxy*0.25,1);
		shader.setUniform4f("scale" , scale.x,scale.y,1,1);	

		loopk(-2,2) loopj(-2,2) // each level has 4x4 patches
		{
			if(i!=levels-1) if(k==-1||k==0) if(j==-1||j==0) continue;

			vec3f offset(ox+float(j),oy+float(k),0);
			if(k>=0) offset.y-=1.0/float(grid); // adjust offset for proper overlapping
			if(j>=0) offset.x-=1.0/float(grid); // adjust offset for proper overlapping

			//cull
			int xp=0,xm=0,yp=0,ym=0,zp=0;
			looplmn(0,0,0,2,2,2)
			{
				vec3f v = scale*(offset+vec3f(l,m,float(-n)*0.05)); // bbox vector
				vec4f cs = mat * vec4f(v.x,v.y,v.z,1); // clipspace
				if(cs.z< cs.w) zp++;				
				if(cs.x<-cs.w) xm++;	if(cs.x>cs.w) xp++;
				if(cs.y<-cs.w) ym++;	if(cs.y>cs.w) yp++;
			}
			if(zp==0 || xm==8 || xp==8 || ym==8 || yp==8)continue; // skip if invisible
			
			//render
			shader.setUniform4f("offset", offset.x,offset.y,0,0);
			if(wireframe)	glDrawArrays( GL_LINES, 0, vert.size()/3);
			else			glDrawArrays( GL_TRIANGLE_STRIP, 0, vert.size()/3);
		}
		sxy*=0.5;
	}	
	shader.end();

	// Disable VBO
	glDisableClientState(GL_VERTEX_ARRAY);									CHECK_GL_ERROR();
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);								CHECK_GL_ERROR();
	glutSwapBuffers();
}
///////////////////////////////////////////
int main(int argc, char **argv) 
{ 
  glutInit(&argc, argv);  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
  glutInitWindowSize(1024, 512);  
  glutInitWindowPosition(0, 0);  
  glutCreateWindow("Geometry Clipmaps Example (c) Sven Forstmann 2014");
  glutDisplayFunc(DrawScene);
  glutIdleFunc(DrawScene);
  glewInit();
  wglSwapIntervalEXT(0);
  glutMainLoop();  
  return 0;
}
///////////////////////////////////////////
