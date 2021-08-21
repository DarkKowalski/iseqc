# frozen_string_literal: true

require 'bootsnap'
require 'benchmark'

Bootsnap.setup(
  cache_dir: 'tmp/cache', # Path to your cache
  development_mode: 'development', # Current working environment, e.g. RACK_ENV, RAILS_ENV, etc
  load_path_cache: true, # Optimize the LOAD_PATH with a cache
  compile_cache_iseq: true,                 # Compile Ruby code into ISeq cache, breaks coverage reporting.
  compile_cache_yaml: true                  # Compile YAML into a cache
)

cases = File.join('.', File.dirname(__FILE__), '..', 'tmp', 'bench', 'cases')
raw = Dir[File.join(cases, '*.rb')]
iterations = 100

Benchmark.bmbm do |x|
  x.report('Kernel#require') do
    iterations.times do
      raw.each { |r| require r }
    end
  end

  x.report('Kernel#load') do
    iterations.times do
      raw.each { |r| load r }
    end
  end
end
