# See https://specifications.freedesktop.org/desktop-entry-spec/latest/ar01s02.html
desktop_file = i18n.merge_file(
  input: 'io.github.lxi-tools.lxi-gui.desktop.in',
  output: 'io.github.lxi-tools.lxi-gui.desktop',
  type: 'desktop',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'applications')
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file]
  )
endif

# See https://www.freedesktop.org/software/appstream/docs/chap-Metadata.html#spec-component-location
appstream_file = i18n.merge_file(
  input: 'io.github.lxi-tools.lxi-gui.metainfo.xml.in',
  output: 'io.github.lxi-tools.lxi-gui.metainfo.xml',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'metainfo')
)

appstream_util = find_program('appstream-util', required: false)
if appstream_util.found()
  test('Validate appstream file', appstream_util,
    args: ['validate', appstream_file]
  )
endif

install_data('io.github.lxi-tools.lxi-gui.gschema.xml',
  install_dir: join_paths(get_option('datadir'), 'glib-2.0/schemas')
)

compile_schemas = find_program('glib-compile-schemas', required: false)
if compile_schemas.found()
  test('Validate schema file', compile_schemas,
    args: ['--strict', '--dry-run', meson.current_source_dir()]
  )
endif

subdir('icons')
