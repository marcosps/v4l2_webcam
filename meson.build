project('v4l2_webcam', 'c')
add_project_arguments(['-Wall', '-Werror'], language: 'c')

sdl_dep = dependency('sdl')
sdl_img_dep = dependency('SDL_image')

executable('v4l2_webcam', 'v4l_webcam.c', dependencies: [sdl_dep, sdl_img_dep])
