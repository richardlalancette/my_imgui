#include "ft_textblock_n.h"
#include "common_functions.h"
namespace auto_future
{
	static bool get_font_item(void* data, int idx, const char** out_str)
	{
		vfont_face_name& ft_nm_list = g_font_face_manager.get_font_name_list();
		*out_str = ft_nm_list[idx].c_str();
		return true;
	}
	ft_textblock_n::ft_textblock_n()
		: _txt_area(0.f, 0.f, 0.f, 0.f)
	{
		_pfont_res_set = make_shared<af_font_res_set>(g_font_face_manager);
		memset(_txt_pt._content, 0, MAX_CONTENT_LEN);
		_txt_pt._txt_clr = { 1.f, 1.f, 1.f };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_property_handle(&_txt_pt, 6, [this](void*){
			vfont_face_name& ft_nm_list = g_font_face_manager.get_font_name_list();
			ImGui::Combo("font:", &_txt_pt._font_id, &get_font_item, 0, ft_nm_list.size());
		});
		reg_property_handle(&_txt_pt,8, [this](void*){
			ImGui::DragFloat("Font scale", &_txt_pt._font_scale, 0.005f, 1.f, 10.0f, "%.1f");   // Scale only this font
			
		});
#endif
	}
	void ft_textblock_n::draw()
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 dpos = abpos + winpos;
		vfont_face_name& ft_nm_list = g_font_face_manager.get_font_name_list();
		string font_name = ft_nm_list[_txt_pt._font_id];
		float font_scale = _txt_pt._font_scale;

		const ImVec2 ctnt_size = ImGui::CalcTextSize(_txt_pt._content);
		dpos.x = dpos.x - ctnt_size.x*_txt_pt._txt_alignh_nml;
		dpos.y = dpos.y - ctnt_size.y*_txt_pt._txt_alignv_nml;
		_txt_area.Min = dpos;
		_txt_area.Max = _txt_area.Min + ctnt_size;
		af_vec2 draw_pos{ dpos.x, dpos.y };
		wstring draw_content = utf8ToWstring(_txt_pt._content);
		_pfont_res_set->draw_wstring(font_name, _txt_pt._font_size, draw_pos, _txt_pt._font_scale, draw_content, _txt_pt._txt_clr);
		ft_base::draw();

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())
		{
			ImVec2 winpos = ImGui::GetWindowPos();
			ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
			ImVec2 pos2 = { pos1.x, pos1.y + ctnt_size.y };
			ImVec2 pos3 = { pos1.x + ctnt_size.x, pos1.y + ctnt_size.y };
			ImVec2 pos4 = { pos1.x + ctnt_size.x, pos1.y };

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
			
		}
#endif
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	base_ui_component* ft_textblock_n::get_hit_ui_object(float posx, float posy)
	{
		base_ui_component* hit_opt = ft_base::get_hit_ui_object(posx, posy);
		if (hit_opt)
		{
			return hit_opt;
		}
		ImVec2 mouse_pos(posx, posy);
		if (_txt_area.Contains(mouse_pos))
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