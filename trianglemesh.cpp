#include "trianglemesh.h"
#include <iostream>
#include<fstream>
#include<sstream>
#include<string>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;
using namespace glm;


// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numVertexIndices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
	// ---------------------------------------------------------------------------
    // Feel free to add codes.
    // ---------------------------------------------------------------------------
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	vertices.clear();
	vertexIndices.clear();
	// ---------------------------------------------------------------------------
    // Feel free to add codes.
    // ---------------------------------------------------------------------------
}

// ---------------------------------------------------------------------------
// get the face data for three point(三點成一面的資料)
void TriangleMesh::get_face_data(map<int, map<int, map<int, int> > > &v_table, vector<vector<int> > fd, vector <vec3> v, vector <vec2> vt, vector <vec3> vn) {
	VertexPTN tmp;
	int indx = 0;
	int p = 0, t = 0, n = 0;
	map<int, map<int, map<int, int> > >::iterator it_p;
	map<int, map<int, int> >::iterator it_t;
	map<int, int>::iterator it_n;
	for (int i = 0; i < fd.size(); i++)
	{
		p = fd[i][0];
		t = fd[i][1];
		n = fd[i][2];
		it_p = v_table.find(p);
		if (it_p != v_table.end())
		{
			it_t = v_table[p].find(t);
			if (it_t != v_table[p].end())
			{
				it_n = v_table[p][t].find(n);
				if (it_n != v_table[p][t].end())
				{
					vertexIndices.push_back(v_table[p][t][n]);
				}
				else
				{
					v_table[p][t][n] = indx;
					tmp.position = v[p];
					tmp.texcoord = vt[t];
					tmp.normal = vn[n];
					vertices.push_back(tmp);
					vertexIndices.push_back(indx);
					indx++;
				}
			}
			else
			{
				map<int, int>  ptn_t;
				v_table[p][t] = ptn_t; //沒有t，給這個p加一個key為t，value為空map的node
				v_table[p][t][n] = indx; //沒有n，給這個t加一個key為n，value為indx的node
				tmp.position = v[p];
				tmp.texcoord = vt[t];
				tmp.normal = vn[n];
				vertices.push_back(tmp);
				vertexIndices.push_back(indx);
				indx++;
			}
		}
		else
		{
			map<int, map<int, int> > ptn_p;
			map<int, int>  ptn_t;
			v_table[p] = ptn_p;
			v_table[p][t] = ptn_t;
			v_table[p][t][n] = indx;
			tmp.position = v[p];
			tmp.texcoord = vt[t];
			tmp.normal = vn[n];
			vertices.push_back(tmp);
			vertexIndices.push_back(indx);
			indx++;
		}
	}
}

// get the vertexIndices
vector<unsigned int> TriangleMesh::get_vertexIndices() { return vertexIndices; }

// get the vertices
vector<VertexPTN> TriangleMesh::get_vertices() { return vertices; }

//get the vertices.position
vector<vec3> TriangleMesh::get_verticesP()
{
	vector<vec3> vp;
	for (int i = 0; i < vertices.size(); i++)
		vp.push_back(vertices[i].position);
	return vp;
}

// get the numVertices
int TriangleMesh::get_numVertices() { return numVertices; }

// get the numVertexIndices
int TriangleMesh::get_numVertexIndices() { return numVertexIndices; }

// get the numTriangles
int TriangleMesh::get_numTriangles() { return numTriangles; }

// refind max(x,y,z) and min(x, y, z)
vec3 TriangleMesh::refind_objExtent()
{
	vec3 newObjExtent = vec3(0.0f, 0.0f, 0.0f);
	float max_x = 0.0, max_y = 0.0, max_z = 0.0,
		min_x = 0.0, min_y = 0.0, min_z = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		float x = vertices[i].position[0], y = vertices[i].position[1], z = vertices[i].position[2];
		//cout << "x = " << vertices[i].position[0] << ", y = " << vertices[i].position[1] << ", z = " << vertices[i].position[2] << endl;
		if (x < min_x) min_x = x;
		if (y < min_y) min_y = y;
		if (z < min_z) min_z = z;
		if (x > max_x) max_x = x;
		if (y > max_y) max_y = y;
		if (z > max_z) max_z = z;
	}
	newObjExtent = vec3(max_x - min_x, max_y - min_y, max_z - min_z);
	return newObjExtent;
}

void TriangleMesh::delete_model()
{
	vertices.clear();
	vertexIndices.clear();
	numVertices = 0;
	numVertexIndices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
}

