project('lxi-tools', 'c',
    version : '2.0',
    license : [ 'BSD-3-Clause'],
    meson_version : '>= 0.53.2',
    default_options : [ 'warning_level=2', 'buildtype=release', 'c_std=gnu11', 'b_lundef=false' ]
)

# The tag date of the project_version(), update when the version bumps.
version_date = '2022-01-31'
devel_mode = 0

subdir('src')
subdir('man')

enable_gui = get_option('gui')
if enable_gui
  i18n = import('i18n')

  subdir('data')
  subdir('po')

  meson.add_install_script('build-aux/meson/postinstall.py')
endif
