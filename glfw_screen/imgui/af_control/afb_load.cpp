
//#include <algorithm>
//#include <GLES2/gl2.h>
#include <memory>
//#include <map>
//#include <iostream>
//#include "property_utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


#include "afb_load.h"
#include <msgpack.hpp>
#include "factory.h"
#include "res_output.h"
#include <fstream>
#include "imgui.h"
//#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "af_shader.h"
#include "material.h"
#define DXT5_DECOMPRESSED
#ifdef DXT5_DECOMPRESSED
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif // DEBUG
#include "miniz.h"


void init_ui_component_by_mgo(base_ui_component*&ptar, msgpack::v2::object& obj)
{
	auto obj_arr_sz = obj.via.array.size;
	auto obj_typename = obj.via.array.ptr[0];
	auto obj_tp_nmlen = obj_typename.via.str.size;
	char* str_type_name = new char[obj_tp_nmlen + 1];
	memset(str_type_name, 0, obj_tp_nmlen + 1);
	memcpy(str_type_name, obj_typename.via.str.ptr, obj_tp_nmlen);
	ptar = factory::get().produce(str_type_name);
	vproperty_list vplist;
	int mlen=ptar->collect_property_range(vplist);
	auto mem_obj=obj.via.array.ptr[1];
	auto bin_sz = mem_obj.via.bin.size;
	uint8_t* pmem = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(mem_obj.via.bin.ptr));
	//uint8_t* pmem_bk = pmem;
	for (auto& munit : vplist)
	{
		memcpy(munit._p_head_address, pmem, munit._len);
		pmem += munit._len;
	}
	//int bksz = pmem - pmem_bk;
	//cout << "peroperty_sizeof " << str_type_name << " is " << bin_sz << "bksz is"<<bksz<< endl;
	ptar->link();
	delete[] str_type_name;
	if (obj_arr_sz>2)
	{
		auto childs_obj = obj.via.array.ptr[2];
		auto cd_sz = childs_obj.via.array.size;
		for (size_t ix = 0; ix < cd_sz; ix++)
		{
			auto cd_obj = childs_obj.via.array.ptr[ix];
			base_ui_component* pchild;
			init_ui_component_by_mgo(pchild, cd_obj);
			ptar->add_child(pchild);
		}
	}
}
extern GLuint       g_FontTexture;
//#define _DEBUG
#ifdef _DEBUG
#include <chrono>
#define _CHECK_TIME_CONSUME
#endif

