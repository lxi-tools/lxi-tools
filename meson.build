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
  subdir('src/gui')
endif
