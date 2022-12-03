#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

// OpenGL and FreeGlut headers.
#include <glew.h>
#include <freeglut.h>

// GLM.
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

// C++ STL headers.
#include <iostream>
#include <vector>
#include <map>
using namespace std;


// VertexPTN Declarations.
struct VertexPTN
{
	VertexPTN() {
		position = glm::vec3(0.0f, 0.0f, 0.0f); // v
		normal = glm::vec3(0.0f, 1.0f, 0.0f);   // vn
		texcoord = glm::vec2(0.0f, 0.0f);       // vt
	}
	VertexPTN(glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
		position = p;
		normal = n;
		texcoord = uv;
	}
	glm::vec3 position;		// Vertex position.
	glm::vec3 normal;		// Vertex normal.
	glm::vec2 texcoord;		// Vertex texture coordinate (will be used later HWs)
};

// TriangleMesh Declarations.
class TriangleMesh
{
public:
	// TriangleMesh Public Methods.
	TriangleMesh();
	~TriangleMesh();
	
	// Load the model from an *.OBJ file.
	bool LoadFromFile(const std::string& filePath, const bool normalized = true);
	
	// Apply transform on CPU.
	void ApplyTransformCPU(const glm::mat4x4& mvpMatrix);

	// Show model information.
	void ShowInfo();

	// ---------------------------------------------------------------------------
	// Add new methods if needed.
	vector<unsigned int> get_vertexIndices();
	vector<VertexPTN> get_vertices();
	vector<glm::vec3> get_verticesP();
	int get_numVertices();
	int get_numVertexIndices();
	int get_numTriangles();
	glm::vec3 refind_objExtent();
	void delete_model();
	void get_face_data(map<int, map<int, map<int, int> > > &v_table, vector<vector<int> > fd, vector <glm::vec3> v, vector <glm::vec2> vt, vector <glm::vec3> vn);
	void vertex_table(vector <glm::vec3> p, vector <glm::vec2> t, vector <glm::vec3> n);
	// ---------------------------------------------------------------------------
	vector <vector<vector<short> > > v_table_3d;
	vector <int> v_table_1d;
private:
	// TriangleMesh Private Methods.
	// ---------------------------------------------------------------------------
	// Add new methods if needed.
	// ---------------------------------------------------------------------------

	// TriangleMesh Private Data.
	std::vector<VertexPTN> vertices;
	std::vector<unsigned int> vertexIndices;
	int numVertices;
	int numVertexIndices;
	int numTriangles;
	glm::vec3 objCenter;
	glm::vec3 objExtent;
	// ---------------------------------------------------------------------------
	// Add new data if needed.
	// ---------------------------------------------------------------------------
};

#endif
