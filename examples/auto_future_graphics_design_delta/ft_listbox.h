#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_listbox :
		public ft_base
	{
		DEF_STRUCT_WITH_INIT(intl_pt, _lt_pt,
			(bool, _vertical, { true }),
			(float, _sizew, { 100.f }),
			(float, _sizeh, { 100.f }),
			(float, _rangex, { 1.f }),
			(float, _rangey, { 100.f }),
			(float, _scroll_value, {0}))
		float _scroll_value = { 0.f };
		float scroll_max();
		void set_scroll_value(float scvalue);
	public:
		ft_listbox();
		void set_logic_scroll_value(float scvalue);
		void draw();
		void add_child(base_ui_component* pchild);
		void remove_child(base_ui_component* pchild);
		ImVec2 get_logic_range()
		{
			return ImVec2(_lt_pt._rangex, _lt_pt._rangey);
		}

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		base_ui_component* get_hit_ui_object(float posx, float posy);
		bool init_from_json(Value& jvalue);
		bool init_json_unit(Value& junit);
#endif

		bool handle_mouse();

	};
	REGISTER_CONTROL(ft_listbox)
}