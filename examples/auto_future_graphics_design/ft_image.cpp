#include "ft_image.h"
#include "res_output.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "common_functions.h"
/*
x'=(x-a)cos��+(y-b)sin��+a
y'=-(x-a)sin��+(y-b)cos��+b
*/

namespace auto_future
{
	ft_image::ft_image()
		:ft_base()
	{
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_img_pt, 4, [this](void*){
			ImGui::Combo("anchor type:", &_img_pt._anchor_type, "top left\0top right\0bottom right\0bottom left\0center\0\0");
		});
#endif
	}
	void ft_image::draw()
	{
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id();
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		if (_img_pt._texture_index_txt >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", _img_pt._texture_index_txt);
			_img_pt._texture_index_txt = 0;

		}
		int texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		int texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;
		float sizew = _img_pt._sizew;
		float sizeh = _img_pt._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

		ImVec2 uv0 = ImVec2(ptext_cd[_img_pt._texture_index_txt]._x0 / texture_width, ptext_cd[_img_pt._texture_index_txt]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[_img_pt._texture_index_txt]._x0 / texture_width, (ptext_cd[_img_pt._texture_index_txt]._y1) / texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[_img_pt._texture_index_txt]._x1) / texture_width, (ptext_cd[_img_pt._texture_index_txt]._y1) / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[_img_pt._texture_index_txt]._x1) / texture_width, (ptext_cd[_img_pt._texture_index_txt]._y0) / texture_height);

		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _img_pt._aposx + winpos.x, offsety + _img_pt._aposy + winpos.y };
		if (_img_pt._angle_srd != 0.f)
		{
			pos1 = rotate_point_by_zaxis(pos1, _img_pt._angle_srd, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, _img_pt._angle_srd, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, _img_pt._angle_srd, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, _img_pt._angle_srd, axisBasePos);
		}

		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
		ft_base::draw();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())//draw envelope
		{
			ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
			ImVec2 editunit(edit_unit_len, edit_unit_len);
			ImVec2 pos1a = pos1 - editunit;
			ImVec2 pos1b = pos1 + editunit;
			ImGui::RenderFrame(pos1a, pos1b, col);
			ImVec2 pos2a = pos2 - editunit;
			ImVec2 pos2b = pos2 + editunit;
			ImGui::RenderFrame(pos2a, pos2b, col);
			ImVec2 pos3a = pos3 - editunit;
			ImVec2 pos3b = pos3 + editunit;
			ImGui::RenderFrame(pos3a, pos3b, col);
			ImVec2 pos4a = pos4 - editunit;
			ImVec2 pos4b = pos4 + editunit;
			ImGui::RenderFrame(pos4a, pos4b, col);
			ImVec2 posaa = axisBasePos - editunit;
			ImVec2 posab = axisBasePos + editunit;
			ImGui::RenderFrame(posaa, posab, col);

		}
#endif

	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	base_ui_component* ft_image::get_hit_ui_object(float posx, float posy)
	{
		base_ui_component* hit_opt = ft_base::get_hit_ui_object(posx, posy);
		if (hit_opt)
		{
			return hit_opt;
		}
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1(pos0.x + _img_pt._sizew, pos0.y + _img_pt._sizeh);
		ImRect cover_area(pos0, pos1);
		ImVec2 mouse_pos(posx, posy);
		if (cover_area.Contains(mouse_pos))
		{
			return this;
		}
		else
		{
			return nullptr;
		}
	}

#endif
}