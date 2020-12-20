project('in_xsf', 'cpp', version: '1.11.1',
        default_options: ['optimization=g',
                          'warning_level=3',
                          'cpp_std=c++20',
                          'c_std=c2x',
                          'b_coverage=true'])


inc = include_directories('include')


run_target('sonarqube_report',
           command: 'scripts/sonarqube_report.sh')


subdir('src')
subdir('src/in_2sf')
subdir('src/in_gsf')
subdir('src/in_ncsf')
subdir('src/in_snsf')
subdir('src/in_xsf_framework')