void afb_load::load_afb(const char* afb_file)
{
#ifdef _CHECK_TIME_CONSUME
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto currentTime = lastTime;
	int delta = 0;
#define _STR(x) #x
#define STR(x) _STR(x)
#define TIME_CHECK(x) 	currentTime = std::chrono::high_resolution_clock::now();\
	delta = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(currentTime - lastTime).count();\
		printf(STR(x)" consume %d milli secs\n", delta);lastTime = currentTime;
	
#else
#define TIME_CHECK(x)
#endif 

#if 0
	msgpack::unpacker unpac;
	ifstream fin;
	fin.open(afb_file, ios::binary );
	if (!fin.is_open())
	{
		printf("invalid afb file:%s\n", afb_file);
	}
	else{
		printf("%s is opened\n",afb_file);
	}
	auto file_size=fin.tellg();
	fin.seekg(0, ios::end);
	file_size = fin.tellg() - file_size;
	fin.seekg(0, ios::beg);
	int afb_name_len = strlen(afb_file);
	if (afb_file[afb_name_len-1]=='C'||afb_file[afb_name_len-1]=='c')
	{
		uint8_t* pbuff = new uint8_t[file_size];
		fin.read((char*)pbuff, file_size);
		fin.close();

		int pre_buff_size = file_size * 60;
		uint8_t* pin_buff = new uint8_t[pre_buff_size];
		mz_stream stream = {};
		mz_inflateInit(&stream);
		stream.avail_in = (int)file_size;
		stream.next_in = pbuff;
		stream.avail_out = pre_buff_size;
		stream.next_out = pin_buff;
		mz_inflate(&stream, Z_SYNC_FLUSH);
		if (mz_inflateEnd(&stream) != Z_OK)
		{
			printf("fail to inflate!\n");
			delete[] pbuff;
			delete[] pin_buff;
			return;
		}
		unpac.reserve_buffer(stream.total_out);
		memcpy(unpac.buffer(), pin_buff, stream.total_out);
		unpac.buffer_consumed(static_cast<size_t>(stream.total_out));
		delete[] pbuff;
		delete[] pin_buff;	
	}
	else
	{
		unpac.reserve_buffer(file_size);
		fin.read(unpac.buffer(), unpac.buffer_capacity());
		unpac.buffer_consumed(static_cast<size_t>(fin.gcount()));
	}
#else
	int fd;
	struct stat sb;
	fd=open(afb_file,O_RDONLY);
	fstat(fd,&sb);
	//msgpack::unpacker unpac;
	//unpac.reserve_buffer(sb.st_size);
	void* membk=mmap(0,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
	if(membk==MAP_FAILED)
	{
		printf("fail to map\n");
		return;
	}
	
	TIME_CHECK(mmap afb)
	//read(fd,unpac.buffer(),sb.st_size);
	close(fd);
	//msgpack::unpacker unpac(sized_reference,membk,sb.st_size,msgpack::unpack_limit());
       //memcpy(unpac.buffer(),membk,sb.st_size);
	
       //unpac.buffer_consumed(sb.st_size);
#endif
	msgpack::object_handle oh=msgpack::unpack((const char*)membk,sb.st_size);
	//msgpack::object_handle oh;
	TIME_CHECK(consumed afb)
	//unpac.next(oh);
	auto obj_w = oh.get();
	base_ui_component::screenw = obj_w.via.array.ptr[0].as<float>();
	base_ui_component::screenh = obj_w.via.array.ptr[1].as<float>();
	ImFontAtlas* atlas = ImGui::GetIO().Fonts;
	atlas->TexWidth = obj_w.via.array.ptr[2].as<int>();
	atlas->TexHeight = obj_w.via.array.ptr[3].as<int>();
	auto font_txt = obj_w.via.array.ptr[4];
	auto txt_sz = font_txt.via.bin.size;
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas->TexWidth, atlas->TexHeight,0, GL_RGBA, GL_UNSIGNED_BYTE, font_txt.via.bin.ptr);
	#ifdef _SEE_SUPPORTED__
	string externsions=(const char*)glGetString(GL_EXTENSIONS);
	printf("externsions:%s\n",externsions.c_str());

	GLint numFormats=0;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS,&numFormats);
	GLint* formats=new GLint[numFormats];
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS,formats);
	for(int i=0;i<numFormats;i++)
    {
        printf("fmt:0x%x\n",formats[i]);
    }
    delete[] formats;
    #endif
#ifndef DXT5_DECOMPRESSED
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas->TexWidth, atlas->TexHeight,0, GL_RGBA, GL_UNSIGNED_BYTE, font_txt.via.bin.ptr);
	//GL_NUM_COMPRESSED_TEXTURE_FORMATS		GL_COMPRESSED_TEXTURE_FORMATS
