bashcompletiondir = get_option('bashcompletiondir')
if bashcompletiondir == ''
  bash_completion_dep = dependency('bash-completion', required: false)
  if bash_completion_dep.found()
    bashcompletiondir = join_paths(get_option('datadir'), 'bash-completion', 'completions')
  endif
endif

message(bashcompletiondir)

if (bashcompletiondir != 'no') and (bashcompletiondir != '')
  install_data('lxi', 'lxi.snap', install_dir: bashcompletiondir)
endif
