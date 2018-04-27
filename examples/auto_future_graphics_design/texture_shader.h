#pragma once
#include "basic3d.h"
class texture_shader :
	public basic_shader
{
	GLuint cubeTextId;
	GLuint _vertex_shader, _fragment_shader, _shader_program;
	GLuint _postion, _color, _texcord;
	GLuint _model, _view, _proj;
public:
	texture_shader(tri_mesh& tgmesh);
	~texture_shader();
	void load_tri_mesh();
	void render_tri_mesh();
	void set_vertex_wrold_matrix(glm::mat4& vtmat);
};
