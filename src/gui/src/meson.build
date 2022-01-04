config_h = configuration_data()
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', 'lxi-gui')
config_h.set_quoted('LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))
configure_file(
  output: 'lxi_gui-config.h',
  configuration: config_h,
)

lxi_gui_includedir = [include_directories('../..')]

lxi_gui_sources = [
  'main.c',
  'lxi_gui-window.c',
  'lxi_gui-application.c',
  'lxi_gui-prefs.c',
  '../../lxilua.c',
  '../../screenshot.c',
  '../../benchmark.c',
  '../../misc.c',
  '../../plugins/screenshot_keysight-dmm.c',
  '../../plugins/screenshot_rigol-dl3000.c',
  '../../plugins/screenshot_siglent-sdg.c',
  '../../plugins/screenshot_keysight-ivx.c',
  '../../plugins/screenshot_rigol-dm3068.c',
  '../../plugins/screenshot_siglent-sdm3000.c',
  '../../plugins/screenshot_rigol-1000z.c',
  '../../plugins/screenshot_rigol-dp800.c',
  '../../plugins/screenshot_siglent-sds.c',
  '../../plugins/screenshot_rigol-2000.c',
  '../../plugins/screenshot_rigol-dsa.c',
  '../../plugins/screenshot_siglent-ssa3000x.c',
  '../../plugins/screenshot_rigol-dg.c',
  '../../plugins/screenshot_rohde-schwarz-hmo-rtb.c',
  '../../plugins/screenshot_rohde-schwarz-ng.c',
  '../../plugins/screenshot_tektronix.c',
  '../../plugins/screenshot_tektronix-3000.c',
  ]

lxi_gui_deps = [
  dependency('gtk4', version: '>= 4.4'),
  dependency('liblxi', required: true),
  dependency('lua-5.2', required: true),
]

gnome = import('gnome')

lxi_gui_sources += gnome.compile_resources(
  'lxi_gui-resources',
  'lxi_gui.gresource.xml',
  c_name: 'lxi_gui',
)

executable('lxi-gui', lxi_gui_sources,
  dependencies: lxi_gui_deps,
  include_directories: lxi_gui_includedir,
  install: true,
)