// ---------------------------------------------------------------------------
// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const bool normalized)
{	
	vertices.clear();
	vertexIndices.clear();
	numVertices = 0;
	numVertexIndices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
	// Parse the OBJ file.
	ifstream file;
	string line;

	// ---------------------------------------------------------------------------
    // Add your implementation.
	float max_x = 0.0, max_y = 0.0, max_z = 0.0,
		min_x = 0.0, min_y = 0.0, min_z = 0.0;
	bool firstPosion = true;

	// load file
	file.open(filePath, ios::in);
	if (!file.is_open()) {
		cout << "file is not open !" << endl;
		exit(0);
		return false;
	}

	vector <vec3> v;
	vector <vec2> vt;
	vector <vec3> vn;
	vector<vector<int> > f;
	map<int, map<int, map<int, int> > > v_table;
	bool lock = false;
	int row = 0, number = 0;
	while (getline(file, line)) {
		if (line.substr(0, 2) == "v ") // 'v' ==> read vertex position (頂點)
		{
			GLfloat x_v, y_v, z_v;
			istringstream s(line.substr(2));
			s >> x_v;
			s >> y_v;
			s >> z_v;
			vec3 p = vec3(x_v, y_v, z_v);
			v.push_back(p);
			if (firstPosion)
			{
				max_x = x_v, max_y = y_v, max_z = z_v;
				min_x = x_v, min_y = y_v, min_z = z_v;
				firstPosion = false;
			}
			else
			{
				if (x_v < min_x) min_x = x_v;
				if (y_v < min_y) min_y = y_v;
				if (z_v < min_z) min_z = z_v;
				if (x_v > max_x) max_x = x_v;
				if (y_v > max_y) max_y = y_v;
				if (z_v > max_z) max_z = z_v;
			}
		}
		else if (line.substr(0, 2) == "vt")
		{
			GLfloat t1, t2;
			istringstream s(line.substr(2));
			s >> t1;
			s >> t2;
			vec2 t = vec2(t1, t2);
			vt.push_back(t);
		}
		else if (line.substr(0, 2) == "vn")
		{
			GLfloat x_n, y_n, z_n;
			istringstream s(line.substr(2));
			s >> x_n;
			s >> y_n;
			s >> z_n;
			vec3 n = vec3(x_n, y_n, z_n);
			vn.push_back(n);
		}
		else if (line.substr(0, 1) == "f") // 'f' ==> read triangle's 3 vertex (哪幾個頂點構成一個面)
		{
			istringstream s(line.substr(2));
			string fd;
			int count = 0;
			vector <vector<int> > f_indx;
			while (s >> fd) // f <-- "p/t/n"
			{				
				replace(fd.begin(), fd.end(), '/', ' ');
				istringstream s_fd(fd);
				string str;
				vector<int> ptn;
				int p = 0, t = 0, n = 0;
				for (int i = 0; i < 3; i++)
				{
					s_fd >> str;
					ptn.push_back(abs(stoi(str)) - 1);
				}
				if (count < 3)
				{
					f.push_back(ptn);
					f_indx.push_back(ptn);
				}
				else
				{
					f.push_back(f_indx[0]);
					f.push_back(f_indx[count - 1]);
					f.push_back(ptn);
					f_indx.push_back(ptn);
				}
				count++;
			}
		}
	}
	file.close();

	get_face_data(v_table, f, v, vt, vn);
	numVertices = vertices.size();
	numVertexIndices = vertexIndices.size();
	numTriangles = vertexIndices.size() / 3;

	objCenter = vec3((max_x + min_x) / 2.0, (max_y + min_y) / 2.0, (max_z + min_z) / 2.0);
	objExtent = vec3(max_x - min_x, max_y - min_y, max_z - min_z);

    // ---------------------------------------------------------------------------
	// Normalize the geometry data.
	if (normalized) {
		cout << "normalized: " << endl;

		// -----------------------------------------------------------------------
		// Add your implementation.
		float max_edge = 0.0;
		max_edge = objExtent.x;
		if (max_edge < objExtent.y) max_edge = objExtent.y;
		if (max_edge < objExtent.z) max_edge = objExtent.z;

		for (int i = 0; i < vertices.size(); i++)
		{
			vec3 tmp = vertices[i].position;
			// move to center
			vertices[i].position = vec3(tmp.x - objCenter.x, tmp.y - objCenter.y, tmp.z - objCenter.z);

			tmp = vertices[i].position;
			// normalize size
			vertices[i].position = vec3(tmp.x / max_edge, tmp.y / max_edge, tmp.z / max_edge);
		}
		objCenter = vec3(0.0f, 0.0f, 0.0f);
		objExtent = refind_objExtent();
		// -----------------------------------------------------------------------
	}
	return true;
}

// Apply transform to vertices using GPU.
void TriangleMesh::ApplyTransformCPU(const glm::mat4x4& mvpMatrix)
{
	for (int i = 0 ; i < numVertices; ++i) {
        glm::vec4 p = mvpMatrix * glm::vec4(vertices[i].position, 1.0f);
        if (p.w != 0.0f) {
            float inv = 1.0f / p.w; 
            vertices[i].position.x = p.x * inv;
            vertices[i].position.y = p.y * inv;
            vertices[i].position.z = p.z * inv;
        }
    }
}

// Show model information.
void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
	cout << "////////////////////////////////////////////////////////" << endl;
	cout << "Press [N] to load next model.\n" << "Press [D] to delete model.\n" << "Press [Esc] to leave." << endl;
	cout << "input[N/D/Esc]: ";
}
