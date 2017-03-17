/* define */

#ifndef H_DEFINE
#define H_DEFINE

#define USE_MSAA

// Simd
#undef _XM_SSE_INTRINSICS_
#define _XM_NO_INTRINSICS_

#ifdef _DEBUG
	#define	DEBUG	1
#else
	#define	DEBUG	0
#endif

#define I_PI ( 3.14159265359f )

#define I_SINGLETON( ClassName )	\
public:								\
	static ClassName* Instance()	\
	{								\
		static ClassName object;	\
		return &object;				\
	}

#define I_NEW( obj ) new obj

#define I_DELETE( p )		\
{							\
	if( p != nullptr )		\
	{						\
		delete ( p );		\
		( p ) = nullptr;	\
	}						\
}

#define I_DELETE_ARRAY( p )	\
{							\
	if( p != nullptr )		\
	{						\
		delete[] ( p );		\
		( p ) = nullptr;	\
	}						\
}

#define I_RELEASE( p )		\
{							\
	if( p != nullptr )		\
	{						\
		( p )->Release();	\
		( p ) = nullptr;	\
	}						\
}

#define I_LISTDELETE( x )								\
{														\
	for( auto obj = x.begin(); obj != x.end(); obj++ )	\
	{													\
		if( *obj != nullptr )							\
		{												\
			delete ( *obj );							\
			*obj = nullptr;								\
		}												\
	}													\
}

#define I_RETURN( x )	\
{						\
	hr = ( x );			\
	if( FAILED( hr ) )	\
	{					\
		return hr;		\
	}					\
}

#define I_RETURN_M( x, y )						\
{												\
	hr = ( x );									\
	if( FAILED( hr ) )							\
	{											\
		MessageBox( NULL, y, "Error", MB_OK );	\
		return hr;								\
	}											\
}

#define I_RETURN_MFNC( x, y, z )				\
{												\
	hr = ( x );									\
	if( FAILED( hr ) )							\
	{											\
		MessageBox( NULL, y, "Error", MB_OK );	\
		z;										\
		return hr;								\
	}											\
}

#define I_RETURN_C( x, y )	\
{							\
	hr = ( x );				\
	if( FAILED( hr ) )		\
	{						\
		return y;			\
	}						\
}

#define I_RETURN_FNC( x, y )	\
{								\
	hr = ( x );					\
	if( FAILED( hr ) )			\
	{							\
		{						\
			y;					\
		}						\
		return hr;				\
	}							\
}

#define I_RETURN_FNC_C( x, y, z )	\
{									\
	hr = ( x );						\
	if( FAILED( hr ) )				\
	{								\
		{							\
			y;						\
		}							\
		return z;					\
	}								\
}

#define I_NULLCHECK( x, y )	\
{							\
	if( x == nullptr )		\
	{						\
		return y;			\
	}						\
}

#define WINDOWCLASSNAME	"WindowClass"
#define WINDOWNAME		"GraphicsDemo"
#define WIDTH			1280
#define HEIGHT			920
#define STRNUM			128

namespace NDefine
{
	enum EnTexPath
	{
		enTexPath_wood,
		enTexPath_floor,
		enTexPath_concrete,
		enTexPath_Particle,
		enTexPath_rocks,
		enTexPath_stones,
		enTexPath_wall,
	};
	static const char* g_pcTexPath[] = 
	{
		"../../Media/Textures/wood.dds",
		"../../Media/Textures/floor.dds",
		"../../Media/Textures/concrete.dds",
		"../../Media/Textures/Particle.dds",
		"../../Media/Textures/rocks.dds",
		"../../Media/Textures/stones.dds",
		"../../Media/Textures/wall.dds",
	};
	enum EnNrmPath
	{
		enNrmPath_stones_NM_height,
		enNrmPath_saint_NM_height,
		enNrmPath_rocks_NM_height,
		enNrmPath_four_NM_height,
		enNrmPath_dent_NM_height,
		enNrmPath_bump_NM_height,
		enNrmPath_wall_NM_height,
	};
	static const char* g_pcNrmPath[] = 
	{
		"../../Media/Textures/stones_NM_height.dds",
		"../../Media/Textures/saint_NM_height.DDS",
		"../../Media/Textures/rocks_NM_height.dds",
		"../../Media/Textures/four_NM_height.DDS",
		"../../Media/Textures/dent_NM_height.DDS",
		"../../Media/Textures/bump_NM_height.DDS",
		"../../Media/Textures/wall_NM_height.DDS",
	};
}

#endif
