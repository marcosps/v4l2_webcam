project('v4l2_webcam', 'c')
add_project_arguments(['-Wall', '-Werror'], language: 'c')

sdl_dep = dependency('sdl')
#TODO: fix sdl_image and remove link_args
#sdl_img_dep = dependency('sdl-image')

executable('v4l2_webcam', 'v4l_webcam.c'
	, link_args: '-lSDL_image'
	, dependencies: [sdl_dep])
