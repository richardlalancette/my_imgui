#include "ft_circle.h"
#include "basic3d.h"


namespace auto_future
{
	ft_circle::ft_circle()
	{
		//reg_property_handle(&_pt, 2, [this](void* memb_adress)
		//{
		//	ImGui::ColorEdit4("block color:", (float*)memb_adress);
		//});
	}
	void ft_circle::draw()
	{
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImU32 col = ImGui::ColorConvertFloat4ToU32(_pt._bkclr);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		int numb_segs = _pt._numb_segs;
		if (numb_segs<20)
		{
			numb_segs = 20;
		}
		if (_pt._solid)
		{
			draw_list->AddCircleFilled(pos0, _pt._radius, col, numb_segs);
		}
		else
		{
			draw_list->AddCircle(pos0, _pt._radius, col, numb_segs, _pt._thickness_utn);
		}
		ft_base::draw();
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	base_ui_component* ft_circle::get_hit_ui_object(float posx, float posy)
	{
		base_ui_component* hit_opt = ft_base::get_hit_ui_object(posx, posy);
		if (hit_opt)
		{
			return hit_opt;
		}
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		glm::vec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		glm::vec2 mouse_pos = { posx, posy };
		float dt=glm::length(mouse_pos - pos0);
		if (dt<=_pt._radius)
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