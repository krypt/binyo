=begin

= Info

binyo - Fast binary IO for Ruby

Copyright (C) 2012-2013
Martin Bosslet <martin.bosslet@gmail.com>
All rights reserved.

= License
See the file 'LICENSE' for further details.

=end

require 'mkmf'

dir_config('profiler')

message "=== binyo - Fast binary IO for Ruby ===\n"

arg = ARGV.shift
if arg
  if arg.include? "-g"
    debug = true
  elsif arg.include? "-p"
    profiler = true
    unless have_library("profiler", "ProfilerStart")
      message "'libprofiler' could not be found.\n"
      exit 1
    end 
  end
end

if CONFIG['GCC'] == 'yes'
  if debug
    flags = "--coverage -g3 -fprofile-arcs -ftest-coverage"
    message "!! set #{flags} for coverage !!"
    $CFLAGS += " #{flags}"
    $DLDFLAGS += " #{flags}"
    $LIBS += " -lgcov"
  end
  if profiler
    message "Linking to profiler library\n"
    pkg_config('profiler')
    $LIBS += " -lprofiler"
  end
end

message "=== Checking Ruby features ===\n"

have_header("ruby/io.h")
have_func("rb_io_check_byte_readable")

create_header
create_makefile("binyo")
message "Done.\n"
