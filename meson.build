project('lxi-tools', 'c',
    version : run_command('version.sh', 'get-vcs').stdout().strip(),
    license : [ 'BSD-3-Clause'],
    meson_version : '>= 0.53.2',
    default_options : [ 'warning_level=2', 'buildtype=release', 'c_std=gnu11', 'b_lundef=false' ]
)

meson.add_dist_script('version.sh', 'set-dist', meson.project_version())

# The tag date of the project_version(), update when the version bumps.
version_date = '2024-11-30'
devel_mode = false

subdir('src')
subdir('man')

enable_gui = get_option('gui')
if enable_gui
  subdir('data')

  meson.add_install_script('build-aux/meson/postinstall.py')
endif
