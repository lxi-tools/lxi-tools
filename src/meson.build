config_h = configuration_data()
config_h.set_quoted('VERSION', meson.project_version())
configure_file(output: 'config.h', configuration: config_h)

lxi_sources = [
  'benchmark.c',
  'discover.c',
  'lxilua.c',
  'main.c',
  'options.c',
  'run.c',
  'scpi.c',
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
  'plugins/screenshot_tektronix.c',
  'plugins/screenshot_tektronix-3000.c',
  ]

lxi_deps = [
  dependency('readline', required: true),
  dependency('lua-5.2', required: true),
  dependency('liblxi', version: '>=1.13', required: true),
]

executable('lxi',
  lxi_sources,
  dependencies: lxi_deps,
  install: true,
)

subdir('bash-completion')