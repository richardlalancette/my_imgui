#include "ft_base.h"
#include <GLFW/glfw3.h>
#include <typeinfo>

namespace auto_future
{
	void ft_base::draw()
	{
		if (!is_visible())
		{
			return;
		}
		for (auto it : _vchilds)
		{
			if (it->is_visible())
			{
				it->draw();
			}
		}
		/*for (auto it = _vchilds.rbegin(); it != _vchilds.rend(); it++)
		{

			if ((*it)->is_visible())
			{
				(*it)->draw();
			}
		}*/
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)	
	base_ui_component* ft_base::get_hit_ui_object(float posx, float posy)
	{
		for (auto it = _vchilds.rbegin(); it != _vchilds.rend(); it++)
		{
			if ((*it)->is_visible())
			{
				base_ui_component* hit_object = (*it)->get_hit_ui_object(posx, posy);
				if (hit_object)
				{
					return hit_object;
				}
			}

			//hit_object = (*it).get_hit_ui_object(posx, posy);
		}
		return nullptr;
	}
#endif

	bool ft_base::handle_mouse()
	{
		if (!is_visible())
		{
			return false;
		}
		for (auto it : _vchilds)
		{
			if (!it->handle_mouse())
				return false;
		}
		return true;
	}
	base_ui_component* ft_base::get_copy_of_object()
	{
		string cname = typeid(*this).name();
		cname = cname.substr(sizeof("class autofuture::"));
		base_ui_component* prtn = factory::get().produce(cname);
		vproperty_list vrtn, vobj;
		prtn->collect_property_range(vrtn);
		this->collect_property_range(vobj);
		property_copy(vrtn, vobj);
		auto icnt = this->get_child_count();
		for (int ii = 0; ii < icnt; ii++)
		{
			auto pchild = this->get_child(ii);
			auto pchd_cpy = pchild->get_copy_of_object();
			prtn->add_child(pchd_cpy);
		}
		return prtn;
	}
	base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname)
	{
		if (tar_ui.get_name() == uname)
		{
			return&tar_ui;
		}
		for (auto it : tar_ui._vchilds)
		{
			base_ui_component* des_ui = find_a_uc_from_uc(*it, uname);
			if (des_ui)
			{
				return des_ui;
			}
		}
		return NULL;
	}
	float base_ui_component::screenw = 1920.f;
	float base_ui_component::screenh =720.f;

	void property_copy(vproperty_list& vdest, vproperty_list& vsource)
	{
		for (size_t i = 0; i < vdest.size(); i++)
		{
			memcpy(vdest[i]._p_head_address, vsource[i]._p_head_address, vdest[i]._len);
		}
	}

	base_ui_component* get_copy_of_object(base_ui_component* byobject)
	{
		string cname = typeid(*byobject).name();
		cname = cname.substr(sizeof("class autofuture::"));
		base_ui_component* prtn = factory::get().produce(cname);
		vproperty_list vrtn, vobj;
		prtn->collect_property_range(vrtn);
		byobject->collect_property_range(vobj);
		property_copy(vrtn, vobj);
		auto icnt = byobject->get_child_count();
		for (int ii = 0; ii < icnt; ii++)
		{
			auto pchild = byobject->get_child(ii);
			auto pchd_cpy = get_copy_of_object(pchild);
			prtn->add_child(pchd_cpy);
		}
		return prtn;
	}
}