#else
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, atlas->TexWidth, atlas->TexHeight, 0, font_txt.via.bin.size, font_txt.via.bin.ptr);
#endif
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	atlas->TexID = (void *)(intptr_t)g_FontTexture;
	TIME_CHECK(font atlas texture)
	auto font_res = obj_w.via.array.ptr[5];
	auto font_cnt = font_res.via.array.size;
	for (size_t ix = 0; ix < font_cnt; ix++)
	{
		atlas->Fonts.push_back(IM_NEW(ImFont));
		auto& font = atlas->Fonts.back();
		font->ContainerAtlas = atlas;
		auto font_unit = font_res.via.array.ptr[ix];
		font->FontSize = font_unit.via.array.ptr[0].as<float>();
		font->Ascent = font_unit.via.array.ptr[1].as<float>();
		font->Descent = font_unit.via.array.ptr[2].as<float>();
		font->MetricsTotalSurface=font_unit.via.array.ptr[3].as<int>();
		auto glyph = font_unit.via.array.ptr[4];
		auto glyph_sz = glyph.via.bin.size;
		auto vglph_sz = glyph_sz / sizeof(ImFontGlyph);
		font->Glyphs.resize(vglph_sz);
		memcpy(font->Glyphs.Data, glyph.via.bin.ptr, glyph_sz);
		auto indexAdvance = font_unit.via.array.ptr[5];
		auto idx_sz = indexAdvance.via.bin.size;
		auto vidx_sz = idx_sz / sizeof(float);
		font->IndexAdvanceX.resize(vidx_sz);
		memcpy(font->IndexAdvanceX.Data, indexAdvance.via.bin.ptr, idx_sz);
		auto lookup = font_unit.via.array.ptr[6];
		auto lookup_sz = lookup.via.bin.size;
		auto vlk_sz = lookup_sz / sizeof(unsigned short);
		font->IndexLookup.resize(vlk_sz);
		memcpy(font->IndexLookup.Data, lookup.via.bin.ptr, lookup_sz);
		font->DirtyLookupTables = false;
		font->FallbackAdvanceX = 7.97315454f;
				//font->DisplayOffset.y = 3.f;
		auto is_cur_font = font_unit.via.array.ptr[7].as<bool>();
		if (is_cur_font)
		{
			ImGui::GetIO().FontDefault = font;
		}
	}
	TIME_CHECK(Font atalas)
	auto obj_format = obj_w.via.array.ptr[6];
	g_output_bin_format._txt_fmt = static_cast<texture_format>(obj_format.via.array.ptr[0].as<int>());
	g_output_bin_format._pgm_fmt = static_cast<program_format>(obj_format.via.array.ptr[1].as<int>());

	auto obj_res = obj_w.via.array.ptr[7];
	auto re_cnt = obj_res.via.array.size;
	function<unsigned int(const char*, int, int, unsigned int)> f_gen_txt;
	if (g_output_bin_format._txt_fmt == en_uncompressed_txt){
		f_gen_txt = [](const char* ptxt_data, int iw, int ih, unsigned int bsz){
			GLuint txt_id;
			glGenTextures(1, &txt_id);
			glBindTexture(GL_TEXTURE_2D, txt_id);
			//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Step3 设定filter参数
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iw, ih,
				0, GL_RGBA, GL_UNSIGNED_BYTE, ptxt_data);

			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return txt_id;
		};
	}
	else if (g_output_bin_format._txt_fmt == en_dxt5){
		f_gen_txt = [](const char* ptxt_data, int iw, int ih, unsigned int bsz){
			GLuint txt_id;
			glGenTextures(1, &txt_id);
			glBindTexture(GL_TEXTURE_2D, txt_id);
			//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Step3 设定filter参数
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, iw, ih, 0, bsz, ptxt_data);

			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			return txt_id;
		};
	}
	for (size_t ix = 0; ix < re_cnt; ix++)
	{
		g_vres_texture_list.emplace_back(res_texture_list());
		int cur_pos = g_vres_texture_list.size() - 1;
		res_texture_list& res_unit = g_vres_texture_list[cur_pos];

		auto bin_res_unit = obj_res.via.array.ptr[ix];
		res_unit.texture_width = bin_res_unit.via.array.ptr[0].as<int>();
		res_unit.texture_height = bin_res_unit.via.array.ptr[1].as<int>();
		auto res_bin = bin_res_unit.via.array.ptr[2];
		auto bin_sz = res_bin.via.bin.size;
		res_unit.texture_id = f_gen_txt(res_bin.via.bin.ptr, res_unit.texture_width, res_unit.texture_height, bin_sz);

		auto res_data = bin_res_unit.via.array.ptr[3];
		auto res_data_sz = res_data.via.array.size;
		for (size_t iy = 0; iy < res_data_sz; iy++)
		{
			res_unit.vtexture_coordinates.emplace_back(res_texture_coordinate());
			int curpos = res_unit.vtexture_coordinates.size() - 1;
			res_texture_coordinate& cd_unit = res_unit.vtexture_coordinates[curpos];
			auto bin_cd_unit = res_data.via.array.ptr[iy];
			auto bin_filen = bin_cd_unit.via.array.ptr[0];
			cd_unit._file_name.reserve(bin_filen.via.str.size + 1);
			cd_unit._file_name.resize(bin_filen.via.str.size + 1);
			memcpy(&cd_unit._file_name[0], bin_filen.via.str.ptr, bin_filen.via.str.size);
			cd_unit._x0 = bin_cd_unit.via.array.ptr[1].as<float>();
			cd_unit._x1 = bin_cd_unit.via.array.ptr[2].as<float>();
			cd_unit._y0 = bin_cd_unit.via.array.ptr[3].as<float>();
			cd_unit._y1 = bin_cd_unit.via.array.ptr[4].as<float>();
		}

	}

	TIME_CHECK(globe texture res)
	auto obj_txt_list = obj_w.via.array.ptr[8];
	auto txt_cnt = obj_txt_list.via.array.size;
	for (size_t ix = 0; ix < txt_cnt;ix++)
	{
		auto txt_unit = obj_txt_list.via.array.ptr[ix];
		auto txt_name = txt_unit.via.array.ptr[0];
		auto txt_width = txt_unit.via.array.ptr[1].as<uint32_t>();
		auto txt_height = txt_unit.via.array.ptr[2].as<uint32_t>();
		auto txt_bin = txt_unit.via.array.ptr[3];
		auto txt_name_sz = txt_name.via.str.size;
		char* txt_kname = new char[txt_name_sz+1];
		memset(txt_kname, 0, txt_name_sz + 1);
		memcpy(txt_kname, txt_name.via.str.ptr, txt_name_sz);
		auto txt_bin_sz = txt_bin.via.bin.size;
		auto a_txt = make_shared<af_texture>();
		a_txt->_width = txt_width;
		a_txt->_height = txt_height;
		a_txt->_txt_id = f_gen_txt(txt_bin.via.bin.ptr, txt_width, txt_height, txt_bin_sz);
		
		g_mtexture_list[txt_kname] = a_txt;
		delete[] txt_kname;

	}
	TIME_CHECK(texture list res)
	auto obj_file_list = obj_w.via.array.ptr[9];
	auto file_cnt = obj_file_list.via.array.size;
	for (size_t ix = 0; ix < file_cnt;ix++)
	{
		auto file_unit = obj_file_list.via.array.ptr[ix];
		auto file_name = file_unit.via.array.ptr[0];
		auto file_bin = file_unit.via.array.ptr[1];
		auto file_name_size = file_name.via.str.size;
		char* file_kname = new char[file_name_size + 1];
		memset(file_kname, 0, file_name_size + 1);
		memcpy(file_kname, file_name.via.str.ptr, file_name_size);
		auto a_file = make_shared<af_file>(file_bin.via.bin.size);
		memcpy(a_file->_pbin, file_bin.via.bin.ptr, a_file->_fsize);
		g_mfiles_list[file_kname] = a_file;
		delete[] file_kname;
	}

	TIME_CHECK(file list res)
	auto obj_shader_list = obj_w.via.array.ptr[10];
	auto shd_cnt = obj_shader_list.via.array.size;
	if (g_output_bin_format._pgm_fmt == en_shader_code)
	{
		for (size_t ix = 0; ix < shd_cnt; ix++)
		{
			auto shd_unit = obj_shader_list.via.array.ptr[ix];
			auto shd_name = shd_unit.via.array.ptr[0];
			auto str_shd_nm_sz = shd_name.via.str.size;
			char* pshd_name = new char[str_shd_nm_sz + 1];
			memset(pshd_name, 0, str_shd_nm_sz + 1);
			memcpy(pshd_name, shd_name.via.str.ptr, str_shd_nm_sz);
			auto shd_vs_code = shd_unit.via.array.ptr[1];
			auto str_vs_code_sz = shd_vs_code.via.str.size;
			char* psd_vs_code = new char[str_vs_code_sz + 1];
			memset(psd_vs_code, 0, str_vs_code_sz + 1);
			memcpy(psd_vs_code, shd_vs_code.via.str.ptr, str_vs_code_sz);
			auto shd_fs_code = shd_unit.via.array.ptr[2];
			auto str_fs_code_sz = shd_fs_code.via.str.size;
			char* psd_fs_code = new char[str_fs_code_sz + 1];
			memset(psd_fs_code, 0, str_fs_code_sz + 1);
			memcpy(psd_fs_code, shd_fs_code.via.str.ptr, str_fs_code_sz);
			g_af_shader_list[pshd_name] = make_shared<af_shader>(psd_vs_code, psd_fs_code);
			delete[] pshd_name;
			delete[] psd_vs_code;
			delete[] psd_fs_code;
		}
	}
	#ifdef GLFW_INCLUDE_ES3
	else if (g_output_bin_format._pgm_fmt == en_shader_bin_general)
	{
		for (size_t ix = 0; ix < shd_cnt; ix++)
		{
			auto shd_unit = obj_shader_list.via.array.ptr[ix];
			auto shd_name = shd_unit.via.array.ptr[0];
			auto str_shd_nm_sz = shd_name.via.str.size;
			char* pshd_name = new char[str_shd_nm_sz + 1];
			memset(pshd_name, 0, str_shd_nm_sz + 1);
			memcpy(pshd_name, shd_name.via.str.ptr, str_shd_nm_sz);
			GLenum binaryFormat = shd_unit.via.array.ptr[1].as<int>();
			auto binCode = shd_unit.via.array.ptr[2];
			auto binCodeSize = binCode.via.bin.size;
			auto pbin = binCode.via.bin.ptr;
			g_af_shader_list[pshd_name] = make_shared<af_shader>(binaryFormat, (void*)pbin, binCodeSize);
			delete[] pshd_name;
		}
	}
	#endif
	TIME_CHECK(shader list res)
	auto obj_material_list = obj_w.via.array.ptr[11];
	auto mtl_cnt = obj_material_list.via.array.size;
	for (size_t ix = 0; ix < mtl_cnt; ix++)
	{
		auto mtl_unit = obj_material_list.via.array.ptr[ix];
		auto mtl_name = mtl_unit.via.array.ptr[0];
		auto mtl_name_size = mtl_name.via.str.size;
		char* pstr_mtl_name = new char[mtl_name_size + 1];
		memset(pstr_mtl_name, 0, mtl_name_size + 1);
		memcpy(pstr_mtl_name, mtl_name.via.str.ptr, mtl_name_size);
		shared_ptr<material> pmtl = make_shared<material>();
		g_material_list[pstr_mtl_name] = pmtl;
		auto shd_name = mtl_unit.via.array.ptr[1];
		auto shd_name_size = shd_name.via.str.size;
		char* pstr_shd_name = new char[shd_name_size + 1];
		memset(pstr_shd_name, 0, shd_name_size+1);
		memcpy(pstr_shd_name, shd_name.via.str.ptr, shd_name_size);
		auto pshd = g_af_shader_list.find(pstr_shd_name);
		if (pshd != g_af_shader_list.end())
		{
			pmtl->set_pshader(pshd->second);
		}
		auto& mpshd_uf = pmtl->get_mp_sd_uf();
		auto shd_uf_list = mtl_unit.via.array.ptr[2];
		auto shd_uf_list_cnt = shd_uf_list.via.array.size;
		for (size_t iy = 0; iy < shd_uf_list_cnt; iy++)
		{
			auto shd_uf_unit = shd_uf_list.via.array.ptr[iy];
			auto shd_uf_name = shd_uf_unit.via.array.ptr[0];
			auto shd_uf_name_sz = shd_uf_name.via.str.size;
			char* pshd_uf_name = new char[shd_uf_name_sz + 1];
			memset(pshd_uf_name, 0, shd_uf_name_sz + 1);
			memcpy(pshd_uf_name, shd_uf_name.via.str.ptr, shd_uf_name_sz);
			auto shd_uf_type=shd_uf_unit.via.array.ptr[1];
			auto shd_uf_type_sz = shd_uf_type.via.str.size;
			char* pshd_uf_type = new char[shd_uf_type_sz + 1];
			memset(pshd_uf_type, 0, shd_uf_type_sz + 1);
			memcpy(pshd_uf_type, shd_uf_type.via.str.ptr, shd_uf_type_sz);

			auto shd_uf_usize=shd_uf_unit.via.array.ptr[2];
			auto shd_uf_el_size=shd_uf_unit.via.array.ptr[3];
			auto shd_uf_utype=shd_uf_unit.via.array.ptr[4];
			
			shared_ptr<shader_uf> pnunf = std::move(fac_shader_uf::get().Create(pshd_uf_type, shd_uf_usize.as<GLuint>(), shd_uf_el_size.as<GLuint>()));
			pnunf->set_type(shd_uf_utype.as<GLenum>());
			auto shd_data = shd_uf_unit.via.array.ptr[5];
			auto wsize = shd_uf_usize.as<GLuint>()*shd_uf_el_size.as<GLuint>();
			memcpy(pnunf->get_data_head(), shd_data.via.bin.ptr,wsize);
			mpshd_uf[pshd_uf_name] = pnunf;
			delete[] pshd_uf_name;
			delete[] pshd_uf_type;
		}
		delete[] pstr_mtl_name;
	}
	TIME_CHECK(materil list res)
	auto obj_ui = obj_w.via.array.ptr[12];
	init_ui_component_by_mgo(_pj, obj_ui);
	TIME_CHECK(control list res)
	munmap(membk,sb.st_size);
}

