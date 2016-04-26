#pragma once

#include <Windows.h>
#pragma warning(disable : 4838)
#include <xnamath.h>
#pragma warning(default : 4838)
#include <mmsystem.h>
#pragma comment( lib, "winmm.lib" )
#include <string>

#pragma warning(disable : 4005)
#include <d3dx11.h>
#pragma warning(default : 4005)

#pragma warning(disable : 4316)
#include <crtdbg.h>
#pragma warning(default : 4316)
#pragma comment( lib, "d3d11.lib" )

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx11d.lib" )

#define new					new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(s)			_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s)		_calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(p, s)		_realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _recalloc(p, c, s)	_recalloc_dbg(p, c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _expand(p, s)		_expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)

#else
#pragma comment( lib, "d3dx11.lib" )
#endif

#pragma comment( lib, "dxgi.lib" )

#define J_PI XM_PI

namespace Javelin {
	typedef union color {
		struct {
			float r, g, b, a;
		}col;
		struct {
			float rgba[4];
		}ary;
		color() {
			col.r = col.g = col.b = col.a = 0.0f;
		}
		color(float r_, float g_, float b_, float a_) {
			col.r = r_;
			col.g = g_;
			col.b = b_;
			col.a = a_;
		}
		color(float rgba_[4]) {
			std::memcpy(ary.rgba, rgba_, sizeof(ary));
		}
		bool operator==(const color& rhs) {
			return (col.r == rhs.col.r) && (col.g == rhs.col.g) && (col.b == rhs.col.b) && (col.a == rhs.col.a);
		}
		bool operator!=(const color& rhs) {
			return !(*this == rhs);
		}
	}COLOR;
}
