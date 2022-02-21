# See https://specifications.freedesktop.org/desktop-entry-spec/latest/ar01s02.html
install_data('io.github.lxi-tools.lxi-gui.desktop',
  install_dir: join_paths(get_option('datadir'), 'applications')
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: files('io.github.lxi-tools.lxi-gui.desktop')
  )
endif

# See https://www.freedesktop.org/software/appstream/docs/chap-Metadata.html#spec-component-location
# Change appdata to metainfo once meson it updated to support it
install_data('io.github.lxi-tools.lxi-gui.appdata.xml',
  install_dir: join_paths(get_option('datadir'), 'metainfo')
)

appstream_util = find_program('appstream-util', required: false)
if appstream_util.found()
  test('Validate appstream file', appstream_util,
    args: ['validate', files('io.github.lxi-tools.lxi-gui.appdata.xml')]
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
