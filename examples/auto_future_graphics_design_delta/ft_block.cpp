#include "ft_block.h"


namespace auto_future
{
	void auto_future::ft_block::draw()
	{
		ft_base::draw();
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos0 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos1(pos0.x + _pt._sizew, pos0.y + _pt._sizew);
		ImVec4 BlockColor(_pt._bkr, _pt._bkg, _pt._bkb, _pt._bka);
		ImU32 col = ImGui::ColorConvertFloat4ToU32(BlockColor);
		ImGui::RenderFrame(pos0, pos1, col);

	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	void ft_block::draw_peroperty_page(int property_part /*= -1*/)
	{
		ft_base::draw_peroperty_page();
		ImGui::Spacing();
		ImGui::Text("size:");
		ImGui::SliderFloat("w", &_pt._sizew, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("h", &_pt._sizeh, 0.f, base_ui_component::screenh);
		ImGui::ColorEdit4("block color:", (float*)&_pt._bkr);
	}

	bool ft_block::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		Value& bsize = jvalue["size"];
		_pt._sizew = bsize["x"].asDouble();
		_pt._sizeh = bsize["y"].asDouble();
		Value& block_color = jvalue["block_color"];
		_pt._bkr = block_color["x"].asDouble();
		_pt._bkg = block_color["y"].asDouble();
		_pt._bkb = block_color["z"].asDouble();
		_pt._bka = block_color["w"].asDouble();
		return true;
	}

	bool ft_block::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		Value bsize(objectValue);
		bsize["x"] = _pt._sizew;
		bsize["y"] = _pt._sizeh;
		junit["size"] = bsize;
		Value block_color(objectValue);
		block_color["x"] = _pt._bkr;
		block_color["y"] = _pt._bkg;
		block_color["z"] = _pt._bkb;
		block_color["w"] = _pt._bka;
		junit["block_color"] = block_color;
		return true;
	}
#endif
}