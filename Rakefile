# frozen_string_literal: true

require 'bundler/gem_tasks'
require 'rake/extensiontask'

gemspec = Gem::Specification.load('iseqc.gemspec')

Rake::ExtensionTask.new('iseqc', gemspec) do |ext|
  ext.ext_dir = 'ext/iseqc'
  ext.lib_dir = 'lib/iseqc'
end

Gem::PackageTask.new(gemspec) do |pkg|
  # If no block is supplied, then define needs to be called to define the task.
end

require 'rubocop/rake_task'
RuboCop::RakeTask.new(:rubocop) do |t|
  t.options = ['--display-cop-names']
end

task default: %w[rubocop compile]
