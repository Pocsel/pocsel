if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	find_path (DirectX_ROOT_DIR
		Include/d3d9.h
		PATHS
		"$ENV{DXSDK_DIR}"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (June 2010)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (February 2010)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (March 2009)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (August 2008)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (June 2008)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (March 2008)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (November 2007)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK (August 2007)"
		"$ENV{ProgramFiles}/Microsoft DirectX SDK"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (June 2010)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (February 2010)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (March 2009)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (August 2008)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (June 2008)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (March 2008)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (November 2007)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK (August 2007)"
		"$ENV{ProgramFiles(x86)}/Microsoft DirectX SDK"
		DOC "DirectX SDK root directory")

	find_path (DirectX_D3D9_INCLUDE_DIR d3d9.h
		PATHS
		"${DirectX_ROOT_DIR}/Include"
		DOC "The directory where d3d9.h resides")

	find_path (DirectX_D3DX9_INCLUDE_DIR d3dx9.h
		PATHS
		"${DirectX_ROOT_DIR}/Include"
		DOC "The directory where d3dx9.h resides")

	if (CMAKE_CL_64)
		find_library (DirectX_D3D9_LIBRARY d3d9
			PATHS
			"${DirectX_ROOT_DIR}/Lib/x64"
			DOC "The directory where d3d9 resides")

		find_library (DirectX_D3DX9_LIBRARY d3dx9
			PATHS
			"${DirectX_ROOT_DIR}/Lib/x64"
			DOC "The directory where d3dx9 resides")
	else()
		find_library (DirectX_D3D9_LIBRARY d3d9
			PATHS
			"${DirectX_ROOT_DIR}/Lib/x86"
			DOC "The directory where d3d9 resides")

		find_library (DirectX_D3DX9_LIBRARY d3dx9
			PATHS
			"${DirectX_ROOT_DIR}/Lib/x86"
			DOC "The directory where d3dx9 resides")
	endif()

	if (DirectX_D3D9_INCLUDE_DIR AND DirectX_D3D9_LIBRARY)
		set (DirectX_D3D9_FOUND 1)
		if (DirectX_D3DX9_INCLUDE_DIR AND DirectX_D3DX9_LIBRARY)
			set (DirectX_D3DX9_FOUND 1)
		endif (DirectX_D3DX9_INCLUDE_DIR AND DirectX_D3DX9_LIBRARY)
	endif (DirectX_D3D9_INCLUDE_DIR AND DirectX_D3D9_LIBRARY)

	mark_as_advanced(
		DirectX_D3D9_INCLUDE_DIR
		DirectX_D3D9_LIBRARY
		DirectX_D3DX9_INCLUDE_DIR
		DirectX_D3DX9_LIBRARY)

endif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

mark_as_advanced (
	DirectX_D3D9_FOUND
	DirectX_D3DX9_FOUND)