// OpenGL and FreeGlut headers.
#include <glew.h>
#include <freeglut.h>

// GLM.
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

// C++ STL headers.
#include <iostream>
#include <vector>
#include <stack>

// My headers.
#include "trianglemesh.h"
#include <conio.h> // dynamic load obj_file headers
#include <windows.h>
#include <commdlg.h>


using namespace std;
using namespace glm;

// Global variables.
const int screenWidth = 600; // 600
const int screenHeight = 600; // 600
TriangleMesh* mesh = nullptr;

// Function prototypes.
void SetupRenderState();
void SetupScene();
void ReleaseResources();
void RenderSceneCB();
void ProcessSpecialKeysCB(int, int, int);
void ProcessKeysCB(unsigned char, int, int);

// my Function prototype.
string openfilename();
void loadbuffer(bool load);
void start_load_file();

// my Global variables.
bool load = false;
mat4x4 MVP;
GLuint vbo, ibo;

// Callback function for glutDisplayFunc.
void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render the triangle mesh.
    // ---------------------------------------------------------------------------
    // Add your implementation.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), 0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glBindBuffer(GL_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, mesh->get_numVertexIndices(), GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    // ---------------------------------------------------------------------------

    glutSwapBuffers();
}

// Callback function for glutSpecialFunc.
void ProcessSpecialKeysCB(int key, int x, int y)
{
    // Handle special (functional) keyboard inputs such as F1, spacebar, page up, etc. 
    switch (key) {
    case GLUT_KEY_F1:
        // Render with point mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case GLUT_KEY_F2:
        // Render with line mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLUT_KEY_F3:
        // Render with fill mode.
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }
}

// Callback function for glutKeyboardFunc.
void ProcessKeysCB(unsigned char key, int x, int y)
{
    // Handle other keyboard inputs those are not defined as special keys.
    switch (key) {
    case 27: // keyboard "ESC"
        // Release memory allocation if needed.
        cout << "Esc" << endl;
        cout << "////////////////////////////////////////////////////////" << endl;
        ReleaseResources();
        exit(0);
        break;
    case 'n':
    case 'N':
        cout << key << "\nNext~~~" << endl;
        mesh->LoadFromFile(openfilename(), true);
        mesh->ShowInfo();
        mesh->ApplyTransformCPU(MVP);
        loadbuffer(load);
        break;
    case 'd':
    case 'D':
        cout << key << "\nDelete~~~~" << endl;
        cout << "--------------------------------------------------------" << endl;
        ReleaseResources();
        mesh->delete_model();
        mesh->ShowInfo();
        RenderSceneCB();
        break;
    default:
        break;
    }
}

void ReleaseResources()
{
    // Release dynamically allocated resources.
    // ---------------------------------------------------------------------------
    // Add your implementation.
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    load = false;
    // ---------------------------------------------------------------------------
}

// Set the color to clear the color buffer
void SetupRenderState()
{
    //glPointSize(10);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    vec4 clearColor = vec4(0.44f, 0.57f, 0.75f, 1.00f);
    glClearColor(
        (GLclampf)(clearColor.r), 
        (GLclampf)(clearColor.g), 
        (GLclampf)(clearColor.b), 
        (GLclampf)(clearColor.a)
    );
}

void SetupScene()
{
    // Load a model from obj file.
    mesh = new TriangleMesh();

    // ---------------------------------------------------------------------------
    // Add your implementation.
    mesh->LoadFromFile(openfilename(), true);
    mesh->ShowInfo();
    // ---------------------------------------------------------------------------
    // Build transformation matrices.
    mat4x4 M(1.0f);
    vec3 cameraPos = vec3(0.0f, 0.5f, 2.0f); // 0.0 0.5 2.0
    vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
    mat4x4 V = lookAt(cameraPos, cameraTarget, cameraUp);

    float fov = 40.0f;
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    float zNear = 0.1f;
    float zFar = 100.0f;
    mat4x4 P = perspective(radians(fov), aspectRatio, zNear, zFar);

    // Apply CPU transformation.
    MVP = P * V * M;
    mesh->ApplyTransformCPU(MVP);

    // ---------------------------------------------------------------------------
    // Add your implementation.
    loadbuffer(load); // Create buffer
    // ---------------------------------------------------------------------------
}

string openfilename() {
    OPENFILENAME ofn;
    wchar_t fileName[MAX_PATH] = L"";
    ZeroMemory(&fileName, sizeof(fileName));
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;  // If have a window to center over, put its HANDLE here
    ofn.lpstrFilter = L"OBJ File\0*.obj*\0"; // LPCWSTR代表“指向常量寬字串的長指標”。 W代表Wide，表示該字串以2位元組字元儲存，而不是普通的char。
    ofn.lpstrDefExt = L""; // 指向包含預設副檔名的緩衝。如果使用者忘記輸入副檔名，GetOpenFileName和GetSaveFileName附加這個副檔名到檔名中。
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    
    // wchar_t2wstring2string
    if (GetOpenFileName(&ofn)) {
        wstring fileNameWS(fileName);
        string fileNameStr(fileNameWS.begin(), fileNameWS.end());
        
        string fnstr; // F:\ComputerGraphics\HW\HW1\ICG2022_HW1_Test_Models\Cube.obj
        int start = 0, length = 1, count = 0;
        for (int i = fileNameStr.length() - 1; i >= 0; i--)
        {
            if (fileNameStr[i] == '\\') break;
            else
            {
               length++;
               start = i;
            }
        }
        fnstr = fileNameStr.substr(start, length);
        cout << "--------------------------------------------------------" << endl;
        cout << "Oh! you choice [" << fnstr << "] !" << endl;
        return fileNameStr;
    }
    return "error";
}

void loadbuffer(bool load)
{
    // Create vertex buffer.
    vector<VertexPTN > _vertices = mesh->get_vertices();
    //vector<vec3> _vertices = mesh->get_verticesP();
    if (!load) glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(VertexPTN) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::vec3) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

    // Create index buffer.
    vector<unsigned int> _vertexIndices = mesh->get_vertexIndices();
    //unsigned int* vertexIndices_array = &_vertexIndices[0]; ==> vector2array
    if (!load) glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _vertexIndices.size(), &_vertexIndices[0], GL_STATIC_DRAW); // sizeof(x) is get memory size
    load =  true;
}

void start_load_file()
{
    cout << "////////////////////////////////////////////////////////" << endl;
    cout << "Press [O] to load the obj file or Press [Esc] to leave." << endl;
    cout << "input[O/Esc]: ";
    int ch;
    while (1) {
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'o') {
                cout << char(ch) << "\npleace choice you want." << endl;
                break;
            }
            else if (ch == 27)
            {
                cout << "Esc" << endl;
                cout << "////////////////////////////////////////////////////////" << endl;
                exit(0);
                break;
            }
        }
    }
}


int main(int argc, char** argv)
{
    //press 'o' to start load obj file
    start_load_file();

    // Setting window properties.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("HW1: OBJ Loader");

    // Initialize GLEW.
    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        cerr << "GLEW initialization error: " 
                  << glewGetErrorString(res) << endl;
        return 1;
    }

    // Initialization.
    SetupRenderState();
    SetupScene();

    // Register callback functions.
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
    glutSpecialFunc(ProcessSpecialKeysCB);
    glutKeyboardFunc(ProcessKeysCB);

    // Start rendering loop.
    glutMainLoop();

    return 0;
}
