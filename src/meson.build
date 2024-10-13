config_h = configuration_data()
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', 'lxi-gui')
config_h.set_quoted('LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))
config_h.set('DEVEL_MODE', devel_mode)
configure_file(output: 'config.h', configuration: config_h)

common_sources = [
  'benchmark.c',
  'lxilua.c',
  'misc.c',
  'screenshot.c',
  'plugins/screenshot_keysight-dmm.c',
  'plugins/screenshot_keysight-psa.c',
  'plugins/screenshot_keysight-pxa.c',
  'plugins/screenshot_rigol-dl3000.c',
  'plugins/screenshot_siglent-sdg.c',
  'plugins/screenshot_keysight-ivx.c',
  'plugins/screenshot_keysight-dso.c',
  'plugins/screenshot_lecroy.c',
  'plugins/screenshot_rigol-dm3068.c',
  'plugins/screenshot_siglent-sdm3000.c',
  'plugins/screenshot_rigol-1000z.c',
  'plugins/screenshot_rigol-dp800.c',
  'plugins/screenshot_siglent-sds.c',
  'plugins/screenshot_rigol-2000.c',
  'plugins/screenshot_rigol-dsa.c',
  'plugins/screenshot_siglent-ssa3000x.c',
  'plugins/screenshot_rigol-dg.c',
  'plugins/screenshot_rohde-schwarz-hmo-rtb.c',
  'plugins/screenshot_rohde-schwarz-ng.c',
  'plugins/screenshot_tektronix.c',
  'plugins/screenshot_tektronix-3000.c',
  'plugins/screenshot_rohde-schwarz-rth.c',
  'plugins/screenshot_tektronix-mso-5.c',
]

lxi_sources = [
  'benchmark.c',
  'discover.c',
  'lxilua.c',
  'main.c',
  'options.c',
  'run.c',
  'scpi.c',
  common_sources,
  ]

foreach name: ['lua-5.4', 'lua-5.3', 'lua-5.2', 'lua-5.1', 'lua']
  lua_dep = dependency(name, version: '>=5.1', required: false)
  if lua_dep.found()
    break
  endif
endforeach
if not lua_dep.found()
  error('Lua could not be found!')
endif

compiler = meson.get_compiler('c')

lxi_deps = [
  compiler.find_library('readline', required: true),
  dependency('liblxi', version: '>=1.13', required: true),
  lua_dep,
]

executable('lxi',
  lxi_sources,
  dependencies: lxi_deps,
  install: true,
)

subdir('bash-completion')

enable_gui = get_option('gui')
if enable_gui

  libglib_dep = dependency('glib-2.0', version: '>= 2.74', required: true,
                                    fallback : ['glib', 'libglib_dep'],
                                    default_options: ['tests=false'])

  libgtk_dep = dependency('gtk4', version: '>= 4.6.0', required: true,
                                    fallback : ['gtk', 'libgtk_dep'],
                                    default_options: ['introspection=disabled',
                                                      'demos=false',
                                                      'build-examples=false',
                                                      'build-tests=false'])

  libcairo_dep = dependency('cairo', required: true)
  libgdk_pixbuf_dep = dependency('gdk-pixbuf-2.0', required: true)

  libgtksourceview_dep = dependency('gtksourceview-5', version: '>= 5.4.0', required: true,
                                    fallback : ['gtksourceview', 'gtksource_dep'],
                                    default_options: ['introspection=disabled'])

  libadwaita_dep = dependency('libadwaita-1', version: '>= 1.2', required: true,
                                    fallback : ['libadwaita', 'libadwaita_dep'],
                                    default_options: ['introspection=disabled',
                                                      'examples=false',
                                                      'tests=false'])

  libjson_glib_dep = dependency('json-glib-1.0', version: '>= 1.4', required: true,
                                    fallback : ['json-glib', 'json_glib_dep'],
                                    default_options: ['introspection=disabled'])

  lxi_gui_sources = [
    'lxi_gui-main.c',
    'lxi_gui-window.c',
    'lxi_gui-application.c',
    'lxi_gui-prefs.c',
    'gtkchart.c',
    common_sources,
    ]

  if(build_machine.system() == 'cygwin')
    convert = find_program('/ucrt64/bin/convert', required: true)
    # Add icon in  exe
    lxi_tools_icon = custom_target('icon',
      input : '../data/icons/hicolor/scalable/apps/io.github.lxi-tools.lxi-gui.svg',
      output : 'lxi-tools.ico',
      command : [convert, '-density', '128x128','-background', 'transparent', '@INPUT@', '@OUTPUT@'])
    lxi_gui_sources += lxi_tools_icon
    windows = import('windows')
    lxi_gui_sources += windows.compile_resources('lxi-tools.rc')
  endif

  lxi_gui_deps = [
    lxi_deps,
    libglib_dep,
    libgtk_dep,
    libcairo_dep,
    libgdk_pixbuf_dep,
    libgtksourceview_dep,
    libadwaita_dep,
    libjson_glib_dep,
  ]

  gnome = import('gnome')

  lxi_gui_sources += gnome.compile_resources(
    'lxi_gui-resources',
    'lxi_gui.gresource.xml',
    c_name: 'lxi_gui',
  )

  executable('lxi-gui', lxi_gui_sources,
    dependencies: lxi_gui_deps,
    install: true,
    win_subsystem: 'windows' # on windows it start with cmd without this settings.
  )

endif
