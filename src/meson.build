config_h = configuration_data()
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', 'lxi-gui')
config_h.set_quoted('LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))
config_h.set10('DEVEL_MODE', devel_mode)
configure_file(output: 'config.h', configuration: config_h)

common_sources = [
  'benchmark.c',
  'lxilua.c',
  'misc.c',
  'screenshot.c',
  'plugins/screenshot_keysight-dmm.c',
  'plugins/screenshot_rigol-dl3000.c',
  'plugins/screenshot_siglent-sdg.c',
  'plugins/screenshot_keysight-ivx.c',
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

lxi_deps = [
  dependency('readline', required: true),
  dependency('lua-5.3', required: true),
  dependency('liblxi', version: '>=1.13', required: true),
]

executable('lxi',
  lxi_sources,
  dependencies: lxi_deps,
  install: true,
)

subdir('bash-completion')

enable_gui = get_option('gui')
if enable_gui

  libglib_dep = dependency('glib-2.0', version: '>= 2.70', required: true,
                                    fallback : ['glib', 'libglib_dep'],
                                    default_options: ['tests=false'])

  libgtk_dep = dependency('gtk4', version: '>= 4.5.0', required: true,
                                    fallback : ['gtk', 'libgtk_dep'],
                                    default_options: ['introspection=disabled',
                                                      'demos=false',
                                                      'build-examples=false',
                                                      'build-tests=false'])

  libgtksourceview_dep = dependency('gtksourceview-5', version: '>= 5.3.3', required: true,
                                    fallback : ['gtksourceview', 'gtksource_dep'],
                                    default_options: ['introspection=disabled'])

  libadwaita_dep = dependency('libadwaita-1', version: '>= 1.0.1', required: true,
                                    fallback : ['libadwaita', 'libadwaita_dep'],
                                    default_options: ['introspection=disabled',
                                                      'examples=false',
                                                      'tests=false'])

  lxi_gui_sources = [
    'lxi_gui-main.c',
    'lxi_gui-window.c',
    'lxi_gui-application.c',
    'lxi_gui-prefs.c',
    'gtkchart.c',
    common_sources,
    ]
  
  lxi_gui_deps = [
    lxi_deps,
    libglib_dep,
    libgtk_dep,
    libgtksourceview_dep,
    libadwaita_dep,
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
  )
  
endif
