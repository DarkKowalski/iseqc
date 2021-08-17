# frozen_string_literal: true

require_relative './lib/iseqc/version'

Gem::Specification.new do |s|
  s.name        = 'iseqc'
  s.version     = Iseqc::VERSION
  s.summary     = 'Iseq compiler and linker for RubyGems'
  s.description = 'Compile ruby to YARV instruction sequence, ' \
                  'link iseq from multiple files into a binary and do fast require/load in MRI'

  s.platform              = Gem::Platform::RUBY
  s.required_ruby_version = '>= 3.0.0'

  s.license = 'MIT'

  s.authors  = ['Kowalski Dark']
  s.email    = ['darkkowalski2012@gmail.com']
  s.homepage = 'https://github.com/darkkowalski/iseqc'

  s.files        = Dir['ext/**/*', 'lib/**/*', 'LICENSE', 'README.md']
  s.require_path = 'lib'
  s.extensions = ['ext/iseqc/extconf.rb']

  s.metadata = {
    'bug_tracker_uri' => 'https://github.com/darkkowalski/iseqc/issues'
  }

  s.add_development_dependency 'minitest', '~> 5.14.4'
  s.add_development_dependency 'minitest-reporters', '~> 1.4.3'
  s.add_development_dependency 'rake', '~> 13.0.6'
  s.add_development_dependency 'rake-compiler', '~> 1.1.1'
  s.add_development_dependency 'rubocop', '~> 1.9.0'
  s.add_development_dependency 'rubocop-minitest', '~> 0.10.3'
  s.add_development_dependency 'rubocop-rake', '~>0.5.1'
end
