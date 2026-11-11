#ifndef AURGLEXT_H
#define AURGLEXT_H

typedef void(WINAPI* AurGlActiveTextureProc)(unsigned int nTexture);
typedef void(WINAPI* AurGlMultiTexCoord2fProc)(unsigned int nTexture,
                                              float s, float t);
typedef const unsigned char*(WINAPI* AurWglGetExtensionsStringProc)(HDC hDC);

typedef void(WINAPI* AurGlBindProgramProc)(unsigned int nTarget,
                                           unsigned int nProgram);
typedef void(WINAPI* AurGlProgramEnvParameter4fProc)(unsigned int nTarget,
    unsigned int nIndex, float x, float y, float z, float w);
typedef void(WINAPI* AurGlBindFragmentShaderProc)(unsigned int nShader);
typedef void(WINAPI* AurGlSetFragmentShaderConstantProc)(unsigned int nConstant,
                                                        const float* pfValue);

typedef AurWglGetExtensionsStringProc PFNWGLGETEXTENSIONSSTRINGARBPROC;
typedef AurGlMultiTexCoord2fProc PFNGLMULTITEXCOORD2FARBPROC;
typedef void (WINAPI* PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
typedef void (WINAPI* PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (WINAPI* PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (WINAPI* PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (WINAPI* PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef AurGlActiveTextureProc PFNGLACTIVETEXTUREARBPROC;
typedef AurGlActiveTextureProc PFNGLCLIENTACTIVETEXTUREARBPROC;
typedef AurGlBindBufferProc PFNGLBINDBUFFERARBPROC;
typedef AurGlDeleteBuffersProc PFNGLDELETEBUFFERSARBPROC;
typedef AurGlGenBuffersProc PFNGLGENBUFFERSARBPROC;
typedef GLboolean (WINAPI* PFNGLISBUFFERARBPROC) (GLuint buffer);
typedef AurGlBufferDataProc PFNGLBUFFERDATAARBPROC;
typedef AurGlBufferSubDataProc PFNGLBUFFERSUBDATAARBPROC;
typedef void (WINAPI* PFNGLGETBUFFERSUBDATAARBPROC) (GLenum target, int offset, int size, GLvoid *data);
typedef AurGlMapBufferProc PFNGLMAPBUFFERARBPROC;
typedef AurGlUnmapBufferProc PFNGLUNMAPBUFFERARBPROC;
typedef AurGlGetBufferParameterivProc PFNGLGETBUFFERPARAMETERIVARBPROC;
typedef void (WINAPI* PFNGLGETBUFFERPOINTERVARBPROC) (GLenum target, GLenum pname, GLvoid* *params);
typedef void (WINAPI* PFNGLVERTEXATTRIB1SARBPROC) (GLuint index, GLshort x);
typedef void (WINAPI* PFNGLVERTEXATTRIB1FARBPROC) (GLuint index, GLfloat x);
typedef void (WINAPI* PFNGLVERTEXATTRIB1DARBPROC) (GLuint index, GLdouble x);
typedef void (WINAPI* PFNGLVERTEXATTRIB2SARBPROC) (GLuint index, GLshort x, GLshort y);
typedef void (WINAPI* PFNGLVERTEXATTRIB2FARBPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (WINAPI* PFNGLVERTEXATTRIB2DARBPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (WINAPI* PFNGLVERTEXATTRIB3SARBPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (WINAPI* PFNGLVERTEXATTRIB3FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (WINAPI* PFNGLVERTEXATTRIB3DARBPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (WINAPI* PFNGLVERTEXATTRIB4SARBPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (WINAPI* PFNGLVERTEXATTRIB4FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (WINAPI* PFNGLVERTEXATTRIB4DARBPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (WINAPI* PFNGLVERTEXATTRIB4NUBARBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (WINAPI* PFNGLVERTEXATTRIB1SVARBPROC) (GLuint index, const GLshort *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB1FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB1DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB2SVARBPROC) (GLuint index, const GLshort *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB2FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB2DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB3SVARBPROC) (GLuint index, const GLshort *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB3FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB3DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4BVARBPROC) (GLuint index, const GLbyte *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4SVARBPROC) (GLuint index, const GLshort *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4IVARBPROC) (GLuint index, const GLint *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4UBVARBPROC) (GLuint index, const GLubyte *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4USVARBPROC) (GLuint index, const GLushort *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4UIVARBPROC) (GLuint index, const GLuint *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4FVARBPROC) (GLuint index, const GLfloat *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4DVARBPROC) (GLuint index, const GLdouble *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4NBVARBPROC) (GLuint index, const GLbyte *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4NSVARBPROC) (GLuint index, const GLshort *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4NIVARBPROC) (GLuint index, const GLint *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4NUBVARBPROC) (GLuint index, const GLubyte *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4NUSVARBPROC) (GLuint index, const GLushort *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4NUIVARBPROC) (GLuint index, const GLuint *v);
typedef void(WINAPI* PFNGLVERTEXATTRIBPOINTERARBPROC)(unsigned int, int, unsigned int,
                                           unsigned char, int, const void*);
typedef void(WINAPI* PFNGLENABLEVERTEXATTRIBARRAYARBPROC)(unsigned int);
typedef void(WINAPI* PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)(unsigned int);
typedef void(WINAPI* PFNGLPROGRAMSTRINGARBPROC)(unsigned int, unsigned int, int,
                                     const void*);
typedef AurGlBindProgramProc PFNGLBINDPROGRAMARBPROC;
typedef void(WINAPI* PFNGLDELETEPROGRAMSARBPROC)(int, const unsigned int*);
typedef void(WINAPI* PFNGLGENPROGRAMSARBPROC)(int, unsigned int*);
typedef void (WINAPI* PFNGLPROGRAMENVPARAMETER4DARBPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (WINAPI* PFNGLPROGRAMENVPARAMETER4DVARBPROC) (GLenum target, GLuint index, const GLdouble *params);
typedef AurGlProgramEnvParameter4fProc PFNGLPROGRAMENVPARAMETER4FARBPROC;
typedef void (WINAPI* PFNGLPROGRAMENVPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (WINAPI* PFNGLPROGRAMLOCALPARAMETER4DARBPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (WINAPI* PFNGLPROGRAMLOCALPARAMETER4DVARBPROC) (GLenum target, GLuint index, const GLdouble *params);
typedef void (WINAPI* PFNGLPROGRAMLOCALPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (WINAPI* PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (WINAPI* PFNGLGETPROGRAMENVPARAMETERDVARBPROC) (GLenum target, GLuint index, GLdouble *params);
typedef void (WINAPI* PFNGLGETPROGRAMENVPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
typedef void (WINAPI* PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC) (GLenum target, GLuint index, GLdouble *params);
typedef void (WINAPI* PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
typedef void (WINAPI* PFNGLGETPROGRAMIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (WINAPI* PFNGLGETPROGRAMSTRINGARBPROC) (GLenum target, GLenum pname, GLvoid *string);
typedef void (WINAPI* PFNGLGETVERTEXATTRIBDVARBPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (WINAPI* PFNGLGETVERTEXATTRIBFVARBPROC) (GLuint index, GLenum pname, GLfloat *params);
typedef void (WINAPI* PFNGLGETVERTEXATTRIBIVARBPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (WINAPI* PFNGLGETVERTEXATTRIBPOINTERVARBPROC) (GLuint index, GLenum pname, GLvoid* *pointer);
typedef unsigned char(WINAPI* PFNGLISPROGRAMARBPROC)(unsigned int);
typedef AurGlCombinerParameterfvProc PFNGLCOMBINERPARAMETERFVNVPROC;
typedef void (WINAPI* PFNGLCOMBINERPARAMETERIVNVPROC) (GLenum pname, const GLint *params);
typedef void (WINAPI* PFNGLCOMBINERPARAMETERFNVPROC) (GLenum pname, GLfloat param);
typedef void(WINAPI* PFNGLCOMBINERPARAMETERINVPROC)(unsigned int, int);
typedef void(WINAPI* PFNGLCOMBINERINPUTNVPROC)(unsigned int, unsigned int, unsigned int,
                                    unsigned int, unsigned int, unsigned int);
typedef void(WINAPI* PFNGLCOMBINEROUTPUTNVPROC)(unsigned int, unsigned int, unsigned int,
                                     unsigned int, unsigned int, unsigned int,
                                     unsigned int, unsigned char, unsigned char,
                                     unsigned char);
typedef void(WINAPI* PFNGLFINALCOMBINERINPUTNVPROC)(unsigned int, unsigned int,
                                         unsigned int, unsigned int);
typedef void (WINAPI* PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params);
typedef void (WINAPI* PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params);
typedef void (WINAPI* PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLfloat *params);
typedef void (WINAPI* PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLint *params);
typedef void (WINAPI* PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (GLenum variable, GLenum pname, GLfloat *params);
typedef void (WINAPI* PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (GLenum variable, GLenum pname, GLint *params);
typedef AurGlCombinerStageParameterfvProc PFNGLCOMBINERSTAGEPARAMETERFVNVPROC;
typedef void (WINAPI* PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (WINAPI* PFNGLFOGCOORDFEXTPROC) (GLfloat coord);
typedef void (WINAPI* PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (WINAPI* PFNGLUNLOCKARRAYSEXTPROC) (void);
typedef void (WINAPI* PFNGLSECONDARYCOLOR3FVEXTPROC) (const GLfloat *v);
typedef void(WINAPI* PFNGLGENPROGRAMSNVPROC)(int, unsigned int*);
typedef void(WINAPI* PFNGLDELETEPROGRAMSNVPROC)(int, const unsigned int*);
typedef void(WINAPI* PFNGLLOADPROGRAMNVPROC)(unsigned int, unsigned int, int,
                                  const unsigned char*);
typedef AurGlBindProgramProc PFNGLBINDPROGRAMNVPROC;
typedef void(WINAPI* PFNGLTRACKMATRIXNVPROC)(unsigned int, unsigned int, unsigned int,
                                  unsigned int);
typedef AurGlProgramEnvParameter4fProc PFNGLPROGRAMPARAMETER4FNVPROC;
typedef void (WINAPI* PFNGLEXECUTEPROGRAMNVPROC) (GLenum target, GLuint id, const GLfloat *params);
typedef void (WINAPI* PFNGLGETPROGRAMPARAMETERFVNVPROC) (GLenum target, GLuint index, GLenum pname, GLfloat *params);
typedef void(WINAPI* PFNGLVERTEXATTRIBPOINTERNVPROC)(unsigned int, int, unsigned int,
                                          int, const void*);
typedef unsigned char(WINAPI* PFNGLISPROGRAMNVPROC)(unsigned int);
typedef void (WINAPI* PFNGLVERTEXATTRIB1FVNVPROC) (GLuint index, const GLfloat *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB2FVNVPROC) (GLuint index, const GLfloat *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB3FVNVPROC) (GLuint index, const GLfloat *v);
typedef void (WINAPI* PFNGLVERTEXATTRIB4FVNVPROC) (GLuint index, const GLfloat *v);
typedef void* (WINAPI* PFNWGLALLOCATEMEMORYNVPROC)(int, float, float, float);
typedef void (WINAPI* PFNWGLFREEMEMORYNVPROC)(void*);
typedef void (WINAPI* PFNGLVERTEXARRAYRANGENVPROC) (GLsizei length, const GLvoid *pointer);
typedef void (WINAPI* PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
typedef void (WINAPI* PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
typedef GLboolean (WINAPI* PFNGLISFENCENVPROC) (GLuint fence);
typedef GLboolean (WINAPI* PFNGLTESTFENCENVPROC) (GLuint fence);
typedef void (WINAPI* PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
typedef void (WINAPI* PFNGLFINISHFENCENVPROC) (GLuint fence);
typedef void (WINAPI* PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);
typedef void (WINAPI* PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (GLenum target, GLint level, GLvoid *img);
typedef AurGlCompressedTexImage2DProc PFNGLCOMPRESSEDTEXIMAGE2DARBPROC;
typedef void (WINAPI* PFNGLPNTRIANGLESIATIPROC) (GLenum pname, GLint param);
typedef void (WINAPI* PFNGLPNTRIANGLESFATIPROC) (GLenum pname, GLfloat param);
typedef AurWglChoosePixelFormatProc PFNWGLCHOOSEPIXELFORMATARBPROC;
typedef AurWglCreatePbufferProc PFNWGLCREATEPBUFFERARBPROC;
typedef AurWglGetPbufferDCProc PFNWGLGETPBUFFERDCARBPROC;
typedef int(WINAPI* PFNWGLRELEASEPBUFFERDCARBPROC)(PBUFFERARB*, HDC);
typedef int(WINAPI* PFNWGLDESTROYPBUFFERARBPROC)(PBUFFERARB*);
typedef AurWglGetPixelFormatAttribivProc PFNWGLGETPIXELFORMATATTRIBIVARBPROC;
typedef BOOL (WINAPI* PFNWGLQUERYPBUFFERARBPROC)(PBUFFERARB*, int, int*);
typedef AurWglMakeContextCurrentProc PFNWGLMAKECONTEXTCURRENTARBPROC;
typedef AurWglTexImageProc PFNWGLBINDTEXIMAGEARBPROC;
typedef AurWglTexImageProc PFNWGLRELEASETEXIMAGEARBPROC;
typedef unsigned int(WINAPI* PFNGLGENFRAGMENTSHADERSATIPROC)(unsigned int);
typedef AurGlBindFragmentShaderProc PFNGLBINDFRAGMENTSHADERATIPROC;
typedef void(WINAPI* PFNGLDELETEFRAGMENTSHADERATIPROC)(unsigned int);
typedef void(WINAPI* PFNGLBEGINFRAGMENTSHADERATIPROC)();
typedef void(WINAPI* PFNGLENDFRAGMENTSHADERATIPROC)();
typedef void(WINAPI* PFNGLPASSTEXCOORDATIPROC)(unsigned int, unsigned int, unsigned int);
typedef void(WINAPI* PFNGLSAMPLEMAPATIPROC)(unsigned int, unsigned int, unsigned int);
typedef void(WINAPI* PFNGLCOLORFRAGMENTOP1ATIPROC)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
typedef void(WINAPI* PFNGLCOLORFRAGMENTOP2ATIPROC)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int);
typedef void(WINAPI* PFNGLCOLORFRAGMENTOP3ATIPROC)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int);
typedef void(WINAPI* PFNGLALPHAFRAGMENTOP1ATIPROC)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int);
typedef void(WINAPI* PFNGLALPHAFRAGMENTOP2ATIPROC)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int);
typedef void(WINAPI* PFNGLALPHAFRAGMENTOP3ATIPROC)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
typedef AurGlSetFragmentShaderConstantProc PFNGLSETFRAGMENTSHADERCONSTANTATIPROC;
typedef void (WINAPI* PFNGLBEGINVERTEXSHADEREXTPROC) (void);
typedef void (WINAPI* PFNGLENDVERTEXSHADEREXTPROC) (void);
typedef void (WINAPI* PFNGLBINDVERTEXSHADEREXTPROC) (GLuint id);
typedef GLuint (WINAPI* PFNGLGENVERTEXSHADERSEXTPROC) (GLuint range);
typedef void (WINAPI* PFNGLDELETEVERTEXSHADERSEXTPROC) (GLuint id);
typedef void (WINAPI* PFNGLSHADEROP1EXTPROC) (GLenum op, GLuint res, GLuint arg1);
typedef void (WINAPI* PFNGLSHADEROP2EXTPROC) (GLenum op, GLuint res, GLuint arg1, GLuint arg2);
typedef void (WINAPI* PFNGLSHADEROP3EXTPROC) (GLenum op, GLuint res, GLuint arg1, GLuint arg2, GLuint arg3);
typedef void (WINAPI* PFNGLSWIZZLEEXTPROC) (GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, GLenum outW);
typedef void (WINAPI* PFNGLWRITEMASKEXTPROC) (GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, GLenum outW);
typedef void (WINAPI* PFNGLINSERTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
typedef void (WINAPI* PFNGLEXTRACTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
typedef GLuint (WINAPI* PFNGLGENSYMBOLSEXTPROC) (GLenum datatype, GLenum storagetype, GLenum range, GLuint components);
typedef void (WINAPI* PFNGLSETINVARIANTEXTPROC) (GLuint id, GLenum type, const GLvoid *addr);
typedef void (WINAPI* PFNGLSETLOCALCONSTANTEXTPROC) (GLuint id, GLenum type, const GLvoid *addr);
typedef void (WINAPI* PFNGLVARIANTBVEXTPROC) (GLuint id, const GLbyte *addr);
typedef void (WINAPI* PFNGLVARIANTSVEXTPROC) (GLuint id, const GLshort *addr);
typedef void (WINAPI* PFNGLVARIANTIVEXTPROC) (GLuint id, const GLint *addr);
typedef void (WINAPI* PFNGLVARIANTFVEXTPROC) (GLuint id, const GLfloat *addr);
typedef void (WINAPI* PFNGLVARIANTDVEXTPROC) (GLuint id, const GLdouble *addr);
typedef void (WINAPI* PFNGLVARIANTUBEXTPROC) (GLuint id, const GLubyte *addr);
typedef void (WINAPI* PFNGLVARIANTUSEXTPROC) (GLuint id, const GLushort *addr);
typedef void (WINAPI* PFNGLVARIANTUIEXTPROC) (GLuint id, const GLuint *addr);
typedef void (WINAPI* PFNGLVARIANTPOINTEREXTPROC) (GLuint id, GLenum type, GLuint stride, const GLvoid *addr);
typedef void (WINAPI* PFNGLENABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
typedef void (WINAPI* PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
typedef GLuint (WINAPI* PFNGLBINDLIGHTPARAMETEREXTPROC) (GLenum light, GLenum value);
typedef GLuint (WINAPI* PFNGLBINDMATERIALPARAMETEREXTPROC) (GLenum face, GLenum value);
typedef GLuint (WINAPI* PFNGLBINDTEXGENPARAMETEREXTPROC) (GLenum unit, GLenum coord, GLenum value);
typedef GLuint (WINAPI* PFNGLBINDTEXTUREUNITPARAMETEREXTPROC) (GLenum unit, GLenum value);
typedef GLuint (WINAPI* PFNGLBINDPARAMETEREXTPROC) (GLenum value);
typedef GLboolean (WINAPI* PFNGLISVARIANTENABLEDEXTPROC) (GLuint id, GLenum cap);
typedef void (WINAPI* PFNGLGETVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value, GLboolean *data);
typedef void (WINAPI* PFNGLGETVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value, GLint *data);
typedef void (WINAPI* PFNGLGETVARIANTFLOATVEXTPROC) (GLuint id, GLenum value, GLfloat *data);
typedef void (WINAPI* PFNGLGETVARIANTPOINTERVEXTPROC) (GLuint id, GLenum value, GLvoid* *data);
typedef void (WINAPI* PFNGLGETINVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value, GLboolean *data);
typedef void (WINAPI* PFNGLGETINVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value, GLint *data);
typedef void (WINAPI* PFNGLGETINVARIANTFLOATVEXTPROC) (GLuint id, GLenum value, GLfloat *data);
typedef void (WINAPI* PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC) (GLuint id, GLenum value, GLboolean *data);
typedef void (WINAPI* PFNGLGETLOCALCONSTANTINTEGERVEXTPROC) (GLuint id, GLenum value, GLint *data);
typedef void (WINAPI* PFNGLGETLOCALCONSTANTFLOATVEXTPROC) (GLuint id, GLenum value, GLfloat *data);
typedef AurGlActiveStencilFaceProc PFNGLACTIVESTENCILFACEEXTPROC;
typedef HANDLE (WINAPI* PFNWGLCREATEBUFFERREGIONARBPROC)(HDC, int, unsigned int);
typedef void (WINAPI* PFNWGLDELETEBUFFERREGIONARBPROC)(HANDLE);
typedef BOOL (WINAPI* PFNWGLSAVEBUFFERREGIONARBPROC)(HANDLE, int, int, int, int);
typedef BOOL (WINAPI* PFNWGLRESTOREBUFFERREGIONARBPROC)(HANDLE, int, int, int, int, int, int);
typedef AurWglSwapIntervalProc PFNWGLSWAPINTERVALEXTPROC;
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC)();

extern "C"
{
    extern PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB;
    extern PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
    extern PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB;
    extern PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB;
    extern PFNGLMULTITEXCOORD4FVARBPROC glMultiTexCoord4fvARB;
    extern PFNGLISBUFFERARBPROC glIsBufferARB;
    extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
    extern PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;
    extern PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB;
    extern PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
    extern PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB;
    extern PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB;
    extern PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
    extern PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB;
    extern PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB;
    extern PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
    extern PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB;
    extern PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB;
    extern PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
    extern PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB;
    extern PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB;
    extern PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB;
    extern PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
    extern PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB;
    extern PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB;
    extern PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
    extern PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB;
    extern PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB;
    extern PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
    extern PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB;
    extern PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB;
    extern PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB;
    extern PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;
    extern PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB;
    extern PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB;
    extern PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB;
    extern PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
    extern PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB;
    extern PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB;
    extern PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB;
    extern PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB;
    extern PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB;
    extern PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB;
    extern PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB;
    extern PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4dARB;
    extern PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dvARB;
    extern PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
    extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4dARB;
    extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dvARB;
    extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
    extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB;
    extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdvARB;
    extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfvARB;
    extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB;
    extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB;
    extern PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
    extern PFNGLGETPROGRAMSTRINGARBPROC glGetProgramStringARB;
    extern PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdvARB;
    extern PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfvARB;
    extern PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribivARB;
    extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointervARB;
    extern PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
    extern PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV;
    extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV;
    extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV;
    extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
    extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;
    extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
    extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;
    extern PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT;
    extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;
    extern PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
    extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;
    extern PFNGLSECONDARYCOLOR3FVEXTPROC glSecondaryColor3fvEXT;
    extern void(WINAPI* glAlphaFragmentOp1ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int);
    extern PFNGLEXECUTEPROGRAMNVPROC glExecuteProgramNV;
    extern PFNGLGETPROGRAMPARAMETERFVNVPROC glGetProgramParameterfvNV;
    extern PFNGLVERTEXATTRIB1FVNVPROC glVertexAttrib1fvNV;
    extern PFNGLVERTEXATTRIB2FVNVPROC glVertexAttrib2fvNV;
    extern PFNGLVERTEXATTRIB3FVNVPROC glVertexAttrib3fvNV;
    extern PFNGLVERTEXATTRIB4FVNVPROC glVertexAttrib4fvNV;
    extern PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
    extern PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
    extern PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
    extern PFNGLDELETEFENCESNVPROC glDeleteFencesNV;
    extern PFNGLGENFENCESNVPROC glGenFencesNV;
    extern PFNGLISFENCENVPROC glIsFenceNV;
    extern PFNGLTESTFENCENVPROC glTestFenceNV;
    extern PFNGLGETFENCEIVNVPROC glGetFenceivNV;
    extern PFNGLFINISHFENCENVPROC glFinishFenceNV;
    extern PFNGLSETFENCENVPROC glSetFenceNV;
    extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB;
    extern PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
    extern PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI;
    extern PFNWGLQUERYPBUFFERARBPROC wglQueryPBufferARB;
    extern PFNGLBEGINVERTEXSHADEREXTPROC glBeginVertexShaderEXT;
    extern PFNGLENDVERTEXSHADEREXTPROC glEndVertexShaderEXT;
    extern PFNGLBINDVERTEXSHADEREXTPROC glBindVertexShaderEXT;
    extern PFNGLGENVERTEXSHADERSEXTPROC glGenVertexShadersEXT;
    extern PFNGLDELETEVERTEXSHADERSEXTPROC glDeleteVertexShadersEXT;
    extern PFNGLSHADEROP1EXTPROC glShaderOp1EXT;
    extern PFNGLSHADEROP2EXTPROC glShaderOp2EXT;
    extern PFNGLSHADEROP3EXTPROC glShaderOp3EXT;
    extern PFNGLSWIZZLEEXTPROC glSwizzleEXT;
    extern PFNGLWRITEMASKEXTPROC glWriteMaskEXT;
    extern PFNGLINSERTCOMPONENTEXTPROC glInsertComponentEXT;
    extern PFNGLEXTRACTCOMPONENTEXTPROC glExtractComponentEXT;
    extern PFNGLGENSYMBOLSEXTPROC glGenSymbolsEXT;
    extern PFNGLSETINVARIANTEXTPROC glSetInvariantEXT;
    extern PFNGLSETLOCALCONSTANTEXTPROC glSetLocalConstantEXT;
    extern PFNGLVARIANTBVEXTPROC glVariantbvEXT;
    extern PFNGLVARIANTSVEXTPROC glVariantsvEXT;
    extern PFNGLVARIANTIVEXTPROC glVariantivEXT;
    extern PFNGLVARIANTFVEXTPROC glVariantfvEXT;
    extern PFNGLVARIANTDVEXTPROC glVariantdvEXT;
    extern PFNGLVARIANTUBEXTPROC glVariantubEXT;
    extern PFNGLVARIANTUSEXTPROC glVariantusEXT;
    extern PFNGLVARIANTUIEXTPROC glVariantuiEXT;
    extern PFNGLVARIANTPOINTEREXTPROC glVariantPointerEXT;
    extern PFNGLENABLEVARIANTCLIENTSTATEEXTPROC glEnableVariantClientStateEXT;
    extern PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC glDisableVariantClientStateEXT;
    extern PFNGLBINDLIGHTPARAMETEREXTPROC glBindLightParameterEXT;
    extern PFNGLBINDMATERIALPARAMETEREXTPROC glBindMaterialParameterEXT;
    extern PFNGLBINDTEXGENPARAMETEREXTPROC glBindTexGenParameterEXT;
    extern PFNGLBINDTEXTUREUNITPARAMETEREXTPROC glBindTextureUnitParameterEXT;
    extern PFNGLBINDPARAMETEREXTPROC glBindParameterEXT;
    extern PFNGLISVARIANTENABLEDEXTPROC glIsVariantEnabledEXT;
    extern PFNGLGETVARIANTBOOLEANVEXTPROC glGetVariantBooleanvEXT;
    extern PFNGLGETVARIANTINTEGERVEXTPROC glGetVariantIntegervEXT;
    extern PFNGLGETVARIANTFLOATVEXTPROC glGetVariantFloatvEXT;
    extern PFNGLGETVARIANTPOINTERVEXTPROC glGetVariantPointervEXT;
    extern PFNGLGETINVARIANTBOOLEANVEXTPROC glGetInvariantBooleanvEXT;
    extern PFNGLGETINVARIANTINTEGERVEXTPROC glGetInvariantIntegervEXT;
    extern PFNGLGETINVARIANTFLOATVEXTPROC glGetInvariantFloatvEXT;
    extern PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC glGetLocalConstantBooleanvEXT;
    extern PFNGLGETLOCALCONSTANTINTEGERVEXTPROC glGetLocalConstantIntegervEXT;
    extern PFNGLGETLOCALCONSTANTFLOATVEXTPROC glGetLocalConstantFloatvEXT;
    extern PFNWGLCREATEBUFFERREGIONARBPROC wglCreateBufferRegionARB;
    extern PFNWGLDELETEBUFFERREGIONARBPROC wglDeleteBufferRegionARB;
    extern PFNWGLSAVEBUFFERREGIONARBPROC wglSaveBufferRegionARB;
    extern PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB;
    extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
    extern AurGlActiveTextureProc glActiveTextureARB;
    extern AurGlBindProgramProc glBindProgramARB;
    extern AurGlBindProgramProc glBindProgramNV;
    extern AurGlBindFragmentShaderProc glBindFragmentShaderATI;
    extern unsigned int(WINAPI* glGenFragmentShadersATI)(unsigned int);
    extern void(WINAPI* glGenProgramsARB)(int, unsigned int*);
    extern void(WINAPI* glDeleteProgramsARB)(int, const unsigned int*);
    extern AurGlActiveStencilFaceProc glActiveStencilFaceEXT;
    extern void(WINAPI* glBeginFragmentShaderATI)();
    extern void(WINAPI* glEndFragmentShaderATI)();
    extern void(WINAPI* glDeleteFragmentShaderATI)(unsigned int);
    extern void(WINAPI* glPassTexCoordATI)(unsigned int, unsigned int, unsigned int);
    extern void(WINAPI* glSampleMapATI)(unsigned int, unsigned int, unsigned int);
    extern AurGlSetFragmentShaderConstantProc glSetFragmentShaderConstantATI;
    extern AurGlActiveTextureProc glClientActiveTextureARB;
    extern AurGlMultiTexCoord2fProc glMultiTexCoord2fARB;
    extern AurGlProgramEnvParameter4fProc glProgramEnvParameter4fARB;
    extern AurGlProgramEnvParameter4fProc glProgramParameter4fNV;
    extern void(WINAPI* glProgramStringARB)(unsigned int, unsigned int, int,
                                     const void*);
    extern void(WINAPI* glGenProgramsNV)(int, unsigned int*);
    extern void(WINAPI* glDeleteProgramsNV)(int, const unsigned int*);
    extern unsigned char(WINAPI* glIsProgramARB)(unsigned int);
    extern unsigned char(WINAPI* glIsProgramNV)(unsigned int);
    extern void(WINAPI* glLoadProgramNV)(unsigned int, unsigned int, int,
                                  const unsigned char*);
    extern void(WINAPI* glTrackMatrixNV)(unsigned int, unsigned int, unsigned int,
                                  unsigned int);
    extern void(WINAPI* glVertexAttribPointerARB)(unsigned int, int, unsigned int,
                                           unsigned char, int, const void*);
    extern void(WINAPI* glVertexAttribPointerNV)(unsigned int, int, unsigned int,
                                          int, const void*);
    extern void(WINAPI* glEnableVertexAttribArrayARB)(unsigned int);
    extern void(WINAPI* glDisableVertexAttribArrayARB)(unsigned int);
    extern AurGlCombinerParameterfvProc glCombinerParameterfvNV;
    extern AurGlCombinerStageParameterfvProc glCombinerStageParameterfvNV;
    extern void(WINAPI* glCombinerParameteriNV)(unsigned int, int);
    extern void(WINAPI* glCombinerInputNV)(unsigned int, unsigned int, unsigned int,
                                    unsigned int, unsigned int, unsigned int);
    extern void(WINAPI* glCombinerOutputNV)(unsigned int, unsigned int, unsigned int,
                                     unsigned int, unsigned int, unsigned int,
                                     unsigned int, unsigned char, unsigned char,
                                     unsigned char);
    extern void(WINAPI* glFinalCombinerInputNV)(unsigned int, unsigned int,
                                         unsigned int, unsigned int);
    extern AurGlBindBufferProc glBindBufferARB;
    extern AurGlMapBufferProc glMapBufferARB;
    extern AurGlUnmapBufferProc glUnmapBufferARB;
    extern AurGlGetBufferParameterivProc glGetBufferParameterivARB;
    extern AurGlDeleteBuffersProc glDeleteBuffersARB;
    extern AurGlGenBuffersProc glGenBuffersARB;
    extern AurGlBufferDataProc glBufferDataARB;
    extern AurGlBufferSubDataProc glBufferSubDataARB;
    extern AurGlCompressedTexImage2DProc glCompressedTexImage2DARB;
    extern void(WINAPI* glColorFragmentOp1ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    extern void(WINAPI* glColorFragmentOp2ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int);
    extern void(WINAPI* glAlphaFragmentOp2ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int);
    extern void(WINAPI* glColorFragmentOp3ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int);
    extern void(WINAPI* glAlphaFragmentOp3ATI)(unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int,
        unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    extern AurWglChoosePixelFormatProc wglChoosePixelFormatARB;
    extern AurWglGetPixelFormatAttribivProc wglGetPixelFormatAttribivARB;
    extern AurWglCreatePbufferProc wglCreatePBufferARB;
    extern AurWglGetPbufferDCProc wglGetPBufferDCARB;
    extern AurWglTexImageProc wglBindTexImageARB;
    extern AurWglTexImageProc wglReleaseTexImageARB;
    extern AurWglMakeContextCurrentProc wglMakeContextCurrentARB;
    extern int(WINAPI* wglReleasePBufferDCARB)(PBUFFERARB*, HDC);
    extern int(WINAPI* wglDestroyPBufferARB)(PBUFFERARB*);
    extern AurWglGetExtensionsStringProc wglGetExtensionsStringARB;
    extern AurWglSwapIntervalProc wglSwapIntervalEXT;
}

extern unsigned int lookup;

#endif
