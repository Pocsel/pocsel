/*
#include "MD5ModelLoaderPCH.h"
#include "Helpers.h"
#include "MD5Model.h"
#include "PivotCamera.h"

MD5Model g_Model;
PivotCamera g_Camera;

glm::vec3 g_InitialCameraPosition = glm::vec3(0, 0, 81);
glm::vec3 g_InitialCameraPivot = glm::vec3(-1,1,32);
glm::vec3 g_InitialCameraRotation = glm::vec3(-71,4,0);

int g_iWindowWidth = 800;
int g_iWindowHeight = 600;
int g_iGLUTWindowHandle = 0;
int g_iErrorCode = 0;

bool g_bLeftMouseDown = false;
bool g_bRightMouseDown = false;

glm::vec2 g_MouseCurrent = glm::vec2(0);
glm::vec2 g_MousePrevious = glm::vec2(0);
glm::vec2 g_MouseDelta = glm::vec2(0);

GLfloat g_LighPos[] = { -39.749374, -6.182379, 46.334176, 1.0f };       // This is the position of the 'lamp' joint in the test mesh in object-local space
GLfloat g_LightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat g_LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat g_LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat g_LighAttenuation0 = 1.0f;
GLfloat g_LighAttenuation1 = 0.0f;
GLfloat g_LighAttenuation2 = 0.0f;

void InitGL( int argc, char* argv[] );
void DisplayGL();
void IdleGL();
void KeyboardGL( unsigned char c, int x, int y );
void MouseGL( int button, int state, int x, int y );
void MotionGL( int x, int y );
void PassiveMotionGL( int x, int y );
void ReshapeGL( int w, int h );

// We're exiting, cleanup the allocated resources.
void Cleanup();

int main( int argc, char* argv[] )
{
    atexit(Cleanup);

    InitGL( argc, argv );

    // Setup some nice initial values for our test model.
    g_Camera.SetPivot( g_InitialCameraPivot );
    g_Camera.SetRotate( g_InitialCameraRotation );
    g_Camera.SetTranslate( g_InitialCameraPosition );

    // Load a model file
    g_Model.LoadModel( "data/Boblamp/boblampclean.md5mesh" );
    g_Model.LoadAnim( "data/Boblamp/boblampclean.md5anim" );

    glutMainLoop();
}

void Cleanup()
{
    if ( g_iGLUTWindowHandle != 0 )
    {
        glutDestroyWindow( g_iGLUTWindowHandle );
        g_iGLUTWindowHandle = 0;
    }
}


void InitGL( int argc, char* argv[] )
{
    std::cout << "Initialise OpenGL..." << std::endl;

    glutInit(&argc, argv);
    int iScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int iScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH );

    glutInitWindowPosition( ( iScreenWidth - g_iWindowWidth ) / 2,
        ( iScreenHeight - g_iWindowHeight ) / 2 );
    glutInitWindowSize( g_iWindowWidth, g_iWindowHeight );

    g_iGLUTWindowHandle = glutCreateWindow( "MD5 Model Loader" );

    // Register GLUT callbacks
    glutDisplayFunc(DisplayGL);
    glutIdleFunc(IdleGL);
    glutMouseFunc(MouseGL);
    glutMotionFunc(MotionGL);
    glutPassiveMotionFunc(PassiveMotionGL);
    glutKeyboardFunc(KeyboardGL);
    glutReshapeFunc(ReshapeGL);

    // Setup initial GL State
    glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );
    glClearDepth( 1.0f );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, g_LightAmbient );

    glLightfv( GL_LIGHT0, GL_DIFFUSE, g_LightDiffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, g_LightSpecular );
    glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, g_LighAttenuation0 );
    glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, g_LighAttenuation1 );
    glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, g_LighAttenuation2 );

    glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

    std::cout << "Initialise OpenGL: Success!" << std::endl;
}

void DrawAxis( float fScale, glm::vec3 translate = glm::vec3(0) )
{
    glPushAttrib( GL_ENABLE_BIT );

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );

    glPushMatrix();
    glTranslatef( translate.x, translate.y, translate.z );
    glScalef( fScale, fScale, fScale );
    glColor3f( 0.0f, 0.0f, 1.0f );

    glBegin( GL_LINES );
    {
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 0.0 );
        glVertex3f( 1.0f, 0.0f, 0.0f );

        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 1.0f, 0.0f );

        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( 0.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 1.0f );
    }
    glEnd();

    glPopMatrix();

    glPopAttrib();
}

void DisplayGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    g_Camera.ApplyViewTransform();

    DrawAxis( 20.0f, g_Camera.GetPivot() );

    glLightfv( GL_LIGHT0, GL_POSITION, g_LighPos );

    // Draw a debug sphere showing where the light is.positioned
    glTranslatef( g_LighPos[0], g_LighPos[1], g_LighPos[2] );
        glDisable( GL_LIGHTING );
        glColor3f( 1.0f, 1.0f, 0.0f );
        glutWireSphere( 1.0f, 8, 8 );
        glEnable( GL_LIGHTING );
    glTranslatef( -g_LighPos[0], -g_LighPos[1], -g_LighPos[2] );

    g_Model.Render();

    glutSwapBuffers();
    glutPostRedisplay();
}

void IdleGL()
{
    static ElapsedTime elapsedTime( 1/30.0f );
    float fDeltaTime = elapsedTime.GetElapsedTime();

    g_Model.Update( fDeltaTime );

    glutPostRedisplay();
}

void KeyboardGL( unsigned char c, int x, int y )
{
    switch ( c )
    {
    case 'r':
    case 'R':
        {   // Reset camera view
            g_Camera.SetPivot( g_InitialCameraPivot );
            g_Camera.SetRotate( g_InitialCameraRotation );
            g_Camera.SetTranslate( g_InitialCameraPosition );
        }
        break;
    case '\033': // escape quits
        {
            exit(0);
        }
        break;
    }

    glutPostRedisplay();
}

void MouseGL( int button, int state, int x, int y )
{
    if ( button == GLUT_LEFT_BUTTON )
    {
        g_bLeftMouseDown = ( state == GLUT_DOWN );
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        g_bRightMouseDown = ( state == GLUT_DOWN );
    }
    g_MousePrevious = glm::vec2( x, y );
}

void MotionGL( int x, int y )
{
    static ElapsedTime elapsedTime;
    float fDeltaTime = elapsedTime.GetElapsedTime();

    g_MouseCurrent = glm::vec2( x, y );
    g_MouseDelta = ( g_MousePrevious - g_MouseCurrent );

    // Update the camera
    if ( g_bLeftMouseDown && g_bRightMouseDown )
    {
        g_Camera.TranslateZ( g_MouseDelta.y );
    }
    else if ( g_bLeftMouseDown )
    {
        g_Camera.AddPitch( -g_MouseDelta.y );
        g_Camera.AddYaw( -g_MouseDelta.x );
    }
    else if ( g_bRightMouseDown )
    {
        g_Camera.TranslatePivotX( g_MouseDelta.x );
        g_Camera.TranslatePivotY( -g_MouseDelta.y );
    }

    g_MousePrevious = g_MouseCurrent;
}

void PassiveMotionGL( int x, int y )
{
}

void ReshapeGL( int w, int h )
{
    std::cout << "ReshapGL( " << w << ", " << h << " );" << std::endl;

    g_iWindowWidth = w;
    g_iWindowHeight = std::max(h,1);

    g_Camera.SetViewport( 0, 0, g_iWindowWidth, g_iWindowHeight );
    g_Camera.SetProjection( 45.0f, (float)g_iWindowWidth/(float)g_iWindowHeight, 0.1f, 1000.0f );

    g_Camera.ApplyViewport();
    g_Camera.ApplyProjectionTransform();

    glutPostRedisplay();
}
*/
