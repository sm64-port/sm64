/* KallistiGL for KallistiOS ##version##

   libgl/glu.h
   Copyright (C) 2013-2014 Josh "PH3NOM" Pearson
   Copyright (C) 2016 Lawrence Sebald

   Some functionality adapted from the original KOS libgl:
   Copyright (C) 2001 Dan Potter
   Copyright (C) 2002 Benoit Miller

*/

#ifndef __GL_GLU_H
#define __GL_GLU_H

#include <sys/cdefs.h>
__BEGIN_DECLS

#include "gl.h"

#define GLU_FALSE 0
#define GLU_TRUE  1

GLAPI void APIENTRY gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);

/* gluPerspective - Set the Perspective for Rendering. */
GLAPI void APIENTRY gluPerspective(GLdouble fovy, GLdouble aspect,
                                   GLdouble zNear, GLdouble zFar);

/* gluLookAt - Set Camera Position for Rendering. */
GLAPI void APIENTRY gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
                              GLfloat centerx, GLfloat centery, GLfloat centerz,
                              GLfloat upx, GLfloat upy, GLfloat upz);

GLAPI GLint APIENTRY gluBuild2DMipmaps( GLenum target,GLint internalFormat, GLsizei width, 
                                        GLsizei height, GLenum format, GLenum type, const void *data );

GLAPI const GLubyte* APIENTRY gluErrorString(GLenum error);

__END_DECLS

#endif /* !__GL_GLU_H */
