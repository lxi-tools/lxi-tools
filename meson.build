project('lxi-tools', 'c',
    version : '1.22',
    license : [ 'BSD-3-Clause'],
    meson_version : '>= 0.53.2',
    default_options : [ 'warning_level=2', 'buildtype=release', 'c_std=gnu11', 'b_lundef=false' ]
)

# The tag date of the project_version(), update when the version bumps.
version_date='2021-12-23'

subdir('src')
subdir('man')

enable_gui = get_option('gui')
if enable_gui

  libglib_dep = dependency('glib-2.0', version: '>=2.70.0',
    fallback : ['glib','libglib_dep'])

  libgir_dep = dependency('gobject-introspection-1.0', version: '>= 1.70',
    fallback : ['gobject-introspection','girepo_dep'])

  libgtksourceview_dep = dependency('gtksourceview-5', version: '>= 5.3',
    fallback : ['gtksourceview', 'gtksource_dep'])

  subdir('src/gui')

endif
