#define ULOC( program, s ) glGetUniformLocation( (program), (s))
#define nUNIFORM1(type, program, s, v0) glUniform1##type( ULOC((program), (s)), (v0) )
#define nUNIFORM2(type, program, s, v0, v1) glUniform2##type( ULOC((program), (s)), (v0), (v1)  )
#define nUNIFORM3(type, program, s, v0, v1, v2) glUniform3##type( ULOC((program), (s)), (v0), (v1), (v2) )
#define nUNIFORM4(type, program, s, v0, v1, v2, v3) glUniform4##type( ULOC((program), (s)), (v0), (v1), (v2), (v3) )

#define nUNIFORMV(type, program, s, v0) glUniform##type##v( ULOC((program), (s)), 1, (v0) )
#define nUNIFORMVV(type, program, s, c, v0) glUniform##type##v( ULOC((program), (s)), (c), (v0) )

#define nUNIFORMM(type, program, s, v0) glUniformMatrix##type##v( ULOC((program), (s)), 1, GL_FALSE, (const GLfloat*) (v0))
#define nUNIFORMMT(type, program, s, v0) glUniformMatrix##type##v( ULOC((program), (s)), 1, GL_TRUE, (const GLfloat*) (v0))
#define nUNIFORMMV(type, program, s, c, v0) glUniformMatrix##type##v( ULOC((program), (s)), (c), GL_FALSE, (const GLfloat*) (v0))
#define nUNIFORMMVT(type, program, s, c, v0) glUniformMatrix##type##v( ULOC((program), (s)), (c), GL_TRUE, (const GLfloat*) (v0))
