project(kritafilters)

add_subdirectory(libkra)

if(JPEG_FOUND AND HAVE_LCMS2)
	add_subdirectory(jpeg)
endif()

if(TIFF_FOUND AND JPEG_FOUND)
	add_subdirectory(tiff)
endif()

if(PNG_FOUND)
    add_subdirectory(png)
    add_subdirectory(csv)
endif()

if(OpenEXR_FOUND)
    add_subdirectory(exr)
endif()

if(Poppler_Qt${QT_MAJOR_VERSION}_FOUND)
    add_subdirectory(pdf)
endif()

if(OpenJPEG_FOUND AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    add_subdirectory(jp2)
endif()

if(KF5KDcraw_FOUND OR KDcrawQt6_FOUND)
    add_subdirectory(raw)
endif()

add_subdirectory(svg)
add_subdirectory(qimageio)
add_subdirectory(ora)
add_subdirectory(xcf)
add_subdirectory(psd)
add_subdirectory(qml)
add_subdirectory(tga)
add_subdirectory(heightmap)
add_subdirectory(brush)
add_subdirectory(spriter)
add_subdirectory(kra)

if (GIF_FOUND)
    add_subdirectory(gif)
endif()

if (HEIF_FOUND)
    add_subdirectory(heif)
endif()

add_subdirectory(krz)

if (WebP_FOUND)
    add_subdirectory(webp)
endif()

if (JPEGXL_FOUND)
    add_subdirectory(jxl)
endif()

add_subdirectory(rgbe)
